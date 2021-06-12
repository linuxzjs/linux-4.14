// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/mm/vmscan.c
 *
 *  Copyright (C) 1991, 1992, 1993, 1994  Linus Torvalds
 *
 *  Swap reorganised 29.12.95, Stephen Tweedie.
 *  kswapd added: 7.1.96  sct
 *  Removed kswapd_ctl limits, and swap out as many pages as needed
 *  to bring the system back to freepages.high: 2.4.97, Rik van Riel.
 *  Zone aware kswapd started 02/00, Kanoj Sarcar (kanoj@sgi.com).
 *  Multiqueue VM started 5.8.00, Rik van Riel.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/module.h>
#include <linux/gfp.h>
#include <linux/kernel_stat.h>
#include <linux/swap.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/vmpressure.h>
#include <linux/vmstat.h>
#include <linux/file.h>
#include <linux/writeback.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>	/* for try_to_release_page(),
					buffer_heads_over_limit */
#include <linux/mm_inline.h>
#include <linux/backing-dev.h>
#include <linux/rmap.h>
#include <linux/topology.h>
#include <linux/cpu.h>
#include <linux/cpuset.h>
#include <linux/compaction.h>
#include <linux/notifier.h>
#include <linux/rwsem.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/memcontrol.h>
#include <linux/delayacct.h>
#include <linux/sysctl.h>
#include <linux/oom.h>
#include <linux/prefetch.h>
#include <linux/printk.h>
#include <linux/dax.h>

#include <asm/tlbflush.h>
#include <asm/div64.h>

#include <linux/swapops.h>
#include <linux/balloon_compaction.h>

#include "internal.h"

#define CREATE_TRACE_POINTS
#include <trace/events/vmscan.h>

struct scan_control {
	/* How many pages shrink_list() should reclaim */
	unsigned long nr_to_reclaim;

	/* This context's GFP mask */
	gfp_t gfp_mask;

	/* Allocation order */
	int order;

	/*
	 * Nodemask of nodes allowed by the caller. If NULL, all nodes
	 * are scanned.
	 */
	nodemask_t	*nodemask;

	/*
	 * The memory cgroup that hit its limit and as a result is the
	 * primary target of this reclaim invocation.
	 */
	struct mem_cgroup *target_mem_cgroup;

	/* Scan (total_size >> priority) pages at once */
	int priority;

	/* The highest zone to isolate pages for reclaim from */
	enum zone_type reclaim_idx;

	/* Writepage batching in laptop mode; RECLAIM_WRITE */
	unsigned int may_writepage:1;

	/* Can mapped pages be reclaimed? */
	unsigned int may_unmap:1;

	/* Can pages be swapped as part of reclaim? */
	unsigned int may_swap:1;

	/*
	 * Cgroups are not reclaimed below their configured memory.low,
	 * unless we threaten to OOM. If any cgroups are skipped due to
	 * memory.low and nothing was reclaimed, go back for memory.low.
	 */
	unsigned int memcg_low_reclaim:1;
	unsigned int memcg_low_skipped:1;

	unsigned int hibernation_mode:1;

	/* One of the zones is ready for compaction */
	unsigned int compaction_ready:1;

	/* Incremented by the number of inactive pages that were scanned */
	unsigned long nr_scanned;

	/* Number of pages freed so far during a call to shrink_zones() */
	unsigned long nr_reclaimed;
};

#ifdef ARCH_HAS_PREFETCH
#define prefetch_prev_lru_page(_page, _base, _field)			\
	do {								\
		if ((_page)->lru.prev != _base) {			\
			struct page *prev;				\
									\
			prev = lru_to_page(&(_page->lru));		\
			prefetch(&prev->_field);			\
		}							\
	} while (0)
#else
#define prefetch_prev_lru_page(_page, _base, _field) do { } while (0)
#endif

#ifdef ARCH_HAS_PREFETCHW
#define prefetchw_prev_lru_page(_page, _base, _field)			\
	do {								\
		if ((_page)->lru.prev != _base) {			\
			struct page *prev;				\
									\
			prev = lru_to_page(&(_page->lru));		\
			prefetchw(&prev->_field);			\
		}							\
	} while (0)
#else
#define prefetchw_prev_lru_page(_page, _base, _field) do { } while (0)
#endif

/*
 * From 0 .. 100.  Higher means more swappy.
 */
int vm_swappiness = 60;
/*
 * The total number of pages which are beyond the high watermark within all
 * zones.
 */
unsigned long vm_total_pages;

static LIST_HEAD(shrinker_list);
static DECLARE_RWSEM(shrinker_rwsem);

#ifdef CONFIG_MEMCG
static bool global_reclaim(struct scan_control *sc)
{
	return !sc->target_mem_cgroup;
}

/**
 * sane_reclaim - is the usual dirty throttling mechanism operational?
 * @sc: scan_control in question
 *
 * The normal page dirty throttling mechanism in balance_dirty_pages() is
 * completely broken with the legacy memcg and direct stalling in
 * shrink_page_list() is used for throttling instead, which lacks all the
 * niceties such as fairness, adaptive pausing, bandwidth proportional
 * allocation and configurability.
 *
 * This function tests whether the vmscan currently in progress can assume
 * that the normal dirty throttling mechanism is operational.
 */
static bool sane_reclaim(struct scan_control *sc)
{
	struct mem_cgroup *memcg = sc->target_mem_cgroup;

	if (!memcg)
		return true;
#ifdef CONFIG_CGROUP_WRITEBACK
	if (cgroup_subsys_on_dfl(memory_cgrp_subsys))
		return true;
#endif
	return false;
}
#else
static bool global_reclaim(struct scan_control *sc)
{
	return true;
}

static bool sane_reclaim(struct scan_control *sc)
{
	return true;
}
#endif

/*
 * This misses isolated pages which are not accounted for to save counters.
 * As the data only determines if reclaim or compaction continues, it is
 * not expected that isolated pages will be a dominating factor.
 */
unsigned long zone_reclaimable_pages(struct zone *zone)
{
	unsigned long nr;

	nr = zone_page_state_snapshot(zone, NR_ZONE_INACTIVE_FILE) +
		zone_page_state_snapshot(zone, NR_ZONE_ACTIVE_FILE);
	if (get_nr_swap_pages() > 0)
		nr += zone_page_state_snapshot(zone, NR_ZONE_INACTIVE_ANON) +
			zone_page_state_snapshot(zone, NR_ZONE_ACTIVE_ANON);

	return nr;
}

unsigned long pgdat_reclaimable_pages(struct pglist_data *pgdat)
{
	unsigned long nr;

	nr = node_page_state_snapshot(pgdat, NR_ACTIVE_FILE) +
	     node_page_state_snapshot(pgdat, NR_INACTIVE_FILE) +
	     node_page_state_snapshot(pgdat, NR_ISOLATED_FILE);

	if (get_nr_swap_pages() > 0)
		nr += node_page_state_snapshot(pgdat, NR_ACTIVE_ANON) +
		      node_page_state_snapshot(pgdat, NR_INACTIVE_ANON) +
		      node_page_state_snapshot(pgdat, NR_ISOLATED_ANON);

	return nr;
}

/**
 * lruvec_lru_size -  Returns the number of pages on the given LRU list.
 * @lruvec: lru vector
 * @lru: lru to use
 * @zone_idx: zones to consider (use MAX_NR_ZONES for the whole LRU list)
 */
unsigned long lruvec_lru_size(struct lruvec *lruvec, enum lru_list lru, int zone_idx)
{
	unsigned long lru_size;
	int zid;

	if (!mem_cgroup_disabled())
		lru_size = mem_cgroup_get_lru_size(lruvec, lru);
	else
		lru_size = node_page_state(lruvec_pgdat(lruvec), NR_LRU_BASE + lru);

	for (zid = zone_idx + 1; zid < MAX_NR_ZONES; zid++) {
		struct zone *zone = &lruvec_pgdat(lruvec)->node_zones[zid];
		unsigned long size;

		if (!managed_zone(zone))
			continue;

		if (!mem_cgroup_disabled())
			size = mem_cgroup_get_zone_lru_size(lruvec, lru, zid);
		else
			size = zone_page_state(&lruvec_pgdat(lruvec)->node_zones[zid],
				       NR_ZONE_LRU_BASE + lru);
		lru_size -= min(size, lru_size);
	}

	return lru_size;

}

/*
 * Add a shrinker callback to be called from the vm.
 */
int register_shrinker(struct shrinker *shrinker)
{
	size_t size = sizeof(*shrinker->nr_deferred);

	if (shrinker->flags & SHRINKER_NUMA_AWARE)
		size *= nr_node_ids;

	shrinker->nr_deferred = kzalloc(size, GFP_KERNEL);
	if (!shrinker->nr_deferred)
		return -ENOMEM;

	down_write(&shrinker_rwsem);
	list_add_tail(&shrinker->list, &shrinker_list);
	up_write(&shrinker_rwsem);
	return 0;
}
EXPORT_SYMBOL(register_shrinker);

/*
 * Remove one
 */
void unregister_shrinker(struct shrinker *shrinker)
{
	if (!shrinker->nr_deferred)
		return;
	down_write(&shrinker_rwsem);
	list_del(&shrinker->list);
	up_write(&shrinker_rwsem);
	kfree(shrinker->nr_deferred);
	shrinker->nr_deferred = NULL;
}
EXPORT_SYMBOL(unregister_shrinker);

#define SHRINK_BATCH 128

static unsigned long do_shrink_slab(struct shrink_control *shrinkctl,
				    struct shrinker *shrinker,
				    unsigned long nr_scanned,
				    unsigned long nr_eligible)
{
	unsigned long freed = 0;
	unsigned long long delta;
	long total_scan;
	long freeable;
	long nr;
	long new_nr;
    /* 获取收缩slab 的zone区域值 */
	int nid = shrinkctl->nid;
    /* 如果shrinker收缩器中未指定一次要处理的slab cache对象的数量，则替换SHRINK_BATCH 128， */
	long batch_size = shrinker->batch ? shrinker->batch
					  : SHRINK_BATCH;
	long scanned = 0, next_deferred;
    /* 这是一个回调函数，如果获取的空闲page个数为0，则直接return 0，结束shrinker的收缩操作,当前slab能够释放的objects个数 */
	freeable = shrinker->count_objects(shrinker, shrinkctl);
	if (freeable == 0)
		return 0;

	/*
	 * copy the current shrinker scan count into a local variable
	 * and zero it so that other concurrent shrinker invocations
	 * don't also do this scanning work.
	 */
	/* shrinker回收的slab cache页面数 */
	nr = atomic_long_xchg(&shrinker->nr_deferred[nid], 0);
    /* 扫描总的slab cache数 */
	total_scan = nr;
    /*  */
	delta = (4 * nr_scanned) / shrinker->seeks;
	delta *= freeable;
	do_div(delta, nr_eligible + 1);
	total_scan += delta;
	if (total_scan < 0) {
		pr_err("shrink_slab: %pF negative objects to delete nr=%ld\n",
		       shrinker->scan_objects, total_scan);
		total_scan = freeable;
		next_deferred = nr;
	} else
		next_deferred = total_scan;

	/*
	 * We need to avoid excessive windup on filesystem shrinkers
	 * due to large numbers of GFP_NOFS allocations causing the
	 * shrinkers to return -1 all the time. This results in a large
	 * nr being built up so when a shrink that can do some work
	 * comes along it empties the entire cache due to nr >>>
	 * freeable. This is bad for sustaining a working set in
	 * memory.
	 *
	 * Hence only allow the shrinker to scan the entire cache when
	 * a large delta change is calculated directly.
	 */
	if (delta < freeable / 4)
		total_scan = min(total_scan, freeable / 2);

	/*
	 * Avoid risking looping forever due to too large nr value:
	 * never try to free more than twice the estimate number of
	 * freeable entries.
	 */
	if (total_scan > freeable * 2)
		total_scan = freeable * 2;

	trace_mm_shrink_slab_start(shrinker, shrinkctl, nr,
				   nr_scanned, nr_eligible,
				   freeable, delta, total_scan);

	/*
	 * Normally, we should not scan less than batch_size objects in one
	 * pass to avoid too frequent shrinker calls, but if the slab has less
	 * than batch_size objects in total and we are really tight on memory,
	 * we will try to reclaim all available objects, otherwise we can end
	 * up failing allocations although there are plenty of reclaimable
	 * objects spread over several slabs with usage less than the
	 * batch_size.
	 *
	 * We detect the "tight on memory" situations by looking at the total
	 * number of objects we want to scan (total_scan). If it is greater
	 * than the total number of objects on slab (freeable), we must be
	 * scanning at high prio and therefore should try to reclaim as much as
	 * possible.
	 */
	while (total_scan >= batch_size ||
	       total_scan >= freeable) {
		unsigned long ret;
		unsigned long nr_to_scan = min(batch_size, total_scan);

		shrinkctl->nr_to_scan = nr_to_scan;
		shrinkctl->nr_scanned = nr_to_scan;
		ret = shrinker->scan_objects(shrinker, shrinkctl);
		if (ret == SHRINK_STOP)
			break;
		freed += ret;

		count_vm_events(SLABS_SCANNED, shrinkctl->nr_scanned);
		total_scan -= shrinkctl->nr_scanned;
		scanned += shrinkctl->nr_scanned;

		cond_resched();
	}

	if (next_deferred >= scanned)
		next_deferred -= scanned;
	else
		next_deferred = 0;
	/*
	 * move the unused scan count back into the shrinker in a
	 * manner that handles concurrent updates. If we exhausted the
	 * scan, there is no need to do an update.
	 */
	if (next_deferred > 0)
		new_nr = atomic_long_add_return(next_deferred,
						&shrinker->nr_deferred[nid]);
	else
		new_nr = atomic_long_read(&shrinker->nr_deferred[nid]);

	trace_mm_shrink_slab_end(shrinker, nid, freed, nr, new_nr, total_scan);
	return freed;
}

/**
 * shrink_slab - shrink slab caches
 * @gfp_mask: allocation context
 * @nid: node whose slab caches to target
 * @memcg: memory cgroup whose slab caches to target
 * @nr_scanned: pressure numerator
 * @nr_eligible: pressure denominator
 *
 * Call the shrink functions to age shrinkable caches.
 *
 * @nid is passed along to shrinkers with SHRINKER_NUMA_AWARE set,
 * unaware shrinkers will receive a node id of 0 instead.
 *
 * @memcg specifies the memory cgroup to target. If it is not NULL,
 * only shrinkers with SHRINKER_MEMCG_AWARE set will be called to scan
 * objects from the memory cgroup specified. Otherwise, only unaware
 * shrinkers are called.
 *
 * @nr_scanned and @nr_eligible form a ratio that indicate how much of
 * the available objects should be scanned.  Page reclaim for example
 * passes the number of pages scanned and the number of pages on the
 * LRU lists that it considered on @nid, plus a bias in @nr_scanned
 * when it encountered mapped pages.  The ratio is further biased by
 * the ->seeks setting of the shrink function, which indicates the
 * cost to recreate an object relative to that of an LRU page.
 *
 * Returns the number of reclaimed slab objects.
 */
static unsigned long shrink_slab(gfp_t gfp_mask, int nid,
				 struct mem_cgroup *memcg,
				 unsigned long nr_scanned,
				 unsigned long nr_eligible)
{
	struct shrinker *shrinker;
	unsigned long freed = 0;
    /* 如果memcg为真
     * 且memcg_kmem_enabled、mem_cgroup_onlin任何一个为假的话说明memcg不在线，则直接return 0结束页面的收缩
     * 此时代表该zone没有任何可以进行页面的伸缩的可能性，同时放弃对该zone的尝试
     */
	if (memcg && (!memcg_kmem_enabled() || !mem_cgroup_online(memcg)))
		return 0;
    /* 如果被扫描的page页数为0,则设置被扫描的最大的页面数为SWAP_CLUSTER_MAX也就是32 */
	if (nr_scanned == 0)
		nr_scanned = SWAP_CLUSTER_MAX;
    /* 
     * 如果shrinker_rwsem没有获取到锁则直接跳转到out处，return 1
     * 此时则继续对该zone区域进行收缩操作
     */
	if (!down_read_trylock(&shrinker_rwsem)) {
		/*
		 * If we would return 0, our callers would understand that we
		 * have nothing else to shrink and give up trying. By returning
		 * 1 we keep it going and assume we'll be able to shrink next
		 * time.
		 */
		freed = 1;
		goto out;
	}
    /* 遍历shrinker_list完成slab cahce的收缩回收操作 */
	list_for_each_entry(shrinker, &shrinker_list, list) {
		struct shrink_control sc = {
			.gfp_mask = gfp_mask,
			.nid = nid,
			.memcg = memcg,
		};

		/*
		 * If kernel memory accounting is disabled, we ignore
		 * SHRINKER_MEMCG_AWARE flag and call all shrinkers
		 * passing NULL for memcg.
		 */
		 /* 如果memcg功能开启，且memcg没有设置SHRINKER_MEMCG_AWARE该类型，则直接continue跳过该shrinker */
		if (memcg_kmem_enabled() &&
		    !!memcg != !!(shrinker->flags & SHRINKER_MEMCG_AWARE))
			continue;
        /* 如果设flag类型没有设置SHRINKER_NUMA_AWARE则设置sc.nid = 0,
         * 不再slab cache的扫描操作
         */
		if (!(shrinker->flags & SHRINKER_NUMA_AWARE))
			sc.nid = 0;
        /* 完成对slab cache的扫描回收操作 */
		freed += do_shrink_slab(&sc, shrinker, nr_scanned, nr_eligible);
		/*
		 * Bail out if someone want to register a new shrinker to
		 * prevent the regsitration from being stalled for long periods
		 * by parallel ongoing shrinking.
		 */
		if (rwsem_is_contended(&shrinker_rwsem)) {
			freed = freed ? : 1;
			break;
		}
	}
    /* 释放锁 */
	up_read(&shrinker_rwsem);
out:
	cond_resched();//当前进程需要被调度，该进程休眠
	return freed;
}

void drop_slab_node(int nid)
{
	unsigned long freed;

	do {
		struct mem_cgroup *memcg = NULL;

		freed = 0;
		do {
			freed += shrink_slab(GFP_KERNEL, nid, memcg,
					     1000, 1000);
		} while ((memcg = mem_cgroup_iter(NULL, memcg, NULL)) != NULL);
	} while (freed > 10);
}

void drop_slab(void)
{
	int nid;

	for_each_online_node(nid)
		drop_slab_node(nid);
}

static inline int is_page_cache_freeable(struct page *page)
{
	/*
	 * A freeable page cache page is referenced only by the caller
	 * that isolated the page, the page cache radix tree and
	 * optional buffer heads at page->private.
	 */
	int radix_pins = PageTransHuge(page) && PageSwapCache(page) ?
		HPAGE_PMD_NR : 1;
	return page_count(page) - page_has_private(page) == 1 + radix_pins;
}

static int may_write_to_inode(struct inode *inode, struct scan_control *sc)
{
	if (current->flags & PF_SWAPWRITE)
		return 1;
	if (!inode_write_congested(inode))
		return 1;
	if (inode_to_bdi(inode) == current->backing_dev_info)
		return 1;
	return 0;
}

/*
 * We detected a synchronous write error writing a page out.  Probably
 * -ENOSPC.  We need to propagate that into the address_space for a subsequent
 * fsync(), msync() or close().
 *
 * The tricky part is that after writepage we cannot touch the mapping: nothing
 * prevents it from being freed up.  But we have a ref on the page and once
 * that page is locked, the mapping is pinned.
 *
 * We're allowed to run sleeping lock_page() here because we know the caller has
 * __GFP_FS.
 */
static void handle_write_error(struct address_space *mapping,
				struct page *page, int error)
{
	lock_page(page);
	if (page_mapping(page) == mapping)
		mapping_set_error(mapping, error);
	unlock_page(page);
}

/* possible outcome of pageout() */
typedef enum {
	/* failed to write page out, page is locked */
	PAGE_KEEP,
	/* move page to the active list, page is locked */
	PAGE_ACTIVATE,
	/* page has been sent to the disk successfully, page is unlocked */
	PAGE_SUCCESS,
	/* page is clean and locked */
	PAGE_CLEAN,
} pageout_t;

/*
 * pageout is called by shrink_page_list() for each dirty page.
 * Calls ->writepage().
 */
static pageout_t pageout(struct page *page, struct address_space *mapping,
			 struct scan_control *sc)
{
	/*
	 * If the page is dirty, only perform writeback if that write
	 * will be non-blocking.  To prevent this allocation from being
	 * stalled by pagecache activity.  But note that there may be
	 * stalls if we need to run get_block().  We could test
	 * PagePrivate for that.
	 *
	 * If this process is currently in __generic_file_write_iter() against
	 * this page's queue, we can perform writeback even if that
	 * will block.
	 *
	 * If the page is swapcache, write it back even if that would
	 * block, for some throttling. This happens by accident, because
	 * swap_backing_dev_info is bust: it doesn't reflect the
	 * congestion state of the swapdevs.  Easy to fix, if needed.
	 */
	if (!is_page_cache_freeable(page))
		return PAGE_KEEP;
	if (!mapping) {
		/*
		 * Some data journaling orphaned pages can have
		 * page->mapping == NULL while being dirty with clean buffers.
		 */
		if (page_has_private(page)) {
			if (try_to_free_buffers(page)) {
				ClearPageDirty(page);
				pr_info("%s: orphaned page\n", __func__);
				return PAGE_CLEAN;
			}
		}
		return PAGE_KEEP;
	}
	if (mapping->a_ops->writepage == NULL)
		return PAGE_ACTIVATE;
	if (!may_write_to_inode(mapping->host, sc))
		return PAGE_KEEP;

	if (clear_page_dirty_for_io(page)) {
		int res;
		struct writeback_control wbc = {
			.sync_mode = WB_SYNC_NONE,
			.nr_to_write = SWAP_CLUSTER_MAX,
			.range_start = 0,
			.range_end = LLONG_MAX,
			.for_reclaim = 1,
		};

		SetPageReclaim(page);
		res = mapping->a_ops->writepage(page, &wbc);
		if (res < 0)
			handle_write_error(mapping, page, res);
		if (res == AOP_WRITEPAGE_ACTIVATE) {
			ClearPageReclaim(page);
			return PAGE_ACTIVATE;
		}

		if (!PageWriteback(page)) {
			/* synchronous write or broken a_ops? */
			ClearPageReclaim(page);
		}
		trace_mm_vmscan_writepage(page);
		inc_node_page_state(page, NR_VMSCAN_WRITE);
		return PAGE_SUCCESS;
	}

	return PAGE_CLEAN;
}

/*
 * Same as remove_mapping, but if the page is removed from the mapping, it
 * gets returned with a refcount of 0.
 */
static int __remove_mapping(struct address_space *mapping, struct page *page,
			    bool reclaimed)
{
	unsigned long flags;
	int refcount;

	BUG_ON(!PageLocked(page));
	BUG_ON(mapping != page_mapping(page));

	spin_lock_irqsave(&mapping->tree_lock, flags);
	/*
	 * The non racy check for a busy page.
	 *
	 * Must be careful with the order of the tests. When someone has
	 * a ref to the page, it may be possible that they dirty it then
	 * drop the reference. So if PageDirty is tested before page_count
	 * here, then the following race may occur:
	 *
	 * get_user_pages(&page);
	 * [user mapping goes away]
	 * write_to(page);
	 *				!PageDirty(page)    [good]
	 * SetPageDirty(page);
	 * put_page(page);
	 *				!page_count(page)   [good, discard it]
	 *
	 * [oops, our write_to data is lost]
	 *
	 * Reversing the order of the tests ensures such a situation cannot
	 * escape unnoticed. The smp_rmb is needed to ensure the page->flags
	 * load is not satisfied before that of page->_refcount.
	 *
	 * Note that if SetPageDirty is always performed via set_page_dirty,
	 * and thus under tree_lock, then this ordering is not required.
	 */
	if (unlikely(PageTransHuge(page)) && PageSwapCache(page))
		refcount = 1 + HPAGE_PMD_NR;
	else
		refcount = 2;
	if (!page_ref_freeze(page, refcount))
		goto cannot_free;
	/* note: atomic_cmpxchg in page_freeze_refs provides the smp_rmb */
	if (unlikely(PageDirty(page))) {
		page_ref_unfreeze(page, refcount);
		goto cannot_free;
	}

	if (PageSwapCache(page)) {
		swp_entry_t swap = { .val = page_private(page) };
		mem_cgroup_swapout(page, swap);
		__delete_from_swap_cache(page);
		spin_unlock_irqrestore(&mapping->tree_lock, flags);
		put_swap_page(page, swap);
	} else {
		void (*freepage)(struct page *);
		void *shadow = NULL;

		freepage = mapping->a_ops->freepage;
		/*
		 * Remember a shadow entry for reclaimed file cache in
		 * order to detect refaults, thus thrashing, later on.
		 *
		 * But don't store shadows in an address space that is
		 * already exiting.  This is not just an optizimation,
		 * inode reclaim needs to empty out the radix tree or
		 * the nodes are lost.  Don't plant shadows behind its
		 * back.
		 *
		 * We also don't store shadows for DAX mappings because the
		 * only page cache pages found in these are zero pages
		 * covering holes, and because we don't want to mix DAX
		 * exceptional entries and shadow exceptional entries in the
		 * same page_tree.
		 */
		if (reclaimed && page_is_file_cache(page) &&
		    !mapping_exiting(mapping) && !dax_mapping(mapping))
			shadow = workingset_eviction(mapping, page);
		__delete_from_page_cache(page, shadow);
		spin_unlock_irqrestore(&mapping->tree_lock, flags);

		if (freepage != NULL)
			freepage(page);
	}

	return 1;

cannot_free:
	spin_unlock_irqrestore(&mapping->tree_lock, flags);
	return 0;
}

