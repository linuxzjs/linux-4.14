/*
 * mm/rmap.c - physical to virtual reverse mappings
 *
 * Copyright 2001, Rik van Riel <riel@conectiva.com.br>
 * Released under the General Public License (GPL).
 *
 * Simple, low overhead reverse mapping scheme.
 * Please try to keep this thing as modular as possible.
 *
 * Provides methods for unmapping each kind of mapped page:
 * the anon methods track anonymous pages, and
 * the file methods track pages belonging to an inode.
 *
 * Original design by Rik van Riel <riel@conectiva.com.br> 2001
 * File methods by Dave McCracken <dmccr@us.ibm.com> 2003, 2004
 * Anonymous methods by Andrea Arcangeli <andrea@suse.de> 2004
 * Contributions by Hugh Dickins 2003, 2004
 */

/*
 * Lock ordering in mm:
 *
 * inode->i_mutex	(while writing or truncating, not reading or faulting)
 *   mm->mmap_sem
 *     page->flags PG_locked (lock_page)
 *       hugetlbfs_i_mmap_rwsem_key (in huge_pmd_share)
 *         mapping->i_mmap_rwsem
 *           anon_vma->rwsem
 *             mm->page_table_lock or pte_lock
 *               zone_lru_lock (in mark_page_accessed, isolate_lru_page)
 *               swap_lock (in swap_duplicate, swap_info_get)
 *                 mmlist_lock (in mmput, drain_mmlist and others)
 *                 mapping->private_lock (in __set_page_dirty_buffers)
 *                   mem_cgroup_{begin,end}_page_stat (memcg->move_lock)
 *                     mapping->tree_lock (widely used)
 *                 inode->i_lock (in set_page_dirty's __mark_inode_dirty)
 *                 bdi.wb->list_lock (in set_page_dirty's __mark_inode_dirty)
 *                   sb_lock (within inode_lock in fs/fs-writeback.c)
 *                   mapping->tree_lock (widely used, in set_page_dirty,
 *                             in arch-dependent flush_dcache_mmap_lock,
 *                             within bdi.wb->list_lock in __sync_single_inode)
 *
 * anon_vma->rwsem,mapping->i_mutex      (memory_failure, collect_procs_anon)
 *   ->tasklist_lock
 *     pte map lock
 */

#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/sched/task.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/ksm.h>
#include <linux/rmap.h>
#include <linux/rcupdate.h>
#include <linux/export.h>
#include <linux/memcontrol.h>
#include <linux/mmu_notifier.h>
#include <linux/migrate.h>
#include <linux/hugetlb.h>
#include <linux/backing-dev.h>
#include <linux/page_idle.h>
#include <linux/memremap.h>
#include <linux/userfaultfd_k.h>

#include <asm/tlbflush.h>

#include <trace/events/tlb.h>

#include "internal.h"

static struct kmem_cache *anon_vma_cachep;
static struct kmem_cache *anon_vma_chain_cachep;

/*通过将分配的anon_vma结构的成员的程度设置为1，可以确定它在顶部，并使父节点和根节点指向自身*/
static inline struct anon_vma *anon_vma_alloc(void)
{
	struct anon_vma *anon_vma;

	anon_vma = kmem_cache_alloc(anon_vma_cachep, GFP_KERNEL);
	if (anon_vma) {
		atomic_set(&anon_vma->refcount, 1);
		anon_vma->degree = 1;	/* Reference for first vma */
		anon_vma->parent = anon_vma;
		/*
		 * Initialise the anon_vma root to point to itself. If called
		 * from fork, the root will be reset to the parents anon_vma.
		 */
		anon_vma->root = anon_vma;
	}

	return anon_vma;
}

/*释放anon_vma结构体*/
static inline void anon_vma_free(struct anon_vma *anon_vma)
{
	VM_BUG_ON(atomic_read(&anon_vma->refcount));

	/*
	 * Synchronize against page_lock_anon_vma_read() such that
	 * we can safely hold the lock without the anon_vma getting
	 * freed.
	 *
	 * Relies on the full mb implied by the atomic_dec_and_test() from
	 * put_anon_vma() against the acquire barrier implied by
	 * down_read_trylock() from page_lock_anon_vma_read(). This orders:
	 *
	 * page_lock_anon_vma_read()	VS	put_anon_vma()
	 *   down_read_trylock()		  atomic_dec_and_test()
	 *   LOCK				  MB
	 *   atomic_read()			  rwsem_is_locked()
	 *
	 * LOCK should suffice since the actual taking of the lock must
	 * happen _before_ what follows.
	 */
	might_sleep();
	if (rwsem_is_locked(&anon_vma->root->rwsem)) {
		anon_vma_lock_write(anon_vma);
		anon_vma_unlock_write(anon_vma);
	}

	kmem_cache_free(anon_vma_cachep, anon_vma);
}

static inline struct anon_vma_chain *anon_vma_chain_alloc(gfp_t gfp)
{
	return kmem_cache_alloc(anon_vma_chain_cachep, gfp);
}

static void anon_vma_chain_free(struct anon_vma_chain *anon_vma_chain)
{
	kmem_cache_free(anon_vma_chain_cachep, anon_vma_chain);
}

static void anon_vma_chain_link(struct vm_area_struct *vma,
				struct anon_vma_chain *avc,
				struct anon_vma *anon_vma)
{
    /*avc指向vma与anon_vma*/
	avc->vma = vma;
	avc->anon_vma = anon_vma;
	list_add(&avc->same_vma, &vma->anon_vma_chain);//将avc添加到vma的anon_vma_chain列表中
	anon_vma_interval_tree_insert(avc, &anon_vma->rb_root);//将avc添加到anon_vma的RB树中
}

/**
 * __anon_vma_prepare - attach an anon_vma to a memory region
 * @vma: the memory region in question
 *
 * This makes sure the memory mapping described by 'vma' has
 * an 'anon_vma' attached to it, so that we can associate the
 * anonymous pages mapped into it with that anon_vma.
 *
 * The common case will be that we already have one, which
 * is handled inline by anon_vma_prepare(). But if
 * not we either need to find an adjacent mapping that we
 * can re-use the anon_vma from (very common when the only
 * reason for splitting a vma has been mprotect()), or we
 * allocate a new one.
 *
 * Anon-vma allocations are very subtle, because we may have
 * optimistically looked up an anon_vma in page_lock_anon_vma_read()
 * and that may actually touch the spinlock even in the newly
 * allocated vma (it depends on RCU to make sure that the
 * anon_vma isn't actually destroyed).
 *
 * As a result, we need to do proper anon_vma locking even
 * for the new allocation. At the same time, we do not want
 * to do any locking for the common case of already having
 * an anon_vma.
 *
 * This must be called with the mmap_sem held for reading.
 */
int __anon_vma_prepare(struct vm_area_struct *vma)
{
	struct mm_struct *mm = vma->vm_mm;
	struct anon_vma *anon_vma, *allocated;
	struct anon_vma_chain *avc;

	might_sleep();
    /*分配创建一个anon_vma_chain也就是avc，并判断是否创建成功，如果失败goto out_enomem*/
	avc = anon_vma_chain_alloc(GFP_KERNEL);
	if (!avc)
		goto out_enomem;
    /*
    如果相邻的vma可以可并，则合并公用一个anon_vma，如果不能合并则通过anon_vma = anon_vma_alloc()
    重新分配一个anon_vma,
    */
	anon_vma = find_mergeable_anon_vma(vma);
	allocated = NULL;
	if (!anon_vma) {
		anon_vma = anon_vma_alloc();
		if (unlikely(!anon_vma))
			goto out_enomem_free_avc;
		allocated = anon_vma;
	}

	anon_vma_lock_write(anon_vma);
	/* page_table_lock to protect against threads */
	spin_lock(&mm->page_table_lock);
	if (likely(!vma->anon_vma)) {
		vma->anon_vma = anon_vma;
        /*通过rb红黑树，链表 将vma, avc， av关联起来*/
		anon_vma_chain_link(vma, avc, anon_vma);
		/* vma reference or self-parent link for new root */
		anon_vma->degree++;
		allocated = NULL;
		avc = NULL;
	}
	spin_unlock(&mm->page_table_lock);
	anon_vma_unlock_write(anon_vma);
    /*如果anon_vma和anon_vma_chain无法附加到vma，则分配的anon_vma和anon_vma_chain被释放*/
	if (unlikely(allocated))
		put_anon_vma(allocated);
	if (unlikely(avc))
		anon_vma_chain_free(avc);

	return 0;

 out_enomem_free_avc:
	anon_vma_chain_free(avc);
 out_enomem:
	return -ENOMEM;
}

/*
 * This is a useful helper function for locking the anon_vma root as
 * we traverse the vma->anon_vma_chain, looping over anon_vma's that
 * have the same vma.
 *
 * Such anon_vma's should have the same root, so you'd expect to see
 * just a single mutex_lock for the whole traversal.
 */
static inline struct anon_vma *lock_anon_vma_root(struct anon_vma *root, struct anon_vma *anon_vma)
{
	struct anon_vma *new_root = anon_vma->root;
	if (new_root != root) {
		if (WARN_ON_ONCE(root))
			up_write(&root->rwsem);
		root = new_root;
		down_write(&root->rwsem);
	}
	return root;
}

static inline void unlock_anon_vma_root(struct anon_vma *root)
{
	if (root)
		up_write(&root->rwsem);
}

