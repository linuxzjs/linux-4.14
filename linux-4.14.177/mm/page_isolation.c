// SPDX-License-Identifier: GPL-2.0
/*
 * linux/mm/page_isolation.c
 */

#include <linux/mm.h>
#include <linux/page-isolation.h>
#include <linux/pageblock-flags.h>
#include <linux/memory.h>
#include <linux/hugetlb.h>
#include <linux/page_owner.h>
#include <linux/migrate.h>
#include "internal.h"

#define CREATE_TRACE_POINTS
#include <trace/events/page_isolation.h>

static int set_migratetype_isolate(struct page *page,
				bool skip_hwpoisoned_pages)
{
	struct zone *zone;
	unsigned long flags, pfn;
	struct memory_isolate_notify arg;
	int notifier_ret;
	int ret = -EBUSY;
    /* 获取page所在的zone区域 */
	zone = page_zone(page);

	spin_lock_irqsave(&zone->lock, flags);
    /* 获取page对应的页框号，并完成arg的初始化 */
	pfn = page_to_pfn(page);
	arg.start_pfn = pfn;
	arg.nr_pages = pageblock_nr_pages;
	arg.pages_found = 0;

	/*
	 * It may be possible to isolate a pageblock even if the
	 * migratetype is not MIGRATE_MOVABLE. The memory isolation
	 * notifier chain is used by balloon drivers to return the
	 * number of pages in a range that are held by the balloon
	 * driver to shrink memory. If all the pages are accounted for
	 * by balloons, are free, or on the LRU, isolation can continue.
	 * Later, for example, when memory hotplug notifier runs, these
	 * pages reported as "can be isolated" should be isolated(freed)
	 * by the balloon driver through the memory notifier chain.
	 */
	notifier_ret = memory_isolate_notify(MEM_ISOLATE_COUNT, &arg);
	notifier_ret = notifier_to_errno(notifier_ret);
	if (notifier_ret)
		goto out;
	/*
	 * FIXME: Now, memory hotplug doesn't call shrink_slab() by itself.
	 * We just check MOVABLE pages.
	 */
	 /* 如果zone 区域的page不存在unmovable_pages，则直接return false，换句话说当zone page是
	  * MIGRATE_MOVABLE、MIGRATE_CMA类型的，是可以进行page迁移的，此时设置ret = 0；
	  */
	if (!has_unmovable_pages(zone, page, arg.pages_found,
				 skip_hwpoisoned_pages))
		ret = 0;

	/*
	 * immobile means "not-on-lru" pages. If immobile is larger than
	 * removable-by-driver pages reported by notifier, we'll fail.
	 */

out:
    /* 如果ret = 0,则if(!0)为真，进入判断当中，完成内存隔离操作 */
	if (!ret) {
		unsigned long nr_pages;
        /* 获取当前page的迁移类型，相当于page的备份迁移类型 */
		int migratetype = get_pageblock_migratetype(page);
        /* 设置当前pageblock迁移类型为隔离类型 */
		set_pageblock_migratetype(page, MIGRATE_ISOLATE);
        /* nr_isolate_pageblock自加1，得到zone区域当中设置MIGRATE_ISOLATE迁移类型的pageblock个数 */
		zone->nr_isolate_pageblock++;
        /* 将当前的pageblock内所有的page：pageblock_nr_pages以MIGRATE_ISOLATE迁移类型根据条件移动到
         * 对应的zone->free_area[order].free_list[MIGRATE_ISOLATE]当中完成隔离操作
         * 对应的代码操作操作就是list_move(&page->lru,&zone->free_area[order].free_list[migratetype]);
         */
		nr_pages = move_freepages_block(zone, page, MIGRATE_ISOLATE,
									NULL);
        /* 从之前的migratetype迁移类型当中减去MIGRATE_MOVABLE、MIGRATE_CMA这两种被隔离的page，得到的是buddy system可以分配的空闲内存*/
		__mod_zone_freepage_state(zone, -nr_pages, migratetype);
	}

	spin_unlock_irqrestore(&zone->lock, flags);
	if (!ret)
		drain_all_pages(zone);
	return ret;
}