/*
 * Attempt to detach a locked page from its ->mapping.  If it is dirty or if
 * someone else has a ref on the page, abort and return 0.  If it was
 * successfully detached, return 1.  Assumes the caller has a single ref on
 * this page.
 */
int remove_mapping(struct address_space *mapping, struct page *page)
{
	if (__remove_mapping(mapping, page, false)) {
		/*
		 * Unfreezing the refcount with 1 rather than 2 effectively
		 * drops the pagecache ref for us without requiring another
		 * atomic operation.
		 */
		page_ref_unfreeze(page, 1);
		return 1;
	}
	return 0;
}

/**
 * putback_lru_page - put previously isolated page onto appropriate LRU list
 * @page: page to be put back to appropriate lru list
 *
 * Add previously isolated @page to appropriate LRU list.
 * Page may still be unevictable for other reasons.
 *
 * lru_lock must not be held, interrupts must be enabled.
 */
void putback_lru_page(struct page *page)
{
	bool is_unevictable;
	int was_unevictable = PageUnevictable(page);

	VM_BUG_ON_PAGE(PageLRU(page), page);

redo:
	ClearPageUnevictable(page);

	if (page_evictable(page)) {
		/*
		 * For evictable pages, we can use the cache.
		 * In event of a race, worst case is we end up with an
		 * unevictable page on [in]active list.
		 * We know how to handle that.
		 */
		is_unevictable = false;
		lru_cache_add(page);
	} else {
		/*
		 * Put unevictable pages directly on zone's unevictable
		 * list.
		 */
		is_unevictable = true;
		add_page_to_unevictable_list(page);
		/*
		 * When racing with an mlock or AS_UNEVICTABLE clearing
		 * (page is unlocked) make sure that if the other thread
		 * does not observe our setting of PG_lru and fails
		 * isolation/check_move_unevictable_pages,
		 * we see PG_mlocked/AS_UNEVICTABLE cleared below and move
		 * the page back to the evictable list.
		 *
		 * The other side is TestClearPageMlocked() or shmem_lock().
		 */
		smp_mb();
	}

	/*
	 * page's status can change while we move it among lru. If an evictable
	 * page is on unevictable list, it never be freed. To avoid that,
	 * check after we added it to the list, again.
	 */
	if (is_unevictable && page_evictable(page)) {
		if (!isolate_lru_page(page)) {
			put_page(page);
			goto redo;
		}
		/* This means someone else dropped this page from LRU
		 * So, it will be freed or putback to LRU again. There is
		 * nothing to do here.
		 */
	}

	if (was_unevictable && !is_unevictable)
		count_vm_event(UNEVICTABLE_PGRESCUED);
	else if (!was_unevictable && is_unevictable)
		count_vm_event(UNEVICTABLE_PGCULLED);

	put_page(page);		/* drop ref from isolate */
}

enum page_references {
	PAGEREF_RECLAIM,
	PAGEREF_RECLAIM_CLEAN,
	PAGEREF_KEEP,
	PAGEREF_ACTIVATE,
};

static enum page_references page_check_references(struct page *page,
						  struct scan_control *sc)
{
	int referenced_ptes, referenced_page;
	unsigned long vm_flags;

	referenced_ptes = page_referenced(page, 1, sc->target_mem_cgroup,
					  &vm_flags);
	referenced_page = TestClearPageReferenced(page);

	/*
	 * Mlock lost the isolation race with us.  Let try_to_unmap()
	 * move the page to the unevictable list.
	 */
	if (vm_flags & VM_LOCKED)
		return PAGEREF_RECLAIM;

	if (referenced_ptes) {
		if (PageSwapBacked(page))
			return PAGEREF_ACTIVATE;
		/*
		 * All mapped pages start out with page table
		 * references from the instantiating fault, so we need
		 * to look twice if a mapped file page is used more
		 * than once.
		 *
		 * Mark it and spare it for another trip around the
		 * inactive list.  Another page table reference will
		 * lead to its activation.
		 *
		 * Note: the mark is set for activated pages as well
		 * so that recently deactivated but used pages are
		 * quickly recovered.
		 */
		SetPageReferenced(page);

		if (referenced_page || referenced_ptes > 1)
			return PAGEREF_ACTIVATE;

		/*
		 * Activate file-backed executable pages after first usage.
		 */
		if (vm_flags & VM_EXEC)
			return PAGEREF_ACTIVATE;

		return PAGEREF_KEEP;
	}

	/* Reclaim if clean, defer dirty pages to writeback */
	if (referenced_page && !PageSwapBacked(page))
		return PAGEREF_RECLAIM_CLEAN;

	return PAGEREF_RECLAIM;
}

/* Check if a page is dirty or under writeback */
static void page_check_dirty_writeback(struct page *page,
				       bool *dirty, bool *writeback)
{
	struct address_space *mapping;

	/*
	 * Anonymous pages are not handled by flushers and must be written
	 * from reclaim context. Do not stall reclaim based on them
	 */
	if (!page_is_file_cache(page) ||
	    (PageAnon(page) && !PageSwapBacked(page))) {
		*dirty = false;
		*writeback = false;
		return;
	}

	/* By default assume that the page flags are accurate */
	*dirty = PageDirty(page);
	*writeback = PageWriteback(page);

	/* Verify dirty/writeback state if the filesystem supports it */
	if (!page_has_private(page))
		return;

	mapping = page_mapping(page);
	if (mapping && mapping->a_ops->is_dirty_writeback)
		mapping->a_ops->is_dirty_writeback(page, dirty, writeback);
}

struct reclaim_stat {
	unsigned nr_dirty;
	unsigned nr_unqueued_dirty;
	unsigned nr_congested;
	unsigned nr_writeback;
	unsigned nr_immediate;
	unsigned nr_activate;
	unsigned nr_ref_keep;
	unsigned nr_unmap_fail;
};

/*
 * shrink_page_list() returns the number of reclaimed pages
 */
static unsigned long shrink_page_list(struct list_head *page_list,
				      struct pglist_data *pgdat,
				      struct scan_control *sc,
				      enum ttu_flags ttu_flags,
				      struct reclaim_stat *stat,
				      bool force_reclaim)
{
     /* 初始化两个链表头 */
	LIST_HEAD(ret_pages);
    /* 这个链表保存本次回收就可以立即进行释放的页 */
	LIST_HEAD(free_pages);
	int pgactivate = 0;
	unsigned nr_unqueued_dirty = 0;
	unsigned nr_dirty = 0;
	unsigned nr_congested = 0;
	unsigned nr_reclaimed = 0;
	unsigned nr_writeback = 0;
	unsigned nr_immediate = 0;
	unsigned nr_ref_keep = 0;
	unsigned nr_unmap_fail = 0;
     /* 检查是否需要调度，需要则调度 */
	cond_resched();
    /* 将page_list中的页一个一个释放，至道page_list为空NULL */
	while (!list_empty(page_list)) {
		struct address_space *mapping;
		struct page *page;
		int may_enter_fs;
		enum page_references references = PAGEREF_RECLAIM_CLEAN;
		bool dirty, writeback;
        /* 检查是否需要调度，需要则调度 */
		cond_resched();
        /* 从page_list末尾拿出一个页 */
		page = lru_to_page(page_list);
        /* 将page从当前lru当中删除，也就是从page_list中删除 */
		list_del(&page->lru);
        /* 尝试对此页上锁，如果无法上锁，说明此页正在被其他路径控制，跳转到keep 
         * 对页上锁后，所有访问此页的进程都会加入到zone->wait_table[hash_ptr(page, zone->wait_table_bits)]
         */
		if (!trylock_page(page))
			goto keep;
         /* 在page_list的页一定都是非活动的 */
		VM_BUG_ON_PAGE(PageActive(page), page);
        /* 扫描的页数量++ */
		sc->nr_scanned++;
        /* 如果此页被锁在内存中，则跳转到cull_mlocked */  
		if (unlikely(!page_evictable(page)))
			goto activate_locked;
         /* 如果扫描控制结构中标识不允许进行unmap操作，并且此页有被映射到页表中，跳转到keep_locked */
		if (!sc->may_unmap && page_mapped(page))
			goto keep_locked;

		/* Double the slab pressure for mapped and swapcache pages */
		if ((page_mapped(page) || PageSwapCache(page)) &&
		    !(PageAnon(page) && !PageSwapBacked(page)))
			sc->nr_scanned++;
         /* 本次回收是否允许执行IO操作 */
		may_enter_fs = (sc->gfp_mask & __GFP_FS) ||
			(PageSwapCache(page) && (sc->gfp_mask & __GFP_IO));

		/*
		 * The number of dirty pages determines if a zone is marked
		 * reclaim_congested which affects wait_iff_congested. kswapd
		 * will stall and start writing pages if the tail of the LRU
		 * is all dirty unqueued pages.
		 */
		/* 检查是否是脏页还有此页是否正在回写到磁盘 
         * 这里面主要判断页描述符的PG_dirty和PG_writeback两个标志
         * 匿名页当加入swapcache后，就会被标记PG_dirty
         * 如果文件所属文件系统有特定is_dirty_writeback操作，则执行文件系统特定的is_dirty_writeback操作
         */
		page_check_dirty_writeback(page, &dirty, &writeback);
         /* 如果是脏页或者正在回写的页，脏页数量++ */
		if (dirty || writeback)
			nr_dirty++;
        /* 是脏页但并没有正在回写，则增加没有进行回写的脏页计数 */
		if (dirty && !writeback)
			nr_unqueued_dirty++;

		/*
		 * Treat this page as congested if the underlying BDI is or if
		 * pages are cycling through the LRU so quickly that the
		 * pages marked for immediate reclaim are making it to the
		 * end of the LRU a second time.
		 */
		/* 获取此页对应的address_space，如果此页是匿名页，则为NULL */
		mapping = page_mapping(page);
         /* 如果此页映射的文件所在的磁盘设备等待队列中有数据(正在进行IO处理)或者此页已经在进行回写回收 */
		if (((dirty || writeback) && mapping &&
		     inode_write_congested(mapping->host)) ||
		    (writeback && PageReclaim(page)))
			nr_congested++; /* 可能比较晚才能进行阻塞回写的页的数量 因为磁盘设备现在繁忙，队列中有太多需要写入的数据 */

		/*
		 * If a page at the tail of the LRU is under writeback, there
		 * are three cases to consider.
		 *
		 * 1) If reclaim is encountering an excessive number of pages
		 *    under writeback and this page is both under writeback and
		 *    PageReclaim then it indicates that pages are being queued
		 *    for IO but are being recycled through the LRU before the
		 *    IO can complete. Waiting on the page itself risks an
		 *    indefinite stall if it is impossible to writeback the
		 *    page due to IO error or disconnected storage so instead
		 *    note that the LRU is being scanned too quickly and the
		 *    caller can stall after page list has been processed.
		 *
		 * 2) Global or new memcg reclaim encounters a page that is
		 *    not marked for immediate reclaim, or the caller does not
		 *    have __GFP_FS (or __GFP_IO if it's simply going to swap,
		 *    not to fs). In this case mark the page for immediate
		 *    reclaim and continue scanning.
		 *
		 *    Require may_enter_fs because we would wait on fs, which
		 *    may not have submitted IO yet. And the loop driver might
		 *    enter reclaim, and deadlock if it waits on a page for
		 *    which it is needed to do the write (loop masks off
		 *    __GFP_IO|__GFP_FS for this reason); but more thought
		 *    would probably show more reasons.
		 *
		 * 3) Legacy memcg encounters a page that is already marked
		 *    PageReclaim. memcg does not have any dirty pages
		 *    throttling so we could easily OOM just because too many
		 *    pages are in writeback and there is nothing else to
		 *    reclaim. Wait for the writeback to complete.
		 *
		 * In cases 1) and 2) we activate the pages to get them out of
		 * the way while we continue scanning for clean pages on the
		 * inactive list and refilling from the active list. The
		 * observation here is that waiting for disk writes is more
		 * expensive than potentially causing reloads down the line.
		 * Since they're marked for immediate reclaim, they won't put
		 * memory pressure on the cache working set any longer than it
		 * takes to write them to disk.
		 */
		 /* 此页正在进行回写到磁盘，对于正在回写到磁盘的页，是无法进行回收的，除非等待此页回写完成 
         * 此页正在进行回写有两种情况:
         * 1.此页是正常的进行回写(脏太久了)
         * 2.此页是刚不久前进行内存回收时，导致此页进行回写的
         */
		if (PageWriteback(page)) {
			/* Case 1 above */
             /* 下面的判断都是基于此页正在进行回写到磁盘为前提 */
            /* 当前处于kswapd内核进程，并且此页正在进行回收(可能在等待IO)，然后zone也表明了很多页正在进行回写 
             * 说明此页是已经在回写到磁盘，并且也正在进行回收的，本次回收不需要对此页进行回收
             */
			if (current_is_kswapd() &&
			    PageReclaim(page) &&
			    test_bit(PGDAT_WRITEBACK, &pgdat->flags)) {
			    /* 增加nr_immediate计数，此计数说明此页准备就可以回收了 */
				nr_immediate++;
                /* 跳转到keep_locked */
				goto activate_locked;

			/* Case 2 above */
            /* 此页正在进行正常的回写(不是因为要回收此页才进行的回写)
             * 两种情况会进入这里:
             * 1.本次是针对整个zone进行内存回收的
             * 2.本次回收不允许进行IO操作
             * 那么就标记这个页要回收，本次回收不对此页进行回收，当此页回写完成后，会判断这个PG_reclaim标记，如果置位了，把此页放到非活动lru链表末尾
             * 快速回收会进入这种情况
             */
			} else if (sane_reclaim(sc) ||
			    !PageReclaim(page) || !may_enter_fs) {
				/*
				 * This is slightly racy - end_page_writeback()
				 * might have just cleared PageReclaim, then
				 * setting PageReclaim here end up interpreted
				 * as PageReadahead - but that does not matter
				 * enough to care.  What we do want is for this
				 * page to have PageReclaim set next time memcg
				 * reclaim reaches the tests above, so it will
				 * then wait_on_page_writeback() to avoid OOM;
				 * and it's also appropriate in global reclaim.
				 */
				 /* 设置此页正在进行回收，因为此页正在进行回写，那设置称为进行回收后，回写完成后此页会被放到非活动lru链表末尾 */
				SetPageReclaim(page);
                 /* 增加需要回写计数器 */
				nr_writeback++;
				goto activate_locked;

			/* Case 3 above */
			} else {
			    /* 等待此页回写完成，回写完成后，尝试对此页进行回收，应该只有针对某个memcg进行回收时才会进入这 */
				unlock_page(page);
				wait_on_page_writeback(page);
				/* then go back and try same page again */
				list_add_tail(&page->lru, page_list);
				continue;
			}
		}
        /*
         * 此次回收时非强制进行回收，那要先判断此页需不需要移动到活动lru链表
         * 如果是匿名页，只要最近此页被进程访问过，则将此页移动到活动lru链表头部，否则回收
         * 如果是映射可执行文件的文件页，只要最近被进程访问过，就放到活动lru链表，否则回收
         * 如果是其他的文件页，如果最近被多个进程访问过，移动到活动lru链表，如果只被1个进程访问过，但是PG_referenced置位了，也放入活动lru链表，其他情况回收
         */
		if (!force_reclaim)
			references = page_check_references(page, sc);
        /* 当此次回收时非强制进行回收时 */
		switch (references) {
		case PAGEREF_ACTIVATE: /* 将页放到活动lru链表中 */
			goto activate_locked;
		case PAGEREF_KEEP:/* 页继续保存在非活动lru链表中 */
			nr_ref_keep++;
			goto keep_locked;
        /* 这两个在下面的代码都会尝试回收此页 
         * 注意页所属的vma标记了VM_LOCKED时也会是PAGEREF_RECLAIM，因为后面会要把此页放到lru_unevictable_page链表
         */
		case PAGEREF_RECLAIM:
		case PAGEREF_RECLAIM_CLEAN:
			; /* try to reclaim the page below */
		}

		/*
		 * Anonymous process memory has backing store?
		 * Try to allocate it some swap space here.
		 * Lazyfree page could be freed directly
		 */
		/* page为匿名页，但是又不处于swapcache中，这里会尝试将其加入到swapcache中，这个swapcache作为swap缓冲区 
         * 当页被换出或换入时，会先加入到swapcache，当完全换出或者完全换入时，才会从swapcache中移除
         * 有了此swapcache，当一个页进行换出时，一个进程访问此页，可以直接从swapcache中获取此页的映射，然后swapcache终止此页的换出操作，这样就不用等页要完全换出后，再重新换回来
         */
		if (PageAnon(page) && PageSwapBacked(page)) {
			if (!PageSwapCache(page)) {
                 /* 如果本次回收禁止io操作，则跳转到keep_locked，让此匿名页继续在非活动lru链表中 */
				if (!(sc->gfp_mask & __GFP_IO))
					goto keep_locked;
				if (PageTransHuge(page)) {
					/* cannot split THP, skip it */
					if (!can_split_huge_page(page, NULL))
						goto activate_locked;
					/*
					 * Split pages without a PMD map right
					 * away. Chances are some or all of the
					 * tail pages can be freed without IO.
					 */
					if (!compound_mapcount(page) &&
					    split_huge_page_to_list(page,
								    page_list))
						goto activate_locked;
				}
              /* 将页page加入到swap_cache，然后这个页被视为文件页，起始就是将页描述符信息保存到以swap页槽偏移量为索引的结点
               * 设置页描述符的private = swap页槽偏移量生成的页表项swp_entry_t，因为后面会设置所有映射了此页的页表项为此swp_entry_t
               * 设置页的PG_swapcache标志，表明此页在swapcache中，正在被换出
               * 标记页page为脏页(PG_dirty)，后面就会被换出
               */
              /* 执行成功后，页属于swapcache，并且此页的page->_count会++，但是由于引用此页的进程页表没有设置，进程还是可以正常访问这个页 */
				if (!add_to_swap(page)) {
					if (!PageTransHuge(page))
						goto activate_locked;
					/* Fallback to swap normal pages */
					if (split_huge_page_to_list(page,
								    page_list))
						goto activate_locked;
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
					count_vm_event(THP_SWPOUT_FALLBACK);
#endif
					if (!add_to_swap(page))
                        /* 失败，将此页加入到活动lru链表中 */
						goto activate_locked;
				}

				may_enter_fs = 1; /* 设置可能会用到文件系统相关的操作 */

				/* Adding to swap updated mapping */
                  /* 获取此匿名页所在的swapcache的address_space，这个是根据page->private中保存的swp_entry_t获得 */
				mapping = page_mapping(page);
			}
		} else if (unlikely(PageTransHuge(page))) {
			/* Split file THP */
			if (split_huge_page_to_list(page, page_list))
				goto keep_locked;
		}

		/*
		 * The page is mapped into the page tables of one or more
		 * processes. Try to unmap it here.
		 */
		/* 这里是要对所有映射了此page的页表进行设置
         * 匿名页会把对应的页表项设置为之前获取的swp_entry_t
         */
		if (page_mapped(page)) {
			enum ttu_flags flags = ttu_flags | TTU_BATCH_FLUSH;
            
			if (unlikely(PageTransHuge(page)))
				flags |= TTU_SPLIT_HUGE_PMD;
            /* 对所有映射了此页的进程的页表进行此页的unmap操作
             * ttu_flags基本都有TTU_UNMAP标志
             * 如果是匿名页，那么page->private中是一个带有swap页槽偏移量的swp_entry_t，此后这个swp_entry_t可以转为页表项
             * 执行完此后，匿名页在swapcache中，而对于引用了此页的进程而言，此页已经在swap中
             * 但是当此匿名页还没有完全写到swap中时，如果此时有进程访问此页，会将此页映射到此进程页表中，并取消此页放入swap中的操作，放回匿名页的lru链表(在缺页中断中完成)
             * 而对于文件页，只需要清空映射了此页的进程页表的页表项，不需要设置新的页表项
             * 每一个进程unmap此页，此页的page->_count--
             * 如果反向映射过程中page->_count == 0，则释放此页
             */
			if (!try_to_unmap(page, flags)) {
				nr_unmap_fail++;
				goto activate_locked;
			}
		}
        /* 如果页为脏页，有两种页
         * 一种是当匿名页加入到swapcache中时，就被标记为了脏页
         * 一种是脏的文件页
         */
		if (PageDirty(page)) {
			/*
			 * Only kswapd can writeback filesystem pages
			 * to avoid risk of stack overflow. But avoid
			 * injecting inefficient single-page IO into
			 * flusher writeback as much as possible: only
			 * write pages when we've encountered many
			 * dirty pages, and when we've already scanned
			 * the rest of the LRU for clean pages and see
			 * the same dirty pages again (PageReclaim).
			 */
			/* 只有kswapd内核线程能够进行文件页的回写操作(kswapd中不会造成栈溢出?)，但是只有当zone中有很多脏页时，kswapd也才能进行脏文件页的回写
             * 此标记说明zone的脏页很多，在回收时隔离出来的页都是没有进行回写的脏页时设置
             * 也就是此zone脏页不够多，kswapd不用进行回写操作
             * 当短时间内多次对此zone执行内存回收后，这个ZONE_DIRTY就会被设置，这样做的理由是: 优先回收匿名页和干净的文件页，说不定回收完这些zone中空闲内存就足够了，不需要再进行内存回收了
             * 而对于匿名页，无论是否是kswapd都可以进行回写
             */
			if (page_is_file_cache(page) &&
			    (!current_is_kswapd() || !PageReclaim(page) ||
			     !test_bit(PGDAT_DIRTY, &pgdat->flags))) {
				/*
				 * Immediately reclaim when written back.
				 * Similar in principal to deactivate_page()
				 * except we already have the page isolated
				 * and know it's dirty
				 */
				  /* 增加优先回收页的数量 */
				inc_node_page_state(page, NR_VMSCAN_IMMEDIATE);
                /* 设置此页需要回收，这样当此页回写完成后，就会被放入到非活动lru链表尾部 
                 * 不过可惜这里只能等kswapd内核线程对此页进行回写，要么就等系统到期后自动将此页进行回写，非kswapd线程都不能对文件页进行回写
                 */
				SetPageReclaim(page);
                 /* 让页移动到非活动lru链表头部，如上所说，当回写完成后，页会被移动到非活动lru链表尾部，而内存回收是从非活动lru链表尾部拿页出来回收的 */
				goto activate_locked;
			}
            /* 当zone没有标记ZONE_DIRTY时，kswapd内核线程则会执行到这里 */
            /* 当page_check_references()获取页的状态是PAGEREF_RECLAIM_CLEAN，则跳到keep_locked
             * 页最近没被进程访问过，但此页的PG_referenced被置位
             */
			if (references == PAGEREF_RECLAIM_CLEAN)
				goto keep_locked;
            /* 回收不允许执行文件系统相关操作，则让页移动到非活动lru链表头部 */
			if (!may_enter_fs)
				goto keep_locked;
            /* 回收不允许进行回写，则让页移动到非活动lru链表头部 */
			if (!sc->may_writepage)
				goto keep_locked;

			/*
			 * Page is dirty. Flush the TLB if a writable entry
			 * potentially exists to avoid CPU writes after IO
			 * starts and then write it out here.
			 */
			try_to_unmap_flush_dirty();
            /* 将页进行回写到磁盘，这里只是将页加入到块层，调用结束并不是代表此页已经回写完成
             * 主要调用page->mapping->a_ops->writepage进行回写，对于匿名页，也是swapcache的address_space->a_ops->writepage
             * 页被加入到块层回写队列后，会置位页的PG_writeback，回写完成后清除PG_writeback位，所以在同步模式回写下，结束后PG_writeback位是0的，而异步模式下，PG_writeback很可能为1
             * 此函数中会清除页的PG_dirty标志
             * 会标记页的PG_reclaim
             * 成功将页加入到块层后，页的PG_lock位会清空
             * 也就是在一个页成功进入到回收导致的回写过程中，它的PG_writeback和PG_reclaim标志会置位，而它的PG_dirty和PG_lock标志会清除
             * 而此页成功回写后，它的PG_writeback和PG_reclaim位都会被清除
             */
			switch (pageout(page, mapping, sc)) {
			case PAGE_KEEP:
				goto keep_locked;/* 页会被移动到非活动lru链表头部 */
			case PAGE_ACTIVATE:
				goto activate_locked;/* 页会被移动到活动lru链表 */
			case PAGE_SUCCESS:
                /* 到这里，页的锁已经被释放，也就是PG_lock被清空 
                 * 对于同步回写(一些特殊文件系统只支持同步回写)，这里的PG_writeback、PG_reclaim、PG_dirty、PG_lock标志都是清0的
                 * 对于异步回写，PG_dirty、PG_lock标志都是为0，PG_writeback、PG_reclaim可能为1可能为0(回写完成为0，否则为1)
                 */

                /* 如果PG_writeback被置位，说明此页正在进行回写，这种情况是异步才会发生 */
				if (PageWriteback(page))
					goto keep;
                /* 此页为脏页，这种情况发生在此页最近又被写入了，让其保持在非活动lru链表中 
                 * 还有一种情况，就是匿名页加入到swapcache前，已经没有进程映射此匿名页了，而加入swapcache时不会判断
                 * 但是当对此匿名页进行回写时，会判断此页加入swapcache前是否有进程映射了，如果没有，此页可以直接释放，不需要写入磁盘
                 * 所以在此匿名页回写过程中，就会将此页从swap分区的address_space中的基树拿出来，然后标记为脏页，到这里就会进行判断脏页，之后会释放掉此页
                 */
				if (PageDirty(page))
					goto keep;

				/*
				 * A synchronous write - probably a ramdisk.  Go
				 * ahead and try to reclaim the page.
				 */
				/* 尝试上锁，因为在pageout中会释放page的锁，主要是PG_lock标志 */
				if (!trylock_page(page))
					goto keep;
				if (PageDirty(page) || PageWriteback(page))
					goto keep_locked;
               /* 获取page->mapping */
				mapping = page_mapping(page);
			case PAGE_CLEAN:/* 这个页不是脏页，不需要回写，这种情况只发生在文件页，匿名页当加入到swapcache中时就被设置为脏页 */
				; /* try to free the page below */
			}
		}

		/*
		 * If the page has buffers, try to free the buffer mappings
		 * associated with this page. If we succeed we try to free
		 * the page as well.
		 *
		 * We do this even if the page is PageDirty().
		 * try_to_release_page() does not perform I/O, but it is
		 * possible for a page to have PageDirty set, but it is actually
		 * clean (all its buffers are clean).  This happens if the
		 * buffers were written out directly, with submit_bh(). ext3
		 * will do this, as well as the blockdev mapping.
		 * try_to_release_page() will discover that cleanness and will
		 * drop the buffers and mark the page clean - it can be freed.
		 *
		 * Rarely, pages can have buffers and no ->mapping.  These are
		 * the pages which were not successfully invalidated in
		 * truncate_complete_page().  We try to drop those buffers here
		 * and if that worked, and the page is no longer mapped into
		 * process address space (page_count == 1) it can be freed.
		 * Otherwise, leave the page on the LRU so it is swappable.
		 */
		/* 这里的情况只有页已经完成回写后才会到达这里，比如同步回写时(pageout在页回写完成后才返回)，异步回写时，在运行到此之前已经把页回写到磁盘
         * 没有完成回写的页不会到这里，在pageout()后就跳到keep了
         */
        
        /* 通过页描述符的PAGE_FLAGS_PRIVATE标记判断是否有buffer_head，这个只有文件页有
         * 这里不会通过page->private判断，原因是，当匿名页加入到swapcache时，也会使用page->private，而不会标记PAGE_FLAGS_PRIVATE
         * 只有文件页会使用这个PAGE_FLAGS_PRIVATE，这个标记说明此文件页的page->private指向struct buffer_head链表头
         */
		if (page_has_private(page)) {
            /* 因为页已经回写完成或者是干净不需要回写的页，释放page->private指向struct buffer_head链表，释放后page->private = NULL 
             * 释放时必须要保证此页的PG_writeback位为0，也就是此页已经回写到磁盘中了
             */
			if (!try_to_release_page(page, sc->gfp_mask))
				goto activate_locked; /* 释放失败，把此页移动到活动lru链表 */
            /* 一些特殊的页的mapping为空，比如一些日志的缓冲区，对于这些页如果引用计数为1则进行处理 */
			if (!mapping && page_count(page) == 1) {
                  /* 对此页解锁，清除PG_lock */
				unlock_page(page);
                  /* 对page->_count--，并判断是否为0，如果为0则释放掉此页 */
				if (put_page_testzero(page))
					goto free_it;
				else {
					/*
					 * rare race with speculative reference.
					 * the speculative reference will free
					 * this page shortly, so we may
					 * increment nr_reclaimed here (and
					 * leave it off the LRU).
					 */
					/* 不明白？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？需要进一步分析 */
					nr_reclaimed++;
					continue;
				}
			}
		}

		if (PageAnon(page) && !PageSwapBacked(page)) {
			/* follow __remove_mapping for reference */
			if (!page_ref_freeze(page, 1))
				goto keep_locked;
			if (PageDirty(page)) {
				page_ref_unfreeze(page, 1);
				goto keep_locked;
			}

			count_vm_event(PGLAZYFREED);
			count_memcg_page_event(page, PGLAZYFREED);
		} else if (!mapping || !__remove_mapping(mapping, page, true))
			goto keep_locked;
		/*
		 * At this point, we have no other references and there is
		 * no way to pick any more up (removed from LRU, removed
		 * from pagecache). Can use non-atomic bitops now (and
		 * we obviously don't have to worry about waking up a process
		 * waiting on the page lock, because there are no references.
		 */
		__ClearPageLocked(page);
free_it:
        /* page->_count为0才会到这 */
        
        /* 此页可以马上回收，会把它加入到free_pages链表
         * 到这里的页有三种情况，本次进行同步回写的页，干净的不需要回写的页，之前异步回收时完成异步回写的页
         * 之前回收进行异步回写的页，不会立即释放，因为上次回收时，对这些页进行的工作有: 
         * 匿名页: 加入swapcache，反向映射修改了映射了此页的进程页表项，将此匿名页回写到磁盘，将此页保存到非活动匿名页lru链表尾部
         * 文件页: 反向映射修改了映射了此页的进程页表项，将此文件页回写到磁盘，将此页保存到非活动文件页lru链表尾部
         * 也就是异步情况这两种页都没有进行实际的回收，而在这些页回写完成后，再进行回收时，这两种页的流程都会到这里进行回收
         * 也就是本次能够真正回收到的页，可能是之前进行回收时已经处理得差不多并回写完成的页
         */
        
        /* 回收页数量++ */
		nr_reclaimed++;

		/*
		 * Is there need to periodically free_page_list? It would
		 * appear not as the counts should be low
		 */
		if (unlikely(PageTransHuge(page))) {
			mem_cgroup_uncharge(page);
			(*get_compound_page_dtor(page))(page);
		} else
			list_add(&page->lru, &free_pages); /* 加入到free_pages链表 */
		continue;/* 继续遍历页 */

activate_locked:
		/* Not a candidate for swapping, so reclaim swap space. */
        /* 这种是持有页锁(PG_lock)，并且需要把页移动到活动lru链表中的情况 */
        
        /* 如果此页为匿名页并且放入了swapcache中，并且swap使用率已经超过了50% */
		if (PageSwapCache(page) && (mem_cgroup_swap_full(page) ||
						PageMlocked(page)))
			try_to_free_swap(page); /* 将此页从swapcache的基树中拿出来 */
		VM_BUG_ON_PAGE(PageActive(page), page);
		if (!PageMlocked(page)) {
             /* 设置此页为活动页 */
			SetPageActive(page);
              /* 需要放回到活动lru链表的页数量 */
			pgactivate++;
			count_memcg_page_event(page, PGACTIVATE);
		}
keep_locked:
		unlock_page(page);
keep:
        /* 希望页保持在原来的lru链表中的情况 */

        /* 把页加入到ret_pages链表中 */
		list_add(&page->lru, &ret_pages);
		VM_BUG_ON_PAGE(PageLRU(page) || PageUnevictable(page), page);
	}

	mem_cgroup_uncharge_list(&free_pages);
	try_to_unmap_flush();
    /* 将free_pages中的页释放 */
	free_hot_cold_page_list(&free_pages, true);
     /* 将ret_pages链表加入到page_list中 */
	list_splice(&ret_pages, page_list);
	count_vm_events(PGACTIVATE, pgactivate);

	if (stat) {
		stat->nr_dirty = nr_dirty;
		stat->nr_congested = nr_congested;
		stat->nr_unqueued_dirty = nr_unqueued_dirty;
		stat->nr_writeback = nr_writeback;
		stat->nr_immediate = nr_immediate;
		stat->nr_activate = pgactivate;
		stat->nr_ref_keep = nr_ref_keep;
		stat->nr_unmap_fail = nr_unmap_fail;
	}
	return nr_reclaimed;
}