/*
 * Attach the anon_vmas from src to dst.
 * Returns 0 on success, -ENOMEM on failure.
 *
 * If dst->anon_vma is NULL this function tries to find and reuse existing
 * anon_vma which has no vmas and only one child anon_vma. This prevents
 * degradation of anon_vma hierarchy to endless linear chain in case of
 * constantly forking task. On the other hand, an anon_vma with more than one
 * child isn't reused even if there was no alive vma, thus rmap walker has a
 * good chance of avoiding scanning the whole hierarchy when it searches where
 * page is mapped.
 */
/* dst为子进程的vma，src为父进程的vma */
int anon_vma_clone(struct vm_area_struct *dst, struct vm_area_struct *src)
{
	struct anon_vma_chain *avc, *pavc;
	struct anon_vma *root = NULL;
    /* 遍历父进程的每个anon_vma_chain链表中的结点，保存在pavc中 */
	list_for_each_entry_reverse(pavc, &src->anon_vma_chain, same_vma) {
		struct anon_vma *anon_vma;
        /* 分配一个新的avc结构 */
		avc = anon_vma_chain_alloc(GFP_NOWAIT | __GFP_NOWARN);
		if (unlikely(!avc)) {
             /* 如果分配失败 */
			unlock_anon_vma_root(root);
			root = NULL;
            /* 重新分配一次avc*/
			avc = anon_vma_chain_alloc(GFP_KERNEL);
			if (!avc)
				goto enomem_failure;
		}
        /* 获取父结点的pavc指向的anon_vma */
		anon_vma = pavc->anon_vma;
        /* 对anon_vma的root上锁
         * 如果root != anon_vma->root，则对root上锁，并返回anon_vma->root
         * 第一次循环，root = NULL
         */
		root = lock_anon_vma_root(root, anon_vma);
        /* 
         * 设置新的avc->vma指向子进程的vma
         * 设置新的avc->anon_vma指向父进程anon_vma_chain结点指向的anon_vma(这个anon_vma不一定属于父进程)
         * 将新的avc->same_vma加入到子进程的anon_vma_chain链表头部
         * 将新的avc->rb加入到父进程anon_vma_chain结点指向的anon_vma
         * 简单的描述就是vma(子进程) <---------> avc <----------> av(父进程)
         */
		anon_vma_chain_link(dst, avc, anon_vma);

		/*
		 * Reuse existing anon_vma if its degree lower than two,
		 * that means it has no vma and only one anon_vma child.
		 *
		 * Do not chose parent anon_vma, otherwise first child
		 * will always reuse it. Root anon_vma is never reused:
		 * it has self-parent reference and at least one child.
		 */
		if (!dst->anon_vma && anon_vma != src->anon_vma &&
				anon_vma->degree < 2)
			dst->anon_vma = anon_vma;
	}
	if (dst->anon_vma)
		dst->anon_vma->degree++;
	unlock_anon_vma_root(root);  /* 释放根的锁 */
	return 0;

 enomem_failure:
	/*
	 * dst->anon_vma is dropped here otherwise its degree can be incorrectly
	 * decremented in unlink_anon_vmas().
	 * We can safely do this because callers of anon_vma_clone() don't care
	 * about dst->anon_vma if anon_vma_clone() failed.
	 */
	dst->anon_vma = NULL;
	unlink_anon_vmas(dst);
	return -ENOMEM;
}

/*
 * Attach vma to its own anon_vma, as well as to the anon_vmas that
 * the corresponding VMA in the parent process is attached to.
 * Returns 0 on success, non-zero on failure.
 */
/*该函数必须明确一点：vma是子进程的虚拟空间， pvma是父进程虚拟地址空间;vma是目的dst目的地址空间，pvma是src源地址空间*/
int anon_vma_fork(struct vm_area_struct *vma, struct vm_area_struct *pvma)
{
	struct anon_vma_chain *avc;
	struct anon_vma *anon_vma;
	int error;
    
	/* Don't bother if the parent process has no anon_vma here. */
    /*判断父进程pvma是否指定anon_vma如果不存在，则直接return 0，如果存在则继续向下执行*/
	if (!pvma->anon_vma)
		return 0;

	/* Drop inherited anon_vma, we'll reuse existing or allocate new. */
	vma->anon_vma = NULL;//初始化vma设置为NULL

	/*
	 * First, attach the new VMA to the parent VMA's anon_vmas,
	 * so rmap can find non-COWed pages in child processes.
	 */
	 /* 这里开始先检查父进程的此vma是否有anon_vma，有则继续，而上面进行了判断，只有父进程的此vma有anon_vma才会执行到这里
     * 这里会遍历父进程的vma的anon_vma_chain链表，对每个结点新建一个anon_vma_chain，然后
     * 设置新的avc->vma指向子进程的vma
     * 设置新的avc->anon_vma指向父进程anon_vma_chain结点指向的anon_vma(这个anon_vma不一定属于父进程)
     * 将新的avc->same_vma加入到子进程的anon_vma_chain链表中
     * 将新的avc->rb加入到父进程anon_vma_chain结点指向的anon_vma
     * 最终达到的结果就是通过新创建的avc链接上父进程av,同时链接子进程的vma;
     * av(父进程) <----------> avc <---------> vma(子进程)
     */
	error = anon_vma_clone(vma, pvma);
	if (error)
		return error;

	/* An existing anon_vma has been reused, all done then. */
    /*如果子进程vma获取到的anon_vma已经被重用则直接return 0*/
	if (vma->anon_vma)
		return 0;

	/* Then add our own anon_vma. */
    /* 分配一个anon_vma结构用于子进程，将其refcount设为1，anon_vma->root指向本身
     * 即使此vma是用于映射文件的，也会分配一个anon_vma
     */
	anon_vma = anon_vma_alloc();
	if (!anon_vma)
		goto out_error;
     /*为子进程创建一个新的anon_vma_chain，也就是avc*/
	avc = anon_vma_chain_alloc(GFP_KERNEL);
	if (!avc)
		goto out_error_free_anon_vma;

	/*
	 * The root anon_vma's spinlock is the lock actually used when we
	 * lock any of the anon_vmas in this anon_vma tree.
	 */
	/* 将新的anon_vma的root指向父进程的anon_vma的root */
	anon_vma->root = pvma->anon_vma->root;
    /* 将新的anon_vma的parent指向父进程的anon_vma*/
	anon_vma->parent = pvma->anon_vma;
	/*
	 * With refcounts, an anon_vma can stay around longer than the
	 * process it belongs to. The root anon_vma needs to be pinned until
	 * this anon_vma is freed, because the lock lives in the root.
	 */
	/* 对父进程与子进程的anon_vma共同的root的refcount进行+1 */
	get_anon_vma(anon_vma->root);
	/* Mark this anon_vma as the one where our new (COWed) pages go. */
    /* 将上面建立链接的anon_vma赋值到子进程vma的anon_vma上*/
	vma->anon_vma = anon_vma;
    /* 对这个新的anon_vma上锁 */
	anon_vma_lock_write(anon_vma);
    /* 新的avc的vma指向子进程的vma
     * 新的avc的anon_vma指向子进程vma的anon_vma
     * 新的avc的same_vma加入到子进程vma的anon_vma_chain链表的头部
     * 新的avc的rb加入到子进程vma的anon_vma的红黑树中
     * 也就是子进程的av(子进程) <----------> avc(子进程) <---------> vma(子进程)
     */
	anon_vma_chain_link(vma, avc, anon_vma);
	anon_vma->parent->degree++;
    /* 对这个anon_vma解锁 */
	anon_vma_unlock_write(anon_vma);

	return 0;

 out_error_free_anon_vma:
	put_anon_vma(anon_vma);
 out_error:
	unlink_anon_vmas(vma);
	return -ENOMEM;
}

void unlink_anon_vmas(struct vm_area_struct *vma)
{
	struct anon_vma_chain *avc, *next;
	struct anon_vma *root = NULL;

	/*
	 * Unlink each anon_vma chained to the VMA.  This list is ordered
	 * from newest to oldest, ensuring the root anon_vma gets freed last.
	 */
	list_for_each_entry_safe(avc, next, &vma->anon_vma_chain, same_vma) {
		struct anon_vma *anon_vma = avc->anon_vma;

		root = lock_anon_vma_root(root, anon_vma);
		anon_vma_interval_tree_remove(avc, &anon_vma->rb_root);

		/*
		 * Leave empty anon_vmas on the list - we'll need
		 * to free them outside the lock.
		 */
		if (RB_EMPTY_ROOT(&anon_vma->rb_root.rb_root)) {
			anon_vma->parent->degree--;
			continue;
		}

		list_del(&avc->same_vma);
		anon_vma_chain_free(avc);
	}
	if (vma->anon_vma)
		vma->anon_vma->degree--;
	unlock_anon_vma_root(root);

	/*
	 * Iterate the list once more, it now only contains empty and unlinked
	 * anon_vmas, destroy them. Could not do before due to __put_anon_vma()
	 * needing to write-acquire the anon_vma->root->rwsem.
	 */
	list_for_each_entry_safe(avc, next, &vma->anon_vma_chain, same_vma) {
		struct anon_vma *anon_vma = avc->anon_vma;

		VM_WARN_ON(anon_vma->degree);
		put_anon_vma(anon_vma);

		list_del(&avc->same_vma);
		anon_vma_chain_free(avc);
	}
}