/* 将迁移类型为MIGRATE_ISOLATE的pageblock的migratetype类型修改为其他非MIGRATE_ISOLATE，便于后续的内存回收 */
static void unset_migratetype_isolate(struct page *page, unsigned migratetype)
{
	struct zone *zone;
	unsigned long flags, nr_pages;
	bool isolated_page = false;
	unsigned int order;
	unsigned long pfn, buddy_pfn;
	struct page *buddy;
    /* 获取当前page所在的zone区域 */
	zone = page_zone(page);
	spin_lock_irqsave(&zone->lock, flags);
    /* 当pageblock迁移类型为MIGRATE_ISOLATE，则不进行goto跳转，继续向下执行，isolated_page = false
     * 向下继续进行pageblock migratetype属性的修改
     * 当pageblock迁移类型为非MIGRATE_ISOLATE时，该判断为真，直接goto out跳转，因为pageblock不是MIGRATE_ISOLATE
     * 也就没有必要进行后续的pageblock检查，以及migratetype的修改，直接结束该函数的执行
     */
	if (!is_migrate_isolate_page(page))
		goto out;

	/*
	 * Because freepage with more than pageblock_order on isolated
	 * pageblock is restricted to merge due to freepage counting problem,
	 * it is possible that there is free buddy page.
	 * move_freepages_block() doesn't care of merge so we need other
	 * approach in order to merge them. Isolation and free will make
	 * these pages to be merged.
	 */
	/* 如果当前page在buddy system当中管理则进入判断 */
	if (PageBuddy(page)) {
        /* 获取当前page的order值 */
		order = page_order(page);
        /* 如果当前page的order >pageblock_order(也就是MAX_ORDER-1 = 10) */
		if (order >= pageblock_order) {
            /* 获取 page的页框号 */
			pfn = page_to_pfn(page);
            /* 根据页框号，以及page order获取budd的页框号buddy_pfn */
			buddy_pfn = __find_buddy_pfn(pfn, order);
            /*根据当前页以及buddy_pfn可以找到page的buddy页 */
			buddy = page + (buddy_pfn - pfn);
            /* 当buddy_pfn页框号有效，且当前page buddy的pageblock的迁移类型不是MIGRATE_ISOLATE
             * 则进入判断当中，直接将pageblock通过__isolate_free_page完成释放同时修改pageblock的migratetoye为非MIGRATE_ISOLATE
             * 通常将free的page设置为MIGRATE_MOVABLE，便于后续buddy system内存分配时使用
             * 当buddy_pfn非法，或者时buddy migratetype为MIGRATE_ISOLATE，则不进入判断，继续向下执行
             */
			if (pfn_valid_within(buddy_pfn) &&
			    !is_migrate_isolate_page(buddy)) {
			    /* 完成当前page的隔离操作，设置当前page migratetype 的隔离*/
				__isolate_free_page(page, order);
                /* 设置isolated_page = true说明已经进行过freepage的isolate操作 */
				isolated_page = true;
			}
		}
	}

	/*
	 * If we isolate freepage with more than pageblock_order, there
	 * should be no freepage in the range, so we could avoid costly
	 * pageblock scanning for freepage moving.
	 */
	/* 当isolated_page = true，则说明pageblock 已经被__isolate_free_page释放，此时不进入判断，后续通过set_pageblock_migratetype
	 * 修改pageblock中的page的migratetype，也就是MIGRATE_CMA
	 * 当isolated_page = false 进入判断当中：说明pageblock migratetype为MIGRATE_ISOLATE
	 * 将pageblock的MIGRATE_ISOLATE通过move_freepages_block函数根据migratetype此处就是MIGRATE_CMA，完成空闲页面的修改
	 * 对应的效果就是zone->free_area[order].free_list[MIGRATE_CMA]，同时通过__mod_zone_freepage_state函数将分离出来的nr_pages个数添加到
	 * zone的NR_FREE_PAGES空闲页面数，可以供buddy system可以分配的空闲内存使用
	 */
	if (!isolated_page) {
		nr_pages = move_freepages_block(zone, page, migratetype, NULL);
		__mod_zone_freepage_state(zone, nr_pages, migratetype);
	}
    /* 设置当前pageblock为MIGRATE_CMA类型，完成pageblock的unset migratetype isolate操作，将page从isolate隔离状态当中释放出啦 */
	set_pageblock_migratetype(page, migratetype);
    /* 由于ZONE当中的pageblock从lsolate隔离状态中分离出来，所以zone当中被隔离的pageblock减少，所以自减1 */
	zone->nr_isolate_pageblock--;
out:
	spin_unlock_irqrestore(&zone->lock, flags);
	if (isolated_page) {
		post_alloc_hook(page, order, __GFP_MOVABLE);
		__free_pages(page, order);
	}
}

static inline struct page *
__first_valid_page(unsigned long pfn, unsigned long nr_pages)
{
	int i;

	for (i = 0; i < nr_pages; i++) {
		struct page *page;

		if (!pfn_valid_within(pfn + i))
			continue;
		page = pfn_to_online_page(pfn + i);
		if (!page)
			continue;
		return page;
	}
	return NULL;
}

/*
 * start_isolate_page_range() -- make page-allocation-type of range of pages
 * to be MIGRATE_ISOLATE.
 * @start_pfn: The lower PFN of the range to be isolated.
 * @end_pfn: The upper PFN of the range to be isolated.
 * @migratetype: migrate type to set in error recovery.
 *
 * Making page-allocation-type to be MIGRATE_ISOLATE means free pages in
 * the range will never be allocated. Any free pages and pages freed in the
 * future will not be allocated again.
 *
 * start_pfn/end_pfn must be aligned to pageblock_order.
 * Returns 0 on success and -EBUSY if any part of range cannot be isolated.
 */