unsigned long reclaim_clean_pages_from_list(struct zone *zone,
					    struct list_head *page_list)
{
	struct scan_control sc = {
		.gfp_mask = GFP_KERNEL,
		.priority = DEF_PRIORITY,
		.may_unmap = 1,
	};
	unsigned long ret;
	struct page *page, *next;
	LIST_HEAD(clean_pages);

	list_for_each_entry_safe(page, next, page_list, lru) {
		if (page_is_file_cache(page) && !PageDirty(page) &&
		    !__PageMovable(page) && !PageUnevictable(page)) {
			ClearPageActive(page);
			list_move(&page->lru, &clean_pages);
		}
	}

	ret = shrink_page_list(&clean_pages, zone->zone_pgdat, &sc,
			TTU_IGNORE_ACCESS, NULL, true);
	list_splice(&clean_pages, page_list);
	mod_node_page_state(zone->zone_pgdat, NR_ISOLATED_FILE, -ret);
	return ret;
}

/*
 * Attempt to remove the specified page from its LRU.  Only take this page
 * if it is of the appropriate PageActive status.  Pages which are being
 * freed elsewhere are also ignored.
 *
 * page:	page to consider
 * mode:	one of the LRU isolation modes defined above
 *
 * returns 0 on success, -ve errno on failure.
 */
int __isolate_lru_page(struct page *page, isolate_mode_t mode)
{
	int ret = -EINVAL;

	/* Only take pages on the LRU. */
	/* 如果该page不再lru当中则直接return放弃isolate隔离操作 */
	if (!PageLRU(page))
		return ret;

	/* Compaction should not handle unevictable pages but CMA can do so */
	/* 如果page不能被撤销且mode当中没有设置ISOLATE_UNEVICTABLE，则直接return 放弃隔离操作 */
	if (PageUnevictable(page) && !(mode & ISOLATE_UNEVICTABLE))
		return ret;

	ret = -EBUSY;

	/*
	 * To minimise LRU disruption, the caller can indicate that it only
	 * wants to isolate pages it will be able to operate on without
	 * blocking - clean pages for the most part.
	 *
	 * ISOLATE_ASYNC_MIGRATE is used to indicate that it only wants to pages
	 * that it is possible to migrate without blocking
	 */
	/*
	 * 如果mode设置链异步迁移的标志则进入if判断当中
	 */
	if (mode & ISOLATE_ASYNC_MIGRATE) {
		/* All the caller can do on PageWriteback is block */
		/* 如果该page需要会写则直接return 放弃page页面的迁移 */
		if (PageWriteback(page))
			return ret;
		/* 如果page为脏，则进入判断当中 */
		if (PageDirty(page)) {
			struct address_space *mapping;
			bool migrate_dirty;

			/*
			 * Only pages without mappings or that have a
			 * ->migratepage callback are possible to migrate
			 * without blocking. However, we can be racing with
			 * truncation so it's necessary to lock the page
			 * to stabilise the mapping as truncation holds
			 * the page lock until after the page is removed
			 * from the page cache.
			 */
			/* 获取page锁失败则直接return 放弃page页面的迁移 */
			if (!trylock_page(page))
				return ret;
			/* 找到当前page所指向的address_space */
			mapping = page_mapping(page);
			/* 如果mapping为空，或者是mmapin不为空则返回migrate_dirty */
			migrate_dirty = !mapping || mapping->a_ops->migratepage;
			/* 解锁 */
			unlock_page(page);
			/* 如果migrate_dirty = false说明mapping不为空，且该page不能被迁移，则直接return 放弃page的隔离迁移 */
			if (!migrate_dirty)
				return ret;
		}
	}
	/* 如果请求取消映射到该模式，则映射的页面将放弃分离 */
	if ((mode & ISOLATE_UNMAPPED) && page_mapped(page))
		return ret;
	/* 如果参考计数器不为0，则递增1。清除lru标志位并成功返回 */
	if (likely(get_page_unless_zero(page))) {
		/*
		 * Be careful not to clear PageLRU until after we're
		 * sure the page is not being freed elsewhere -- the
		 * page release code relies on it.
		 */
		ClearPageLRU(page);
		ret = 0;
	}

	return ret;
}


/*
 * Update LRU sizes after isolating pages. The LRU size updates must
 * be complete before mem_cgroup_update_lru_size due to a santity check.
 */
static __always_inline void update_lru_sizes(struct lruvec *lruvec,
			enum lru_list lru, unsigned long *nr_zone_taken)
{
	int zid;

	for (zid = 0; zid < MAX_NR_ZONES; zid++) {
		if (!nr_zone_taken[zid])
			continue;

		__update_lru_size(lruvec, lru, zid, -nr_zone_taken[zid]);
#ifdef CONFIG_MEMCG
		mem_cgroup_update_lru_size(lruvec, lru, zid, -nr_zone_taken[zid]);
#endif
	}

}

/*
 * zone_lru_lock is heavily contended.  Some of the functions that
 * shrink the lists perform better by taking out a batch of pages
 * and working on them outside the LRU lock.
 *
 * For pagecache intensive workloads, this function is the hottest
 * spot in the kernel (apart from copy_*_user functions).
 *
 * Appropriate locks must be held before calling this function.
 *
 * @nr_to_scan:	The number of eligible pages to look through on the list.
 * @lruvec:	The LRU vector to pull pages from.
 * @dst:	The temp list to put pages on to.
 * @nr_scanned:	The number of pages that were scanned.
 * @sc:		The scan_control struct for this reclaim session
 * @mode:	One of the LRU isolation modes
 * @lru:	LRU list id for isolating
 *
 * returns how many pages were moved onto *@dst.
 */
static unsigned long isolate_lru_pages(unsigned long nr_to_scan,
		struct lruvec *lruvec, struct list_head *dst,
		unsigned long *nr_scanned, struct scan_control *sc,
		isolate_mode_t mode, enum lru_list lru)
{
	struct list_head *src = &lruvec->lists[lru];
	unsigned long nr_taken = 0;
	unsigned long nr_zone_taken[MAX_NR_ZONES] = { 0 };
	unsigned long nr_skipped[MAX_NR_ZONES] = { 0, };
	unsigned long skipped = 0;
	unsigned long scan, total_scan, nr_pages;
	LIST_HEAD(pages_skipped);

	scan = 0;
	/* 设置total_scan总的扫描此时为0，并在for循环当中累加
	 * scan扫描次数小于nr_to_scan预定的扫描次数，且扫描到的pages小于nr_to_scan，再最后src不能为空NULL；满足这以上三点就进入for循环当中
	 */
	for (total_scan = 0;
	     scan < nr_to_scan && nr_taken < nr_to_scan && !list_empty(src);
	     total_scan++) {
		struct page *page;
		/* 将src的lru转换为page的起始地址，相当于将lru转换为pages */
		page = lru_to_page(src);
		prefetchw_prev_lru_page(page, src, flags);

		VM_BUG_ON_PAGE(!PageLRU(page), page);
		/* 获取pages所在的zone,如果zone > reclaim_idx(想要回收的当前zone),
		 * 满足条件的情况下，将下lrus对应的pages删除，再执行continue跳过高zone空间
		 */
		if (page_zonenum(page) > sc->reclaim_idx) {
			list_move(&page->lru, &pages_skipped);
			nr_skipped[page_zonenum(page)]++;
			continue;
		}

		/*
		 * Do not count skipped pages because that makes the function
		 * return with no isolated pages if the LRU mostly contains
		 * ineligible pages.  This causes the VM to not reclaim any
		 * pages, triggering a premature OOM.
		 */
		scan++;
		/* 将page按照mode的模式完成页面的隔离 */
		switch (__isolate_lru_page(page, mode)) {
		case 0:
			/* 如果page是巨型页，则获取出page是多少个单独的page组成 
			 * nr_pages就是page的个数，
			 * nr_zone_taken[page_zonenum(page)] += nr_pages;该zone当中可以回收的page个数
			 * list_move(&page->lru, dst);将page从lru当中转移到dst链表当中完成隔离操作
			 */
			nr_pages = hpage_nr_pages(page);
			nr_taken += nr_pages;
			nr_zone_taken[page_zonenum(page)] += nr_pages;
			list_move(&page->lru, dst);
			break;

		case -EBUSY:
			/* else it is being freed elsewhere */
			list_move(&page->lru, src);
			continue;

		default:
			BUG();
		}
	}

	/*
	 * Splice any skipped pages to the start of the LRU list. Note that
	 * this disrupts the LRU order when reclaiming for lower zones but
	 * we cannot splice to the tail. If we did then the SWAP_CLUSTER_MAX
	 * scanning would soon rescan the same pages to skip and put the
	 * system at risk of premature OOM.
	 */
	/* 判断pages_skipped是否为空，如果不为空则进入判断当中 */
	if (!list_empty(&pages_skipped)) {
		int zid;
		/* pages_skipped的pages移动到src起始链表的头部 */
		list_splice(&pages_skipped, src);
		/* 遍历Node的zone
		 * 如果nr_skipped[zid]为空则直接continue跳过该zone
		 * 如果nr_skipped[zid]不为空则skipped自加统计Node当中的skip跳过的pages的个数
		 */
		for (zid = 0; zid < MAX_NR_ZONES; zid++) {
			if (!nr_skipped[zid])
				continue;

			__count_zid_vm_events(PGSCAN_SKIP, zid, nr_skipped[zid]);
			skipped += nr_skipped[zid];
		}
	}
	*nr_scanned = total_scan;
	trace_mm_vmscan_lru_isolate(sc->reclaim_idx, sc->order, nr_to_scan,
				    total_scan, skipped, nr_taken, mode, lru);
	update_lru_sizes(lruvec, lru, nr_zone_taken);//更新lruve
	/*  return 扫描的pages数 */
	return nr_taken;
}

/**
 * isolate_lru_page - tries to isolate a page from its LRU list
 * @page: page to isolate from its LRU list
 *
 * Isolates a @page from an LRU list, clears PageLRU and adjusts the
 * vmstat statistic corresponding to whatever LRU list the page was on.
 *
 * Returns 0 if the page was removed from an LRU list.
 * Returns -EBUSY if the page was not on an LRU list.
 *
 * The returned page will have PageLRU() cleared.  If it was found on
 * the active list, it will have PageActive set.  If it was found on
 * the unevictable list, it will have the PageUnevictable bit set. That flag
 * may need to be cleared by the caller before letting the page go.
 *
 * The vmstat statistic corresponding to the list on which the page was
 * found will be decremented.
 *
 * Restrictions:
 * (1) Must be called with an elevated refcount on the page. This is a
 *     fundamentnal difference from isolate_lru_pages (which is called
 *     without a stable reference).
 * (2) the lru_lock must not be held.
 * (3) interrupts must be enabled.
 */
int isolate_lru_page(struct page *page)
{
	int ret = -EBUSY;

	VM_BUG_ON_PAGE(!page_count(page), page);
	WARN_RATELIMIT(PageTail(page), "trying to isolate tail page");

	if (PageLRU(page)) {
		struct zone *zone = page_zone(page);
		struct lruvec *lruvec;

		spin_lock_irq(zone_lru_lock(zone));
		lruvec = mem_cgroup_page_lruvec(page, zone->zone_pgdat);
		if (PageLRU(page)) {
			int lru = page_lru(page);
			get_page(page);
			ClearPageLRU(page);
			del_page_from_lru_list(page, lruvec, lru);
			ret = 0;
		}
		spin_unlock_irq(zone_lru_lock(zone));
	}
	return ret;
}

/*
 * A direct reclaimer may isolate SWAP_CLUSTER_MAX pages from the LRU list and
 * then get resheduled. When there are massive number of tasks doing page
 * allocation, such sleeping direct reclaimers may keep piling up on each CPU,
 * the LRU list will go small and be scanned faster than necessary, leading to
 * unnecessary swapping, thrashing and OOM.
 */
static int too_many_isolated(struct pglist_data *pgdat, int file,
		struct scan_control *sc)
{
	unsigned long inactive, isolated;

	if (current_is_kswapd())
		return 0;

	if (!sane_reclaim(sc))
		return 0;

	if (file) {
		inactive = node_page_state(pgdat, NR_INACTIVE_FILE);
		isolated = node_page_state(pgdat, NR_ISOLATED_FILE);
	} else {
		inactive = node_page_state(pgdat, NR_INACTIVE_ANON);
		isolated = node_page_state(pgdat, NR_ISOLATED_ANON);
	}

	/*
	 * GFP_NOIO/GFP_NOFS callers are allowed to isolate more pages, so they
	 * won't get blocked by normal direct-reclaimers, forming a circular
	 * deadlock.
	 */
	if ((sc->gfp_mask & (__GFP_IO | __GFP_FS)) == (__GFP_IO | __GFP_FS))
		inactive >>= 3;

	return isolated > inactive;
}

static noinline_for_stack void
putback_inactive_pages(struct lruvec *lruvec, struct list_head *page_list)
{
	struct zone_reclaim_stat *reclaim_stat = &lruvec->reclaim_stat;
	struct pglist_data *pgdat = lruvec_pgdat(lruvec);
	LIST_HEAD(pages_to_free);

	/*
	 * Put back any unfreeable pages.
	 */
	while (!list_empty(page_list)) {
		struct page *page = lru_to_page(page_list);
		int lru;

		VM_BUG_ON_PAGE(PageLRU(page), page);
		list_del(&page->lru);
		if (unlikely(!page_evictable(page))) {
			spin_unlock_irq(&pgdat->lru_lock);
			putback_lru_page(page);
			spin_lock_irq(&pgdat->lru_lock);
			continue;
		}

		lruvec = mem_cgroup_page_lruvec(page, pgdat);

		SetPageLRU(page);
		lru = page_lru(page);
		add_page_to_lru_list(page, lruvec, lru);

		if (is_active_lru(lru)) {
			int file = is_file_lru(lru);
			int numpages = hpage_nr_pages(page);
			reclaim_stat->recent_rotated[file] += numpages;
		}
		if (put_page_testzero(page)) {
			__ClearPageLRU(page);
			__ClearPageActive(page);
			del_page_from_lru_list(page, lruvec, lru);

			if (unlikely(PageCompound(page))) {
				spin_unlock_irq(&pgdat->lru_lock);
				mem_cgroup_uncharge(page);
				(*get_compound_page_dtor(page))(page);
				spin_lock_irq(&pgdat->lru_lock);
			} else
				list_add(&page->lru, &pages_to_free);
		}
	}

	/*
	 * To save our caller's stack, now use input list for pages to free.
	 */
	list_splice(&pages_to_free, page_list);
}