static void anon_vma_ctor(void *data)
{
	struct anon_vma *anon_vma = data;

	init_rwsem(&anon_vma->rwsem);
	atomic_set(&anon_vma->refcount, 0);
	anon_vma->rb_root = RB_ROOT_CACHED;
}

/*
创建一个anon_vma slub缓存，slub缓存是为了分配anon_vma结构，并且在初始化时调用anon_vma_ctor（）函数完成该结构体的初始化操作
并创建slub缓存以分配anon_vma_chain结构。
*/
void __init anon_vma_init(void)
{
	anon_vma_cachep = kmem_cache_create("anon_vma", sizeof(struct anon_vma),
			0, SLAB_TYPESAFE_BY_RCU|SLAB_PANIC|SLAB_ACCOUNT,
			anon_vma_ctor);
	anon_vma_chain_cachep = KMEM_CACHE(anon_vma_chain,
			SLAB_PANIC|SLAB_ACCOUNT);
}

/*
 * Getting a lock on a stable anon_vma from a page off the LRU is tricky!
 *
 * Since there is no serialization what so ever against page_remove_rmap()
 * the best this function can do is return a locked anon_vma that might
 * have been relevant to this page.
 *
 * The page might have been remapped to a different anon_vma or the anon_vma
 * returned may already be freed (and even reused).
 *
 * In case it was remapped to a different anon_vma, the new anon_vma will be a
 * child of the old anon_vma, and the anon_vma lifetime rules will therefore
 * ensure that any anon_vma obtained from the page will still be valid for as
 * long as we observe page_mapped() [ hence all those page_mapped() tests ].
 *
 * All users of this function must be very careful when walking the anon_vma
 * chain and verify that the page in question is indeed mapped in it
 * [ something equivalent to page_mapped_in_vma() ].
 *
 * Since anon_vma's slab is DESTROY_BY_RCU and we know from page_remove_rmap()
 * that the anon_vma pointer from page->mapping is valid if there is a
 * mapcount, we can dereference the anon_vma after observing those.
 */
struct anon_vma *page_get_anon_vma(struct page *page)
{
	struct anon_vma *anon_vma = NULL;
	unsigned long anon_mapping;

	rcu_read_lock();
	anon_mapping = (unsigned long)READ_ONCE(page->mapping);
	if ((anon_mapping & PAGE_MAPPING_FLAGS) != PAGE_MAPPING_ANON)
		goto out;
	if (!page_mapped(page))
		goto out;

	anon_vma = (struct anon_vma *) (anon_mapping - PAGE_MAPPING_ANON);
	if (!atomic_inc_not_zero(&anon_vma->refcount)) {
		anon_vma = NULL;
		goto out;
	}

	/*
	 * If this page is still mapped, then its anon_vma cannot have been
	 * freed.  But if it has been unmapped, we have no security against the
	 * anon_vma structure being freed and reused (for another anon_vma:
	 * SLAB_TYPESAFE_BY_RCU guarantees that - so the atomic_inc_not_zero()
	 * above cannot corrupt).
	 */
	if (!page_mapped(page)) {
		rcu_read_unlock();
		put_anon_vma(anon_vma);
		return NULL;
	}
out:
	rcu_read_unlock();

	return anon_vma;
}

/*
 * Similar to page_get_anon_vma() except it locks the anon_vma.
 *
 * Its a little more complex as it tries to keep the fast path to a single
 * atomic op -- the trylock. If we fail the trylock, we fall back to getting a
 * reference like with page_get_anon_vma() and then block on the mutex.
 */
struct anon_vma *page_lock_anon_vma_read(struct page *page)
{
	struct anon_vma *anon_vma = NULL;
	struct anon_vma *root_anon_vma;
	unsigned long anon_mapping;

	rcu_read_lock();
	anon_mapping = (unsigned long)READ_ONCE(page->mapping);
	if ((anon_mapping & PAGE_MAPPING_FLAGS) != PAGE_MAPPING_ANON)
		goto out;
	if (!page_mapped(page))
		goto out;

	anon_vma = (struct anon_vma *) (anon_mapping - PAGE_MAPPING_ANON);
	root_anon_vma = READ_ONCE(anon_vma->root);
	if (down_read_trylock(&root_anon_vma->rwsem)) {
		/*
		 * If the page is still mapped, then this anon_vma is still
		 * its anon_vma, and holding the mutex ensures that it will
		 * not go away, see anon_vma_free().
		 */
		if (!page_mapped(page)) {
			up_read(&root_anon_vma->rwsem);
			anon_vma = NULL;
		}
		goto out;
	}

	/* trylock failed, we got to sleep */
	if (!atomic_inc_not_zero(&anon_vma->refcount)) {
		anon_vma = NULL;
		goto out;
	}

	if (!page_mapped(page)) {
		rcu_read_unlock();
		put_anon_vma(anon_vma);
		return NULL;
	}

	/* we pinned the anon_vma, its safe to sleep */
	rcu_read_unlock();
	anon_vma_lock_read(anon_vma);

	if (atomic_dec_and_test(&anon_vma->refcount)) {
		/*
		 * Oops, we held the last refcount, release the lock
		 * and bail -- can't simply use put_anon_vma() because
		 * we'll deadlock on the anon_vma_lock_write() recursion.
		 */
		anon_vma_unlock_read(anon_vma);
		__put_anon_vma(anon_vma);
		anon_vma = NULL;
	}

	return anon_vma;

out:
	rcu_read_unlock();
	return anon_vma;
}

void page_unlock_anon_vma_read(struct anon_vma *anon_vma)
{
	anon_vma_unlock_read(anon_vma);
}

#ifdef CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH
/*
 * Flush TLB entries for recently unmapped pages from remote CPUs. It is
 * important if a PTE was dirty when it was unmapped that it's flushed
 * before any IO is initiated on the page to prevent lost writes. Similarly,
 * it must be flushed before freeing to prevent data leakage.
 */
void try_to_unmap_flush(void)
{
	struct tlbflush_unmap_batch *tlb_ubc = &current->tlb_ubc;

	if (!tlb_ubc->flush_required)
		return;

	arch_tlbbatch_flush(&tlb_ubc->arch);
	tlb_ubc->flush_required = false;
	tlb_ubc->writable = false;
}

/* Flush iff there are potentially writable TLB entries that can race with IO */
void try_to_unmap_flush_dirty(void)
{
	struct tlbflush_unmap_batch *tlb_ubc = &current->tlb_ubc;

	if (tlb_ubc->writable)
		try_to_unmap_flush();
}

static void set_tlb_ubc_flush_pending(struct mm_struct *mm, bool writable)
{
	struct tlbflush_unmap_batch *tlb_ubc = &current->tlb_ubc;

	arch_tlbbatch_add_mm(&tlb_ubc->arch, mm);
	tlb_ubc->flush_required = true;

	/*
	 * Ensure compiler does not re-order the setting of tlb_flush_batched
	 * before the PTE is cleared.
	 */
	barrier();
	mm->tlb_flush_batched = true;

	/*
	 * If the PTE was dirty then it's best to assume it's writable. The
	 * caller must use try_to_unmap_flush_dirty() or try_to_unmap_flush()
	 * before the page is queued for IO.
	 */
	if (writable)
		tlb_ubc->writable = true;
}

/*
 * Returns true if the TLB flush should be deferred to the end of a batch of
 * unmap operations to reduce IPIs.
 */
static bool should_defer_flush(struct mm_struct *mm, enum ttu_flags flags)
{
	bool should_defer = false;

	if (!(flags & TTU_BATCH_FLUSH))
		return false;

	/* If remote CPUs need to be flushed then defer batch the flush */
	if (cpumask_any_but(mm_cpumask(mm), get_cpu()) < nr_cpu_ids)
		should_defer = true;
	put_cpu();

	return should_defer;
}

/*
 * Reclaim unmaps pages under the PTL but do not flush the TLB prior to
 * releasing the PTL if TLB flushes are batched. It's possible for a parallel
 * operation such as mprotect or munmap to race between reclaim unmapping
 * the page and flushing the page. If this race occurs, it potentially allows
 * access to data via a stale TLB entry. Tracking all mm's that have TLB
 * batching in flight would be expensive during reclaim so instead track
 * whether TLB batching occurred in the past and if so then do a flush here
 * if required. This will cost one additional flush per reclaim cycle paid
 * by the first operation at risk such as mprotect and mumap.
 *
 * This must be called under the PTL so that an access to tlb_flush_batched
 * that is potentially a "reclaim vs mprotect/munmap/etc" race will synchronise
 * via the PTL.
 */
void flush_tlb_batched_pending(struct mm_struct *mm)
{
	if (mm->tlb_flush_batched) {
		flush_tlb_mm(mm);

		/*
		 * Do not allow the compiler to re-order the clearing of
		 * tlb_flush_batched before the tlb is flushed.
		 */
		barrier();
		mm->tlb_flush_batched = false;
	}
}
#else
static void set_tlb_ubc_flush_pending(struct mm_struct *mm, bool writable)
{
}

static bool should_defer_flush(struct mm_struct *mm, enum ttu_flags flags)
{
	return false;
}
#endif /* CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH */

/*
 * At what user virtual address is page expected in vma?
 * Caller should check the page is actually part of the vma.
 */