int start_isolate_page_range(unsigned long start_pfn, unsigned long end_pfn,
			     unsigned migratetype, bool skip_hwpoisoned_pages)
{
	unsigned long pfn;
	unsigned long undo_pfn;
	struct page *page;

	BUG_ON(!IS_ALIGNED(start_pfn, pageblock_nr_pages));
	BUG_ON(!IS_ALIGNED(end_pfn, pageblock_nr_pages));
    /*
     * 以pageblock_nr_pages为单位将满足条件的pageblock的migratetype设置为MIGRATE_ISOLATE，
     * 将对应的page 迁移类型设置为隔离后，buddy system就无法使用，无法进行内存分配
     * 隔离完成后return 0；完成pageblock大范围的内存隔离操作
     */
	for (pfn = start_pfn;
	     pfn < end_pfn;
	     pfn += pageblock_nr_pages) {
		page = __first_valid_page(pfn, pageblock_nr_pages);
        /* 当page有效时，使用set_migratetype_isolate完成页面的隔离，设置page migratetype为MIGRATE_ISOLATE */
		if (page &&
		    set_migratetype_isolate(page, skip_hwpoisoned_pages)) {
			undo_pfn = pfn;
			goto undo;
		}
	}
	return 0;
undo:
    /*
     * 如果在pageblock当中没有合法的page，或者是无法设置page的迁移模式为MIGRATE_ISOLATE
     * 则进入该循环当中，此时将对start ~ start + undo_pfn的page设置为非隔离状态
     * undo_pfn = 根据上文for循环，循环的次数，假设是n 则undo_pfn = start_pfn +  n*pageblock_nr_pages
     */
	for (pfn = start_pfn;
	     pfn < undo_pfn;
	     pfn += pageblock_nr_pages) {
	    /* 如果pfn不合法则直接continue跳过这个pageblock,也就是向下偏移pageblock_nr_pages */
		struct page *page = pfn_to_online_page(pfn);
		if (!page)
			continue;
        /*
         * 将上文设置MIGRATE_ISOLATE隔离状态的pageblock分离出来，设置为MIGRATE_CMA，并更新相关参数，
         */
		unset_migratetype_isolate(page, migratetype);
	}

	return -EBUSY;
}

/*
 * Make isolated pages available again.
 */
int undo_isolate_page_range(unsigned long start_pfn, unsigned long end_pfn,
			    unsigned migratetype)
{
	unsigned long pfn;
	struct page *page;

	BUG_ON(!IS_ALIGNED(start_pfn, pageblock_nr_pages));
	BUG_ON(!IS_ALIGNED(end_pfn, pageblock_nr_pages));

	for (pfn = start_pfn;
	     pfn < end_pfn;
	     pfn += pageblock_nr_pages) {
		page = __first_valid_page(pfn, pageblock_nr_pages);
		if (!page || !is_migrate_isolate_page(page))
			continue;
		unset_migratetype_isolate(page, migratetype);
	}
	return 0;
}
/*
 * Test all pages in the range is free(means isolated) or not.
 * all pages in [start_pfn...end_pfn) must be in the same zone.
 * zone->lock must be held before call this.
 *
 * Returns the last tested pfn.
 */
static unsigned long
__test_page_isolated_in_pageblock(unsigned long pfn, unsigned long end_pfn,
				  bool skip_hwpoisoned_pages)
{
	struct page *page;

	while (pfn < end_pfn) {
		if (!pfn_valid_within(pfn)) {
			pfn++;
			continue;
		}
		page = pfn_to_page(pfn);
		if (PageBuddy(page))
			/*
			 * If the page is on a free list, it has to be on
			 * the correct MIGRATE_ISOLATE freelist. There is no
			 * simple way to verify that as VM_BUG_ON(), though.
			 */
			pfn += 1 << page_order(page);
		else if (skip_hwpoisoned_pages && PageHWPoison(page))
			/* A HWPoisoned page cannot be also PageBuddy */
			pfn++;
		else
			break;
	}

	return pfn;
}

/* Caller should ensure that requested range is in a single zone */
int test_pages_isolated(unsigned long start_pfn, unsigned long end_pfn,
			bool skip_hwpoisoned_pages)
{
	unsigned long pfn, flags;
	struct page *page;
	struct zone *zone;

	/*
	 * Note: pageblock_nr_pages != MAX_ORDER. Then, chunks of free pages
	 * are not aligned to pageblock_nr_pages.
	 * Then we just check migratetype first.
	 */
	for (pfn = start_pfn; pfn < end_pfn; pfn += pageblock_nr_pages) {
		page = __first_valid_page(pfn, pageblock_nr_pages);
		if (page && !is_migrate_isolate_page(page))
			break;
	}
	page = __first_valid_page(start_pfn, end_pfn - start_pfn);
	if ((pfn < end_pfn) || !page)
		return -EBUSY;
	/* Check all pages are free or marked as ISOLATED */
	zone = page_zone(page);
	spin_lock_irqsave(&zone->lock, flags);
	pfn = __test_page_isolated_in_pageblock(start_pfn, end_pfn,
						skip_hwpoisoned_pages);
	spin_unlock_irqrestore(&zone->lock, flags);

	trace_test_pages_isolated(start_pfn, end_pfn, pfn);

	return pfn < end_pfn ? -EBUSY : 0;
}

struct page *alloc_migrate_target(struct page *page, unsigned long private,
				  int **resultp)
{
	return new_page_nodemask(page, numa_node_id(), &node_states[N_MEMORY]);
}