/*
 * If a kernel thread (such as nfsd for loop-back mounts) services
 * a backing device by writing to the page cache it sets PF_LESS_THROTTLE.
 * In that case we should only throttle if the backing device it is
 * writing to is congested.  In other cases it is safe to throttle.
 */
static int current_may_throttle(void)
{
	return !(current->flags & PF_LESS_THROTTLE) ||
		current->backing_dev_info == NULL ||
		bdi_write_congested(current->backing_dev_info);
}

/*
 * shrink_inactive_list() is a helper for shrink_node().  It returns the number
 * of reclaimed pages
 */
/* 对lruvec这个lru链表描述符中的lru类型的lru链表进行内存回收，这个lru类型一定是LRU_INACTIVE_ANON或者LRU_INACTIVE_FILE类型
 * nr_to_scan: 最多扫描多少个页框
 * lruvec: lru链表描述符，里面有5个lru链表
 * sc: 扫描控制结构
 * lru: 需要扫描的lru链表
 * 返回本次回收的页框数量
 */
static noinline_for_stack unsigned long
shrink_inactive_list(unsigned long nr_to_scan, struct lruvec *lruvec,
		     struct scan_control *sc, enum lru_list lru)
{
	LIST_HEAD(page_list);
	unsigned long nr_scanned;
	unsigned long nr_reclaimed = 0;
	unsigned long nr_taken;
	struct reclaim_stat stat = {};
	isolate_mode_t isolate_mode = 0;
    /* 此非活动lru是否为非活动文件页lru */
	int file = is_file_lru(lru);
    /* lru所属的zone */
	struct pglist_data *pgdat = lruvec_pgdat(lruvec);
	struct zone_reclaim_stat *reclaim_stat = &lruvec->reclaim_stat;
	bool stalled = false;
    
    /* 如果隔离的页数量多于非活动的页数量，则是隔离太多页了，个人猜测这里是控制并发
     * 当zone的NR_INACTIVE_FILE/ANON < NR_ISOLATED_ANON时，有一种情况是其他CPU也在对此zone进行内存回收，所以NR_ISOLATED_ANON比较高
     */
	while (unlikely(too_many_isolated(pgdat, file, sc))) {
        /*
         * 如果stalled = false;则msleep(100)休眠100ms;后设置stalled = true;如果是并发进行内存回收，另一个CPU可能也在执行内存回收
         * 如果stalled = true则说明能弄释放的page为0
         */
		if (stalled)
			return 0;

		/* wait a bit for the reclaimer. */
		msleep(100);
		stalled = true;

		/* We are about to die and free our memory. Return now. */
         /* 当前进程被其他进程kill了，这里接受到了kill信号 */
		if (fatal_signal_pending(current))
			return SWAP_CLUSTER_MAX;
	}
    /* 将当前cpu的pagevec中的页放入到lru链表中 
     * 而其他CPU的pagevec中的页则不会放回到lru链表中
     * 这样做似乎是因为效率问题
     */
	lru_add_drain();

	if (!sc->may_unmap)
		isolate_mode |= ISOLATE_UNMAPPED;
    
    /* 对lru链表上锁 */
	spin_lock_irq(&pgdat->lru_lock);
    /* 从lruvec这个lru链表描述符的lru类型的lru链表中隔离最多nr_to_scan个页出来，隔离时是从lru链表尾部开始拿，然后放到page_list 
     * 返回隔离了多少个此非活动lru链表的页框
     */
	nr_taken = isolate_lru_pages(nr_to_scan, lruvec, &page_list,
				     &nr_scanned, sc, isolate_mode, lru);
    /* 此zone对应隔离的ANON/FILE页框数量 */
	__mod_node_page_state(pgdat, NR_ISOLATED_ANON + file, nr_taken);
    /* 设置zone当前扫描的页框数，扫描的可以是ANON/FILE不同类型的页框数，
     * 当file = 0时，为anon，如果file = true则说明时file类型页面
     */
	reclaim_stat->recent_scanned[file] += nr_taken;
    /*
     * 1. 判断current当前进程是否为kswapd进程
     * 2. sc是否是一个全局的sc，
     * 如果以上两个条件都满足则扫描的页框数量统计到zone的PGSCAN_KSWAPD
     * 如果条件1不满足，条件2满足则扫描的页框数统计到zone的PGSCAN_DIRECT当中
     */
	if (current_is_kswapd()) {
		if (global_reclaim(sc))
			__count_vm_events(PGSCAN_KSWAPD, nr_scanned);
		count_memcg_events(lruvec_memcg(lruvec), PGSCAN_KSWAPD,
				   nr_scanned);
	} else {
		if (global_reclaim(sc))
			__count_vm_events(PGSCAN_DIRECT, nr_scanned);
		count_memcg_events(lruvec_memcg(lruvec), PGSCAN_DIRECT,
				   nr_scanned);
	}
	spin_unlock_irq(&pgdat->lru_lock);
    /* 隔离出来的页数量为0 */
	if (nr_taken == 0)
		return 0;
    /* 对page_list中的页进行内存回收，此函数是回收非活动page页面的核心函数，也就是shrink_inactive_list的核心函数 */
	nr_reclaimed = shrink_page_list(&page_list, pgdat, sc, 0,
				&stat, false);

	spin_lock_irq(&pgdat->lru_lock);
    /*
     * 1. 判断current当前进程是否为kswapd进程
     * 2. sc是否是一个全局的sc，
     * 如果以上两个条件都满足则扫描的页框数量统计到zone的PGSCAN_KSWAPD
     * 如果条件1不满足，条件2满足则扫描的页框数统计到zone的PGSCAN_DIRECT当中
     */
	if (current_is_kswapd()) {
		if (global_reclaim(sc))
			__count_vm_events(PGSTEAL_KSWAPD, nr_reclaimed);
		count_memcg_events(lruvec_memcg(lruvec), PGSTEAL_KSWAPD,
				   nr_reclaimed);
	} else {
		if (global_reclaim(sc))
			__count_vm_events(PGSTEAL_DIRECT, nr_reclaimed);
		count_memcg_events(lruvec_memcg(lruvec), PGSTEAL_DIRECT,
				   nr_reclaimed);
	}
    
    /* 
     * 将page_list中剩余的页放回它对应的lru链表中，这里的页有三种情况:
     * 1.最近被访问了，放到活动lru链表头部
     * 2.此页需要锁在内存中，加入到unevictablelru链表
     * 3.此页为非活动页，移动到非活动lru链表头部
     * 当页正在进行回写回收，当回写完成后，通过判断页的PG_reclaim可知此页正在回收，会把页移动到非活动lru链表末尾，具体见end_page_writeback()函数
     * 加入lru的页page->_refcount--
     * 因为隔离出来时page->_refcount++，而在lru中是不需要对page->_refcount++的
     */
	putback_inactive_pages(lruvec, &page_list);
    /* 更新此zone对应隔离的ANON/FILE页框数量，这里减掉了nr_taken，与此函数之前相对应 */
	__mod_node_page_state(pgdat, NR_ISOLATED_ANON + file, -nr_taken);

	spin_unlock_irq(&pgdat->lru_lock);

	mem_cgroup_uncharge_list(&page_list);
    /* 释放page_list中剩余的页到伙伴系统中的每CPU页高速缓存中，以冷页处理 
     * 这里剩余的就是page->_refcount == 0的页
     */
	free_hot_cold_page_list(&page_list, true);

	/*
	 * If reclaim is isolating dirty pages under writeback, it implies
	 * that the long-lived page allocation rate is exceeding the page
	 * laundering rate. Either the global limits are not being effective
	 * at throttling processes due to the page distribution throughout
	 * zones or there is heavy usage of a slow backing device. The
	 * only option is to throttle from reclaim context which is not ideal
	 * as there is no guarantee the dirtying process is throttled in the
	 * same way balance_dirty_pages() manages.
	 *
	 * Once a zone is flagged ZONE_WRITEBACK, kswapd will count the number
	 * of pages under pages flagged for immediate reclaim and stall if any
	 * are encountered in the nr_immediate check below.
	 */
	 /* 隔离出来的页都在进行回写(但不是回收造成的回写) */
	if (stat.nr_writeback && stat.nr_writeback == nr_taken)
         /* 标记ZONE的ZONE_WRITEBACK，标记此zone许多页在回写 */
		set_bit(PGDAT_WRITEBACK, &pgdat->flags);

	/*
	 * If dirty pages are scanned that are not queued for IO, it
	 * implies that flushers are not doing their job. This can
	 * happen when memory pressure pushes dirty pages to the end of
	 * the LRU before the dirty limits are breached and the dirty
	 * data has expired. It can also happen when the proportion of
	 * dirty pages grows not through writes but through memory
	 * pressure reclaiming all the clean cache. And in some cases,
	 * the flushers simply cannot keep up with the allocation
	 * rate. Nudge the flusher threads in case they are asleep.
	 */
	/* 说明没有在队列当中的io没有完成flush操作，启动flusher_thread线程完成flush操作 */
	if (stat.nr_unqueued_dirty == nr_taken)
		wakeup_flusher_threads(0, WB_REASON_VMSCAN);

	/*
	 * Legacy memcg will stall in page writeback so avoid forcibly
	 * stalling here.
	 */
	if (sane_reclaim(sc)) {
		/*
		 * Tag a zone as congested if all the dirty pages scanned were
		 * backed by a congested BDI and wait_iff_congested will stall.
		 */
		if (stat.nr_dirty && stat.nr_dirty == stat.nr_congested)
			set_bit(PGDAT_CONGESTED, &pgdat->flags);

		/* Allow kswapd to start writing pages during reclaim. */
		if (stat.nr_unqueued_dirty == nr_taken)
			set_bit(PGDAT_DIRTY, &pgdat->flags);

		/*
		 * If kswapd scans pages marked marked for immediate
		 * reclaim and under writeback (nr_immediate), it implies
		 * that pages are cycling through the LRU faster than
		 * they are written so also forcibly stall.
		 */
		  /* 有一些页是因为回收导致它们在回写，则等待一下设备，等待时间是100ms */
		if (stat.nr_immediate && current_may_throttle())
			congestion_wait(BLK_RW_ASYNC, HZ/10);
	}

	/*
	 * Stall direct reclaim for IO completions if underlying BDIs or zone
	 * is congested. Allow kswapd to continue until it starts encountering
	 * unqueued dirty pages or cycling through the LRU too quickly.
	 */
	 /* 非kswapd的情况下，如果现在设备回写压力较大,等待100ms */
	if (!sc->hibernation_mode && !current_is_kswapd() &&
	    current_may_throttle())
		wait_iff_congested(pgdat, BLK_RW_ASYNC, HZ/10);

	trace_mm_vmscan_lru_shrink_inactive(pgdat->node_id,
			nr_scanned, nr_reclaimed,
			stat.nr_dirty,  stat.nr_writeback,
			stat.nr_congested, stat.nr_immediate,
			stat.nr_activate, stat.nr_ref_keep,
			stat.nr_unmap_fail,
			sc->priority, file);
	return nr_reclaimed;
}

/*
 * This moves pages from the active list to the inactive list.
 *
 * We move them the other way if the page is referenced by one or more
 * processes, from rmap.
 *
 * If the pages are mostly unmapped, the processing is fast and it is
 * appropriate to hold zone_lru_lock across the whole operation.  But if
 * the pages are mapped, the processing is slow (page_referenced()) so we
 * should drop zone_lru_lock around each page.  It's impossible to balance
 * this, so instead we remove the pages from the LRU while processing them.
 * It is safe to rely on PG_active against the non-LRU pages in here because
 * nobody will play with that bit on a non-LRU page.
 *
 * The downside is that we have to touch page->_refcount against each page.
 * But we had to alter page->flags anyway.
 *
 * Returns the number of pages moved to the given lru.
 */

static unsigned move_active_pages_to_lru(struct lruvec *lruvec,
				     struct list_head *list,
				     struct list_head *pages_to_free,
				     enum lru_list lru)
{
	struct pglist_data *pgdat = lruvec_pgdat(lruvec);
	struct page *page;
	int nr_pages;
	int nr_moved = 0;

	while (!list_empty(list)) {
		page = lru_to_page(list);
		lruvec = mem_cgroup_page_lruvec(page, pgdat);

		VM_BUG_ON_PAGE(PageLRU(page), page);
		SetPageLRU(page);

		nr_pages = hpage_nr_pages(page);
		update_lru_size(lruvec, lru, page_zonenum(page), nr_pages);
		list_move(&page->lru, &lruvec->lists[lru]);

		if (put_page_testzero(page)) {
			__ClearPageLRU(page);
			__ClearPageActive(page);
			del_page_from_lru_list(page, lruvec, lru);

			if (unlikely(PageCompound(page))) {
				spin_unlock_irq(&pgdat->lru_lock);
				mem_cgroup_uncharge(page);
				(*get_compound_page_dtor(page))(page);
				spin_lock_irq(&pgdat->lru_lock);
			} else
				list_add(&page->lru, pages_to_free);
		} else {
			nr_moved += nr_pages;
		}
	}

	if (!is_active_lru(lru)) {
		__count_vm_events(PGDEACTIVATE, nr_moved);
		count_memcg_events(lruvec_memcg(lruvec), PGDEACTIVATE,
				   nr_moved);
	}

	return nr_moved;
}
                     
/*
 * 从lruvec中的lru类型的链表中获取一些页，并移动到非活动lru链表头部，注意此函数会以lru参数为类型，比如lru参数为LRU_ACTIVE_ANON，那只会处理ANON类型的页，不会处理FILE类型的页
 * 只有代码段的页最近被访问了，会将其加入到活动lru链表头部，其他页即使最近被访问了，也移动到非活动lru链表
 * 从lruvec中的lru类型的链表中拿出一些页之后，会判断这些页的去处，然后将page->_refcount = 1的页进行释放，因为说明此页只有隔离的时候对其page->_refcount进行了++，已经没有进程或模块引用此页
 * 将其释放到伙伴系统的每CPU高速缓存中
 * nr_to_scan: 默认是32，扫描次数，如果扫描的全是普通页，那最多扫描32个页，如果全是大页，最多扫描(大页/普通页)*32个页
 * lruvec: 需要扫描的lru链表(里面包括一个zone中所有类型的lru链表)
 * sc: 扫描控制结构
 * lru: 需要扫描的类型，是active_file或者active_anon的lru链表
 */
static void shrink_active_list(unsigned long nr_to_scan,
			       struct lruvec *lruvec,
			       struct scan_control *sc,
			       enum lru_list lru)
{
	unsigned long nr_taken;
	unsigned long nr_scanned;
	unsigned long vm_flags;
    /* 从lru中获取到的页存放在这，到最后这里面还有剩余的页的话，就把它们释放回伙伴系统 */
	LIST_HEAD(l_hold);	/* The pages which were snipped off */
    /* 移动到活动lru链表头部的页的链表 */
	LIST_HEAD(l_active);
    /* 将要移动到非活动lru链表的页放在这 */
	LIST_HEAD(l_inactive);
	struct page *page;
    /* lruvec的统计结构 */
	struct zone_reclaim_stat *reclaim_stat = &lruvec->reclaim_stat;
	unsigned nr_deactivate, nr_activate;
	unsigned nr_rotated = 0;
	isolate_mode_t isolate_mode = 0;
    /* lru是否属于LRU_INACTIVE_FILE或者LRU_ACTIVE_FILE */
	int file = is_file_lru(lru);
    /* lruvec所属的zone */
	struct pglist_data *pgdat = lruvec_pgdat(lruvec);
    
    /* 将当前CPU的多个pagevec中的页都放入lru链表中 */
	lru_add_drain();
    /* 从kswapd调用过来的情况下，sc->may_unmap为1
     * 直接内存回收的情况，sc->may_unmap为1
     * 快速内存回收的情况，sc->may_unmap与zone_reclaim_mode有关
     */
	if (!sc->may_unmap)
		isolate_mode |= ISOLATE_UNMAPPED;
    
    /* 对zone上锁 */
	spin_lock_irq(&pgdat->lru_lock);
    /* 从lruvec中lru类型链表的尾部拿出一些页隔离出来，放入到l_hold中，lru类型一般是LRU_ACTIVE_ANON或LRU_ACTIVE_FILE
     * 也就是从活动的lru链表中隔离出一些页，从活动lru链表的尾部依次拿出
     * 当sc->may_unmap为0时，则不会将有进程映射的页隔离出来
     * 当sc->may_writepage为0时，则不会将脏页和正在回写的页隔离出来
     * 隔离出来的页会page->_refcount++
     * nr_taken保存拿出的页的数量
     */
	nr_taken = isolate_lru_pages(nr_to_scan, lruvec, &l_hold,
				     &nr_scanned, sc, isolate_mode, lru);

	__mod_node_page_state(pgdat, NR_ISOLATED_ANON + file, nr_taken);
	reclaim_stat->recent_scanned[file] += nr_taken;
     /* 做统计 */
	__count_vm_events(PGREFILL, nr_scanned);
	count_memcg_events(lruvec_memcg(lruvec), PGREFILL, nr_scanned);
    /* 释放lru链表锁 */
	spin_unlock_irq(&pgdat->lru_lock);
    /* 将l_hold中的页一个一个处理 */
	while (!list_empty(&l_hold)) {
        /* 是否需要调度，需要则调度 */
		cond_resched();
         /* 将页从l_hold中拿出来 */
		page = lru_to_page(&l_hold);
        /* 将page从lru链表当中删除 */
		list_del(&page->lru);
        
        /* 如果页是unevictable(不可回收)的，则放回到LRU_UNEVICTABLE这个lru链表中，这个lru链表中的页不能被交换出去 */
		if (unlikely(!page_evictable(page))) {
            /* 放回到page所应该属于的lru链表中 
             * 而这里实际上是将页放到zone的LRU_UNEVICTABLE链表中
             */
			putback_lru_page(page);
			continue;
		}
         /* buffer_heads的数量超过了结点允许的最大值的情况 */
		if (unlikely(buffer_heads_over_limit)) {
             /* 文件页才有的page才有PAGE_FLAGS_PRIVATE标志 */
			if (page_has_private(page) && trylock_page(page)) {
				if (page_has_private(page))
					try_to_release_page(page, 0);/* 释放此文件页所拥有的buffer_head链表中的buffer_head，并且page->_refcount-- */
				unlock_page(page);
			}
		}
        
        /* 检查此页面最近是否有被访问过，通过映射了此页的页表项的Accessed进行检查，并且会清除页表项的Accessed标志
         * 如果此页最近被访问过，返回的是Accessed为1的数量页表项数量
         */
		if (page_referenced(page, 0, sc->target_mem_cgroup,
				    &vm_flags)) {
		    /* 如果是大页，则记录一共多少个页，如果是普通页，则是1 */
			nr_rotated += hpage_nr_pages(page);
			/*
			 * Identify referenced, file-backed active pages and
			 * give them one more trip around the active list. So
			 * that executable code get better chances to stay in
			 * memory under moderate memory pressure.  Anon pages
			 * are not likely to be evicted by use-once streaming
			 * IO, plus JVM can create lots of anon VM_EXEC pages,
			 * so we ignore them here.
			 */
			/* 如果此页映射的是代码段，则将其放到l_active链表中，此链表之后会把页放入页对应的活动lru链表中
             * 可以看出对于代码段的页，还是比较倾向于将它们放到活动文件页lru链表的
             * 当代码段没被访问过时，也是有可能换到非活动文件页lru链表的
             */
			if ((vm_flags & VM_EXEC) && page_is_file_cache(page)) {
				list_add(&page->lru, &l_active);
				continue;
			}
		}
        /* 将页放到l_inactive链表中
         * 只有最近访问过的代码段的页不会被放入，其他即使被访问过了，也会被放入l_inactive
         */
		ClearPageActive(page);	/* we are de-activating */
		list_add(&page->lru, &l_inactive);
	}

	/*
	 * Move pages back to the lru list.
	 */
	spin_lock_irq(&pgdat->lru_lock);
	/*
	 * Count referenced pages from currently used mappings as rotated,
	 * even though only some of them are actually re-activated.  This
	 * helps balance scan pressure between file and anonymous pages in
	 * get_scan_count.
	 */
	 /* 记录的是最近被加入到活动lru链表的页数量，之后这些页被返回到active链表 */
	reclaim_stat->recent_rotated[file] += nr_rotated;
    /* 将l_active链表中的页移动到lruvec->lists[lru]中，这里是将active的页移动到active的lru链表头部 */
	nr_activate = move_active_pages_to_lru(lruvec, &l_active, &l_hold, lru);
     /* 将l_inactive链表中的页移动到lruvec->lists[lru - LRU_ACITVE]中，这里是将active的页移动到inactive的lru头部 */
	nr_deactivate = move_active_pages_to_lru(lruvec, &l_inactive, &l_hold, lru - LRU_ACTIVE);
	__mod_node_page_state(pgdat, NR_ISOLATED_ANON + file, -nr_taken);
	spin_unlock_irq(&pgdat->lru_lock);

	mem_cgroup_uncharge_list(&l_hold);
     /* 剩下的页的处理，剩下的都是page->_refcount为0的页，作为冷页放回到伙伴系统的每CPU单页框高速缓存中 */
	free_hot_cold_page_list(&l_hold, true);
	trace_mm_vmscan_lru_shrink_active(pgdat->node_id, nr_taken, nr_activate,
			nr_deactivate, nr_rotated, sc->priority, file);
}

/*
 * The inactive anon list should be small enough that the VM never has
 * to do too much work.
 *
 * The inactive file list should be small enough to leave most memory
 * to the established workingset on the scan-resistant active list,
 * but large enough to avoid thrashing the aggregate readahead window.
 *
 * Both inactive lists should also be large enough that each inactive
 * page has a chance to be referenced again before it is reclaimed.
 *
 * If that fails and refaulting is observed, the inactive list grows.
 *
 * The inactive_ratio is the target ratio of ACTIVE to INACTIVE pages
 * on this LRU, maintained by the pageout code. A zone->inactive_ratio
 * of 3 means 3:1 or 25% of the pages are kept on the inactive list.
 *
 * total     target    max
 * memory    ratio     inactive
 * -------------------------------------
 *   10MB       1         5MB
 *  100MB       1        50MB
 *    1GB       3       250MB
 *   10GB      10       0.9GB
 *  100GB      31         3GB
 *    1TB     101        10GB
 *   10TB     320        32GB
 */
static bool inactive_list_is_low(struct lruvec *lruvec, bool file,
				 struct scan_control *sc, bool trace)
{
	enum lru_list active_lru = file * LRU_FILE + LRU_ACTIVE;
	struct pglist_data *pgdat = lruvec_pgdat(lruvec);
	enum lru_list inactive_lru = file * LRU_FILE;
	unsigned long inactive, active;
	unsigned long inactive_ratio;
	unsigned long refaults;
	unsigned long gb;

	/*
	 * If we don't have swap space, anonymous page deactivation
	 * is pointless.
	 */
	if (!file && !total_swap_pages)
		return false;

	inactive = lruvec_lru_size(lruvec, inactive_lru, sc->reclaim_idx);
	active = lruvec_lru_size(lruvec, active_lru, sc->reclaim_idx);

	/*
	 * When refaults are being observed, it means a new workingset
	 * is being established. Disable active list protection to get
	 * rid of the stale workingset quickly.
	 */
	refaults = lruvec_page_state(lruvec, WORKINGSET_ACTIVATE);
	if (file && lruvec->refaults != refaults) {
		inactive_ratio = 0;
	} else {
		gb = (inactive + active) >> (30 - PAGE_SHIFT);
		if (gb)
			inactive_ratio = int_sqrt(10 * gb);
		else
			inactive_ratio = 1;
	}

	if (trace)
		trace_mm_vmscan_inactive_list_is_low(pgdat->node_id, sc->reclaim_idx,
			lruvec_lru_size(lruvec, inactive_lru, MAX_NR_ZONES), inactive,
			lruvec_lru_size(lruvec, active_lru, MAX_NR_ZONES), active,
			inactive_ratio, file);

	return inactive * inactive_ratio < active;
}
                 