unsigned long page_address_in_vma(struct page *page, struct vm_area_struct *vma)
{
	unsigned long address;
	if (PageAnon(page)) {
		struct anon_vma *page__anon_vma = page_anon_vma(page);
		/*
		 * Note: swapoff's unuse_vma() is more efficient with this
		 * check, and needs it to match anon_vma when KSM is active.
		 */
		if (!vma->anon_vma || !page__anon_vma ||
		    vma->anon_vma->root != page__anon_vma->root)
			return -EFAULT;
	} else if (page->mapping) {
		if (!vma->vm_file || vma->vm_file->f_mapping != page->mapping)
			return -EFAULT;
	} else
		return -EFAULT;
	address = __vma_address(page, vma);
	if (unlikely(address < vma->vm_start || address >= vma->vm_end))
		return -EFAULT;
	return address;
}

pmd_t *mm_find_pmd(struct mm_struct *mm, unsigned long address)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd = NULL;
	pmd_t pmde;

	pgd = pgd_offset(mm, address);
	if (!pgd_present(*pgd))
		goto out;

	p4d = p4d_offset(pgd, address);
	if (!p4d_present(*p4d))
		goto out;

	pud = pud_offset(p4d, address);
	if (!pud_present(*pud))
		goto out;

	pmd = pmd_offset(pud, address);
	/*
	 * Some THP functions use the sequence pmdp_huge_clear_flush(), set_pmd_at()
	 * without holding anon_vma lock for write.  So when looking for a
	 * genuine pmde (in which to find pte), test present and !THP together.
	 */
	pmde = *pmd;
	barrier();
	if (!pmd_present(pmde) || pmd_trans_huge(pmde))
		pmd = NULL;
out:
	return pmd;
}

struct page_referenced_arg {
	int mapcount;
	int referenced;
	unsigned long vm_flags;
	struct mem_cgroup *memcg;
};
/*
 * arg: page_referenced_arg will be passed
 */
static bool page_referenced_one(struct page *page, struct vm_area_struct *vma,
			unsigned long address, void *arg)
{
	struct page_referenced_arg *pra = arg;
	struct page_vma_mapped_walk pvmw = {
		.page = page,
		.vma = vma,
		.address = address,
	};
	int referenced = 0;

	while (page_vma_mapped_walk(&pvmw)) {
		address = pvmw.address;

		if (vma->vm_flags & VM_LOCKED) {
			page_vma_mapped_walk_done(&pvmw);
			pra->vm_flags |= VM_LOCKED;
			return false; /* To break the loop */
		}

		if (pvmw.pte) {
			if (ptep_clear_flush_young_notify(vma, address,
						pvmw.pte)) {
				/*
				 * Don't treat a reference through
				 * a sequentially read mapping as such.
				 * If the page has been used in another mapping,
				 * we will catch it; if this other mapping is
				 * already gone, the unmap path will have set
				 * PG_referenced or activated the page.
				 */
				if (likely(!(vma->vm_flags & VM_SEQ_READ)))
					referenced++;
			}
		} else if (IS_ENABLED(CONFIG_TRANSPARENT_HUGEPAGE)) {
			if (pmdp_clear_flush_young_notify(vma, address,
						pvmw.pmd))
				referenced++;
		} else {
			/* unexpected pmd-mapped page? */
			WARN_ON_ONCE(1);
		}

		pra->mapcount--;
	}

	if (referenced)
		clear_page_idle(page);
	if (test_and_clear_page_young(page))
		referenced++;

	if (referenced) {
		pra->referenced++;
		pra->vm_flags |= vma->vm_flags;
	}

	if (!pra->mapcount)
		return false; /* To break the loop */

	return true;
}

static bool invalid_page_referenced_vma(struct vm_area_struct *vma, void *arg)
{
	struct page_referenced_arg *pra = arg;
	struct mem_cgroup *memcg = pra->memcg;

	if (!mm_match_cgroup(vma->vm_mm, memcg))
		return true;

	return false;
}

/**
 * page_referenced - test if the page was referenced
 * @page: the page to test
 * @is_locked: caller holds lock on the page
 * @memcg: target memory cgroup
 * @vm_flags: collect encountered vma->vm_flags who actually referenced the page
 *
 * Quick test_and_clear_referenced for all mappings to a page,
 * returns the number of ptes which referenced the page.
 */
int page_referenced(struct page *page,
		    int is_locked,
		    struct mem_cgroup *memcg,
		    unsigned long *vm_flags)
{
	int we_locked = 0;
	struct page_referenced_arg pra = {
		.mapcount = total_mapcount(page),
		.memcg = memcg,
	};
	struct rmap_walk_control rwc = {
		.rmap_one = page_referenced_one,
		.arg = (void *)&pra,
		.anon_lock = page_lock_anon_vma_read,
	};

	*vm_flags = 0;
	if (!page_mapped(page))
		return 0;

	if (!page_rmapping(page))
		return 0;

	if (!is_locked && (!PageAnon(page) || PageKsm(page))) {
		we_locked = trylock_page(page);
		if (!we_locked)
			return 1;
	}

	/*
	 * If we are reclaiming on behalf of a cgroup, skip
	 * counting on behalf of references from different
	 * cgroups
	 */
	if (memcg) {
		rwc.invalid_vma = invalid_page_referenced_vma;
	}

	rmap_walk(page, &rwc);
	*vm_flags = pra.vm_flags;

	if (we_locked)
		unlock_page(page);

	return pra.referenced;
}

static bool page_mkclean_one(struct page *page, struct vm_area_struct *vma,
			    unsigned long address, void *arg)
{
	struct page_vma_mapped_walk pvmw = {
		.page = page,
		.vma = vma,
		.address = address,
		.flags = PVMW_SYNC,
	};
	unsigned long start = address, end;
	int *cleaned = arg;

	/*
	 * We have to assume the worse case ie pmd for invalidation. Note that
	 * the page can not be free from this function.
	 */
	end = min(vma->vm_end, start + (PAGE_SIZE << compound_order(page)));
	mmu_notifier_invalidate_range_start(vma->vm_mm, start, end);

	while (page_vma_mapped_walk(&pvmw)) {
		unsigned long cstart, cend;
		int ret = 0;

		cstart = address = pvmw.address;
		if (pvmw.pte) {
			pte_t entry;
			pte_t *pte = pvmw.pte;

			if (!pte_dirty(*pte) && !pte_write(*pte))
				continue;

			flush_cache_page(vma, address, pte_pfn(*pte));
			entry = ptep_clear_flush(vma, address, pte);
			entry = pte_wrprotect(entry);
			entry = pte_mkclean(entry);
			set_pte_at(vma->vm_mm, address, pte, entry);
			cend = cstart + PAGE_SIZE;
			ret = 1;
		} else {
#ifdef CONFIG_TRANSPARENT_HUGE_PAGECACHE
			pmd_t *pmd = pvmw.pmd;
			pmd_t entry;

			if (!pmd_dirty(*pmd) && !pmd_write(*pmd))
				continue;

			flush_cache_page(vma, address, page_to_pfn(page));
			entry = pmdp_huge_clear_flush(vma, address, pmd);
			entry = pmd_wrprotect(entry);
			entry = pmd_mkclean(entry);
			set_pmd_at(vma->vm_mm, address, pmd, entry);
			cstart &= PMD_MASK;
			cend = cstart + PMD_SIZE;
			ret = 1;
#else
			/* unexpected pmd-mapped page? */
			WARN_ON_ONCE(1);
#endif
		}

		if (ret) {
			mmu_notifier_invalidate_range(vma->vm_mm, cstart, cend);
			(*cleaned)++;
		}
	}

	mmu_notifier_invalidate_range_end(vma->vm_mm, start, end);

	return true;
}

static bool invalid_mkclean_vma(struct vm_area_struct *vma, void *arg)
{
	if (vma->vm_flags & VM_SHARED)
		return false;

	return true;
}

int page_mkclean(struct page *page)
{
	int cleaned = 0;
	struct address_space *mapping;
	struct rmap_walk_control rwc = {
		.arg = (void *)&cleaned,
		.rmap_one = page_mkclean_one,
		.invalid_vma = invalid_mkclean_vma,
	};

	BUG_ON(!PageLocked(page));

	if (!page_mapped(page))
		return 0;

	mapping = page_mapping(page);
	if (!mapping)
		return 0;

	rmap_walk(page, &rwc);

	return cleaned;
}
EXPORT_SYMBOL_GPL(page_mkclean);

/**
 * page_move_anon_rmap - move a page to our anon_vma
 * @page:	the page to move to our anon_vma
 * @vma:	the vma the page belongs to
 *
 * When a page belongs exclusively to one process after a COW event,
 * that page can be moved into the anon_vma that belongs to just that
 * process, so the rmap code will not search the parent or sibling
 * processes.
 */
void page_move_anon_rmap(struct page *page, struct vm_area_struct *vma)
{
	struct anon_vma *anon_vma = vma->anon_vma;

	page = compound_head(page);

	VM_BUG_ON_PAGE(!PageLocked(page), page);
	VM_BUG_ON_VMA(!anon_vma, vma);

	anon_vma = (void *) anon_vma + PAGE_MAPPING_ANON;
	/*
	 * Ensure that anon_vma and the PAGE_MAPPING_ANON bit are written
	 * simultaneously, so a concurrent reader (eg page_referenced()'s
	 * PageAnon()) will not see one without the other.
	 */
	WRITE_ONCE(page->mapping, (struct address_space *) anon_vma);
}