/*
 * 对lru链表进行处理
 * lru: lru链表的类型
 * nr_to_scan: 需要扫描的页框数量，此值 <= 32，当链表长度不足32时，就为链表长度
 * lruvec: lru链表描述符，与lru参数结合就得出待处理的lru链表
 * sc: 扫描控制结构
 */
static unsigned long shrink_list(enum lru_list lru, unsigned long nr_to_scan,
				 struct lruvec *lruvec, struct scan_control *sc)
{
     /* 如果lru类型是活动lru(包括活动匿名页lru和活动文件页lru) */
	if (is_active_lru(lru)) {
        /* 如果此活动lru对应的非活动lru链表中维护的页框数量太少，则会从活动lru链表中移动一些到对应非活动lru链表中 
         * 这里需要注意，文件页和匿名页的非活动lru链表中是否少计算方式是不同的
         * 匿名页的话，有一个经验值表示大概多少匿名页保存到非活动匿名页lru链表
         * 文件页的话，大概非活动文件页数量要大于活动文件页
         * 而如果遇到page->_count == 0的页，则会将它们释放到每CPU页框高速缓存中
         */
		if (inactive_list_is_low(lruvec, is_file_lru(lru), sc, true))
            /* 从活动lru中移动一些页框到非活动lru中，移动nr_to_scan个，nr_to_scan <= 32，从活动lru链表末尾拿出页框移动到非活动lru链表头 
             * 只有代码段的页最近被访问了，会将其加入到活动lru链表头部，其他页即使最近被访问了，也移动到非活动lru链表
             */
			shrink_active_list(nr_to_scan, lruvec, sc, lru);
		return 0;
	}
    /* 如果lru类似是非活动lru，那么会对此lru类型的lru链表中的页框进行回收 */
	return shrink_inactive_list(nr_to_scan, lruvec, sc, lru);
}

enum scan_balance {
	SCAN_EQUAL,
	SCAN_FRACT,
	SCAN_ANON,
	SCAN_FILE,
};

/*
 * Determine how aggressively the anon and file LRU lists should be
 * scanned.  The relative value of each set of LRU lists is determined
 * by looking at the fraction of the pages scanned we did rotate back
 * onto the active list instead of evict.
 *
 * nr[0] = anon inactive pages to scan; nr[1] = anon active pages to scan
 * nr[2] = file inactive pages to scan; nr[3] = file active pages to scan
 */
static void get_scan_count(struct lruvec *lruvec, struct mem_cgroup *memcg,
			   struct scan_control *sc, unsigned long *nr,
			   unsigned long *lru_pages)
{
	int swappiness = mem_cgroup_swappiness(memcg);//获取memcg是否在进行sawpping交换操作
	/* 将lruvec的内存回收状态赋值给reclaim_stat */
	struct zone_reclaim_stat *reclaim_stat = &lruvec->reclaim_stat;
	u64 fraction[2];
	u64 denominator = 0;	/* gcc */
	/* 获取lruvec获取该page的pgdat也就是zone */
	struct pglist_data *pgdat = lruvec_pgdat(lruvec);
	unsigned long anon_prio, file_prio;
	enum scan_balance scan_balance;
	unsigned long anon, file;
	unsigned long ap, fp;
	enum lru_list lru;

	/* If we have no swap space, do not bother scanning anon pages. */
	/* 如果没有没有swap交换空间，就不再进行匿名页面anon pages的扫描，
	 * 设置scan_balance = SCAN_FILE之进行FILE页面的扫描回收
	 */
	if (!sc->may_swap || mem_cgroup_get_nr_swap_pages(memcg) <= 0) {
		scan_balance = SCAN_FILE;
		goto out;
	}

	/*
	 * Global reclaim will swap to prevent OOM even with no
	 * swappiness, but memcg users want to use this knob to
	 * disable swapping for individual groups completely when
	 * using the memory controller's swap limit feature would be
	 * too expensive.
	 */
	/* 如果sc不是全局的内存回收，且swappiness = 0则设置scan_balance = SCAN_FILE之进FILE文件页面的回收 */
	if (!global_reclaim(sc) && !swappiness) {
		scan_balance = SCAN_FILE;
		goto out;
	}

	/*
	 * Do not apply any pressure balancing cleverness when the
	 * system is close to OOM, scan both anon and file equally
	 * (unless the swappiness setting disagrees with swapping).
	 */
	/* 如果sc内存回收设置优先级为0为最高优先级，且swappiness不为0，
	 * 则设置scan_balance = SCAN_EQUAL做平等公平平等的内存扫描
	 */
	if (!sc->priority && swappiness) {
		scan_balance = SCAN_EQUAL;
		goto out;
	}

	/*
	 * Prevent the reclaimer from falling into the cache trap: as
	 * cache pages start out inactive, every cache fault will tip
	 * the scan balance towards the file LRU.  And as the file LRU
	 * shrinks, so does the window for rotation from references.
	 * This means we have a runaway feedback loop where a tiny
	 * thrashing file LRU becomes infinitely more attractive than
	 * anon pages.  Try to detect this based on file LRU size.
	 */
	/* 判断sc是否为全局的内存扫描结构体 */
	if (global_reclaim(sc)) {
		unsigned long pgdatfile;//node当中的文件页面
		unsigned long pgdatfree;//node当中的空闲页面，也就是没有被使用的可用页面
		int z;
		unsigned long total_high_wmark = 0;
		/* 当前node当中所有的freepage空闲页面总和 */
		pgdatfree = sum_zone_node_page_state(pgdat->node_id, NR_FREE_PAGES);
		/* 当前node当中file文件页面的总和，包含活动文件页面，非活动空闲页面只和 */
		pgdatfile = node_page_state(pgdat, NR_ACTIVE_FILE) +
			   node_page_state(pgdat, NR_INACTIVE_FILE);
		/* 在当前node当中，遍历node当中所有的zone */
		for (z = 0; z < MAX_NR_ZONES; z++) {
			struct zone *zone = &pgdat->node_zones[z];
			/* 判断zone当中的是否存在buddy system管理的pages页面，如果没有则直接continue跳过当前zone,扫描下一个zone */
			if (!managed_zone(zone))
				continue;
			/* 累加NODE当中各个zone对应的high wmark得出node最高水位之和 */
			total_high_wmark += high_wmark_pages(zone);
		}
		/* 如果freepage可用页面 + 文件页面的总和 <= node区域的最高水位总和则进入，当前判断当中 */
		if (unlikely(pgdatfile + pgdatfree <= total_high_wmark)) {
			/*
			 * Force SCAN_ANON if there are enough inactive
			 * anonymous pages on the LRU in eligible zones.
			 * Otherwise, the small LRU gets thrashed.
			 */
			/* 判断是否存在足够的非活动匿名页面，如果存在足够的非活动匿名页面则设置scan_balance = SCAN_ANON
			*  强制的对ANON页面进行扫描回收
 			*/
			if (!inactive_list_is_low(lruvec, false, sc, false) &&
			    lruvec_lru_size(lruvec, LRU_INACTIVE_ANON, sc->reclaim_idx)
					>> sc->priority) {
				scan_balance = SCAN_ANON;
				goto out;
			}
		}
	}

	/*
	 * If there is enough inactive page cache, i.e. if the size of the
	 * inactive list is greater than that of the active list *and* the
	 * inactive list actually has some pages to scan on this priority, we
	 * do not reclaim anything from the anonymous working set right now.
	 * Without the second condition we could end up never scanning an
	 * lruvec even if it has plenty of old anonymous pages unless the
	 * system is under heavy pressure.
	 */
	/* 如果非活动页面数量大于活动页面数且lruves当中非活动页面INACTIVE_FILE >> 左偏当前扫描优先级大于0 
	 * 则之扫描FILE文件页，设置scan_balance = SCAN_FILE
	 */
	if (!inactive_list_is_low(lruvec, true, sc, false) &&
	    lruvec_lru_size(lruvec, LRU_INACTIVE_FILE, sc->reclaim_idx) >> sc->priority) {
		scan_balance = SCAN_FILE;
		goto out;
	}
	/* 上面条件都不满足的情况下，设置scan_balance = SCAN_FRACT此时计算匿名页面，file文件页面的比例完成内存回收的计算设置 */
	scan_balance = SCAN_FRACT;

	/*
	 * With swappiness at 100, anonymous and file have the same priority.
	 * This scanning priority is essentially the inverse of IO cost.
	 */
	/* 将swappiness的值赋值给anon_prio作为匿名页面的回收优先级，swappiness的范围是0～100之间
	 * 200 - anon_prio匿名页面的回收的优先级，则就是等于文件页的回收优先级 
	 */
	anon_prio = swappiness;
	file_prio = 200 - anon_prio;

	/*
	 * OK, so we have swap space and a fair amount of page cache
	 * pages.  We use the recently rotated / recently scanned
	 * ratios to determine how valuable each cache is.
	 *
	 * Because workloads change over time (and to avoid overflow)
	 * we keep these statistics as a floating average, which ends
	 * up weighing recent references more than old ones.
	 *
	 * anon in [0], file in [1]
	 */
	/* 获取当前Node当中zone的anon page 、file pages的数量的总和，这其中包含活动页面以及非活动页面 */
	anon  = lruvec_lru_size(lruvec, LRU_ACTIVE_ANON, MAX_NR_ZONES) +
		lruvec_lru_size(lruvec, LRU_INACTIVE_ANON, MAX_NR_ZONES);
	file  = lruvec_lru_size(lruvec, LRU_ACTIVE_FILE, MAX_NR_ZONES) +
		lruvec_lru_size(lruvec, LRU_INACTIVE_FILE, MAX_NR_ZONES);

	spin_lock_irq(&pgdat->lru_lock);//获取lru_lock自旋锁
	/* 如果最近匿名页面扫描次数 > 计算出的匿名页面总和的25%
	 * 则设置reclaim_stat->recent_scanned[0] /= 2;降低这个最近扫描比例，
	 * 对于reclaim_stat->recent_rotated[0] /= 2也是这样，降低旋转页面的比例
	 * 作用是什么？为什么要这样做？
	 */
	if (unlikely(reclaim_stat->recent_scanned[0] > anon / 4)) {
		reclaim_stat->recent_scanned[0] /= 2;
		reclaim_stat->recent_rotated[0] /= 2;
	}
	/* 如果最近文件页面扫描次数 > 计算出的文件页面总和的25%
	 * 则设置reclaim_stat->recent_scanned[1] /= 2;降低这个最近扫描比例，
	 * 对于reclaim_stat->recent_rotated[1] /= 2;也是这样，降低旋转页面的比例
	 * 作用是什么？为什么要做此类设置
	 */
	if (unlikely(reclaim_stat->recent_scanned[1] > file / 4)) {
		reclaim_stat->recent_scanned[1] /= 2;
		reclaim_stat->recent_rotated[1] /= 2;
	}

	/*
	 * The amount of pressure on anon vs file pages is inversely
	 * proportional to the fraction of recently scanned pages on
	 * each list that were recently referenced and in active use.
	 */
	/*
	 * 根基当前扫描的页面数，旋转的页面数计算anon_page页面回收的优先级
	 */
	ap = anon_prio * (reclaim_stat->recent_scanned[0] + 1);
	ap /= reclaim_stat->recent_rotated[0] + 1;
	/* 根基当前扫描的页面数，旋转的页面数计算file_page页面回收的优先级 */
	fp = file_prio * (reclaim_stat->recent_scanned[1] + 1);
	fp /= reclaim_stat->recent_rotated[1] + 1;
	spin_unlock_irq(&pgdat->lru_lock);
	/* 将回收的优先级赋值给fraction, 
	 * fraction[0] = ap;匿名页面的回收优先级
	 * fraction[1] = fp;文件页面的回收优先级
	 */
	fraction[0] = ap;
	fraction[1] = fp;
	denominator = ap + fp + 1;//回收优先级总和
out:
	*lru_pages = 0;
	for_each_evictable_lru(lru) {
		/* 判断当前lru链表是否为file文件表 */
		int file = is_file_lru(lru);
		unsigned long size;
		unsigned long scan;
		/* 获取当前回收的reclaim_idx ZONE到最大的MAX_NR_ZONES之间的lru的数量，也就是可以扫描的lru的个数 */
		size = lruvec_lru_size(lruvec, lru, sc->reclaim_idx);
		/* size/2^priority得出需要扫描的次数 */
		scan = size >> sc->priority;
		/*
		 * If the cgroup's already been deleted, make sure to
		 * scrape out the remaining cache.
		 */
		/* 如果scan = 0且memcg online则说明该部分该部分功能被关闭 */
		if (!scan && !mem_cgroup_online(memcg))
			scan = min(size, SWAP_CLUSTER_MAX);//此时从size，SWAP_CLUSTER_MAX最小的一个赋值给scan最为本次内存回收的次数

		switch (scan_balance) {
		case SCAN_EQUAL:
			/* Scan lists relative to size */
			break;
		case SCAN_FRACT:
			/*
			 * Scan types proportional to swappiness and
			 * their relative recent reclaim efficiency.
			 * Make sure we don't miss the last page on
			 * the offlined memory cgroups because of a
			 * round-off error.
			 */
			/*当这两种情况都存在时需要anon,file lru都需要进行内存回收时需要计算出scan扫描的次数 */
			scan = mem_cgroup_online(memcg) ?
			       div64_u64(scan * fraction[file], denominator) :
			       DIV64_U64_ROUND_UP(scan * fraction[file],
						  denominator);
			break;
		case SCAN_FILE:
		case SCAN_ANON:
			/* Scan one type exclusively */
			if ((scan_balance == SCAN_FILE) != file) {
				size = 0;
				scan = 0;
			}
			break;
		default:
			/* Look ma, no brain */
			BUG();
		}

		*lru_pages += size;
		nr[lru] = scan;
	}
}

/*
 * This is a basic per-node page freer.  Used by both kswapd and direct reclaim.
 */
static void shrink_node_memcg(struct pglist_data *pgdat, struct mem_cgroup *memcg,
			      struct scan_control *sc, unsigned long *lru_pages)
{
	struct lruvec *lruvec = mem_cgroup_lruvec(pgdat, memcg);
	unsigned long nr[NR_LRU_LISTS];
	unsigned long targets[NR_LRU_LISTS];
	unsigned long nr_to_scan;
	enum lru_list lru;
	unsigned long nr_reclaimed = 0;
    /* 需要回收的页框数量 */
	unsigned long nr_to_reclaim = sc->nr_to_reclaim;
	struct blk_plug plug;
	bool scan_adjusted;

    /* 对这个lru链表描述符中的每个lru链表，计算它们本次扫描应该扫描的页框数量 
     * 计算好的每个lru链表需要扫描的页框数量保存在nr中，扫描能回收的pages数保存在lru_pages当中
     * 每个lru链表需要扫描多少与sc->priority有关，sc->priority越小，那么扫描得越多
     */
	get_scan_count(lruvec, memcg, sc, nr, lru_pages);

	/* Record the original scan target for proportional adjustments later */
    /* 将nr的数据复制到targets中 */
	memcpy(targets, nr, sizeof(nr));

	/*
	 * Global reclaiming within direct reclaim at DEF_PRIORITY is a normal
	 * event that can occur when there is little memory pressure e.g.
	 * multiple streaming readers/writers. Hence, we do not abort scanning
	 * when the requested number of pages are reclaimed when scanning at
	 * DEF_PRIORITY on the assumption that the fact we are direct
	 * reclaiming implies that kswapd is not keeping up and it is best to
	 * do a batch of work at once. For memcg reclaim one check is made to
	 * abort proportional reclaim if either the file or anon lru has already
	 * dropped to zero at the first pass.
	 */
	/* 是否将nr[]中的数量页数都扫描完才停止
     * 如果是针对全局整个zone进行扫描，并且不是在kswapd内核线程中调用的，优先级为默认优先级，就会无视需要回收的页框数量，
     * 只有将nr[]中的数量页数都扫描完才停止
     * 快速回收不会这样做(快速回收的优先级不是DEF_PRIORITY)
     */
	scan_adjusted = (global_reclaim(sc) && !current_is_kswapd() &&
			 sc->priority == DEF_PRIORITY);
    /* 初始化这个struct blk_plug
     * 主要初始化list，mq_list，cb_list这三个链表头
     * 然后current->plug = plug
     */
	blk_start_plug(&plug);
    /* 如果LRU_INACTIVE_ANON，LRU_ACTIVE_FILE，LRU_INACTIVE_FILE这三个其中一个需要扫描的页框数没有扫描完，那扫描就会继续 
     * 注意这里不会判断LRU_ACTIVE_ANON需要扫描的页框数是否扫描完，这里原因大概是因为系统不太希望对匿名页lru链表中的页回收
     */
	while (nr[LRU_INACTIVE_ANON] || nr[LRU_ACTIVE_FILE] ||
					nr[LRU_INACTIVE_FILE]) {
		unsigned long nr_anon, nr_file, percentage;
		unsigned long nr_scanned;
        
        /* 以LRU_INACTIVE_ANON，LRU_ACTIVE_ANON，LRU_INACTIVE_FILE，LRU_ACTIVE_FILE这个顺序遍历lru链表 
         * 然后对遍历到的lru链表进行扫描，一次最多32个页框
         */
		for_each_evictable_lru(lru) {
            /* nr[lru类型]如果有页框需要扫描 */
			if (nr[lru]) {
                /* 获取本次需要扫描的页框数量，nr[lru]与SWAP_CLUSTER_MAX的最小值 
                 * 也就是每一轮最多只扫描SWAP_CLUSTER_MAX(32)个页框
                 */
				nr_to_scan = min(nr[lru], SWAP_CLUSTER_MAX);
                /* nr[lru类型]减掉本次需要扫描的页框数量 */
				nr[lru] -= nr_to_scan;
                /* 对此lru类型的lru链表进行内存回收 
                 * 一次扫描的页框数是nr[lru]与SWAP_CLUSTER_MAX的最小值，也就是如果全部能回收，一次也就只能回收SWAP_CLUSTER_MAX(32)个页框
                 * 都是从lru链表末尾向前扫描
                 * 本次回收的页框数保存在nr_reclaimed中
                 */
				nr_reclaimed += shrink_list(lru, nr_to_scan,
							    lruvec, sc);
			}
		}

		cond_resched();
        /* 没有回收到足够页框，或者需要忽略需要回收的页框数量，尽可能多的回收页框，则继续进行回收
         * 当scan_adjusted为真时，扫描到nr[三个类型]数组中的数都为0为止，会忽略是否回收到足够页框，即使回收到足够页框也继续进行扫描
         * 也就是尽可能的回收页框，越多越好，alloc_pages()会是这种情况
         */
		if (nr_reclaimed < nr_to_reclaim || scan_adjusted)
			continue;

		/*
		 * For kswapd and memcg, reclaim at least the number of pages
		 * requested. Ensure that the anon and file LRUs are scanned
		 * proportionally what was requested by get_scan_count(). We
		 * stop reclaiming one LRU and reduce the amount scanning
		 * proportional to the original scan target.
		 */
		 /* kswapd和针对某个memcg进行回收的情况中会调用到此，已经回收到了足够数量的页框，调用到此是用于判断是否还要继续扫描，因为已经回收到了足够页框了 */
         /* 扫描一遍后，剩余需要扫描的文件页数量和匿名页数量 */
		nr_file = nr[LRU_INACTIVE_FILE] + nr[LRU_ACTIVE_FILE];
		nr_anon = nr[LRU_INACTIVE_ANON] + nr[LRU_ACTIVE_ANON];

		/*
		 * It's just vindictive to attack the larger once the smaller
		 * has gone to zero.  And given the way we stop scanning the
		 * smaller below, this makes sure that we only make one nudge
		 * towards proportionality once we've got nr_to_reclaim.
		 */
		/* 已经扫描完成了，退出循环 */
		if (!nr_file || !nr_anon)
			break;
        
        /* 下面就是计算再扫描多少页框，会对nr[]中的数进行相应的减少 
         * 调用到这里肯定是kswapd进程或者针对memcg的页框回收，并且已经回收到了足够的页框了
         * 如果nr[]中还剩余很多数量的页框没有扫描，这里就通过计算，减少一些nr[]待扫描的数量
         * 设置scan_adjusted，之后把nr[]中剩余的数量扫描完成
         */
		if (nr_file > nr_anon) {
             /* 剩余需要扫描的文件页多于剩余需要扫描的匿名页时 */
            /* 原始的需要扫描匿名页数量 */
			unsigned long scan_target = targets[LRU_INACTIVE_ANON] +
						targets[LRU_ACTIVE_ANON] + 1;
			lru = LRU_BASE;
             /* 计算剩余的需要扫描的匿名页数量占 */
			percentage = nr_anon * 100 / scan_target;
		} else {
            /* 剩余需要扫描的文件页少于剩余需要扫描的匿名页时 */
			unsigned long scan_target = targets[LRU_INACTIVE_FILE] +
						targets[LRU_ACTIVE_FILE] + 1;
			lru = LRU_FILE;
			percentage = nr_file * 100 / scan_target;
		}

		/* Stop scanning the smaller of the LRU */
		nr[lru] = 0;
		nr[lru + LRU_ACTIVE] = 0;

		/*
		 * Recalculate the other LRU scan count based on its original
		 * scan target and the percentage scanning already complete
		 */
		lru = (lru == LRU_FILE) ? LRU_BASE : LRU_FILE;
		nr_scanned = targets[lru] - nr[lru];
		nr[lru] = targets[lru] * (100 - percentage) / 100;
		nr[lru] -= min(nr[lru], nr_scanned);

		lru += LRU_ACTIVE;
		nr_scanned = targets[lru] - nr[lru];
		nr[lru] = targets[lru] * (100 - percentage) / 100;
		nr[lru] -= min(nr[lru], nr_scanned);

		scan_adjusted = true;
	}
	blk_finish_plug(&plug);
     /* 总共回收的页框数量 */
	sc->nr_reclaimed += nr_reclaimed;

	/*
	 * Even if we did not try to evict anon pages at all, we want to
	 * rebalance the anon lru active/inactive ratio.
	 */
	/* 非活动匿名页lru链表中页数量太少 */
    /* 从活动匿名页lru链表中移动一些页去非活动匿名页lru链表，最多32个 */
	if (inactive_list_is_low(lruvec, false, sc, true))
		shrink_active_list(SWAP_CLUSTER_MAX, lruvec,
				   sc, LRU_ACTIVE_ANON);
}

/* Use reclaim/compaction for costly allocs or under memory pressure */
static bool in_reclaim_compaction(struct scan_control *sc)
{
	if (IS_ENABLED(CONFIG_COMPACTION) && sc->order &&
			(sc->order > PAGE_ALLOC_COSTLY_ORDER ||
			 sc->priority < DEF_PRIORITY - 2))
		return true;

	return false;
}

/*
 * Reclaim/compaction is used for high-order allocation requests. It reclaims
 * order-0 pages before compacting the zone. should_continue_reclaim() returns
 * true if more pages should be reclaimed such that when the page allocator
 * calls try_to_compact_zone() that it will have enough free pages to succeed.
 * It will give up earlier than that if there is difficulty reclaiming pages.
 */
static inline bool should_continue_reclaim(struct pglist_data *pgdat,
					unsigned long nr_reclaimed,
					unsigned long nr_scanned,
					struct scan_control *sc)
{
	unsigned long pages_for_compaction;
	unsigned long inactive_lru_pages;
	int z;

	/* If not in reclaim/compaction mode, stop */
    /* 如果sc当中没有设置内存回收或者是内存碎片整理模式则直接推出 */
	if (!in_reclaim_compaction(sc))
		return false;

	/* Consider stopping depending on scan and reclaim activity */
    /* 如果sc设置了__GFP_RETRY_MAYFAIL则进入一下判断当中
     * 1. 如果nr_reclaimed nr_scanned都为空说明扫描的次数，与回收的page都为0则直接return false
     * 2. 如果__GFP_RETRY_MAYFAIL没有使用该flag则如果回收的页面数为0则直接return false
     */
	if (sc->gfp_mask & __GFP_RETRY_MAYFAIL) {
		/*
		 * For __GFP_RETRY_MAYFAIL allocations, stop reclaiming if the
		 * full LRU list has been scanned and we are still failing
		 * to reclaim pages. This full LRU scan is potentially
		 * expensive but a __GFP_RETRY_MAYFAIL caller really wants to succeed
		 */
		if (!nr_reclaimed && !nr_scanned)
			return false;
	} else {
		/*
		 * For non-__GFP_RETRY_MAYFAIL allocations which can presumably
		 * fail without consequence, stop if we failed to reclaim
		 * any pages from the last SWAP_CLUSTER_MAX number of
		 * pages that were scanned. This will return to the
		 * caller faster at the risk reclaim/compaction and
		 * the resulting allocation attempt fails
		 */
		if (!nr_reclaimed)
			return false;
	}

	/*
	 * If we have not reclaimed enough pages for compaction and the
	 * inactive lists are large enough, continue reclaiming
	 */
	/* 根据order计算出内存回收两倍的页面数也就是2*1^order */
	pages_for_compaction = compact_gap(sc->order);
    /* 获取当前zone中非活动file类型页面的个数 */
	inactive_lru_pages = node_page_state(pgdat, NR_INACTIVE_FILE);
    /* 如果swap交换的pages页面大于0，则inactive_lru_pages加上当前zone的非活动ANON匿名页面的个数，这样就获取到了zone 所有的非活动页面数，也就是能回收的最大页面数 */
	if (get_nr_swap_pages() > 0)
		inactive_lru_pages += node_page_state(pgdat, NR_INACTIVE_ANON);
    /* 如果想要回收的页数pages小于内存整理回收的order请求两倍的页面，
     * 且非活动lru页面数 >      2倍的内存分配请求页面数 则直接return true  继续进行内存回收操作，目的在于想要回收更多的page
     */
	if (sc->nr_reclaimed < pages_for_compaction &&
			inactive_lru_pages > pages_for_compaction)
		return true;

	/* If compaction would go ahead or the allocation would succeed, stop */
    /* 遍历该zone区域，直到reclaim_idx为止 */
	for (z = 0; z <= sc->reclaim_idx; z++) {
		struct zone *zone = &pgdat->node_zones[z];
		if (!managed_zone(zone))//如果该zone区域不在buddy system管理区域当中则直接continue跳过zone区域
			continue;
      
        /*
         * 如果碎片整理成功，则说明由足够的freetype空闲页面可供内存分配请求使用，直接return false 退出内存回收，直接分配内存即可
         * 如果内存碎片整理继续则说明该zone当中没有足够的freepages空闲页面供内存分配使用，在该zone区域做内存回收是没有意义的，无法获取到足够的页面供内存分配使用
         * 所以直接return false不再做内存回收操作。
         * 所以针对这两种情况直接结束内存回收操作即可。
         */
		switch (compaction_suitable(zone, sc->order, 0, sc->reclaim_idx)) {
		case COMPACT_SUCCESS:
		case COMPACT_CONTINUE:
			return false;
		default:
			/* check next zone */
			;
		}
	}
    /* 遍历的所有区域中压缩均未成功，则返回true表示压缩应继续 */
	return true;
}
                    
/* 对zone进行内存回收 
* 返回是否回收到了页框，而不是是否回收到了sc中指定数量的页框
* 即使没回收到sc中指定数量的页框，只要回收到了页框，就返回真
*/
static bool shrink_node(pg_data_t *pgdat, struct scan_control *sc)
{
	struct reclaim_state *reclaim_state = current->reclaim_state;
	unsigned long nr_reclaimed, nr_scanned;
	bool reclaimable = false;

	do {
         /* 当内存回收是针对整个zone时，sc->target_mem_cgroup为NULL */
		struct mem_cgroup *root = sc->target_mem_cgroup;
		struct mem_cgroup_reclaim_cookie reclaim = {
			.pgdat = pgdat,
			.priority = sc->priority,
		};
		unsigned long node_lru_pages = 0;
		struct mem_cgroup *memcg;
        /* 记录本次回收开始前回收到的页框数量，第一次是0 */
		nr_reclaimed = sc->nr_reclaimed;
        /* 记录本次内存回收前扫描的页框的数量 */
		nr_scanned = sc->nr_scanned;
        /* 获取最上层的memcg
         * 如果没有指定开始的root，则默认是root_mem_cgroup
         * root_mem_cgroup管理的每个zone的lru链表就是每个zone完整的lru链表
         */
		memcg = mem_cgroup_iter(root, NULL, &reclaim);
		do {
			unsigned long lru_pages;
			unsigned long reclaimed;
			unsigned long scanned;
            /* 如果memcg->memory < memcg->low,则进入判断当中  */
			if (mem_cgroup_low(root, memcg)) {
                /* 如果memcg_low_reclaim不为真则设置memcg_low_skipped = 1 continue跳过该memcg
                 * memcg是什么？作用是什么？如何使用？
                 */
				if (!sc->memcg_low_reclaim) {
					sc->memcg_low_skipped = 1;
					continue;
				}
                /* 设置memcg状态为MEMCG_LOW */
				mem_cgroup_event(memcg, MEMCG_LOW);
			}
            /* 记录本次回收开始前回收到的页框数量 
             * 第一次时是0
             */
			reclaimed = sc->nr_reclaimed;
            /* 记录本次回收开始前扫描过的页框数量
             * 第一次时是0
             */
			scanned = sc->nr_scanned;
            /* 对此memcg的lru链表进行回收工作 
             * 此lru链表中的所有页都是属于此zone的
             * 每个memcg中都会为每个zone维护一个lru链表
             */
			shrink_node_memcg(pgdat, memcg, sc, &lru_pages);
            /* node_lru_pages += lru_pages获取到本次扫描能够回收的page数量 */
			node_lru_pages += lru_pages;

			if (memcg)
				shrink_slab(sc->gfp_mask, pgdat->node_id,
					    memcg, sc->nr_scanned - scanned,
					    lru_pages);

			/* Record the group's reclaim efficiency */
			vmpressure(sc->gfp_mask, memcg, false,
				   sc->nr_scanned - scanned,
				   sc->nr_reclaimed - reclaimed);

			/*
			 * Direct reclaim and kswapd have to scan all memory
			 * cgroups to fulfill the overall scan target for the
			 * node.
			 *
			 * Limit reclaim, on the other hand, only cares about
			 * nr_to_reclaim pages to be reclaimed and it will
			 * retry with decreasing priority if one round over the
			 * whole hierarchy is not sufficient.
			 */
			if (!global_reclaim(sc) &&
					sc->nr_reclaimed >= sc->nr_to_reclaim) {
				mem_cgroup_iter_break(root, memcg);
				break;
			}
		} while ((memcg = mem_cgroup_iter(root, memcg, &reclaim)));

		/*
		 * Shrink the slab caches in the same proportion that
		 * the eligible LRU pages were scanned.
		 */
		 /* 如果是对于整个zone进行回收，那么会遍历所有memcg，对所有memcg中此zone的lru链表进行回收 
          * 而如果只是针对某个memcg进行回收，如果回收到了足够内存则返回，如果没回收到足够内存，则对此memcg下面的memcg进行回收
          */
		if (global_reclaim(sc))
			shrink_slab(sc->gfp_mask, pgdat->node_id, NULL,
				    sc->nr_scanned - nr_scanned,
				    node_lru_pages);

		if (reclaim_state) {
			sc->nr_reclaimed += reclaim_state->reclaimed_slab;
			reclaim_state->reclaimed_slab = 0;
		}

		/* Record the subtree's reclaim efficiency */
        /* 计算此memcg的内存压力，保存到memcg->vmpressure */
		vmpressure(sc->gfp_mask, sc->target_mem_cgroup, true,
			   sc->nr_scanned - nr_scanned,
			   sc->nr_reclaimed - nr_reclaimed);

		if (sc->nr_reclaimed - nr_reclaimed)
			reclaimable = true;
    /* 判断是否再次此zone进行内存回收 
     * 继续对此zone进行内存回收有两种情况:
     * 1. 没有回收到比目标order值多一倍的数量页框，并且非活动lru链表中的页框数量 > 目标order多一倍的页
     * 2. 此zone不满足内存压缩的条件，则继续对此zone进行内存回收
     * 而当本次内存回收完全没有回收到页框时则返回，这里大概意思就是想回收比order更多的页框
     */
	} while (should_continue_reclaim(pgdat, sc->nr_reclaimed - nr_reclaimed,
					 sc->nr_scanned - nr_scanned, sc));

	/*
	 * Kswapd gives up on balancing particular nodes after too
	 * many failures to reclaim anything from them and goes to
	 * sleep. On reclaim progress, reset the failure counter. A
	 * successful direct reclaim run will revive a dormant kswapd.
	 */
	if (reclaimable)
		pgdat->kswapd_failures = 0;

	return reclaimable;
}

/*
 * Returns true if compaction should go ahead for a costly-order request, or
 * the allocation would already succeed without compaction. Return false if we
 * should reclaim first.
 */
static inline bool compaction_ready(struct zone *zone, struct scan_control *sc)
{
	unsigned long watermark;
	enum compact_result suitable;

	suitable = compaction_suitable(zone, sc->order, 0, sc->reclaim_idx);
	if (suitable == COMPACT_SUCCESS)
		/* Allocation should succeed already. Don't reclaim. */
		return true;
	if (suitable == COMPACT_SKIPPED)
		/* Compaction cannot yet proceed. Do reclaim. */
		return false;

	/*
	 * Compaction is already possible, but it takes time to run and there
	 * are potentially other callers using the pages just freed. So proceed
	 * with reclaim to make a buffer of free pages available to give
	 * compaction a reasonable chance of completing and allocating the page.
	 * Note that we won't actually reclaim the whole buffer in one attempt
	 * as the target watermark in should_continue_reclaim() is lower. But if
	 * we are already above the high+gap watermark, don't reclaim at all.
	 */
	watermark = high_wmark_pages(zone) + compact_gap(sc->order);

	return zone_watermark_ok_safe(zone, 0, watermark, sc->reclaim_idx);
}

/*
 * This is the direct reclaim path, for page-allocating processes.  We only
 * try to reclaim pages from zones which will satisfy the caller's allocation
 * request.
 *
 * If a zone is deemed to be full of pinned pages then just give it a light
 * scan then give up on it.
 */
static void shrink_zones(struct zonelist *zonelist, struct scan_control *sc)
{
	struct zoneref *z;
	struct zone *zone;
	unsigned long nr_soft_reclaimed;
	unsigned long nr_soft_scanned;
	gfp_t orig_mask;
	pg_data_t *last_pgdat = NULL;

	/*
	 * If the number of buffer_heads in the machine exceeds the maximum
	 * allowed level, force direct reclaim to scan the highmem zone as
	 * highmem pages could be pinning lowmem pages storing buffer_heads
	 */
	 /* 将sc->gfp_mask赋值给orig_mask，这里的作用就相当于做了一个备份一样 */
	orig_mask = sc->gfp_mask;
    /* 如果缓冲头数超过了最大允许水平，则在page页扫描中应该包含高端内存区域 */
	if (buffer_heads_over_limit) {
		sc->gfp_mask |= __GFP_HIGHMEM;
		sc->reclaim_idx = gfp_zone(sc->gfp_mask);
	}
    /* 遍历zonelist，回收可用的内存空间 */
	for_each_zone_zonelist_nodemask(zone, z, zonelist,
					sc->reclaim_idx, sc->nodemask) {
		/*
		 * Take care memory controller reclaiming has small influence
		 * to global LRU.
		 */
		 /* 判断检索的对象是否为全局lru，如果是则进入判断当中 */
		if (global_reclaim(sc)) {
            /* 如果zone区域中，由于GFP_KERNEL和__GFP_HARDWALL标志的设置导致在内存回收时不允许使用该zone区域，则continue跳过该zone区域 */
			if (!cpuset_zone_allowed(zone,
						 GFP_KERNEL | __GFP_HARDWALL))
				continue;

			/*
			 * If we already have plenty of memory free for
			 * compaction in this zone, don't free any more.
			 * Even though compaction is invoked for any
			 * non-zero order, only frequent costly order
			 * reclamation is disruptive enough to become a
			 * noticeable problem, like transparent huge
			 * page allocations.
			 */
			/* 同时满足三个条件则跳过该zone区域：
			 * 1. CONFIG_COMPACTION内存碎片整理功能开启
			 * 2. 需要分配的内存order > PAGE_ALLOC_COSTLY_ORDER也就是昂贵的order = 3
			 * 3. compaction_ready(zone, sc)说明该zone区域可以通过comact内存碎片整理，实现内存分配的，
			 * 基于着三点,如果同时满足则不在对该zone进行内存回收操作，直接进行内存碎片整理即可。
			 */
			if (IS_ENABLED(CONFIG_COMPACTION) &&
			    sc->order > PAGE_ALLOC_COSTLY_ORDER &&
			    compaction_ready(zone, sc)) {
				sc->compaction_ready = true;
				continue;
			}

			/*
			 * Shrink each node in the zonelist once. If the
			 * zonelist is ordered by zone (not the default) then a
			 * node may be shrunk multiple times but in that case
			 * the user prefers lower zones being preserved.
			 */
			/* 如果zone->zone_pgdat = last_pgdat说明该zone已经进行过内存回收操作了，这里直接continue跳过该zone */
			if (zone->zone_pgdat == last_pgdat)
				continue;

			/*
			 * This steals pages from memory cgroups over softlimit
			 * and returns the number of reclaimed pages and
			 * scanned pages. This works for global memory pressure
			 * and balancing, not for a memcg's limit.
			 */
			/*
			 * 获取zone回收能够获得的page个数，
			 * 并将zone内存回收获得pages数目给nr_reclaimed + nr_soft_reclaimed回收page数，
			 * nr_scanned + nr_soft_scanned获得扫描的次数
             */
			nr_soft_scanned = 0;
			nr_soft_reclaimed = mem_cgroup_soft_limit_reclaim(zone->zone_pgdat,
						sc->order, sc->gfp_mask,
						&nr_soft_scanned);
			sc->nr_reclaimed += nr_soft_reclaimed;
			sc->nr_scanned += nr_soft_scanned;
			/* need some check for avoid more shrink_zone() */
		}

		/* See comment about same check for global reclaim above */
        /* 如果zone->zone_pgdat = last_pgdat说明该zone已经进行过内存回收操作了，这里直接continue跳过该zone */
		if (zone->zone_pgdat == last_pgdat)
			continue;
		last_pgdat = zone->zone_pgdat;
        /* 对该zone进行下一步的内存回收操作 */
		shrink_node(zone->zone_pgdat, sc);
	}

	/*
	 * Restore to original mask to avoid the impact on the caller if we
	 * promoted it to __GFP_HIGHMEM.
	 */
	/*将备份的orig_mask重新赋值给sc->gfp_mask完成备份的目的 */
	sc->gfp_mask = orig_mask;
}

static void snapshot_refaults(struct mem_cgroup *root_memcg, pg_data_t *pgdat)
{
	struct mem_cgroup *memcg;

	memcg = mem_cgroup_iter(root_memcg, NULL, NULL);
	do {
		unsigned long refaults;
		struct lruvec *lruvec;

		lruvec = mem_cgroup_lruvec(pgdat, memcg);
		refaults = lruvec_page_state(lruvec, WORKINGSET_ACTIVATE);
		lruvec->refaults = refaults;
	} while ((memcg = mem_cgroup_iter(root_memcg, memcg, NULL)));
}

/*
 * This is the main entry point to direct page reclaim.
 *
 * If a full scan of the inactive list fails to free enough memory then we
 * are "out of memory" and something needs to be killed.
 *
 * If the caller is !__GFP_FS then the probability of a failure is reasonably
 * high - the zone may be full of dirty or under-writeback pages, which this
 * caller can't do much about.  We kick the writeback threads and take explicit
 * naps in the hope that some of these pages can be written.  But if the
 * allocating task holds filesystem locks which prevent writeout this might not
 * work, and the allocation attempt will fail.
 *
 * returns:	0, if no pages reclaimed
 * 		else, the number of pages reclaimed
 */
static unsigned long do_try_to_free_pages(struct zonelist *zonelist,
					  struct scan_control *sc)
{
	int initial_priority = sc->priority;
	pg_data_t *last_pgdat;
	struct zoneref *z;
	struct zone *zone;
retry:
	delayacct_freepages_start();//计算开始内存回收时的时间与delayacct_freepages_end相呼应
    /* 是否使用全局回收变量，如果是则增加ALLOCSTALL stat */
	if (global_reclaim(sc))
		__count_zid_vm_events(ALLOCSTALL, sc->reclaim_idx, 1);

	do {
        /*
         * 完成内存回收压力等级的计算，通过不同的压力等级判断后续内存回收的方式
         * low level正常回收，medium level就是开始swaping，critical就是内存严重不足启动OOM释放内存
         */
		vmpressure_prio(sc->gfp_mask, sc->target_mem_cgroup,
				sc->priority);
        /* 重置扫描次数 */
		sc->nr_scanned = 0;
        /* 回收页面，更新sc结构体参数 */
		shrink_zones(zonelist, sc);
        /* 如果扫描回收的空闲页数大于等于扫描的非活动页面则直接break，说明该zone当中的非活动页面都已经被成功回收
         * 如果扫描回收的空闲页面数小于非活动页面数则继续向下操作
         */
		if (sc->nr_reclaimed >= sc->nr_to_reclaim)
			break;
        /* 已准备好压缩，则它会跳出循环进行处理，这里内存回收时无法操作的，需要先进行内存compaction压缩操作 */
		if (sc->compaction_ready)
			break;

		/*
		 * If we're getting trouble reclaiming, start doing
		 * writepage even in laptop mode.
		 */
		/*
		 * 将优先级再提高两个步骤来设置处理时的写页功能
         */
		if (sc->priority < DEF_PRIORITY - 2)
			sc->may_writepage = 1;
	} while (--sc->priority >= 0);//将优先级提高到最高为0
   
	last_pgdat = NULL;
     /* 更新该zone节点或该节点的memcg lru的引用 */
	for_each_zone_zonelist_nodemask(zone, z, zonelist, sc->reclaim_idx,
					sc->nodemask) {
		if (zone->zone_pgdat == last_pgdat)
			continue;
		last_pgdat = zone->zone_pgdat;
		snapshot_refaults(sc->target_mem_cgroup, zone->zone_pgdat);
	}
    /* 获取页面回收后的时间 */
	delayacct_freepages_end();
    /* 内存回收的空闲页面的数量，如果为真则直接return nr_reclaimed 返回回收成功的pages页的数量                         */
	if (sc->nr_reclaimed)
		return sc->nr_reclaimed;

	/* Aborted reclaim to try compaction? don't OOM, then */
    /* 内存回收终止，尝试进行内存compaction碎片整理，不要触发oom ，return 1 */
	if (sc->compaction_ready)
		return 1;

	/* Untapped cgroup reserves?  Don't OOM, retry. */
    /* 设置了sc-> memcg_low_skipped，则仅在第一次重试时，优先级才变回原始请求优先级 */
	if (sc->memcg_low_skipped) {
		sc->priority = initial_priority;
		sc->memcg_low_reclaim = 1;
		sc->memcg_low_skipped = 0;
		goto retry;
	}

	return 0;
}

static bool allow_direct_reclaim(pg_data_t *pgdat)
{
	struct zone *zone;
	unsigned long pfmemalloc_reserve = 0;
	unsigned long free_pages = 0;
	int i;
	bool wmark_ok;
    /* 如果内存回收失败此时超过MAX_RECLAIM_RETRIES也就是16次时，则直接return true,不在进行内存的回收操作
     * 这里可以看作时一种保护机制，防止对同一个zone进行无限的回收
     */
	if (pgdat->kswapd_failures >= MAX_RECLAIM_RETRIES)
		return true;
    /* 查询低于ZONE_NORMAL的zone类型的地址，这里我们时ARM64为架构只有一个Node0，所以按照这样只能从Node0当中获取内存 */
	for (i = 0; i <= ZONE_NORMAL; i++) {
        /* 获取当前node节点当中的zone，根据ARM64架构分析只有NODE0，也就是获取node0当中的ZONE_DMA、ZONE_NORMAL区域 */
		zone = &pgdat->node_zones[i];
		if (!managed_zone(zone))//判断当前zone是否在buddy system当中，如果不在则continue，跳过该zone，如果存在则继续向下执行
			continue;
        /* 判断zone当中可回收的page是否为NULL，如果为空则说明该zone当中不存在可以回收的page，则直接continue跳过该zone,扫描下一个zone */
		if (!zone_reclaimable_pages(zone))
			continue;
        /*
         * pfmemalloc_reserve = pfmemalloc_reserve + z->watermark[WMARK_MIN]获取ZONE_DMA、ZONE_NORMAL的保留的最小的page数
         * free_pages 则是获取zone当中可用的NR_FREE_PAGES空闲页，获取ZONE_DMA、ZONE_NORMAL的空闲的page
         */
		pfmemalloc_reserve += min_wmark_pages(zone);
		free_pages += zone_page_state(zone, NR_FREE_PAGES);
	}

	/* If there are no reserves (unexpected config) then do not throttle */
    /* 如果pfmemalloc_reserve =   0则return true说明该zone当中没有保留的空闲页则不再进行内存内存回收                       */
	if (!pfmemalloc_reserve)
		return true;
    /* 如果空闲页面的总数大于lowmem区域的总最小水印值的50％，则wmark_ok = true，说明存在足够的freepage可供分配，无需做内存回收操作
     * 如果空闲页面的总述小于lowmem区域的最小水印的50% 则wmark_ok = false，说明是需要对该zone或者是node进行内存回收的
     */
	wmark_ok = free_pages > pfmemalloc_reserve / 2;

	/* kswapd must be awake if processes are being throttled */
    /* 如果lowmem区域的可用freepage空闲页面小于最小水印的50％，则当前任务进入睡眠状态，唤醒kswapd，进行异步回收操作。
     */
	if (!wmark_ok && waitqueue_active(&pgdat->kswapd_wait)) {
		pgdat->kswapd_classzone_idx = min(pgdat->kswapd_classzone_idx,
						(enum zone_type)ZONE_NORMAL);
		wake_up_interruptible(&pgdat->kswapd_wait);
	}

	return wmark_ok;
}

/*
 * Throttle direct reclaimers if backing storage is backed by the network
 * and the PFMEMALLOC reserve for the preferred node is getting dangerously
 * depleted. kswapd will continue to make progress and wake the processes
 * when the low watermark is reached.
 *
 * Returns true if a fatal signal was delivered during throttling. If this
 * happens, the page allocator should not consider triggering the OOM killer.
 */
static bool throttle_direct_reclaim(gfp_t gfp_mask, struct zonelist *zonelist,
					nodemask_t *nodemask)
{
	struct zoneref *z;
	struct zone *zone;
	pg_data_t *pgdat = NULL;

	/*
	 * Kernel threads should not be throttled as they may be indirectly
	 * responsible for cleaning pages necessary for reclaim to make forward
	 * progress. kjournald for example may enter direct reclaim while
	 * committing a transaction where throttling it could forcing other
	 * processes to block on log_wait_commit().
	 */
	/* 如果是内核线程则直接跳转到out，不做内存回收操作 */
	if (current->flags & PF_KTHREAD)
		goto out;

	/*
	 * If a fatal signal is pending, this process should not throttle.
	 * It should return quickly so it can exit and free its memory
	 */
	/* 如果当前current进程在SIGKILL信号的任务的情况下，则直接return false */
	if (fatal_signal_pending(current))
		goto out;

	/*
	 * Check if the pfmemalloc reserves are ok by finding the first node
	 * with a usable ZONE_NORMAL or lower zone. The expectation is that
	 * GFP_KERNEL will be required for allocating network buffers when
	 * swapping over the network so ZONE_HIGHMEM is unusable.
	 *
	 * Throttling is based on the first usable node and throttled processes
	 * wait on a queue until kswapd makes progress and wakes them. There
	 * is an affinity then between processes waking up and where reclaim
	 * progress has been made assuming the process wakes on the same node.
	 * More importantly, processes running on remote nodes will not compete
	 * for remote pfmemalloc reserves and processes on different nodes
	 * should make reasonable progress.
	 */
	/* 扫描zonelist，如果zone > ZONE_NORMAL则直接continue 跳过该zone
	 * 对于ARM64而言能够扫描的只有ZONE_DMA、ZONE_NORMAL
     */
	for_each_zone_zonelist_nodemask(zone, z, zonelist,
					gfp_zone(gfp_mask), nodemask) {
		/*在arm64当中只能扫描ZONE_NORMAL、ZONE_MOVABLE */
		if (zone_idx(zone) > ZONE_NORMAL)
			continue;

		/* Throttle based on the first usable node */
        /* 获取当前zone类型的pgdat变量信息该变量当中包含node信息，判断该zone是否允许直接回收操作 */
		pgdat = zone->zone_pgdat;
		if (allow_direct_reclaim(pgdat))
			goto out;
		break;
	}

	/* If no zone was usable by the allocation flags then do not throttle */
    /* 如果没有可用的zone则直接跳转到out处，说明zone无法做内存回收操作，直接结束操作       */
	if (!pgdat)
		goto out;

	/* Account for the throttling */
	count_vm_event(PGSCAN_DIRECT_THROTTLE);

	/*
	 * If the caller cannot enter the filesystem, it's possible that it
	 * is due to the caller holding an FS lock or performing a journal
	 * transaction in the case of a filesystem like ext[3|4]. In this case,
	 * it is not safe to block on pfmemalloc_wait as kswapd could be
	 * blocked waiting on the same lock. Instead, throttle for up to a
	 * second before continuing.
	 */
	/* 如果gfp_mask不使用文件系统则可以进行直接回收请求，
	 * 该操作最对可以执行HZ也就是1s,无论是否结束都，内存直接回收判断都将结束，且移至check_pending标签
	 * 此处相当于引入了一个timeout机制，方式内存回收循环造成系统卡死
	 */
	if (!(gfp_mask & __GFP_FS)) {
		wait_event_interruptible_timeout(pgdat->pfmemalloc_wait,
			allow_direct_reclaim(pgdat), HZ);

		goto check_pending;
	}

	/* Throttle until kswapd wakes the process */
    /* 如果使用了文件系统进行直接回收的情况下，它会唤醒kswapd以获得空闲页，然后休眠直到允许直接回收 */
	wait_event_killable(zone->zone_pgdat->pfmemalloc_wait,
		allow_direct_reclaim(pgdat));

check_pending:
	if (fatal_signal_pending(current))//如果向当前进程请求了SIGKILL信号，则返回true则说明无法进行内存回收，否则返回false可以进行内存回收
		return true;

out:
	return false;
}