/**
 * __page_set_anon_rmap - set up new anonymous rmap
 * @page:	Page to add to rmap	
 * @vma:	VM area to add page to.
 * @address:	User virtual address of the mapping	
 * @exclusive:	the page is exclusively owned by the current process
 */
static void __page_set_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address, int exclusive)
{
	struct anon_vma *anon_vma = vma->anon_vma;

	BUG_ON(!anon_vma);

	if (PageAnon(page))
		return;

	/*
	 * If the page isn't exclusively mapped into this vma,
	 * we must use the _oldest_ possible anon_vma for the
	 * page mapping!
	 */
	if (!exclusive)
		anon_vma = anon_vma->root;

	anon_vma = (void *) anon_vma + PAGE_MAPPING_ANON;
	page->mapping = (struct address_space *) anon_vma;
	page->index = linear_page_index(vma, address);
}

/**
 * __page_check_anon_rmap - sanity check anonymous rmap addition
 * @page:	the page to add the mapping to
 * @vma:	the vm area in which the mapping is added
 * @address:	the user virtual address mapped
 */
static void __page_check_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
#ifdef CONFIG_DEBUG_VM
	/*
	 * The page's anon-rmap details (mapping and index) are guaranteed to
	 * be set up correctly at this point.
	 *
	 * We have exclusion against page_add_anon_rmap because the caller
	 * always holds the page locked, except if called from page_dup_rmap,
	 * in which case the page is already known to be setup.
	 *
	 * We have exclusion against page_add_new_anon_rmap because those pages
	 * are initially only visible via the pagetables, and the pte is locked
	 * over the call to page_add_new_anon_rmap.
	 */
	BUG_ON(page_anon_vma(page)->root != vma->anon_vma->root);
	BUG_ON(page_to_pgoff(page) != linear_page_index(vma, address));
#endif
}

/**
 * page_add_anon_rmap - add pte mapping to an anonymous page
 * @page:	the page to add the mapping to
 * @vma:	the vm area in which the mapping is added
 * @address:	the user virtual address mapped
 * @compound:	charge the page as compound or small page
 *
 * The caller needs to hold the pte lock, and the page must be locked in
 * the anon_vma case: to serialize mapping,index checking after setting,
 * and to ensure that PageAnon is not being upgraded racily to PageKsm
 * (but PageKsm is never downgraded to PageAnon).
 */
void page_add_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address, bool compound)
{
	do_page_add_anon_rmap(page, vma, address, compound ? RMAP_COMPOUND : 0);
}

/*
 * Special version of the above for do_swap_page, which often runs
 * into pages that are exclusively owned by the current process.
 * Everybody else should continue to use page_add_anon_rmap above.
 */
void do_page_add_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address, int flags)
{
	bool compound = flags & RMAP_COMPOUND;
	bool first;

	if (compound) {
		atomic_t *mapcount;
		VM_BUG_ON_PAGE(!PageLocked(page), page);
		VM_BUG_ON_PAGE(!PageTransHuge(page), page);
		mapcount = compound_mapcount_ptr(page);
		first = atomic_inc_and_test(mapcount);
	} else {
		first = atomic_inc_and_test(&page->_mapcount);
	}

	if (first) {
		int nr = compound ? hpage_nr_pages(page) : 1;
		/*
		 * We use the irq-unsafe __{inc|mod}_zone_page_stat because
		 * these counters are not modified in interrupt context, and
		 * pte lock(a spinlock) is held, which implies preemption
		 * disabled.
		 */
		if (compound)
			__inc_node_page_state(page, NR_ANON_THPS);
		__mod_node_page_state(page_pgdat(page), NR_ANON_MAPPED, nr);
	}
	if (unlikely(PageKsm(page)))
		return;

	VM_BUG_ON_PAGE(!PageLocked(page), page);

	/* address might be in next vma when migration races vma_adjust */
	if (first)
		__page_set_anon_rmap(page, vma, address,
				flags & RMAP_EXCLUSIVE);
	else
		__page_check_anon_rmap(page, vma, address);
}

/**
 * page_add_new_anon_rmap - add pte mapping to a new anonymous page
 * @page:	the page to add the mapping to
 * @vma:	the vm area in which the mapping is added
 * @address:	the user virtual address mapped
 * @compound:	charge the page as compound or small page
 *
 * Same as page_add_anon_rmap but must only be called on *new* pages.
 * This means the inc-and-test can be bypassed.
 * Page does not have to be locked.
 */
void page_add_new_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address, bool compound)
{
	int nr = compound ? hpage_nr_pages(page) : 1;

	VM_BUG_ON_VMA(address < vma->vm_start || address >= vma->vm_end, vma);
	__SetPageSwapBacked(page);
	if (compound) {
		VM_BUG_ON_PAGE(!PageTransHuge(page), page);
		/* increment count (starts at -1) */
		atomic_set(compound_mapcount_ptr(page), 0);
		__inc_node_page_state(page, NR_ANON_THPS);
	} else {
		/* Anon THP always mapped first with PMD */
		VM_BUG_ON_PAGE(PageTransCompound(page), page);
		/* increment count (starts at -1) */
		atomic_set(&page->_mapcount, 0);
	}
	__mod_node_page_state(page_pgdat(page), NR_ANON_MAPPED, nr);
	__page_set_anon_rmap(page, vma, address, 1);
}

/**
 * page_add_file_rmap - add pte mapping to a file page
 * @page: the page to add the mapping to
 *
 * The caller needs to hold the pte lock.
 */
void page_add_file_rmap(struct page *page, bool compound)
{
	int i, nr = 1;

	VM_BUG_ON_PAGE(compound && !PageTransHuge(page), page);
	lock_page_memcg(page);
	if (compound && PageTransHuge(page)) {
		for (i = 0, nr = 0; i < HPAGE_PMD_NR; i++) {
			if (atomic_inc_and_test(&page[i]._mapcount))
				nr++;
		}
		if (!atomic_inc_and_test(compound_mapcount_ptr(page)))
			goto out;
		VM_BUG_ON_PAGE(!PageSwapBacked(page), page);
		__inc_node_page_state(page, NR_SHMEM_PMDMAPPED);
	} else {
		if (PageTransCompound(page) && page_mapping(page)) {
			VM_WARN_ON_ONCE(!PageLocked(page));

			SetPageDoubleMap(compound_head(page));
			if (PageMlocked(page))
				clear_page_mlock(compound_head(page));
		}
		if (!atomic_inc_and_test(&page->_mapcount))
			goto out;
	}
	__mod_lruvec_page_state(page, NR_FILE_MAPPED, nr);
out:
	unlock_page_memcg(page);
}

static void page_remove_file_rmap(struct page *page, bool compound)
{
	int i, nr = 1;

	VM_BUG_ON_PAGE(compound && !PageHead(page), page);
	lock_page_memcg(page);

	/* Hugepages are not counted in NR_FILE_MAPPED for now. */
	if (unlikely(PageHuge(page))) {
		/* hugetlb pages are always mapped with pmds */
		atomic_dec(compound_mapcount_ptr(page));
		goto out;
	}

	/* page still mapped by someone else? */
	if (compound && PageTransHuge(page)) {
		for (i = 0, nr = 0; i < HPAGE_PMD_NR; i++) {
			if (atomic_add_negative(-1, &page[i]._mapcount))
				nr++;
		}
		if (!atomic_add_negative(-1, compound_mapcount_ptr(page)))
			goto out;
		VM_BUG_ON_PAGE(!PageSwapBacked(page), page);
		__dec_node_page_state(page, NR_SHMEM_PMDMAPPED);
	} else {
		if (!atomic_add_negative(-1, &page->_mapcount))
			goto out;
	}

	/*
	 * We use the irq-unsafe __{inc|mod}_lruvec_page_state because
	 * these counters are not modified in interrupt context, and
	 * pte lock(a spinlock) is held, which implies preemption disabled.
	 */
	__mod_lruvec_page_state(page, NR_FILE_MAPPED, -nr);

	if (unlikely(PageMlocked(page)))
		clear_page_mlock(page);
out:
	unlock_page_memcg(page);
}

static void page_remove_anon_compound_rmap(struct page *page)
{
	int i, nr;

	if (!atomic_add_negative(-1, compound_mapcount_ptr(page)))
		return;

	/* Hugepages are not counted in NR_ANON_PAGES for now. */
	if (unlikely(PageHuge(page)))
		return;

	if (!IS_ENABLED(CONFIG_TRANSPARENT_HUGEPAGE))
		return;

	__dec_node_page_state(page, NR_ANON_THPS);

	if (TestClearPageDoubleMap(page)) {
		/*
		 * Subpages can be mapped with PTEs too. Check how many of
		 * themi are still mapped.
		 */
		for (i = 0, nr = 0; i < HPAGE_PMD_NR; i++) {
			if (atomic_add_negative(-1, &page[i]._mapcount))
				nr++;
		}
	} else {
		nr = HPAGE_PMD_NR;
	}

	if (unlikely(PageMlocked(page)))
		clear_page_mlock(page);

	if (nr) {
		__mod_node_page_state(page_pgdat(page), NR_ANON_MAPPED, -nr);
		deferred_split_huge_page(page);
	}
}

/**
 * page_remove_rmap - take down pte mapping from a page
 * @page:	page to remove mapping from
 * @compound:	uncharge the page as compound or small page
 *
 * The caller needs to hold the pte lock.
 */
void page_remove_rmap(struct page *page, bool compound)
{
	if (!PageAnon(page))
		return page_remove_file_rmap(page, compound);

	if (compound)
		return page_remove_anon_compound_rmap(page);

	/* page still mapped by someone else? */
	if (!atomic_add_negative(-1, &page->_mapcount))
		return;

	/*
	 * We use the irq-unsafe __{inc|mod}_zone_page_stat because
	 * these counters are not modified in interrupt context, and
	 * pte lock(a spinlock) is held, which implies preemption disabled.
	 */
	__dec_node_page_state(page, NR_ANON_MAPPED);

	if (unlikely(PageMlocked(page)))
		clear_page_mlock(page);

	if (PageTransCompound(page))
		deferred_split_huge_page(compound_head(page));

	/*
	 * It would be tidy to reset the PageAnon mapping here,
	 * but that might overwrite a racing page_add_anon_rmap
	 * which increments mapcount after us but sets mapping
	 * before us: so leave the reset to free_hot_cold_page,
	 * and remember that it's only reliable while mapped.
	 * Leaving it set also helps swapoff to reinstate ptes
	 * faster for those pages still in swapcache.
	 */
}

/*
 * @arg: enum ttu_flags will be passed to this argument
 */
 /*
  * 对vma进行unmap操作，并对此页的page->_mapcount--，这里面的页可能是文件页也可能是匿名页
  * page: 目标page
  * vma: 获取到的vma
  * address: page在vma所属的进程地址空间中的线性地址
  */