unsigned long try_to_free_pages(struct zonelist *zonelist, int order,
				gfp_t gfp_mask, nodemask_t *nodemask)
{
	unsigned long nr_reclaimed;
	struct scan_control sc = {
        /* 最大回收页框数：32个页框       */
		.nr_to_reclaim = SWAP_CLUSTER_MAX,
		/* 获取当前进程分配内存的设置 */
		.gfp_mask = current_gfp_context(gfp_mask),
		/* 获取当前zone的类型，是ZONE_NORMAL;ZONE_DMA;ZONE_MOVABLE等类型是从真没的zone当中获取的内存 */
		.reclaim_idx = gfp_zone(gfp_mask),
		/* 申请内存的order，通过order可知当前申请内存的大小2^order */
		.order = order,
		/* 获取当前node的类型 */
		.nodemask = nodemask,
		/* 优先级为默认的12 */
		.priority = DEF_PRIORITY,
		/* 与/proc/sys/vm/laptop_mode文件有关
         * laptop_mode为0，则允许进行回写操作，即使允许回写，直接内存回收也不能对脏文件页进行回写
         * 不过允许回写时，可以对非文件页进行回写
         */
		.may_writepage = !laptop_mode,
		 /* 能否进行unmap操作，就是将所有映射了此页的页表项清空 */
		.may_unmap = 1,
		 /* 是否能够进行swap交换，如果不能，在内存回收时则不扫描匿名页lru链表 */
		.may_swap = 1,
	};

	/*
	 * Do not enter reclaim if fatal signal was delivered while throttled.
	 * 1 is returned so that the page allocator does not OOM kill at this
	 * point.
	 */
	/*
	 * 判断当前zonelist当中是否存在可以做内存回收的zone区域，如果存在则直接return true，则zone无需做内存回收直接return 1
	 * 如果当前zonelist当中zone的空闲freepage小于lowmem_reserve的一半则需要向下执行做do_try_to_free_pages完成zonelist的内存回收操作
     */
	if (throttle_direct_reclaim(sc.gfp_mask, zonelist, nodemask))
		return 1;

	trace_mm_vmscan_direct_reclaim_begin(order,
				sc.may_writepage,
				sc.gfp_mask,
				sc.reclaim_idx);
    /* 回收的page数量，
     * returns:	0,说明没有page可以回收
     * 如果回收成功则显示的是the number of pages reclaimed回收页面的个数    
     */
	nr_reclaimed = do_try_to_free_pages(zonelist, &sc);

	trace_mm_vmscan_direct_reclaim_end(nr_reclaimed);

	return nr_reclaimed;
}

#ifdef CONFIG_MEMCG

unsigned long mem_cgroup_shrink_node(struct mem_cgroup *memcg,
						gfp_t gfp_mask, bool noswap,
						pg_data_t *pgdat,
						unsigned long *nr_scanned)
{
	struct scan_control sc = {
		.nr_to_reclaim = SWAP_CLUSTER_MAX,
		.target_mem_cgroup = memcg,
		.may_writepage = !laptop_mode,
		.may_unmap = 1,
		.reclaim_idx = MAX_NR_ZONES - 1,
		.may_swap = !noswap,
	};
	unsigned long lru_pages;

	sc.gfp_mask = (gfp_mask & GFP_RECLAIM_MASK) |
			(GFP_HIGHUSER_MOVABLE & ~GFP_RECLAIM_MASK);

	trace_mm_vmscan_memcg_softlimit_reclaim_begin(sc.order,
						      sc.may_writepage,
						      sc.gfp_mask,
						      sc.reclaim_idx);

	/*
	 * NOTE: Although we can get the priority field, using it
	 * here is not a good idea, since it limits the pages we can scan.
	 * if we don't reclaim here, the shrink_node from balance_pgdat
	 * will pick up pages from other mem cgroup's as well. We hack
	 * the priority and make it zero.
	 */
	shrink_node_memcg(pgdat, memcg, &sc, &lru_pages);

	trace_mm_vmscan_memcg_softlimit_reclaim_end(sc.nr_reclaimed);

	*nr_scanned = sc.nr_scanned;
	return sc.nr_reclaimed;
}

unsigned long try_to_free_mem_cgroup_pages(struct mem_cgroup *memcg,
					   unsigned long nr_pages,
					   gfp_t gfp_mask,
					   bool may_swap)
{
	struct zonelist *zonelist;
	unsigned long nr_reclaimed;
	int nid;
	unsigned int noreclaim_flag;
	struct scan_control sc = {
		.nr_to_reclaim = max(nr_pages, SWAP_CLUSTER_MAX),
		.gfp_mask = (current_gfp_context(gfp_mask) & GFP_RECLAIM_MASK) |
				(GFP_HIGHUSER_MOVABLE & ~GFP_RECLAIM_MASK),
		.reclaim_idx = MAX_NR_ZONES - 1,
		.target_mem_cgroup = memcg,
		.priority = DEF_PRIORITY,
		.may_writepage = !laptop_mode,
		.may_unmap = 1,
		.may_swap = may_swap,
	};

	/*
	 * Unlike direct reclaim via alloc_pages(), memcg's reclaim doesn't
	 * take care of from where we get pages. So the node where we start the
	 * scan does not need to be the current node.
	 */
	nid = mem_cgroup_select_victim_node(memcg);

	zonelist = &NODE_DATA(nid)->node_zonelists[ZONELIST_FALLBACK];

	trace_mm_vmscan_memcg_reclaim_begin(0,
					    sc.may_writepage,
					    sc.gfp_mask,
					    sc.reclaim_idx);

	noreclaim_flag = memalloc_noreclaim_save();
	nr_reclaimed = do_try_to_free_pages(zonelist, &sc);
	memalloc_noreclaim_restore(noreclaim_flag);

	trace_mm_vmscan_memcg_reclaim_end(nr_reclaimed);

	return nr_reclaimed;
}
#endif

static void age_active_anon(struct pglist_data *pgdat,
				struct scan_control *sc)
{
	struct mem_cgroup *memcg;

	if (!total_swap_pages)
		return;

	memcg = mem_cgroup_iter(NULL, NULL, NULL);
	do {
		struct lruvec *lruvec = mem_cgroup_lruvec(pgdat, memcg);

		if (inactive_list_is_low(lruvec, false, sc, true))
			shrink_active_list(SWAP_CLUSTER_MAX, lruvec,
					   sc, LRU_ACTIVE_ANON);

		memcg = mem_cgroup_iter(NULL, memcg, NULL);
	} while (memcg);
}

/*
 * Returns true if there is an eligible zone balanced for the request order
 * and classzone_idx
 */
static bool pgdat_balanced(pg_data_t *pgdat, int order, int classzone_idx)
{
	int i;
	unsigned long mark = -1;
	struct zone *zone;
	/* 遍历Node当中的ZONE */
	for (i = 0; i <= classzone_idx; i++) {
		/* 获取当前zone的 */
		zone = pgdat->node_zones + i;
		/* zone当中如果没有page在buddy system当中则直接continue，检测下一个zone */
		if (!managed_zone(zone))
			continue;
		/* 获取zone的WMARK_HIGH，获取到zone最高的水位线 */
		mark = high_wmark_pages(zone);
		/* 如果order要求分配的内存值小于mark最高水位线，则说明有zone有足够的内存可供分配，所以直接return true可以kswapd休眠 */
		if (zone_watermark_ok_safe(zone, order, mark, classzone_idx))
			return true;
	}

	/*
	 * If a node has no populated zone within classzone_idx, it does not
	 * need balancing by definition. This can happen if a zone-restricted
	 * allocation tries to wake a remote kswapd.
	 */
	if (mark == -1)
		return true;
	/* return false无法休眠 */
	return false;
}

/* Clear pgdat state for congested, dirty or under writeback. */
static void clear_pgdat_congested(pg_data_t *pgdat)
{
	clear_bit(PGDAT_CONGESTED, &pgdat->flags);
	clear_bit(PGDAT_DIRTY, &pgdat->flags);
	clear_bit(PGDAT_WRITEBACK, &pgdat->flags);
}

/*
 * Prepare kswapd for sleeping. This verifies that there are no processes
 * waiting in throttle_direct_reclaim() and that watermarks have been met.
 *
 * Returns true if kswapd is ready to sleep
 */
/* 返回true节点kswapd线程进入sleep状态，否则节点kswapd继续running */
static bool prepare_kswapd_sleep(pg_data_t *pgdat, int order, int classzone_idx)
{
	/*
	 * The throttled processes are normally woken up in balance_pgdat() as
	 * soon as allow_direct_reclaim() is true. But there is a potential
	 * race between when kswapd checks the watermarks and a process gets
	 * throttled. There is also a potential race if processes get
	 * throttled, kswapd wakes, a large process exits thereby balancing the
	 * zones, which causes kswapd to exit balance_pgdat() before reaching
	 * the wake up checks. If kswapd is going to sleep, no process should
	 * be sleeping on pfmemalloc_wait, so wake them now if necessary. If
	 * the wake up is premature, processes will wake kswapd and get
	 * throttled again. The difference from wake ups in balance_pgdat() is
	 * that here we are under prepare_to_wait().
	 */
	/* 
	 * 判断pfmemalloc_wait是否为空，如果不为空则唤醒node节点pfmemalloc_wait队列中所有等待的任务，该操作与直接内存回收有关联
	 */
	if (waitqueue_active(&pgdat->pfmemalloc_wait))
		wake_up_all(&pgdat->pfmemalloc_wait);

	/* Hopeless node, leave it to direct reclaim */
	/* 
	 * 该节点的kswapd在进行的MAX_RECLAIM_RETRIES次内存回收操作中每次回收的内存页数都为0，已经超过最大MAX_RECLAIM_RETRIES内存回收次数
	 * kswapd线程已经无法从节点中回收到内存页主动让kswapd线程睡去所以直接return true，寄希望于直接内存回收 
	 */
	if (pgdat->kswapd_failures >= MAX_RECLAIM_RETRIES)
		return true;
	
	/*
	 * 该zone的空闲内存值减去该zone为classzone_idx对应zone保留内存值后获得的内存值大小若大于zone
	 * 的high水线值zone_balance函数返回True
	 */
	if (pgdat_balanced(pgdat, order, classzone_idx)) {
		clear_pgdat_congested(pgdat);
		return true;
	}

	return false;
}

/*
 * kswapd shrinks a node of pages that are at or below the highest usable
 * zone that is currently unbalanced.
 *
 * Returns true if kswapd scanned at least the requested number of pages to
 * reclaim or if the lack of progress was due to pages under writeback.
 * This is used to determine if the scanning priority needs to be raised.
 */
static bool kswapd_shrink_node(pg_data_t *pgdat,
			       struct scan_control *sc)
{
	struct zone *zone;
	int z;

	/* Reclaim a number of pages proportional to the number of zones */
	/* 初始化此次期望回收多少页面 */
	sc->nr_to_reclaim = 0;
	for (z = 0; z <= sc->reclaim_idx; z++) {
		zone = pgdat->node_zones + z;
		if (!managed_zone(zone))
			continue;

		sc->nr_to_reclaim += max(high_wmark_pages(zone), SWAP_CLUSTER_MAX);
	}

	/*
	 * Historically care was taken to put equal pressure on all zones but
	 * now pressure is applied based on node LRU order.
	 */
	/* 对节点中低于sc->reclaim_idx的非平衡zone区域进行回收操作 */
	shrink_node(pgdat, sc);

	/*
	 * Fragmentation may mean that the system cannot be rebalanced for
	 * high-order allocations. If twice the allocation size has been
	 * reclaimed then recheck watermarks only at order-0 to prevent
	 * excessive reclaim. Assume that a process requested a high-order
	 * can direct reclaim/compact.
	 */
	if (sc->order && sc->nr_reclaimed >= compact_gap(sc->order))
		sc->order = 0;
	/* 此次对当前内存节点的回收操作扫描的页面总数若大于等于期望回收的页面数返回True，反之则返回False */
	return sc->nr_scanned >= sc->nr_to_reclaim;
}

/*
 * For kswapd, balance_pgdat() will reclaim pages across a node from zones
 * that are eligible for use by the caller until at least one zone is
 * balanced.
 *
 * Returns the order kswapd finished reclaiming at.
 *
 * kswapd scans the zones in the highmem->normal->dma direction.  It skips
 * zones which have free_pages > high_wmark_pages(zone), but once a zone is
 * found to have free_pages <= high_wmark_pages(zone), any page is that zone
 * or lower is eligible for reclaim until at least one usable zone is
 * balanced.
 */
static int balance_pgdat(pg_data_t *pgdat, int order, int classzone_idx)
{
	int i;
	unsigned long nr_soft_reclaimed;
	unsigned long nr_soft_scanned;
	struct zone *zone;
	/* 初始化scan_control结构体 */
	struct scan_control sc = {
		.gfp_mask = GFP_KERNEL,
		.order = order,
		.priority = DEF_PRIORITY,
		.may_writepage = !laptop_mode,
		.may_unmap = 1,
		.may_swap = 1,
	};
	count_vm_event(PAGEOUTRUN);

	do {
		/* 保存本次内存回收之前，当前进程回收到的页面数，如果是第一次则nr_reclaimed = 0 */
		unsigned long nr_reclaimed = sc.nr_reclaimed;
		bool raise_priority = true;

		sc.reclaim_idx = classzone_idx;

		/*
		 * If the number of buffer_heads exceeds the maximum allowed
		 * then consider reclaiming from all zones. This has a dual
		 * purpose -- on 64-bit systems it is expected that
		 * buffer_heads are stripped during active rotation. On 32-bit
		 * systems, highmem pages can pin lowmem memory and shrinking
		 * buffers can relieve lowmem pressure. Reclaim may still not
		 * go ahead if all eligible zones for the original allocation
		 * request are balanced to avoid excessive reclaim from kswapd.
		 */
		/* 如果buffer_heads超过了允许的最大值，则将考虑从MAX_NR_ZONES - 1 ～ 0之间的zone回收该节点当中的内存 */
		if (buffer_heads_over_limit) {
			/* 遍历node的全部zone区域，考虑对Node的所有zone区域从(MAX_NR_ZONES - 1) ~ 0之间进行内存回收操作
			 * 这里其实就是回收NODE节点当中的所有zone区域
			 */
			for (i = MAX_NR_ZONES - 1; i >= 0; i--) {
				/* 将i赋值给pgdat->node_zones作为下一次循环的起始zone使用 */
				zone = pgdat->node_zones + i;
				/* 如果该区域当中没有buddy system管理的page内存则，直接continue跳过该区域 */
				if (!managed_zone(zone))
					continue;
				/* 将当前zone值赋值给sc.reclaim_idx */
				sc.reclaim_idx = i;
				break;
			}
		}

		/*
		 * Only reclaim if there are no eligible zones. Note that
		 * sc.reclaim_idx is not used as buffer_heads_over_limit may
		 * have adjusted it.
		 */
		/* 如果该zone区域是平衡，说明当前zone当中有足够的内存可以供内存分配使用，不再进行内存回收操作，则直接跳转到out，返回当前order值 */
		if (pgdat_balanced(pgdat, sc.order, classzone_idx))
			goto out;

		/*
		 * Do some background aging of the anon list, to give
		 * pages a chance to be referenced before reclaiming. All
		 * pages are rotated regardless of classzone as this is
		 * about consistent aging.
		 */
		/*
		 * 若当前节点不活跃匿名页lru链表中页面数量过低（通过inactive_list_is_low函数判断），则通过
		 * shrink_active_list函数将该节点活跃匿名页lru链表上的页迁移到不活跃匿名页lru链表上去。
		 */
		age_active_anon(pgdat, &sc);

		/*
		 * If we're getting trouble reclaiming, start doing writepage
		 * even in laptop mode.
		 */
		/* 如果当前优先级小于（DEF_PRIORITY - 2）也就是10，则设置sc.may_writepage = 1则内存回收允许回写 */
		if (sc.priority < DEF_PRIORITY - 2)
			sc.may_writepage = 1;

		/* Call soft limit reclaim before calling shrink_node. */
		sc.nr_scanned = 0;
		nr_soft_scanned = 0;
		nr_soft_reclaimed = mem_cgroup_soft_limit_reclaim(pgdat, sc.order,
						sc.gfp_mask, &nr_soft_scanned);
		sc.nr_reclaimed += nr_soft_reclaimed;

		/*
		 * There should be no need to raise the scanning priority if
		 * enough pages are already being scanned that that high
		 * watermark would be met at 100% efficiency.
		 */
		/*
		* 扫描节点的lru链表，进行内存回收（sc用于控制节点内存回收）
		* (1)扫描的页面数不小于期望回收页面数，此次不更新扫描优先级（因为不需要继续加大扫描量）
		* (2)进行内存回收时扫描该节点的整个node，这样做是为了避免zone的老化速度不同
		* 如果内存回收成功，则设置raise_priority = false减少内存回收的次数的累加
		*/
		if (kswapd_shrink_node(pgdat, &sc))
			raise_priority = false;

		/*
		 * If the low watermark is met there is no need for processes
		 * to be throttled on pfmemalloc_wait as they should not be
		 * able to safely make forward progress. Wake them
		 */
		/*
		 * 判断当前NODE当中是否存在休眠等待的内存回收进程，且对该zone区域进行直接的内存回收
		 * 如果两个条件全部满足，则同时唤醒当前pgdat等待队列里面全部的内存回收进程wake_up_all(&pgdat->pfmemalloc_wait)
		 */
		if (waitqueue_active(&pgdat->pfmemalloc_wait) &&
				allow_direct_reclaim(pgdat))
			wake_up_all(&pgdat->pfmemalloc_wait);

		/* Check if kswapd should be suspending */
		/* 尝试解封kswapd,或者是停止该进程时则直接break不再做内存回收操作 */
		if (try_to_freeze() || kthread_should_stop())
			break;

		/*
		 * Raise priority if scanning rate is too low or there was no
		 * progress in reclaiming pages
		 */
		/* nr_reclaimed代表已经回收的page页也是上次回收的页面数，sc.nr_reclaimed代表本次需要回收的页，
		 * nr_reclaimed = sc.nr_reclaimed - nr_reclaimed如果nr_reclaimed > 0说明进程本次回收是回收到里一定的page数，比上次回收得到的页数要多，本次内存回收是成功的
		 * 如果nr_reclaimed <= 0说明说明我们内存回收已经完成此时!nr_reclaimed = true，说明本次回收操作后得到的页面与上次相同，则说明本次回收并没有回收到页面，
		 * 说明本次zone区域的内存回收是失败的
		 * 或者是上面kswapd_shrink_node内存回收成功后raise_priority = false如果raise_priority = true说明回收的page不满足内存分配需要
		 * 不管以上哪一个条件成立则：sc.priority--增加内存回收的优先级，继续执行while循环
		 */
		nr_reclaimed = sc.nr_reclaimed - nr_reclaimed;
		if (raise_priority || !nr_reclaimed)
			sc.priority--;
	} while (sc.priority >= 1);
	/* 此次回收页面数为0，pgdat->kswapd_failures计算+1，统计内存分配失败的循环次数 */
	if (!sc.nr_reclaimed)
		pgdat->kswapd_failures++;

out:
	snapshot_refaults(NULL, pgdat);
	/*
	 * Return the order kswapd stopped reclaiming at as
	 * prepare_kswapd_sleep() takes it into account. If another caller
	 * entered the allocator slow path while kswapd was awake, order will
	 * remain at the higher level.
	 */
	/* return order */
	return sc.order;
}

/*
 * The pgdat->kswapd_classzone_idx is used to pass the highest zone index to be
 * reclaimed by kswapd from the waker. If the value is MAX_NR_ZONES which is not
 * a valid index then either kswapd runs for first time or kswapd couldn't sleep
 * after previous reclaim attempt (node is still unbalanced). In that case
 * return the zone index of the previous kswapd reclaim cycle.
 */
static enum zone_type kswapd_classzone_idx(pg_data_t *pgdat,
					   enum zone_type prev_classzone_idx)
{
	if (pgdat->kswapd_classzone_idx == MAX_NR_ZONES)
		return prev_classzone_idx;
	return pgdat->kswapd_classzone_idx;
}