static bool try_to_unmap_one(struct page *page, struct vm_area_struct *vma,
		     unsigned long address, void *arg)
{
	struct mm_struct *mm = vma->vm_mm;
	struct page_vma_mapped_walk pvmw = {
		.page = page,
		.vma = vma,
		.address = address,
	};
	pte_t pteval;
	struct page *subpage;
	bool ret = true;
	unsigned long start = address, end;
	enum ttu_flags flags = (enum ttu_flags)arg;//获取arg的传入的属性

	/* munlock has nothing to gain from examining un-locked vmas */
    /* 对尚未被VM_LOCKED的vma发出了TTU_MUNLOCK请求，则返回成功以跳过该页面*/
	if ((flags & TTU_MUNLOCK) && !(vma->vm_flags & VM_LOCKED))
		return true;
    /* 在请求做页面迁移时，如果该区域设备不是hmm，则返回true以跳过*/
	if (IS_ENABLED(CONFIG_MIGRATION) && (flags & TTU_MIGRATION) &&
	    is_zone_device_page(page) && !is_device_private_page(page))
		return true;
    /* 判断是否为huge 巨型帧页面拆分请求，如果时则进入split_huge_pmd_address，如果不是则继续向下执行*/
	if (flags & TTU_SPLIT_HUGE_PMD) {
		split_huge_pmd_address(vma, address,
				flags & TTU_SPLIT_FREEZE, page);
	}

	/*
	 * For THP, we have to assume the worse case ie pmd for invalidation.
	 * For hugetlb, it could be much worse if we need to do pud
	 * invalidation in the case of pmd sharing.
	 *
	 * Note that the page can not be free in this function as call of
	 * try_to_unmap() must hold a reference on the page.
	 */
	/*
	* start + (PAGE_SIZE << compound_order(page)) 是根据传入的address以及page计算出来的大小
	* vma->vm_end理论上的最小值，
	* 取这其中最小的那个作为end结束地址
	* 同时判断page是否为巨型页面，如果是巨型页面则调用adjust_range_if_pmd_sharing_possible调整start,end的范围
	*/
	end = min(vma->vm_end, start + (PAGE_SIZE << compound_order(page)));
	if (PageHuge(page)) {
		/*
		 * If sharing is possible, start and end will be adjusted
		 * accordingly.
		 */
		adjust_range_if_pmd_sharing_possible(vma, &start, &end);
	}
    /* 调用已注册mmu通知程序的函数（* invalidate_range_start），以指示该函数在对辅助mmu范围执行tlb无效之前启动*/
	mmu_notifier_invalidate_range_start(vma->vm_mm, start, end);
    /*检查pvmw请求的正常映射状态是否正常，如果正常则进入判断*/
	while (page_vma_mapped_walk(&pvmw)) {
        /*该config没有配置所以该想选不会被执行*/
#ifdef CONFIG_ARCH_ENABLE_THP_MIGRATION
		/* PMD-mapped THP migration entry */
		if (!pvmw.pte && (flags & TTU_MIGRATION)) {
			VM_BUG_ON_PAGE(PageHuge(page) || !PageTransCompound(page), page);

			if (!PageAnon(page))
				continue;

			set_pmd_migration_entry(&pvmw, page);
			continue;
		}
#endif

		/*
		 * If the page is mlock()d, we cannot swap it out.
		 * If it's recently referenced (perhaps page_referenced
		 * skipped over this mm) then we should reactivate it.
		 */
		/* 如果flags没有要求忽略mlock的vma */
		if (!(flags & TTU_IGNORE_MLOCK)) {
             /* 如果此vma要求里面的页都锁在内存中，则进入判断语句当中                 */
			if (vma->vm_flags & VM_LOCKED) {
				/* PTE-mapped THP are never mlocked */
                /* 判断该page是否为透明大页，如果是则进入判断循环*/
				if (!PageTransCompound(page)) {
					/*
					 * Holding pte lock, we do *not* need
					 * mmap_sem here
					 */
					/* 将阻止Linux内存回收进程回收这个地址空间的页面 */
					mlock_vma_page(page);
				}
                /* 如果是透明大页则回收该页面，在ARM64架构当中该函数为空NULL*/
				ret = false;
				page_vma_mapped_walk_done(&pvmw);
				break;
			}
			if (flags & TTU_MUNLOCK)//如果没有被锁则continue
				continue;
		}

		/* Unexpected PMD-mapped THP? */
		VM_BUG_ON_PAGE(!pvmw.pte, page);
        /* 此处不理解，page 与 pfn(页帧号) 本身不是一个概念为什么要做加减，目的是为什么？ */
		subpage = page - page_to_pfn(page) + pte_pfn(*pvmw.pte);
		address = pvmw.address;
        /* 判断是否为巨型页，如果是则进入判断当中 */
		if (PageHuge(page)) {
            /* 判断是否为共享大页，如果是则进入判断将清除范围区域的缓存和tlb缓存，
             * 并对辅助MMU的范围执行tlb无效。然后停止循环并停止该过程*/
			if (huge_pmd_unshare(mm, &address, pvmw.pte)) {
				/*
				 * huge_pmd_unshare unmapped an entire PMD
				 * page.  There is no way of knowing exactly
				 * which PMDs may be cached for this mm, so
				 * we must flush them all.  start/end were
				 * already adjusted above to cover this range.
				 */
				flush_cache_range(vma, start, end);
				flush_tlb_range(vma, start, end);
				mmu_notifier_invalidate_range(mm, start, end);

				/*
				 * The ref count of the PMD page was dropped
				 * which is part of the way map counting
				 * is done for shared PMDs.  Return 'true'
				 * here.  When there is no other sharing,
				 * huge_pmd_unshare returns false and we will
				 * unmap the actual page and drop map count
				 * to zero.
				 */
				page_vma_mapped_walk_done(&pvmw);
				break;
			}
		}
        /* 判断迁移功能是否开启，且flags是否设置TTU_MIGRATION迁移类型，在此版本的arm64架构当中is_zone_device_page(page)为false = 0
         * 所以此时无法进入该判断当中，跳过该步骤。
         */
		if (IS_ENABLED(CONFIG_MIGRATION) &&
		    (flags & TTU_MIGRATION) &&
		    is_zone_device_page(page)) {
			swp_entry_t entry;
			pte_t swp_pte;

			pteval = ptep_get_and_clear(mm, pvmw.address, pvmw.pte);

			/*
			 * Store the pfn of the page in a special migration
			 * pte. do_swap_page() will wait until the migration
			 * pte is removed and then restart fault handling.
			 */
			entry = make_migration_entry(page, 0);
			swp_pte = swp_entry_to_pte(entry);
			if (pte_soft_dirty(pteval))
				swp_pte = pte_swp_mksoft_dirty(swp_pte);
			set_pte_at(mm, pvmw.address, pvmw.pte, swp_pte);
			goto discard;
		}
        /* 忽略页表项中的Accessed */
		if (!(flags & TTU_IGNORE_ACCESS)) {
            /* 清除页表项的Accessed标志 */
			if (ptep_clear_flush_young_notify(vma, address,
						pvmw.pte)) {
				ret = false;
				page_vma_mapped_walk_done(&pvmw);
				break;
			}
		}

		/* Nuke the page table entry. */
        /*
        刷新用户虚拟地址的缓存，在ARM64体系结构不执行任何操作。
        如果架构的缓存类型是vivt或vipt别名，则将其刷新
        */
		flush_cache_page(vma, address, pte_pfn(*pvmw.pte));
        /* 清除映射到用户虚拟地址的pte条目，取消映射，然后刷新tlb。如果收到了TTU_BATCH_FLUSH插入请求，则会收集并处理tlb刷新，以提高性能 */
		if (should_defer_flush(mm, flags)) {
			/*
			 * We clear the PTE but do not flush so potentially
			 * a remote CPU could still be writing to the page.
			 * If the entry was previously clean then the
			 * architecture must guarantee that a clear->dirty
			 * transition on a cached TLB entry is written through
			 * and traps if the PTE is unmapped.
			 */
			pteval = ptep_get_and_clear(mm, address, pvmw.pte);

			set_tlb_ubc_flush_pending(mm, pte_dirty(pteval));
		} else {
              /* 获取页表项内容，保存到pteval中，然后清空页表项 */
			pteval = ptep_clear_flush(vma, address, pvmw.pte);
		}

		/* Move the dirty bit to the page. Now the pte is gone. */
        /* 如果页表项标记了此页为脏页 */
		if (pte_dirty(pteval))
			set_page_dirty(page); /* 设置页描述符的PG_dirty标记 */

		/* Update high watermark before we lower rss */
         /* 更新进程所拥有的最大页框数 */
		update_hiwater_rss(mm);
         /* 此页是被标记为"坏页"的页，这种页用于内核纠正一些错误，是否用于边界检查? */
		if (PageHWPoison(page) && !(flags & TTU_IGNORE_HWPOISON)) {
			pteval = swp_entry_to_pte(make_hwpoison_entry(subpage));
            /* 判断是否为大页 */
			if (PageHuge(page)) {
                /* 获取复合页order, 1 << order相当于知道这个复合页包含多少个page */
				int nr = 1 << compound_order(page);
				hugetlb_count_sub(nr, mm);
				set_huge_swap_pte_at(mm, address,
						     pvmw.pte, pteval,
						     vma_mmu_pagesize(vma));
			} else {
                /* mm_counter(page)函数判断是匿名页还是文件页，如果是PageAnon(page)匿名页则相当于：dec_mm_counter(mm, MM_ANONPAGES);mm的MM_ANONPAGES--
                 * 如果是文件页mm_counter(page) = MM_FILEPAGES 则相当于dec_mm_counter(mm, MM_FILEPAGES);mm的MM_FILEPAGES--
                 * 如果是共享页mm_counter(page) = MM_SHMEMPAGES 则相当于dec_mm_counter(mm, MM_SHMEMPAGES);mm的MM_SHMEMPAGES--
                 */
				dec_mm_counter(mm, mm_counter(page));
                 /* 将交换条目值映射到pte条目 */
				set_pte_at(mm, address, pvmw.pte, pteval);
			}

		} else if (pte_unused(pteval) && !userfaultfd_armed(vma)) {
			/*
			 * The guest indicated that the page content is of no
			 * interest anymore. Simply discard the pte, vmscan
			 * will take care of the rest.
			 * A future reference will then fault in a new zero
			 * page. When userfaultfd is active, we must not drop
			 * this page though, as its main user (postcopy
			 * migration) will not expect userfaults on already
			 * copied pages.
			 */
		    /*
		    * 在userfaultfd vma的pte值未使用的情况下，相关的mm计数器（anon，file，shm）将减少页数，该函数过程如上
            */
			dec_mm_counter(mm, mm_counter(page));
		} else if (IS_ENABLED(CONFIG_MIGRATION) &&
				(flags & (TTU_MIGRATION|TTU_SPLIT_FREEZE))) {
			/*
			 * 如果配置了迁移模块，且flags设置迁移，拆分属性 TTU_MIGRATION|TTU_SPLIT_FREEZE，则进入改判断语句当中
             */
			swp_entry_t entry;
			pte_t swp_pte;
			/*
			 * Store the pfn of the page in a special migration
			 * pte. do_swap_page() will wait until the migration
			 * pte is removed and then restart fault handling.
			 */
			/* 为subpage创建一个页迁移使用的swp_entry_t，此swp_entry_t指向此subpage */
			entry = make_migration_entry(subpage,
					pte_write(pteval));
            /* 将entry转为swp_pte页表项 */
			swp_pte = swp_entry_to_pte(entry);
            /* 页表项有一位用于_PAGE_SOFT_DIRTY，用于kmemcheck */
			if (pte_soft_dirty(pteval))
				swp_pte = pte_swp_mksoft_dirty(swp_pte);
            /* 将配置好的新的页表项swp_pte写入页表项中 */
			set_pte_at(mm, address, pvmw.pte, swp_pte);
		} else if (PageAnon(page)) {/* 如果page是匿名页 */
            /* 获取page->private中保存的内容，调用到try_to_unmap()前会把此页加入到swapcache，然后分配一个以swap页槽偏移量为内容的swp_entry_t */
			swp_entry_t entry = { .val = page_private(subpage) };
			pte_t swp_pte;
			/*
			 * Store the swap location in the pte.
			 * See handle_pte_fault() ...
			 */
			/* 如果该页面以较低的概率与swapbacked和swapcache标志设置不匹配，它将执行辅助mmu的tlb无效，并使例程中止 */
			if (unlikely(PageSwapBacked(page) != PageSwapCache(page))) {
				WARN_ON_ONCE(1);
				ret = false;
				/* We have to invalidate as we cleared the pte */
				page_vma_mapped_walk_done(&pvmw);
				break;
			}

			/* MADV_FREE page check */
            /*
             * 如果它不是SwapBacked页面，它将再次映射该页面并导致例程停止。但是，如果它不是脏页，它将执行辅助mmu的tlb无效，
             * 减少anon mm计数器，然后移至废弃标签以进行以下操作
             */
			if (!PageSwapBacked(page)) {
				if (!PageDirty(page)) {
					dec_mm_counter(mm, MM_ANONPAGES);
					goto discard;
				}

				/*
				 * If the page was redirtied, it cannot be
				 * discarded. Remap the page to page table.
				 */
				set_pte_at(mm, address, pvmw.pte, pteval);
				SetPageSwapBacked(page);
				ret = false;
				page_vma_mapped_walk_done(&pvmw);
				break;
			}
            /* 对于内存回收，基本都是这种情况，因为page在调用到这里之前已经被移动到了swapcache 
             * 而对于内存碎片整理，
             * 检查entry是否有效，并且增加entry对应页槽在swap_info_struct的swap_map的数值，此数值标记此页槽的页有多少个进程引用
             */
			if (swap_duplicate(entry) < 0) {
                /* 检查失败，把原来的页表项内容写回去 */
				set_pte_at(mm, address, pvmw.pte, pteval);
				ret = false;
				page_vma_mapped_walk_done(&pvmw);
				break;
			}
             /* entry有效，并且swap_map中目标页槽的数值也++了 */
            /* 这个if的情况是此vma所属进程的mm没有加入到所有进程的mmlist中(init_mm.mmlist) */
			if (list_empty(&mm->mmlist)) {
				spin_lock(&mmlist_lock);
				if (list_empty(&mm->mmlist))
					list_add(&mm->mmlist, &init_mm.mmlist);
				spin_unlock(&mmlist_lock);
			}
             /* 减少此mm的匿名页统计 */
			dec_mm_counter(mm, MM_ANONPAGES);
             /* 增加此mm的页表中标记了页在swap的页表项的数量 */
			inc_mm_counter(mm, MM_SWAPENTS);
             /* 将entry转为一个页表项 */
			swp_pte = swp_entry_to_pte(entry);
            /* 页表项有一位用于_PAGE_SOFT_DIRTY，用于kmemcheck */
			if (pte_soft_dirty(pteval))
				swp_pte = pte_swp_mksoft_dirty(swp_pte);
            /* 将配置好的新的页表项swp_pte写入页表项中 */
			set_pte_at(mm, address, pvmw.pte, swp_pte);
		} else
			dec_mm_counter(mm, mm_counter_file(page));/* 此页是文件页，仅对此mm的文件页计数--，文件页不需要设置页表项，只需要对页表项进行清空 */
discard:
        /* 主要对此页的页描述符的_mapcount进行--操作，当_mapcount为-1时，表示此页已经没有页表项映射了 */
		page_remove_rmap(subpage, PageHuge(page));
		put_page(page);//释放该page
		mmu_notifier_invalidate_range(mm, address,
					      address + PAGE_SIZE);
	}

	mmu_notifier_invalidate_range_end(vma->vm_mm, start, end);

	return ret;
}

bool is_vma_temporary_stack(struct vm_area_struct *vma)
{
	int maybe_stack = vma->vm_flags & (VM_GROWSDOWN | VM_GROWSUP);

	if (!maybe_stack)
		return false;

	if ((vma->vm_flags & VM_STACK_INCOMPLETE_SETUP) ==
						VM_STACK_INCOMPLETE_SETUP)
		return true;

	return false;
}

static bool invalid_migration_vma(struct vm_area_struct *vma, void *arg)
{
	return is_vma_temporary_stack(vma);
}

static int page_mapcount_is_zero(struct page *page)
{
	return !total_mapcount(page);
}

/**
 * try_to_unmap - try to remove all page table mappings to a page
 * @page: the page to get unmapped
 * @flags: action and flags
 *
 * Tries to remove all the page table entries which are mapping this
 * page, used in the pageout path.  Caller must hold the page lock.
 *
 * If unmap is successful, return true. Otherwise, false.
 */
bool try_to_unmap(struct page *page, enum ttu_flags flags)
{
      /* 反向映射控制结构 */
	struct rmap_walk_control rwc = {
	     /* 对一个vma所属页表进行unmap操作
         * 每次获取一个vma就会对此vma调用一次此函数，在函数里第一件事就是判断获取的vma有没有映射此page
         */
		.rmap_one = try_to_unmap_one,
		.arg = (void *)flags,
		/* 对一个vma进行unmap后会执行此函数 */
		.done = page_mapcount_is_zero,
		/* 用于对整个anon_vma的红黑树进行上锁，用读写信号量，锁是aon_vma的rwsem */
		.anon_lock = page_lock_anon_vma_read,
	};

	/*
	 * During exec, a temporary VMA is setup and later moved.
	 * The VMA is moved under the anon_vma lock but not the
	 * page tables leading to a race where migration cannot
	 * find the migration ptes. Rather than increasing the
	 * locking requirements of exec(), migration skips
	 * temporary VMAs until after exec() completes.
	 */
	/*
	* thp（TTU_SPLIT_FREEZE）的拆分或迁移（TTU_MIGRATION）。
	* 对于除ksm之外的匿名映射页面，在（* invalid_vma）挂钩函数中指定了invalid_migration_vma（）函数
	*/
	if ((flags & (TTU_MIGRATION|TTU_SPLIT_FREEZE))
	    && !PageKsm(page) && PageAnon(page))
		rwc.invalid_vma = invalid_migration_vma;

    /*
    * 判断flags是否设置TTU_RMAP_LOCKED,如果设置则获取锁，、
    * 如果没有设置锁则调用rmap_walk， 对所有映射了此页的vma进行遍历，并取消  映射关系
    */
	if (flags & TTU_RMAP_LOCKED)
		rmap_walk_locked(page, &rwc);
	else
		rmap_walk(page, &rwc);

	return !page_mapcount(page) ? true : false;
}

static int page_not_mapped(struct page *page)
{
	return !page_mapped(page);
};

/**
 * try_to_munlock - try to munlock a page
 * @page: the page to be munlocked
 *
 * Called from munlock code.  Checks all of the VMAs mapping the page
 * to make sure nobody else has this page mlocked. The page will be
 * returned with PG_mlocked cleared if no other vmas have it mlocked.
 */

void try_to_munlock(struct page *page)
{
	struct rmap_walk_control rwc = {
		.rmap_one = try_to_unmap_one,
		.arg = (void *)TTU_MUNLOCK,
		.done = page_not_mapped,
		.anon_lock = page_lock_anon_vma_read,

	};

	VM_BUG_ON_PAGE(!PageLocked(page) || PageLRU(page), page);
	VM_BUG_ON_PAGE(PageCompound(page) && PageDoubleMap(page), page);

	rmap_walk(page, &rwc);
}

void __put_anon_vma(struct anon_vma *anon_vma)
{
	struct anon_vma *root = anon_vma->root;

	anon_vma_free(anon_vma);
	if (root != anon_vma && atomic_dec_and_test(&root->refcount))
		anon_vma_free(root);
}

static struct anon_vma *rmap_walk_anon_lock(struct page *page,
					struct rmap_walk_control *rwc)
{
	struct anon_vma *anon_vma;

	if (rwc->anon_lock)
		return rwc->anon_lock(page);

	/*
	 * Note: remove_migration_ptes() cannot use page_lock_anon_vma_read()
	 * because that depends on page_mapped(); but not all its usages
	 * are holding mmap_sem. Users without mmap_sem are required to
	 * take a reference count to prevent the anon_vma disappearing
	 */
	anon_vma = page_anon_vma(page);
	if (!anon_vma)
		return NULL;

	anon_vma_lock_read(anon_vma);
	return anon_vma;
}

/*
 * rmap_walk_anon - do something to anonymous page using the object-based
 * rmap method
 * @page: the page to be handled
 * @rwc: control variable according to each walk type
 *
 * Find all the mappings of a page using the mapping pointer and the vma chains
 * contained in the anon_vma struct it points to.
 *
 * When called from try_to_munlock(), the mmap_sem of the mm containing the vma
 * where the page was found will be held for write.  So, we won't recheck
 * vm_flags for that VMA.  That should be OK, because that vma shouldn't be
 * LOCKED.
 */
static void rmap_walk_anon(struct page *page, struct rmap_walk_control *rwc,
		bool locked)
{
	struct anon_vma *anon_vma;
	pgoff_t pgoff_start, pgoff_end;
	struct anon_vma_chain *avc;

	if (locked) {
		anon_vma = page_anon_vma(page);
		/* anon_vma disappear under us? */
		VM_BUG_ON_PAGE(!anon_vma, page);
	} else {
		anon_vma = rmap_walk_anon_lock(page, rwc);
	}
	if (!anon_vma)
		return;

	pgoff_start = page_to_pgoff(page);
	pgoff_end = pgoff_start + hpage_nr_pages(page) - 1;
	anon_vma_interval_tree_foreach(avc, &anon_vma->rb_root,
			pgoff_start, pgoff_end) {
		struct vm_area_struct *vma = avc->vma;
		unsigned long address = vma_address(page, vma);

		cond_resched();

		if (rwc->invalid_vma && rwc->invalid_vma(vma, rwc->arg))
			continue;

		if (!rwc->rmap_one(page, vma, address, rwc->arg))
			break;
		if (rwc->done && rwc->done(page))
			break;
	}

	if (!locked)
		anon_vma_unlock_read(anon_vma);
}

/*
 * rmap_walk_file - do something to file page using the object-based rmap method
 * @page: the page to be handled
 * @rwc: control variable according to each walk type
 *
 * Find all the mappings of a page using the mapping pointer and the vma chains
 * contained in the address_space struct it points to.
 *
 * When called from try_to_munlock(), the mmap_sem of the mm containing the vma
 * where the page was found will be held for write.  So, we won't recheck
 * vm_flags for that VMA.  That should be OK, because that vma shouldn't be
 * LOCKED.
 */
static void rmap_walk_file(struct page *page, struct rmap_walk_control *rwc,
		bool locked)
{
	struct address_space *mapping = page_mapping(page);
	pgoff_t pgoff_start, pgoff_end;
	struct vm_area_struct *vma;

	/*
	 * The page lock not only makes sure that page->mapping cannot
	 * suddenly be NULLified by truncation, it makes sure that the
	 * structure at mapping cannot be freed and reused yet,
	 * so we can safely take mapping->i_mmap_rwsem.
	 */
	VM_BUG_ON_PAGE(!PageLocked(page), page);

	if (!mapping)
		return;

	pgoff_start = page_to_pgoff(page);
	pgoff_end = pgoff_start + hpage_nr_pages(page) - 1;
	if (!locked)
		i_mmap_lock_read(mapping);
	vma_interval_tree_foreach(vma, &mapping->i_mmap,
			pgoff_start, pgoff_end) {
		unsigned long address = vma_address(page, vma);

		cond_resched();

		if (rwc->invalid_vma && rwc->invalid_vma(vma, rwc->arg))
			continue;

		if (!rwc->rmap_one(page, vma, address, rwc->arg))
			goto done;
		if (rwc->done && rwc->done(page))
			goto done;
	}

done:
	if (!locked)
		i_mmap_unlock_read(mapping);
}

void rmap_walk(struct page *page, struct rmap_walk_control *rwc)
{
	if (unlikely(PageKsm(page)))
		rmap_walk_ksm(page, rwc);
	else if (PageAnon(page))
		rmap_walk_anon(page, rwc, false);
	else
		rmap_walk_file(page, rwc, false);
}

/* Like rmap_walk, but caller holds relevant rmap lock */
void rmap_walk_locked(struct page *page, struct rmap_walk_control *rwc)
{
	/* no ksm support for now */
	VM_BUG_ON_PAGE(PageKsm(page), page);
	if (PageAnon(page))
		rmap_walk_anon(page, rwc, true);
	else
		rmap_walk_file(page, rwc, true);
}

#ifdef CONFIG_HUGETLB_PAGE
/*
 * The following three functions are for anonymous (private mapped) hugepages.
 * Unlike common anonymous pages, anonymous hugepages have no accounting code
 * and no lru code, because we handle hugepages differently from common pages.
 */
static void __hugepage_set_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address, int exclusive)
{
	struct anon_vma *anon_vma = vma->anon_vma;

	BUG_ON(!anon_vma);

	if (PageAnon(page))
		return;
	if (!exclusive)
		anon_vma = anon_vma->root;

	anon_vma = (void *) anon_vma + PAGE_MAPPING_ANON;
	page->mapping = (struct address_space *) anon_vma;
	page->index = linear_page_index(vma, address);
}

void hugepage_add_anon_rmap(struct page *page,
			    struct vm_area_struct *vma, unsigned long address)
{
	struct anon_vma *anon_vma = vma->anon_vma;
	int first;

	BUG_ON(!PageLocked(page));
	BUG_ON(!anon_vma);
	/* address might be in next vma when migration races vma_adjust */
	first = atomic_inc_and_test(compound_mapcount_ptr(page));
	if (first)
		__hugepage_set_anon_rmap(page, vma, address, 0);
}

void hugepage_add_new_anon_rmap(struct page *page,
			struct vm_area_struct *vma, unsigned long address)
{
	BUG_ON(address < vma->vm_start || address >= vma->vm_end);
	atomic_set(compound_mapcount_ptr(page), 0);
	__hugepage_set_anon_rmap(page, vma, address, 1);
}
#endif /* CONFIG_HUGETLB_PAGE */