static void kswapd_try_to_sleep(pg_data_t *pgdat, int alloc_order, int reclaim_order,
				unsigned int classzone_idx)
{
	long remaining = 0;
	DEFINE_WAIT(wait);

	if (freezing(current) || kthread_should_stop())
		return;
	/* 定义等待队列，并设置kswapd线程的TASK_INTERRUPTIBLE状态 */
	prepare_to_wait(&pgdat->kswapd_wait, &wait, TASK_INTERRUPTIBLE);

	/*
	 * Try to sleep for a short interval. Note that kcompactd will only be
	 * woken if it is possible to sleep for a short interval. This is
	 * deliberate on the assumption that if reclaim cannot keep an
	 * eligible zone balanced that it's also unlikely that compaction will
	 * succeed.
	 */
	/*
	 * prepare_kswapd_sleep判断kswapd能否进入随眠状态,此处睡眠的条件是pgdat中0到classzone_idx的zone
	 * 区域都是balance：返回TRUE--->睡眠
	 */
	if (prepare_kswapd_sleep(pgdat, reclaim_order, classzone_idx)) {
		/*
		 * Compaction records what page blocks it recently failed to
		 * isolate pages from and skips them in the future scanning.
		 * When kswapd is going to sleep, it is reasonable to assume
		 * that pages and compaction may succeed so reset the cache.
		 */
		/* 
		 * 节点的kswapd线程睡眠前清除该节点中所有页块的PB_migrate_skip标记，下次节点的内存整理时，就会对扫描更多
		 * 的页块
		 */
		reset_isolation_suitable(pgdat);

		/*
		 * We have freed the memory, now we should compact it to make
		 * allocation of the requested order possible.
		 */
		/*
		 * 当kswapd休眠时，唤醒kcompactd进程进行内存碎片的整理，alloc_order是内存分配时需要的阶数，也就是需要分配出2^alloc_order个page的连续内存
		 * 在这种情况下page页时足够的，存在不连续的可能，所以需要唤醒wakeup_kcompactd内核进程
		 */
		wakeup_kcompactd(pgdat, alloc_order, classzone_idx);
		/*
		 * 睡眠100ms调用schedule进入睡眠状态，让出cpu，该定时器可被中断，
		 * 若remaining为0则kswapd是周期性主动被唤醒的，不为0说明kswapd在100ms
		 * 睡眠过程中提前被其它任务打断，一般是被wake_up唤醒
		 */
		remaining = schedule_timeout(HZ/10);

		/*
		 * If woken prematurely then reset kswapd_classzone_idx and
		 * order. The values will either be from a wakeup request or
		 * the previous request that slept prematurely.
		 */
		/*
		 * 若remainig不为0，说明kswapd在100ms内提前被其它任务唤醒，属于被动提前唤醒情况，这个时候需要更新
		 * 节点中pgdat->kswapd_classzone_idx和pgdat->kswapd_order成员变量的值。更新值是取
		 * 唤醒请求传递过来的值和最近一次执行kswapd保留的值中的较大值。
		 */
		if (remaining) {
			pgdat->kswapd_classzone_idx = kswapd_classzone_idx(pgdat, classzone_idx);
			pgdat->kswapd_order = max(pgdat->kswapd_order, reclaim_order);
		}
		/* 设置当前current进程为TASK_RUNNING状态，唤醒的进程是否为分配内存的进程？？？？？？？？？？？，后边做验证 */
		finish_wait(&pgdat->kswapd_wait, &wait);
		/* 设置kswapd进程的状态为TASK_INTERRUPTIBLE */
		prepare_to_wait(&pgdat->kswapd_wait, &wait, TASK_INTERRUPTIBLE);
	}

	/*
	 * After a short sleep, check if it was a premature sleep. If not, then
	 * go fully to sleep until explicitly woken up.
	 */
	/*
	 * 如果remaining = 0, !remaining = true说明kswapd进程是主动唤醒的，100m后自动唤醒链自己的kswapd进程
	 * prepare_kswapd_sleep = true 则说明当前NODE节点中0到classzone_idx的zone区域是否都是zone_balanced的
	 */
	if (!remaining &&
	    prepare_kswapd_sleep(pgdat, reclaim_order, classzone_idx)) {
		trace_mm_vmscan_kswapd_sleep(pgdat->node_id);

		/*
		 * vmstat counters are not perfectly accurate and the estimated
		 * value for counters such as NR_FREE_PAGES can deviate from the
		 * true value by nr_online_cpus * threshold. To avoid the zone
		 * watermarks being breached while under pressure, we reduce the
		 * per-cpu vmstat threshold while kswapd is awake and restore
		 * them before going back to sleep.
		 */
		set_pgdat_percpu_threshold(pgdat, calculate_normal_threshold);
		/* 此时如果remaining = 0 说明kswapd进程已经被唤醒，如果kswapd线程没有停止则，schedule休眠让出当前cpu */
		if (!kthread_should_stop())
			schedule();

		set_pgdat_percpu_threshold(pgdat, calculate_pressure_threshold);
	} else {
		/* 如果kswapd被强制唤醒则继续内存回收操作 */
		if (remaining)
			count_vm_event(KSWAPD_LOW_WMARK_HIT_QUICKLY);
		else
			count_vm_event(KSWAPD_HIGH_WMARK_HIT_QUICKLY);
	}
	/* 设置当前current进程为TASK_RUNNING状态，唤醒的进程是否为分配内存的进程？？？？？？？？？？？，后边做验证 */
	finish_wait(&pgdat->kswapd_wait, &wait);
}

/*
 * The background pageout daemon, started as a kernel thread
 * from the init process.
 *
 * This basically trickles out pages so that we have _some_
 * free memory available even if there is no other activity
 * that frees anything up. This is needed for things like routing
 * etc, where we otherwise might have all activity going on in
 * asynchronous contexts that cannot page things out.
 *
 * If there are applications that are active memory-allocators
 * (most normal use), this basically shouldn't matter.
 */
static int kswapd(void *p)
{
	unsigned int alloc_order, reclaim_order;
	/* 获取Node节点当中的zone的最大值，从0～classzone_idx遍历整个Node所有的zone */
	unsigned int classzone_idx = MAX_NR_ZONES - 1;
	pg_data_t *pgdat = (pg_data_t*)p;
	/* 获取当前current进程的结构体 */
	struct task_struct *tsk = current;

	struct reclaim_state reclaim_state = {
		.reclaimed_slab = 0,
	};
	/* 读取请求节点上运行的在线cpumask */
	const struct cpumask *cpumask = cpumask_of_node(pgdat->node_id);
	/* 设置为当前任务，将请求节点的cpumask设置为当前任务 */
	if (!cpumask_empty(cpumask))
		set_cpus_allowed_ptr(tsk, cpumask);
	/* 设置当前进程的内存回收状态 */
	current->reclaim_state = &reclaim_state;

	/*
	 * Tell the memory management that we're a "memory allocator",
	 * and that if we need more memory we should get access to it
	 * regardless (see "__alloc_pages()"). "kswapd" should
	 * never get caught in the normal page freeing logic.
	 *
	 * (Kswapd normally doesn't need memory anyway, but sometimes
	 * you need a small amount of memory in order to be able to
	 * page out something else, and this flag essentially protects
	 * us from recursively trying to free more memory as we're
	 * trying to free the first piece of memory in the first place).
	 */
	/* 设置当前current进程的状态，添加标志：(1)内存分配线程，(2)允许回写到swap，(3)是一个kswapd线程 
	 * 内存回收过程中可能有内存分配，如果发生内存分配的情况将忽略掉所有限制条件尽最大可能尝试内存分配；当前进程也可能将交换页写入交换分区；当前进程是内存回收进程
	 */
	tsk->flags |= PF_MEMALLOC | PF_SWAPWRITE | PF_KSWAPD;
	set_freezable();

	/* 设置当前进程内存回收order = 0 */
	pgdat->kswapd_order = 0;
	/* 
	 * 设置当前node当中kswapd最大的交换zone区域值，内存回收应该就是从0～MAX_NR_ZONES之间进行
	 * 综合下来就是：kswapd_order 从 0 开始，并替换了 MAX_NR_ZONES，以便 kswapd_classzone_idx 可以从顶部进行
	 */
	pgdat->kswapd_classzone_idx = MAX_NR_ZONES;
	for ( ; ; ) {
		bool ret;
		/* 获取order */
		alloc_order = reclaim_order = pgdat->kswapd_order;
		/* 获取当前node的zone区域值将从当前的zone区域获取足够的内存 */
		classzone_idx = kswapd_classzone_idx(pgdat, classzone_idx);
		/*
		 * kswapd_try_to_sleep函数判断kswapd是否进入睡眠状态和让出cpu控制权，同时函数内部也是kswapd被唤
		 * 醒后的线程的入口点，代码的核心就是判断当前node是否有足够的内存可供分配使用，判断的标准就是watermask对比
		 */
kswapd_try_sleep:
		kswapd_try_to_sleep(pgdat, alloc_order, reclaim_order,
					classzone_idx);

		/* Read the new order and classzone_idx */
		/*
		 * kswapd被唤醒后更新局部变量，alloc_order和classzone_idx两个局部变量是kswapd线程流程控制中的重
		 * 要判断依据
		 */
		alloc_order = reclaim_order = pgdat->kswapd_order;
		classzone_idx = kswapd_classzone_idx(pgdat, classzone_idx);
		/* kwapd_order被重置为0，kswapd_classzone_idx被赋值为MAX_NR_ZONES */
		pgdat->kswapd_order = 0;
		pgdat->kswapd_classzone_idx = MAX_NR_ZONES;
		/* 尝试冻结kswapd进程 */
		ret = try_to_freeze();
		if (kthread_should_stop())//如果当前任务的KTHREAD_SHOULD_STOP标志位被置位，则直接break，线程终止
			break;

		/*
		 * We can speed up thawing tasks if we don't call balance_pgdat
		 * after returning from the refrigerator
		 */
		/* 如果冻结成功则直接continue扫描下一个zone,期望获取更多的空闲的page */
		if (ret)
			continue;

		/*
		 * Reclaim begins at the requested order but if a high-order
		 * reclaim fails then kswapd falls back to reclaiming for
		 * order-0. If that happens, kswapd will consider sleeping
		 * for the order it finished reclaiming at (reclaim_order)
		 * but kcompactd is woken to compact for the original
		 * request (alloc_order).
		 */
		trace_mm_vmscan_kswapd_wake(pgdat->node_id, classzone_idx,
						alloc_order);
		/* 枷锁 */
		fs_reclaim_acquire(GFP_KERNEL);
		/*
		 * 调用balance_pgdat对节点内存进行回收，返回回收内存块的order，reclaim_order表明该次回收过程获取
		 * 到的最大连续内存块的阶数
		 */
		reclaim_order = balance_pgdat(pgdat, alloc_order, classzone_idx);
		/* 解锁 */
		fs_reclaim_release(GFP_KERNEL);
		/* 
		 * 如果获得的最大连续内存小于要求得到的最大内存数则直接goto到kswapd_try_sleep重新检测
		 * 如果获取到的最大的连续内存数大于要求的最大连续内存数则直接return 0，完成kswapd操作，休眠该kswapd内核进程
		 * 此处指是针对被动唤醒kwapd线程的情况：
         * 最大阶（alloc_order）内存块回收失败，就会跳转到kswapd_try_sleep处用0阶的reclaim_order进行判
         * 断，这样kswapd线程直接sleep让直接内存回收去回收内存，kswapd在sleep前会唤醒compact内核线程去规
         * 整内存，期望其能规整出alloc_order阶大小的连续内存块
		 */
		if (reclaim_order < alloc_order)
			goto kswapd_try_sleep;
	}
	/* 消除之前设置内存分配属性，同时设置当前进程的内存回收方式为NULL完成Kswap的回收操作 */
	tsk->flags &= ~(PF_MEMALLOC | PF_SWAPWRITE | PF_KSWAPD);
	current->reclaim_state = NULL;

	return 0;
}

/*
 * A zone is low on free memory, so wake its kswapd task to service it.
 */
void wakeup_kswapd(struct zone *zone, int order, enum zone_type classzone_idx)
{
	pg_data_t *pgdat;

	if (!managed_zone(zone))
		return;

	if (!cpuset_zone_allowed(zone, GFP_KERNEL | __GFP_HARDWALL))
		return;
	pgdat = zone->zone_pgdat;

	if (pgdat->kswapd_classzone_idx == MAX_NR_ZONES)
		pgdat->kswapd_classzone_idx = classzone_idx;
	else
		pgdat->kswapd_classzone_idx = max(pgdat->kswapd_classzone_idx,
						  classzone_idx);
	pgdat->kswapd_order = max(pgdat->kswapd_order, order);
	if (!waitqueue_active(&pgdat->kswapd_wait))
		return;

	/* Hopeless node, leave it to direct reclaim */
	if (pgdat->kswapd_failures >= MAX_RECLAIM_RETRIES)
		return;

	if (pgdat_balanced(pgdat, order, classzone_idx))
		return;

	trace_mm_vmscan_wakeup_kswapd(pgdat->node_id, classzone_idx, order);
	wake_up_interruptible(&pgdat->kswapd_wait);
}

#ifdef CONFIG_HIBERNATION
/*
 * Try to free `nr_to_reclaim' of memory, system-wide, and return the number of
 * freed pages.
 *
 * Rather than trying to age LRUs the aim is to preserve the overall
 * LRU order by reclaiming preferentially
 * inactive > active > active referenced > active mapped
 */
unsigned long shrink_all_memory(unsigned long nr_to_reclaim)
{
	struct reclaim_state reclaim_state;
	struct scan_control sc = {
		.nr_to_reclaim = nr_to_reclaim,
		.gfp_mask = GFP_HIGHUSER_MOVABLE,
		.reclaim_idx = MAX_NR_ZONES - 1,
		.priority = DEF_PRIORITY,
		.may_writepage = 1,
		.may_unmap = 1,
		.may_swap = 1,
		.hibernation_mode = 1,
	};
	struct zonelist *zonelist = node_zonelist(numa_node_id(), sc.gfp_mask);
	struct task_struct *p = current;
	unsigned long nr_reclaimed;
	unsigned int noreclaim_flag;

	noreclaim_flag = memalloc_noreclaim_save();
	fs_reclaim_acquire(sc.gfp_mask);
	reclaim_state.reclaimed_slab = 0;
	p->reclaim_state = &reclaim_state;

	nr_reclaimed = do_try_to_free_pages(zonelist, &sc);

	p->reclaim_state = NULL;
	fs_reclaim_release(sc.gfp_mask);
	memalloc_noreclaim_restore(noreclaim_flag);

	return nr_reclaimed;
}
#endif /* CONFIG_HIBERNATION */

/* It's optimal to keep kswapds on the same CPUs as their memory, but
   not required for correctness.  So if the last cpu in a node goes
   away, we get changed to run anywhere: as the first one comes back,
   restore their cpu bindings. */
static int kswapd_cpu_online(unsigned int cpu)
{
	int nid;

	for_each_node_state(nid, N_MEMORY) {
		pg_data_t *pgdat = NODE_DATA(nid);
		const struct cpumask *mask;

		mask = cpumask_of_node(pgdat->node_id);

		if (cpumask_any_and(cpu_online_mask, mask) < nr_cpu_ids)
			/* One of our CPUs online: restore mask */
			set_cpus_allowed_ptr(pgdat->kswapd, mask);
	}
	return 0;
}

/*
 * This kswapd start function will be called by init and node-hot-add.
 * On node-hot-add, kswapd will moved to proper cpus if cpus are hot-added.
 */
int kswapd_run(int nid)
{
	/* 根据nid值获取zone结构体 */
	pg_data_t *pgdat = NODE_DATA(nid);
	int ret = 0;
	/* 如果kswapd进程是否为空，如果不为空，则说明该node对应的kswapd内核进程直接return 0
	 * 如果pgdat->kswapd = NULL说明该zone的kswapd县城还未创建，所以继续向下执行
	 */
	if (pgdat->kswapd)
		return 0;
	/* 用kthread_run创建kswapd内核线程，立刻运行 */
	pgdat->kswapd = kthread_run(kswapd, pgdat, "kswapd%d", nid);
	if (IS_ERR(pgdat->kswapd)) {
		/* failure at boot is fatal */
		BUG_ON(system_state < SYSTEM_RUNNING);
		pr_err("Failed to start kswapd on node %d\n", nid);
		ret = PTR_ERR(pgdat->kswapd);
		pgdat->kswapd = NULL;
	}
	return ret;
}

/*
 * Called by memory hotplug when all memory in a node is offlined.  Caller must
 * hold mem_hotplug_begin/end().
 */
void kswapd_stop(int nid)
{
	struct task_struct *kswapd = NODE_DATA(nid)->kswapd;

	if (kswapd) {
		kthread_stop(kswapd);
		NODE_DATA(nid)->kswapd = NULL;
	}
}

static int __init kswapd_init(void)
{
	int nid, ret;
	/*
	 *根据物理内存的大小设定一个全局变量page_cluster，该参数与磁盘设备驱动相关。在读磁盘前需要经过寻到，
	 *寻到操作很费时，为了省时每次会多读几个页面（预读）。每次预读的页面数就是该函数根据内存的大小计算出
	 *来（一般为2,3或4）。
	 */
	swap_setup();
	/* 遍历系统当中那个node，并执行每个节点的kswapd内核进程，对每个node进行内存交换操作 
	 * 同时明确一点就是每个node只有一个kswapd,
 	 */
	for_each_node_state(nid, N_MEMORY)
 		kswapd_run(nid);
	ret = cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN,
					"mm/vmscan:online", kswapd_cpu_online,
					NULL);
	WARN_ON(ret < 0);
	return 0;
}
/* 初始化该内核模块 */
module_init(kswapd_init)

#ifdef CONFIG_NUMA
/*
 * Node reclaim mode
 *
 * If non-zero call node_reclaim when the number of free pages falls below
 * the watermarks.
 */
int node_reclaim_mode __read_mostly;

#define RECLAIM_OFF 0
#define RECLAIM_ZONE (1<<0)	/* Run shrink_inactive_list on the zone */
#define RECLAIM_WRITE (1<<1)	/* Writeout pages during reclaim */
#define RECLAIM_UNMAP (1<<2)	/* Unmap pages during reclaim */

/*
 * Priority for NODE_RECLAIM. This determines the fraction of pages
 * of a node considered for each zone_reclaim. 4 scans 1/16th of
 * a zone.
 */
#define NODE_RECLAIM_PRIORITY 4

/*
 * Percentage of pages in a zone that must be unmapped for node_reclaim to
 * occur.
 */
int sysctl_min_unmapped_ratio = 1;

/*
 * If the number of slab pages in a zone grows beyond this percentage then
 * slab reclaim needs to occur.
 */
int sysctl_min_slab_ratio = 5;

static inline unsigned long node_unmapped_file_pages(struct pglist_data *pgdat)
{
	unsigned long file_mapped = node_page_state(pgdat, NR_FILE_MAPPED);
	unsigned long file_lru = node_page_state(pgdat, NR_INACTIVE_FILE) +
		node_page_state(pgdat, NR_ACTIVE_FILE);

	/*
	 * It's possible for there to be more file mapped pages than
	 * accounted for by the pages on the file LRU lists because
	 * tmpfs pages accounted for as ANON can also be FILE_MAPPED
	 */
	return (file_lru > file_mapped) ? (file_lru - file_mapped) : 0;
}

/* Work out how many page cache pages we can reclaim in this reclaim_mode */
static unsigned long node_pagecache_reclaimable(struct pglist_data *pgdat)
{
	unsigned long nr_pagecache_reclaimable;
	unsigned long delta = 0;

	/*
	 * If RECLAIM_UNMAP is set, then all file pages are considered
	 * potentially reclaimable. Otherwise, we have to worry about
	 * pages like swapcache and node_unmapped_file_pages() provides
	 * a better estimate
	 */
	if (node_reclaim_mode & RECLAIM_UNMAP)
		nr_pagecache_reclaimable = node_page_state(pgdat, NR_FILE_PAGES);
	else
		nr_pagecache_reclaimable = node_unmapped_file_pages(pgdat);

	/* If we can't clean pages, remove dirty pages from consideration */
	if (!(node_reclaim_mode & RECLAIM_WRITE))
		delta += node_page_state(pgdat, NR_FILE_DIRTY);

	/* Watch for any possible underflows due to delta */
	if (unlikely(delta > nr_pagecache_reclaimable))
		delta = nr_pagecache_reclaimable;

	return nr_pagecache_reclaimable - delta;
}

/*
 * Try to free up some pages from this node through reclaim.
 */
static int __node_reclaim(struct pglist_data *pgdat, gfp_t gfp_mask, unsigned int order)
{
	/* Minimum pages needed in order to stay on node */
	const unsigned long nr_pages = 1 << order;
	struct task_struct *p = current;
	struct reclaim_state reclaim_state;
	unsigned int noreclaim_flag;
	struct scan_control sc = {
		.nr_to_reclaim = max(nr_pages, SWAP_CLUSTER_MAX),
		.gfp_mask = current_gfp_context(gfp_mask),
		.order = order,
		.priority = NODE_RECLAIM_PRIORITY,
		.may_writepage = !!(node_reclaim_mode & RECLAIM_WRITE),
		.may_unmap = !!(node_reclaim_mode & RECLAIM_UNMAP),
		.may_swap = 1,
		.reclaim_idx = gfp_zone(gfp_mask),
	};

	cond_resched();
	/*
	 * We need to be able to allocate from the reserves for RECLAIM_UNMAP
	 * and we also need to be able to write out pages for RECLAIM_WRITE
	 * and RECLAIM_UNMAP.
	 */
	noreclaim_flag = memalloc_noreclaim_save();
	p->flags |= PF_SWAPWRITE;
	fs_reclaim_acquire(sc.gfp_mask);
	reclaim_state.reclaimed_slab = 0;
	p->reclaim_state = &reclaim_state;

	if (node_pagecache_reclaimable(pgdat) > pgdat->min_unmapped_pages) {
		/*
		 * Free memory by calling shrink zone with increasing
		 * priorities until we have enough memory freed.
		 */
		do {
			shrink_node(pgdat, &sc);
		} while (sc.nr_reclaimed < nr_pages && --sc.priority >= 0);
	}

	p->reclaim_state = NULL;
	fs_reclaim_release(gfp_mask);
	current->flags &= ~PF_SWAPWRITE;
	memalloc_noreclaim_restore(noreclaim_flag);
	return sc.nr_reclaimed >= nr_pages;
}

int node_reclaim(struct pglist_data *pgdat, gfp_t gfp_mask, unsigned int order)
{
	int ret;

	/*
	 * Node reclaim reclaims unmapped file backed pages and
	 * slab pages if we are over the defined limits.
	 *
	 * A small portion of unmapped file backed pages is needed for
	 * file I/O otherwise pages read by file I/O will be immediately
	 * thrown out if the node is overallocated. So we do not reclaim
	 * if less than a specified percentage of the node is used by
	 * unmapped file backed pages.
	 */
	if (node_pagecache_reclaimable(pgdat) <= pgdat->min_unmapped_pages &&
	    node_page_state(pgdat, NR_SLAB_RECLAIMABLE) <= pgdat->min_slab_pages)
		return NODE_RECLAIM_FULL;

	/*
	 * Do not scan if the allocation should not be delayed.
	 */
	if (!gfpflags_allow_blocking(gfp_mask) || (current->flags & PF_MEMALLOC))
		return NODE_RECLAIM_NOSCAN;

	/*
	 * Only run node reclaim on the local node or on nodes that do not
	 * have associated processors. This will favor the local processor
	 * over remote processors and spread off node memory allocations
	 * as wide as possible.
	 */
	if (node_state(pgdat->node_id, N_CPU) && pgdat->node_id != numa_node_id())
		return NODE_RECLAIM_NOSCAN;

	if (test_and_set_bit(PGDAT_RECLAIM_LOCKED, &pgdat->flags))
		return NODE_RECLAIM_NOSCAN;

	ret = __node_reclaim(pgdat, gfp_mask, order);
	clear_bit(PGDAT_RECLAIM_LOCKED, &pgdat->flags);

	if (!ret)
		count_vm_event(PGSCAN_ZONE_RECLAIM_FAILED);

	return ret;
}
#endif

/*
 * page_evictable - test whether a page is evictable
 * @page: the page to test
 *
 * Test whether page is evictable--i.e., should be placed on active/inactive
 * lists vs unevictable list.
 *
 * Reasons page might not be evictable:
 * (1) page's mapping marked unevictable
 * (2) page is part of an mlocked VMA
 *
 */
int page_evictable(struct page *page)
{
	int ret;

	/* Prevent address_space of inode and swap cache from being freed */
	rcu_read_lock();
	ret = !mapping_unevictable(page_mapping(page)) && !PageMlocked(page);
	rcu_read_unlock();
	return ret;
}

#ifdef CONFIG_SHMEM
/**
 * check_move_unevictable_pages - check pages for evictability and move to appropriate zone lru list
 * @pages:	array of pages to check
 * @nr_pages:	number of pages to check
 *
 * Checks pages for evictability and moves them to the appropriate lru list.
 *
 * This function is only used for SysV IPC SHM_UNLOCK.
 */
void check_move_unevictable_pages(struct page **pages, int nr_pages)
{
	struct lruvec *lruvec;
	struct pglist_data *pgdat = NULL;
	int pgscanned = 0;
	int pgrescued = 0;
	int i;

	for (i = 0; i < nr_pages; i++) {
		struct page *page = pages[i];
		struct pglist_data *pagepgdat = page_pgdat(page);

		pgscanned++;
		if (pagepgdat != pgdat) {
			if (pgdat)
				spin_unlock_irq(&pgdat->lru_lock);
			pgdat = pagepgdat;
			spin_lock_irq(&pgdat->lru_lock);
		}
		lruvec = mem_cgroup_page_lruvec(page, pgdat);

		if (!PageLRU(page) || !PageUnevictable(page))
			continue;

		if (page_evictable(page)) {
			enum lru_list lru = page_lru_base_type(page);

			VM_BUG_ON_PAGE(PageActive(page), page);
			ClearPageUnevictable(page);
			del_page_from_lru_list(page, lruvec, LRU_UNEVICTABLE);
			add_page_to_lru_list(page, lruvec, lru);
			pgrescued++;
		}
	}

	if (pgdat) {
		__count_vm_events(UNEVICTABLE_PGRESCUED, pgrescued);
		__count_vm_events(UNEVICTABLE_PGSCANNED, pgscanned);
		spin_unlock_irq(&pgdat->lru_lock);
	}
}
#endif /* CONFIG_SHMEM */
