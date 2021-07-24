// SPDX-License-Identifier: GPL-2.0
/*
 * linux/mm/compaction.c
 *
 * Memory compaction for the reduction of external fragmentation. Note that
 * this heavily depends upon page migration to do all the real heavy
 * lifting
 *
 * Copyright IBM Corp. 2007-2010 Mel Gorman <mel@csn.ul.ie>
 */
#include <linux/cpu.h>
#include <linux/swap.h>
#include <linux/migrate.h>
#include <linux/compaction.h>
#include <linux/mm_inline.h>
#include <linux/sched/signal.h>
#include <linux/backing-dev.h>
#include <linux/sysctl.h>
#include <linux/sysfs.h>
#include <linux/page-isolation.h>
#include <linux/kasan.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/page_owner.h>
#include "internal.h"

#ifdef CONFIG_COMPACTION
static inline void count_compact_event(enum vm_event_item item)
{
	count_vm_event(item);
}

static inline void count_compact_events(enum vm_event_item item, long delta)
{
	count_vm_events(item, delta);
}
#else
#define count_compact_event(item) do { } while (0)
#define count_compact_events(item, delta) do { } while (0)
#endif

#if defined CONFIG_COMPACTION || defined CONFIG_CMA

#define CREATE_TRACE_POINTS
#include <trace/events/compaction.h>

#define block_start_pfn(pfn, order)	round_down(pfn, 1UL << (order))
#define block_end_pfn(pfn, order)	ALIGN((pfn) + 1, 1UL << (order))
#define pageblock_start_pfn(pfn)	block_start_pfn(pfn, pageblock_order)
#define pageblock_end_pfn(pfn)		block_end_pfn(pfn, pageblock_order)

static unsigned long release_freepages(struct list_head *freelist)
{
	struct page *page, *next;
	unsigned long high_pfn = 0;

	list_for_each_entry_safe(page, next, freelist, lru) {
		unsigned long pfn = page_to_pfn(page);
		list_del(&page->lru);
		__free_page(page);
		if (pfn > high_pfn)
			high_pfn = pfn;
	}

	return high_pfn;
}

static void map_pages(struct list_head *list)
{
	unsigned int i, order, nr_pages;
	struct page *page, *next;
	LIST_HEAD(tmp_list);

	list_for_each_entry_safe(page, next, list, lru) {
		list_del(&page->lru);

		order = page_private(page);
		nr_pages = 1 << order;

		post_alloc_hook(page, order, __GFP_MOVABLE);
		if (order)
			split_page(page, order);

		for (i = 0; i < nr_pages; i++) {
			list_add(&page->lru, &tmp_list);
			page++;
		}
	}

	list_splice(&tmp_list, list);
}

#ifdef CONFIG_COMPACTION

int PageMovable(struct page *page)
{
	struct address_space *mapping;

	VM_BUG_ON_PAGE(!PageLocked(page), page);
	if (!__PageMovable(page))
		return 0;

	mapping = page_mapping(page);
	if (mapping && mapping->a_ops && mapping->a_ops->isolate_page)
		return 1;

	return 0;
}
EXPORT_SYMBOL(PageMovable);

void __SetPageMovable(struct page *page, struct address_space *mapping)
{
	VM_BUG_ON_PAGE(!PageLocked(page), page);
	VM_BUG_ON_PAGE((unsigned long)mapping & PAGE_MAPPING_MOVABLE, page);
	page->mapping = (void *)((unsigned long)mapping | PAGE_MAPPING_MOVABLE);
}
EXPORT_SYMBOL(__SetPageMovable);

void __ClearPageMovable(struct page *page)
{
	VM_BUG_ON_PAGE(!PageLocked(page), page);
	VM_BUG_ON_PAGE(!PageMovable(page), page);
	/*
	 * Clear registered address_space val with keeping PAGE_MAPPING_MOVABLE
	 * flag so that VM can catch up released page by driver after isolation.
	 * With it, VM migration doesn't try to put it back.
	 */
	page->mapping = (void *)((unsigned long)page->mapping &
				PAGE_MAPPING_MOVABLE);
}
EXPORT_SYMBOL(__ClearPageMovable);

/* Do not skip compaction more than 64 times */
#define COMPACT_MAX_DEFER_SHIFT 6

/*
 * Compaction is deferred when compaction fails to result in a page
 * allocation success. 1 << compact_defer_limit compactions are skipped up
 * to a limit of 1 << COMPACT_MAX_DEFER_SHIFT
 */
void defer_compaction(struct zone *zone, int order)
{
    /* 提高内存碎片整理计数器的阀值，zone的内存碎片整理计数器阀值 ，也就是只有同步整理会增加推迟计数器的阀值
     * 重置zone->compact_considered = 0，zone->compact_defer_shift++;
     */
	zone->compact_considered = 0;
	zone->compact_defer_shift++;
    /*
     * 如果order < zone->compact_order_failed，那么zone->compact_order_failed = order
     */
	if (order < zone->compact_order_failed)
		zone->compact_order_failed = order;
    /*
     * 如果zone->compact_defer_shift   >  COMPACT_MAX_DEFER_SHIFT，推迟扫面延迟的阈值大于COMPACT_MAX_DEFER_SHIFT也就是6
     * 则设置zone->compact_defer_shift = COMPACT_MAX_DEFER_SHIFT;
     * 如果zone->compact_defer_shift < COMPACT_MAX_DEFER_SHIFT就相当于zone->compact_defer_shift++;该阈值增加 1 
     */
	if (zone->compact_defer_shift > COMPACT_MAX_DEFER_SHIFT)
		zone->compact_defer_shift = COMPACT_MAX_DEFER_SHIFT;

	trace_mm_compaction_defer_compaction(zone, order);
}

/* Returns true if compaction should be skipped this time */
bool compaction_deferred(struct zone *zone, int order)
{
    /* zone可压缩的阈值defer_limit = 2^compact_defer_shift       */
	unsigned long defer_limit = 1UL << zone->compact_defer_shift;
    /* 如果order < compact_order_failed 说明该zone是可以被整理压缩的，所以return false此时该zone必须要做内存碎片整理，不应该被skip跳过  */
	if (order < zone->compact_order_failed)
		return false;
/*
 * zone->compact_considered是否小于1UL << zone->compact_defer_shift
       * 小于则推迟，并且zone->compact_considered++，也就是这个函数会主动去推迟此管理区的内存碎片整理
       * 本次请求的order值小于之前失败时的order值，那这次整理必须要进行
       * zone->compact_considered和zone->compact_defer_shift会只有在内存碎片整理完成后，从此zone获取到了连续的1 << order个页框的情况下会重置为0。
       */

	/* Avoid possible overflow */
    /* 如果zone->compact_considered自加后 > derfer_limit则zone->compact_considered = defer_limit;此时直接return false同样说明zone存在足够的空闲
     * page可用于碎片整理,碎片整理的次数已经超过阈值defer_limit，不能再推迟了，所以就必须接受碎片整理
     * 如果compact_considered < defer_limit说明推迟的次数依然小于阈值的给你的defer_limit，则说明该zone依然是可以推迟的，所以return true skip zone
     */
	if (++zone->compact_considered > defer_limit)
		zone->compact_considered = defer_limit;

	if (zone->compact_considered >= defer_limit)
		return false;

	trace_mm_compaction_deferred(zone, order);

	return true;
}

/*
 * Update defer tracking counters after successful compaction of given order,
 * which means an allocation either succeeded (alloc_success == true) or is
 * expected to succeed.
 */
void compaction_defer_reset(struct zone *zone, int order,
		bool alloc_success)
{
    /* 在compact_zone_order内存压缩整理成功的前提下也就是status == COMPACT_SUCCESS
     * 如果设置alloc_success = true完成内存内存分配工作，此时重置compact_considered、compact_defer_shift = 0
     * 如果设置alloc_success = false 内存分配失败则不会做该重置操作
     */
	if (alloc_success) {
		zone->compact_considered = 0;
		zone->compact_defer_shift = 0;
	}
    /* 如果order  >= compact_order_failedz则更新重置该参数，将order + 1 赋给该变量，相当该zone压缩失败的最大值变成了order*2的倍数 */
	if (order >= zone->compact_order_failed)
		zone->compact_order_failed = order + 1;

	trace_mm_compaction_defer_reset(zone, order);
}

/* Returns true if restarting compaction after many failures */
bool compaction_restarting(struct zone *zone, int order)
{
    /* 如果order < zone->compact_order_failed 推迟扫描失败的最大order，则return false  */
	if (order < zone->compact_order_failed)
		return false;
    /*
     * 如果同时满足zone->compact_defer_shift == COMPACT_MAX_DEFER_SHIFT， zone->compact_considered >= 1UL << zone->compact_defer_shift;
     * 则return true
     */
	return zone->compact_defer_shift == COMPACT_MAX_DEFER_SHIFT &&
		zone->compact_considered >= 1UL << zone->compact_defer_shift;
}

/* Returns true if the pageblock should be scanned for pages to isolate. */
static inline bool isolation_suitable(struct compact_control *cc,
					struct page *page)
{
	if (cc->ignore_skip_hint)
		return true;

	return !get_pageblock_skip(page);
}

static void reset_cached_positions(struct zone *zone)
{
	zone->compact_cached_migrate_pfn[0] = zone->zone_start_pfn;
	zone->compact_cached_migrate_pfn[1] = zone->zone_start_pfn;
	zone->compact_cached_free_pfn =
				pageblock_start_pfn(zone_end_pfn(zone) - 1);
}

/*
 * This function is called to clear all cached information on pageblocks that
 * should be skipped for page isolation when the migrate and free page scanner
 * meet.
 */
static void __reset_isolation_suitable(struct zone *zone)
{
    /* 获取zone起始page的页框 */
	unsigned long start_pfn = zone->zone_start_pfn;
     /* 获取zone结束page的页框 */
	unsigned long end_pfn = zone_end_pfn(zone);
	unsigned long pfn;

	zone->compact_blockskip_flush = false;

	/* Walk the zone and mark every pageblock as suitable for isolation */
    /* 设置页框的属性，通过修改页框的属性做到skip过无法合并的pageblock，也就是所谓的隔离isolation */
	for (pfn = start_pfn; pfn < end_pfn; pfn += pageblock_nr_pages) {
		struct page *page;

		cond_resched();
        /* pfn转换成page,同时判断page是否为空，如果page = NULL            则continue 检测下一个pageblock */
		page = pfn_to_online_page(pfn);
		if (!page)
			continue;
        /* 判断该page是否在该zone当中，如果不在则continue            检测下一个pageblock */
		if (zone != page_zone(page))
			continue;
        /* 将zone中所该pageblock的PB_migrate_skip清空 */
		clear_pageblock_skip(page);
	}

	reset_cached_positions(zone);
}

void reset_isolation_suitable(pg_data_t *pgdat)
{
	int zoneid;

	for (zoneid = 0; zoneid < MAX_NR_ZONES; zoneid++) {
		struct zone *zone = &pgdat->node_zones[zoneid];
		if (!populated_zone(zone))
			continue;

		/* Only flush if a full compaction finished recently */
		if (zone->compact_blockskip_flush)
			__reset_isolation_suitable(zone);
	}
}

/*
 * If no pages were isolated then mark this pageblock to be skipped in the
 * future. The information is later cleared by __reset_isolation_suitable().
 */
static void update_pageblock_skip(struct compact_control *cc,
			struct page *page, unsigned long nr_isolated,
			bool migrate_scanner)
{
	struct zone *zone = cc->zone;
	unsigned long pfn;

	if (cc->ignore_skip_hint)
		return;

	if (!page)
		return;

	if (nr_isolated)
		return;

	set_pageblock_skip(page);

	pfn = page_to_pfn(page);

	/* Update where async and sync compaction should restart */
	if (migrate_scanner) {
		if (pfn > zone->compact_cached_migrate_pfn[0])
			zone->compact_cached_migrate_pfn[0] = pfn;
		if (cc->mode != MIGRATE_ASYNC &&
		    pfn > zone->compact_cached_migrate_pfn[1])
			zone->compact_cached_migrate_pfn[1] = pfn;
	} else {
		if (pfn < zone->compact_cached_free_pfn)
			zone->compact_cached_free_pfn = pfn;
	}
}
#else
static inline bool isolation_suitable(struct compact_control *cc,
					struct page *page)
{
	return true;
}

static void update_pageblock_skip(struct compact_control *cc,
			struct page *page, unsigned long nr_isolated,
			bool migrate_scanner)
{
}
#endif /* CONFIG_COMPACTION */

/*
 * Compaction requires the taking of some coarse locks that are potentially
 * very heavily contended. For async compaction, back out if the lock cannot
 * be taken immediately. For sync compaction, spin on the lock if needed.
 *
 * Returns true if the lock is held
 * Returns false if the lock is not held and compaction should abort
 */
static bool compact_trylock_irqsave(spinlock_t *lock, unsigned long *flags,
						struct compact_control *cc)
{
	if (cc->mode == MIGRATE_ASYNC) {
		if (!spin_trylock_irqsave(lock, *flags)) {
			cc->contended = true;
			return false;
		}
	} else {
		spin_lock_irqsave(lock, *flags);
	}

	return true;
}

/*
 * Compaction requires the taking of some coarse locks that are potentially
 * very heavily contended. The lock should be periodically unlocked to avoid
 * having disabled IRQs for a long time, even when there is nobody waiting on
 * the lock. It might also be that allowing the IRQs will result in
 * need_resched() becoming true. If scheduling is needed, async compaction
 * aborts. Sync compaction schedules.
 * Either compaction type will also abort if a fatal signal is pending.
 * In either case if the lock was locked, it is dropped and not regained.
 *
 * Returns true if compaction should abort due to fatal signal pending, or
 *		async compaction due to need_resched()
 * Returns false when compaction can continue (sync compaction might have
 *		scheduled)
 */
static bool compact_unlock_should_abort(spinlock_t *lock,
		unsigned long flags, bool *locked, struct compact_control *cc)
{
	if (*locked) {
		spin_unlock_irqrestore(lock, flags);
		*locked = false;
	}

	if (fatal_signal_pending(current)) {
		cc->contended = true;
		return true;
	}

	if (need_resched()) {
		if (cc->mode == MIGRATE_ASYNC) {
			cc->contended = true;
			return true;
		}
		cond_resched();
	}

	return false;
}

/*
 * Aside from avoiding lock contention, compaction also periodically checks
 * need_resched() and either schedules in sync compaction or aborts async
 * compaction. This is similar to what compact_unlock_should_abort() does, but
 * is used where no lock is concerned.
 *
 * Returns false when no scheduling was needed, or sync compaction scheduled.
 * Returns true when async compaction should abort.
 */
static inline bool compact_should_abort(struct compact_control *cc)
{
	/* async compaction aborts if contended */
	if (need_resched()) {
		if (cc->mode == MIGRATE_ASYNC) {
			cc->contended = true;
			return true;
		}

		cond_resched();
	}

	return false;
}

/*
 * Isolate free pages onto a private freelist. If @strict is true, will abort
 * returning 0 on any invalid PFNs or non-free pages inside of the pageblock
 * (even though it may still end up isolating some pages).
 */
/* 扫描从start_pfn到end_pfn的空闲页框，一般都是一个pageblock的开始页框ID和结束页框ID，并把它们放入到freelist中，
 * 返回此pageblock中总共获得的空闲页框数量
 */
static unsigned long isolate_freepages_block(struct compact_control *cc,
				unsigned long *start_pfn,
				unsigned long end_pfn,
				struct list_head *freelist,
				bool strict)
{
	int nr_scanned = 0, total_isolated = 0;
	struct page *cursor, *valid_page = NULL;
	unsigned long flags = 0;
	bool locked = false;
	unsigned long blockpfn = *start_pfn;
	unsigned int order;

	cursor = pfn_to_page(blockpfn);

	/* Isolate free pages. */
    /* 从pageblock的start向end进行扫描 */
	for (; blockpfn < end_pfn; blockpfn++, cursor++) {
		int isolated;
        /* 当前页框 */
		struct page *page = cursor;

		/*
		 * Periodically drop the lock (if held) regardless of its
		 * contention, to give chance to IRQs. Abort if fatal signal
		 * pending or async compaction detects need_resched()
		 */
		if (!(blockpfn % SWAP_CLUSTER_MAX)
		    && compact_unlock_should_abort(&cc->zone->lock, flags,
								&locked, cc))
			break;

		nr_scanned++;
         /* 检查此页框号是否正确 */
		if (!pfn_valid_within(blockpfn))
			goto isolate_fail;
        
        /* valid_page是开始扫描的页框 */
		if (!valid_page)
			valid_page = page;

		/*
		 * For compound pages such as THP and hugetlbfs, we can save
		 * potentially a lot of iterations if we skip them at once.
		 * The check is racy, but we can consider only valid values
		 * and the only danger is skipping too much.
		 */
		/* 如果page是复合页透明大页，巨型页则进入判断
		 * 获取复合页的order值，如果comp_order < MAX_ORDER则blockpfn += (1UL << comp_order)更新blockpfn起始地址
		 * cursor += (1UL << comp_order) - 1;更新cursor，后跳转到isolate_fail
		 * 如果order >= MAX_ORDER则直接goto isolate_fail
		 */
		if (PageCompound(page)) {
			unsigned int comp_order = compound_order(page);

			if (likely(comp_order < MAX_ORDER)) {
				blockpfn += (1UL << comp_order) - 1;
				cursor += (1UL << comp_order) - 1;
			}

			goto isolate_fail;
		}
        /* 检查此页是否在伙伴系统中，不在说明是正在使用的页框，则跳过 */
		if (!PageBuddy(page))
			goto isolate_fail;

		/*
		 * If we already hold the lock, we can skip some rechecking.
		 * Note that if we hold the lock now, checked_pageblock was
		 * already set in some previous iteration (or strict is true),
		 * so it is correct to skip the suitable migration target
		 * recheck as well.
		 */
		 /* 获取锁 */
		if (!locked) {
			/*
			 * The zone lock must be held to isolate freepages.
			 * Unfortunately this is a very coarse lock and can be
			 * heavily contended if there are parallel allocations
			 * or parallel compactions. For async compaction do not
			 * spin on the lock and we acquire the lock as late as
			 * possible.
			 */
			locked = compact_trylock_irqsave(&cc->zone->lock,
								&flags, cc);
			if (!locked)
				break;

			/* Recheck this is a buddy page under lock */
			if (!PageBuddy(page))
				goto isolate_fail;
		}

		/* Found a free page, will break it into order-0 pages */
        /* 根据page获取该page所属的order */
		order = page_order(page);
        /* 将page开始的连续空闲页框拆分为连续的单个页框，返回数量，order值会在page的页描述符中，并设置pageblock的类型为MIGRATE_MOVABLE */
		isolated = __isolate_free_page(page, order);
		if (!isolated)
			break;
        /* 重置page的order,此处推测order应该变成了0，变成了一个个单独的page */
		set_page_private(page, order);
        /* 更新总共隔离的空闲页框数量 */
		total_isolated += isolated;
         /* 更新nr_freepages总共隔离的空闲页框数量 */
		cc->nr_freepages += isolated;
        /* 将分离后的连续page全部添加到freelist链表当中 */
		list_add_tail(&page->lru, freelist);
        /* 如果strict = false，且migratepages可移动页面 <= freepages自由移动页面
         * 则blockpfn += isolated;，同时break完成pageblock中page的隔离操作
         * 如果nr_freepages > nr_migratepages则说明有足够的自由空闲页面可以提供给可移动页面转移相关内容则直接break，
         * 如果不满足条件则还需需要continue继续执行
         */
		if (!strict && cc->nr_migratepages <= cc->nr_freepages) {
			blockpfn += isolated;
			break;
		}
		/* Advance to the end of split page */
         /* 跳过这段连续空闲页框，因为上面把这段空闲页框全部加入到了freelist中 */
		blockpfn += isolated - 1;
		cursor += isolated - 1;
		continue;

isolate_fail:
		if (strict)
			break;
		else
			continue;

	}
     /* 如果占有锁则释放掉 */
	if (locked)
		spin_unlock_irqrestore(&cc->zone->lock, flags);

	/*
	 * There is a tiny chance that we have read bogus compound_order(),
	 * so be careful to not go outside of the pageblock.
	 */
	if (unlikely(blockpfn > end_pfn))
		blockpfn = end_pfn;

	trace_mm_compaction_isolate_freepages(*start_pfn, blockpfn,
					nr_scanned, total_isolated);

	/* Record how far we have got within the block */
	*start_pfn = blockpfn;

	/*
	 * If strict isolation is requested by CMA then check that all the
	 * pages requested were isolated. If there were any failures, 0 is
	 * returned and CMA will fail.
	 */
	if (strict && blockpfn < end_pfn)
		total_isolated = 0;

	/* Update the pageblock-skip if the whole pageblock was scanned */
    /* 扫描完了此pageblock，如果此pageblock中没有隔离出空闲页框，则标记此pageblock为跳过 */
	if (blockpfn == end_pfn)
		update_pageblock_skip(cc, valid_page, total_isolated, false);

	cc->total_free_scanned += nr_scanned;
	if (total_isolated)
		count_compact_events(COMPACTISOLATED, total_isolated);
    
     /* 返回总共获得的空闲页框 */
	return total_isolated;
}

/**
 * isolate_freepages_range() - isolate free pages.
 * @start_pfn: The first PFN to start isolating.
 * @end_pfn:   The one-past-last PFN.
 *
 * Non-free pages, invalid PFNs, or zone boundaries within the
 * [start_pfn, end_pfn) range are considered errors, cause function to
 * undo its actions and return zero.
 *
 * Otherwise, function returns one-past-the-last PFN of isolated page
 * (which may be greater then end_pfn if end fell in a middle of
 * a free page).
 */
unsigned long
isolate_freepages_range(struct compact_control *cc,
			unsigned long start_pfn, unsigned long end_pfn)
{
	unsigned long isolated, pfn, block_start_pfn, block_end_pfn;
	LIST_HEAD(freelist);

	pfn = start_pfn;
	block_start_pfn = pageblock_start_pfn(pfn);
	if (block_start_pfn < cc->zone->zone_start_pfn)
		block_start_pfn = cc->zone->zone_start_pfn;
	block_end_pfn = pageblock_end_pfn(pfn);

	for (; pfn < end_pfn; pfn += isolated,
				block_start_pfn = block_end_pfn,
				block_end_pfn += pageblock_nr_pages) {
		/* Protect pfn from changing by isolate_freepages_block */
		unsigned long isolate_start_pfn = pfn;

		block_end_pfn = min(block_end_pfn, end_pfn);

		/*
		 * pfn could pass the block_end_pfn if isolated freepage
		 * is more than pageblock order. In this case, we adjust
		 * scanning range to right one.
		 */
		if (pfn >= block_end_pfn) {
			block_start_pfn = pageblock_start_pfn(pfn);
			block_end_pfn = pageblock_end_pfn(pfn);
			block_end_pfn = min(block_end_pfn, end_pfn);
		}

		if (!pageblock_pfn_to_page(block_start_pfn,
					block_end_pfn, cc->zone))
			break;

		isolated = isolate_freepages_block(cc, &isolate_start_pfn,
						block_end_pfn, &freelist, true);

		/*
		 * In strict mode, isolate_freepages_block() returns 0 if
		 * there are any holes in the block (ie. invalid PFNs or
		 * non-free pages).
		 */
		if (!isolated)
			break;

		/*
		 * If we managed to isolate pages, it is always (1 << n) *
		 * pageblock_nr_pages for some non-negative n.  (Max order
		 * page may span two pageblocks).
		 */
	}

	/* __isolate_free_page() does not map the pages */
	map_pages(&freelist);

	if (pfn < end_pfn) {
		/* Loop terminated early, cleanup. */
		release_freepages(&freelist);
		return 0;
	}

	/* We don't use freelists for anything. */
	return pfn;
}

/* Similar to reclaim, but different enough that they don't share logic */
static bool too_many_isolated(struct zone *zone)
{
	unsigned long active, inactive, isolated;

	inactive = node_page_state(zone->zone_pgdat, NR_INACTIVE_FILE) +
			node_page_state(zone->zone_pgdat, NR_INACTIVE_ANON);
	active = node_page_state(zone->zone_pgdat, NR_ACTIVE_FILE) +
			node_page_state(zone->zone_pgdat, NR_ACTIVE_ANON);
	isolated = node_page_state(zone->zone_pgdat, NR_ISOLATED_FILE) +
			node_page_state(zone->zone_pgdat, NR_ISOLATED_ANON);

	return isolated > (inactive + active) / 2;
}

/**
 * isolate_migratepages_block() - isolate all migrate-able pages within
 *				  a single pageblock
 * @cc:		Compaction control structure.
 * @low_pfn:	The first PFN to isolate
 * @end_pfn:	The one-past-the-last PFN to isolate, within same pageblock
 * @isolate_mode: Isolation mode to be used.
 *
 * Isolate all pages that can be migrated from the range specified by
 * [low_pfn, end_pfn). The range is expected to be within same pageblock.
 * Returns zero if there is a fatal signal pending, otherwise PFN of the
 * first page that was not scanned (which may be both less, equal to or more
 * than end_pfn).
 *
 * The pages are isolated on cc->migratepages list (not required to be empty),
 * and cc->nr_migratepages is updated accordingly. The cc->migrate_pfn field
 * is neither read nor updated.
 */
static unsigned long
isolate_migratepages_block(struct compact_control *cc, unsigned long low_pfn,
			unsigned long end_pfn, isolate_mode_t isolate_mode)
{
	struct zone *zone = cc->zone;
	unsigned long nr_scanned = 0, nr_isolated = 0;
	struct lruvec *lruvec;
	unsigned long flags = 0;
	bool locked = false;
	struct page *page = NULL, *valid_page = NULL;
	unsigned long start_pfn = low_pfn;
	bool skip_on_failure = false;
	unsigned long next_skip_pfn = 0;

	/*
	 * Ensure that there are not too many pages isolated from the LRU
	 * list by either parallel reclaimers or compaction. If there are,
	 * delay for some time until fewer pages are isolated
	 */
	/* 检查isolated是否小于LRU链表的(inactive + active) / 2，超过了则表示已经将许多页框隔离出来 */
	while (unlikely(too_many_isolated(zone))) {
		/* async migration should just abort */
		if (cc->mode == MIGRATE_ASYNC)
			return 0;
        
        /* 进行100ms的休眠，等待设备没那么繁忙 */
		congestion_wait(BLK_RW_ASYNC, HZ/10);

		if (fatal_signal_pending(current))
			return 0;
	}
     /* 如果是异步调用cc->mode == MIGRATE_ASYNC，并且当前进程需要调度的话，返回真
      * 只有是异步的操作才能被休眠，调度 return 0结束该程序的执行
      */
	if (compact_should_abort(cc))
		return 0;
    /*
     * 如果设置zone为直接压缩，同时压缩的模式为异步压缩模式则进入判断当中
     */
	if (cc->direct_compaction && (cc->mode == MIGRATE_ASYNC)) {
		skip_on_failure = true; //skip 标志为true
		/* 计算出下一个pageblock的起始页表，相当于low_pfn + 2^order次方，
		 * 此处存在一个疑问就是2^order与pageblock_nr_pages的大小，
		 * 如果2*order次方小于pageblock_nr_pages则肯定无法指向下一个pageblock的起始页表
		 */
		next_skip_pfn = block_end_pfn(low_pfn, cc->order);
	}

	/* Time to isolate some pages for migration */
    /* 遍历每一个页框，此时单位的大小是page，而不是pageblock的大小，就相当于遍历了这个区间里面的每一个page页 */
	for (; low_pfn < end_pfn; low_pfn++) {
        /* 如果设置zone为直接压缩，同时压缩的模式为异步压缩模式
         * skip_on_failure = true;
         * next_skip_pfn = block_end_pfn(low_pfn, cc->order);
         * 如果low_pfn >= next_skip_pfn则说明order是存在问题的，
         */
		if (skip_on_failure && low_pfn >= next_skip_pfn) {
			/*
			 * We have isolated all migration candidates in the
			 * previous order-aligned block, and did not skip it due
			 * to failure. We should migrate the pages now and
			 * hopefully succeed compaction.
			 */
			/* 如果nr_isolated = 1则直接break */
			if (nr_isolated)
				break;

			/*
			 * We failed to isolate in the previous order-aligned
			 * block. Set the new boundary to the end of the
			 * current block. Note we can't simply increase
			 * next_skip_pfn by 1 << order, as low_pfn might have
			 * been incremented by a higher number due to skipping
			 * a compound or a high-order buddy page in the
			 * previous loop iteration.
			 */
			 /* 重置next_skip_pfn */
			next_skip_pfn = block_end_pfn(low_pfn, cc->order);
		}

		/*
		 * Periodically drop the lock (if held) regardless of its
		 * contention, to give chance to IRQs. Abort async compaction
		 * if contended.
		 */
		/* 这里会释放掉zone->lru_lock这个锁 */
		if (!(low_pfn % SWAP_CLUSTER_MAX)
		    && compact_unlock_should_abort(zone_lru_lock(zone), flags,
								&locked, cc))
			break;

		if (!pfn_valid_within(low_pfn))
			goto isolate_fail;
         /* 扫描次数++ */
		nr_scanned++;
        /* 将low_pfn起始地址转化为page */
		page = pfn_to_page(low_pfn);
        /* 设置valid_page */
		if (!valid_page)
			valid_page = page;

		/*
		 * Skip if free. We read page order here without zone lock
		 * which is generally unsafe, but the race window is small and
		 * the worst thing that can happen is that we skip some
		 * potential isolation targets.
		 */
		 /* 检查此页是否处于伙伴系统中，主要是通过page->_mapcount判断，如果在伙伴系统中，则跳过这块空闲内存 */
		if (PageBuddy(page)) {
            /* 获取这个页开始的order次方个页框为伙伴系统的一块内存 */
			unsigned long freepage_order = page_order_unsafe(page);

			/*
			 * Without lock, we cannot be sure that what we got is
			 * a valid page order. Consider only values in the
			 * valid order range to prevent low_pfn overflow.
			 */
			/* 如果满足条件则low_pfn       + 2^freepage_order更新起始页框 */
			if (freepage_order > 0 && freepage_order < MAX_ORDER)
				low_pfn += (1UL << freepage_order) - 1;
			continue;
		}

		/*
		 * Regardless of being on LRU, compound pages such as THP and
		 * hugetlbfs are not to be compacted. We can potentially save
		 * a lot of iterations if we skip them at once. The check is
		 * racy, but we can consider only valid values and the only
		 * danger is skipping too much.
		 */
		/* 如果page是复合页透明大页，巨型页则进入判断
		 * 获取复合页的order值，如果comp_order < MAX_ORDER则low_pfn += (1UL << comp_order)更新low_pfn起始地址
		 * 如果comp_order  >= MAX_ORDER则goto 跳转到isolate_fail
		 */
		if (PageCompound(page)) {
			unsigned int comp_order = compound_order(page);

			if (likely(comp_order < MAX_ORDER))
				low_pfn += (1UL << comp_order) - 1;

			goto isolate_fail;
		}

		/*
		 * Check may be lockless but that's ok as we recheck later.
		 * It's possible to migrate LRU and non-lru movable pages.
		 * Skip any other type of page
		 */
		/* 以下处理此页不在伙伴系统中的情况，表明此页是在使用的页*/
        /* 如果页不处于lru中的处理，isolated的页是不处于lru中的，用于balloon的页也不处于lru中?
         * 可移动的页都会在LRU中，不在LRU中的页都会被跳过，这里就把UNMOVABLE进行跳过
         */
		if (!PageLRU(page)) {
			/*
			 * __PageMovable can return false positive so we need
			 * to verify it under page_lock.
			 */
			if (unlikely(__PageMovable(page)) &&
					!PageIsolated(page)) {
				if (locked) {
					spin_unlock_irqrestore(zone_lru_lock(zone),
									flags);
					locked = false;
				}

				if (!isolate_movable_page(page, isolate_mode))
					goto isolate_success;
			}

			goto isolate_fail;
		}

		/*
		 * Migration will fail if an anonymous page is pinned in memory,
		 * so avoid taking lru_lock and isolating it unnecessarily in an
		 * admittedly racy check.
		 */
		/* 如果是一个匿名页，并且被引用次数大于page->_mapcount，则跳过此页，注释说此页很有可能被锁定在内存中不允许换出，但不知道如何判断的 */
		if (!page_mapping(page) &&
		    page_count(page) > page_mapcount(page))
			goto isolate_fail;

		/*
		 * Only allow to migrate anonymous pages in GFP_NOFS context
		 * because those do not depend on fs locks.
		 */
		if (!(cc->gfp_mask & __GFP_FS) && page_mapping(page))
			goto isolate_fail;

		/* If we already hold the lock, we can skip some rechecking */
        /* 检查是否有上锁，这个锁是zone->lru_lock */
		if (!locked) {
			locked = compact_trylock_irqsave(zone_lru_lock(zone),
								&flags, cc);
			if (!locked)
				break;

			/* Recheck PageLRU and PageCompound under lock */
            /* 没上锁的情况，需要检查是否处于LRU中 */
			if (!PageLRU(page))
				goto isolate_fail;

			/*
			 * Page become compound since the non-locked check,
			 * and it's on LRU. It can only be a THP so the order
			 * is safe to read and it's 0 for tail pages.
			 */
			/* 如果在lru中，检查是否是复合页，做个对齐，防止low_pfn不是页首 */
			if (unlikely(PageCompound(page))) {
				low_pfn += (1UL << compound_order(page)) - 1;
				goto isolate_fail;
			}
		}

		lruvec = mem_cgroup_page_lruvec(page, zone->zone_pgdat);

		/* Try isolate the page */
         /* 将此页从lru中隔离出来 */
		if (__isolate_lru_page(page, isolate_mode) != 0)
			goto isolate_fail;

		VM_BUG_ON_PAGE(PageCompound(page), page);

		/* Successfully isolated */
         /* 如果在cgroup的lru缓冲区，则将此页从lru缓冲区中拿出来 */
		del_page_from_lru_list(page, lruvec, page_lru(page));
		inc_node_page_state(page,
				NR_ISOLATED_ANON + page_is_file_cache(page));

isolate_success:
        /* 将此页加入到本次整理需要移动页链表中，关键点 */
		list_add(&page->lru, &cc->migratepages);
        /* 需要移动的页框数量++ */
		cc->nr_migratepages++;
        /* 隔离数量++ */
		nr_isolated++;

		/*
		 * Record where we could have freed pages by migration and not
		 * yet flushed them to buddy allocator.
		 * - this is the lowest page that was isolated and likely be
		 * then freed by migration.
		 */
		if (!cc->last_migrated_pfn)
			cc->last_migrated_pfn = low_pfn;

		/* Avoid isolating too much */
        /* COMPACT_CLUSTER_MAX代表每次内存碎片整理所能移动的最大页框数量，则break完成页面的隔离 */
		if (cc->nr_migratepages == COMPACT_CLUSTER_MAX) {
			++low_pfn;
			break;
		}
        /* continue扫描下一个page */
		continue;
isolate_fail:
		if (!skip_on_failure)
			continue;

		/*
		 * We have isolated some pages, but then failed. Release them
		 * instead of migrating, as we cannot form the cc->order buddy
		 * page anyway.
		 */
		/* 如果页面的隔离数量       为正，也就是隔离成功则进入判断当中 */
		if (nr_isolated) {
            /* 解锁 */
			if (locked) {
				spin_unlock_irqrestore(zone_lru_lock(zone), flags);
				locked = false;//设置解锁标志，防止重复解锁
			}
            /* 重新设置相关参数 */
			putback_movable_pages(&cc->migratepages);
			cc->nr_migratepages = 0;
			cc->last_migrated_pfn = 0;
			nr_isolated = 0;
		}

		if (low_pfn < next_skip_pfn) {
			low_pfn = next_skip_pfn - 1;
			/*
			 * The check near the loop beginning would have updated
			 * next_skip_pfn too, but this is a bit simpler.
			 */
			next_skip_pfn += 1UL << cc->order;
		}
	}

	/*
	 * The PageBuddy() check could have potentially brought us outside
	 * the range to be scanned.
	 */
	if (unlikely(low_pfn > end_pfn))
		low_pfn = end_pfn;
    /* 解锁 */
	if (locked)
		spin_unlock_irqrestore(zone_lru_lock(zone), flags);

	/*
	 * Update the pageblock-skip information and cached scanner pfn,
	 * if the whole pageblock was scanned without isolating any page.
	 */
	if (low_pfn == end_pfn)
		update_pageblock_skip(cc, valid_page, nr_isolated, true);

	trace_mm_compaction_isolate_migratepages(start_pfn, low_pfn,
						nr_scanned, nr_isolated);

	cc->total_migrate_scanned += nr_scanned;
	if (nr_isolated)
		count_compact_events(COMPACTISOLATED, nr_isolated);

	return low_pfn;
}

/**
 * isolate_migratepages_range() - isolate migrate-able pages in a PFN range
 * @cc:        Compaction control structure.
 * @start_pfn: The first PFN to start isolating.
 * @end_pfn:   The one-past-last PFN.
 *
 * Returns zero if isolation fails fatally due to e.g. pending signal.
 * Otherwise, function returns one-past-the-last PFN of isolated page
 * (which may be greater than end_pfn if end fell in a middle of a THP page).
 */
unsigned long
isolate_migratepages_range(struct compact_control *cc, unsigned long start_pfn,
							unsigned long end_pfn)
{
	unsigned long pfn, block_start_pfn, block_end_pfn;

	/* Scan block by block. First and last block may be incomplete */
	pfn = start_pfn;
	block_start_pfn = pageblock_start_pfn(pfn);
	if (block_start_pfn < cc->zone->zone_start_pfn)
		block_start_pfn = cc->zone->zone_start_pfn;
	block_end_pfn = pageblock_end_pfn(pfn);

	for (; pfn < end_pfn; pfn = block_end_pfn,
				block_start_pfn = block_end_pfn,
				block_end_pfn += pageblock_nr_pages) {

		block_end_pfn = min(block_end_pfn, end_pfn);

		if (!pageblock_pfn_to_page(block_start_pfn,
					block_end_pfn, cc->zone))
			continue;

		pfn = isolate_migratepages_block(cc, pfn, block_end_pfn,
							ISOLATE_UNEVICTABLE);

		if (!pfn)
			break;

		if (cc->nr_migratepages == COMPACT_CLUSTER_MAX)
			break;
	}

	return pfn;
}

#endif /* CONFIG_COMPACTION || CONFIG_CMA */
#ifdef CONFIG_COMPACTION

static bool suitable_migration_source(struct compact_control *cc,
							struct page *page)
{
	int block_mt;
    /* 如果mode迁移类型不是async异步方式或者没有设置直接压缩，则return true                      */
	if ((cc->mode != MIGRATE_ASYNC) || !cc->direct_compaction)
		return true;
    /* 获取pageblock的migrate类型 */
	block_mt = get_pageblock_migratetype(page);
    /* 如果migratetype为MIGRATE_MOVABLE可移动类型则执行  is_migrate_movable，通过该函数block_mt == MIGRATE_CMA || MIGRATE_MOVABLE
     * return block_mt可知该page的移动类型是MIGRATE_CMA，MIGRATE_MOVABLE也或者是这两者的集合，是可以进行页面的迁移整理的。用于判断page的迁移类型与pageblock的迁移类型是否相同
     * 相同则return true, 不同则return false
     * 如果cc->migratetype != MIGRATE_MOVABLE,则将cc->migratetype赋给block_mt;
     */
	if (cc->migratetype == MIGRATE_MOVABLE)
		return is_migrate_movable(block_mt);
	else
		return block_mt == cc->migratetype;
}

/* Returns true if the page is within a block suitable for migration to */
static bool suitable_migration_target(struct compact_control *cc,
							struct page *page)
{
	/* If the page is a large free page, then disallow migration */
	if (PageBuddy(page)) {
		/*
		 * We are checking page_order without zone->lock taken. But
		 * the only small danger is that we skip a potentially suitable
		 * pageblock, so it's not worth to check order for valid range.
		 */
		if (page_order_unsafe(page) >= pageblock_order)
			return false;
	}

	if (cc->ignore_block_suitable)
		return true;

	/* If the block is MIGRATE_MOVABLE or MIGRATE_CMA, allow migration */
	if (is_migrate_movable(get_pageblock_migratetype(page)))
		return true;

	/* Otherwise skip the block */
	return false;
}

/*
 * Test whether the free scanner has reached the same or lower pageblock than
 * the migration scanner, and compaction should thus terminate.
 */
static inline bool compact_scanners_met(struct compact_control *cc)
{
	return (cc->free_pfn >> pageblock_order)
		<= (cc->migrate_pfn >> pageblock_order);
}

/*
 * Based on information in the current compact_control, find blocks
 * suitable for isolating free pages from and then isolate them.
 */
static void isolate_freepages(struct compact_control *cc)
{
	struct zone *zone = cc->zone;
	struct page *page;
	unsigned long block_start_pfn;	/* start of current pageblock */
	unsigned long isolate_start_pfn; /* exact pfn we start at */
	unsigned long block_end_pfn;	/* end of current pageblock */
	unsigned long low_pfn;	     /* lowest pfn scanner is able to scan */
	struct list_head *freelist = &cc->freepages;

	/*
	 * Initialise the free scanner. The starting point is where we last
	 * successfully isolated from, zone-cached value, or the end of the
	 * zone when isolating for the first time. For looping we also need
	 * this pfn aligned down to the pageblock boundary, because we do
	 * block_start_pfn -= pageblock_nr_pages in the for loop.
	 * For ending point, take care when isolating in last pageblock of a
	 * a zone which ends in the middle of a pageblock.
	 * The low boundary is the end of the pageblock the migration scanner
	 * is using.
	 */
	/* 获取开始扫描页框所在的pageblock，并且设置为此pageblock的最后一个页框或者管理区最后一个页框 */
	isolate_start_pfn = cc->free_pfn;
	block_start_pfn = pageblock_start_pfn(cc->free_pfn);
	block_end_pfn = min(block_start_pfn + pageblock_nr_pages,
						zone_end_pfn(zone));
    /* 按pageblock_nr_pages对齐，low_pfn保存的是可迁移页框扫描所在的页框号，但是这里有可能migrate_pfn == free_pfn */
	low_pfn = pageblock_end_pfn(cc->migrate_pfn);

	/*
	 * Isolate free pages until enough are available to migrate the
	 * pages on cc->migratepages. We stop searching if the migrate
	 * and free page scanners meet or enough free pages are isolated.
	 */
    /* 先决条件如下：这个过程与isolate_migratepages()函数的过程是刚好相反的，migratepage是从前向后，freeapges是从后向前
     * 开始扫描空闲页框，从管理区最后一个pageblock向migrate_pfn所在的pageblock扫描
     * block_start_pfn是pageblock开始页框号
     * block_end_pfn是pageblock结束页框号
     */
    /* 循环条件：方向是从后先前，所以这里block_start_pfn -= pageblock_nr_pages,
     * 扫描到low_pfn所在pageblokc或者其后一个pageblock，low_pfn是low_pfn保存的是可迁移页框扫描所在的页框号，并按照pageblock_nr_pages对齐。
     * 当 block_start_pfn >= low_pfn时说明migratepage扫描类型pageblock地址小于freepages类型pageblock说明migratepage与freepage还未相遇
     * 如果block_start_pfn   <  low_pfn则说明migratepage的pageblock 地址已经超过了freepage的pageblock地址，则整个扫描过程结束
     * block_end_pfn = block_start_pfn当前pageblock的起始地址变成了上一个pageblock的结束地址
     * block_start_pfn -= pageblock_nr_pages,此时block_start_pfn变成上一个pageblock的起始地址，
     * 后将block_start_pfn上一个pageblock的起始地址赋给isolate_start_pfn开始扫描
     * migratepage— —— —— —— > < ———— ———— ———— freepages从后向前
     */
	for (; block_start_pfn >= low_pfn;
				block_end_pfn = block_start_pfn,
				block_start_pfn -= pageblock_nr_pages,
				isolate_start_pfn = block_start_pfn) {
		/*
		 * This can iterate a massively long zone without finding any
		 * suitable migration targets, so periodically check if we need
		 * to schedule, or even abort async compaction.
		 */
		/* 由于需要扫描很多页框，所以这里做个检查，执行时间过长则睡眠，一般是32个1024 pageblock页框休眠一下，break扫描
		 * 异步的情况还需要判断运行进程是否需要被调度 */
		if (!(block_start_pfn % (SWAP_CLUSTER_MAX * pageblock_nr_pages))
						&& compact_should_abort(cc))
			break;

         /* 检查block_start_pfn和block_end_pfn，如果没问题，返回block_start_pfn所指的页描述符，也就是pageblock第一页描述符 */
		page = pageblock_pfn_to_page(block_start_pfn, block_end_pfn,
									zone);
		if (!page)
			continue;

		/* Check the block is suitable for migration */
        /* 如果处于伙伴系统中，它所代表的这段连续页框的order值必须小于pageblock的order值
         * 如果不是MIGRATE_MOVABLE或MIGRATE_CMA类型则return false 则continue跳过这段页框块
         * 如果是MIGRATE_MOVABLE或MIGRATE_CMA类型则return true则说明pageblock时存在migrate type的则继续向下执行
         */
		if (!suitable_migration_target(cc, page))
			continue;

		/* If isolation recently failed, do not retry */
         /* 检查cc中是否标记了即使pageblock标记了跳过也对pageblock进行扫描，并且检查此pageblock是否被标记为跳过 */
		if (!isolation_suitable(cc, page))
			continue;

		/* Found a block suitable for isolating free pages from. */
        /* 扫描从isolate_start_pfn到block_end_pfn的空闲页框，并把它们放入到freelist中，返回此pageblock中总共获得的空闲页框数量 
         * 第一轮扫描可能会跳过，应该第一次isolate_start_pfn是等于zone最后一个页框的
         */
		isolate_freepages_block(cc, &isolate_start_pfn, block_end_pfn,
					freelist, false);

		/*
		 * If we isolated enough freepages, or aborted due to lock
		 * contention, terminate.
		 */
		/* 如果cc->nr_freepages >= cc->nr_migratepages且contended说明freepage与migratepage还未相遇，则进入判断当中进一步分析确认 */
		if ((cc->nr_freepages >= cc->nr_migratepages)
							|| cc->contended) {
			/* 如果isolate_start_pfn >= block_end_pfn 说明freepages   扫描的pageblock还为与migratepage扫描的pageblock相遇
			 * isolate_start_pfn = block_start_pfn - pageblock_nr_pages; 此时isolate_start_pfn就是向前一个pageblock的起始freepage扫描地址
			 * 也就是下一个freepage扫描的pageblock的起始页框
			 * 负责如果isolate_start_pfn < block_end_pfn则说名migratepage已经超过了freepage，则break 结束扫描
			 */
			if (isolate_start_pfn >= block_end_pfn) {
				/*
				 * Restart at previous pageblock if more
				 * freepages can be isolated next time.
				 */
				isolate_start_pfn =
					block_start_pfn - pageblock_nr_pages;
			}
			break;
		} else if (isolate_start_pfn < block_end_pfn) {
			/*
			 * If isolation failed early, do not continue
			 * needlessly.
			 */
			break;
		}
	}

	/* __isolate_free_page() does not map the pages */
     /* 设置页表项，设置为内核使用 */
	map_pages(freelist);

	/*
	 * Record where the free scanner will restart next time. Either we
	 * broke from the loop and set isolate_start_pfn based on the last
	 * call to isolate_freepages_block(), or we met the migration scanner
	 * and the loop terminated due to isolate_start_pfn < low_pfn
	 */
	cc->free_pfn = isolate_start_pfn;
}

/*
 * This is a migrate-callback that "allocates" freepages by taking pages
 * from the isolated freelists in the block we are migrating to.
 */
static struct page *compaction_alloc(struct page *migratepage,
					unsigned long data,
					int **result)
{
	struct compact_control *cc = (struct compact_control *)data;
	struct page *freepage;

	/*
	 * Isolate free pages if necessary, and if we are not aborting due to
	 * contention.
	 */
	/* 如果cc中的空闲页框链表为空 */
	if (list_empty(&cc->freepages)) {
         /* 并且cc->contended没有记录错误代码 */
		if (!cc->contended)
			isolate_freepages(cc);/* 从cc->free_pfn开始向前获取空闲页 */

		if (list_empty(&cc->freepages))
			return NULL;
	}
     /* 从cc->freepages链表中拿出一个空闲page */
	freepage = list_entry(cc->freepages.next, struct page, lru);
    /* 将该page从lru当中删除 */
	list_del(&freepage->lru);
	cc->nr_freepages--;
    /* 返回空闲页框 */
	return freepage;
}

/*
 * This is a migrate-callback that "frees" freepages back to the isolated
 * freelist.  All pages on the freelist are from the same zone, so there is no
 * special handling needed for NUMA.
 */
static void compaction_free(struct page *page, unsigned long data)
{
	struct compact_control *cc = (struct compact_control *)data;

	list_add(&page->lru, &cc->freepages);
	cc->nr_freepages++;
}

/* possible outcome of isolate_migratepages */
typedef enum {
	ISOLATE_ABORT,		/* Abort compaction now */
	ISOLATE_NONE,		/* No pages isolated, continue scanning */
	ISOLATE_SUCCESS,	/* Pages isolated, migrate */
} isolate_migrate_t;

/*
 * Allow userspace to control policy on scanning the unevictable LRU for
 * compactable pages.
 */
int sysctl_compact_unevictable_allowed __read_mostly = 1;

/*
 * Isolate all pages that can be migrated from the first suitable block,
 * starting at the block pointed to by the migrate scanner pfn within
 * compact_control.
 */
/*
 * 从cc->migrate_pfn(保存的是扫描可移动页框指针所在的页框号)开始到第一个获取到可移动页框的pageblock结束，获取可移动页框，并放入到cc->migratepages
 */
static isolate_migrate_t isolate_migratepages(struct zone *zone,
					struct compact_control *cc)
{
	unsigned long block_start_pfn;
	unsigned long block_end_pfn;
	unsigned long low_pfn;
	struct page *page;
     /* 保存同步/异步方式，只有异步的情况下能进行移动页框ISOLATE_ASYNC_MIGRATE */
	const isolate_mode_t isolate_mode =
		(sysctl_compact_unevictable_allowed ? ISOLATE_UNEVICTABLE : 0) |
		(cc->mode != MIGRATE_SYNC ? ISOLATE_ASYNC_MIGRATE : 0);

	/*
	 * Start at where we last stopped, or beginning of the zone as
	 * initialized by compact_zone()
	 */
	/* 扫描的起始页框，相当于start_pfn */
	low_pfn = cc->migrate_pfn;
    /* 通过pageblock_start_pfn函数将low_pfn转化为pageblock的起始页框 */
	block_start_pfn = pageblock_start_pfn(low_pfn);
    /* 如果pageblock的start_pfn的起始页框小于zone的起始页框,则更新block_start_pfn将zone->zone_start_pfn赋值给该变量 */
	if (block_start_pfn < zone->zone_start_pfn)
		block_start_pfn = zone->zone_start_pfn;

	/* Only scan within a pageblock boundary */
    /* 获取pageblock的结束页框地址block_end_pfn */
	block_end_pfn = pageblock_end_pfn(low_pfn);

	/*
	 * Iterate over whole pageblocks until we find the first suitable.
	 * Do not cross the free scanner.
	 */
	/*
	 * 如果block_end_pfn <= cc->free_pfn，
	 * low_pfn = block_end_pfn,    将pageblock的结束地址赋给low_pfn
	 * block_start_pfn = block_end_pfn,  将block_end_pfn赋给block_start_pfn
	 * block_end_pfn += pageblock_nr_pages 将block_end_pfn + pageblock_nr_pages（pageblock的大小）
	 * 这样就可以实现zone当中的pageblock的相互链接，将当前pageblock的end_pfn作为下一个pageblock的start_pfn，同时
	 * block_end_pfn + pageblock_nr_pages就是下一个pageblock的end_pfn，这样就能够循环的检测下去。
     */
	for (; block_end_pfn <= cc->free_pfn;
			low_pfn = block_end_pfn,
			block_start_pfn = block_end_pfn,
			block_end_pfn += pageblock_nr_pages) {

		/*
		 * This can potentially iterate a massively long zone with
		 * many pageblocks unsuitable, so periodically check if we
		 * need to schedule, or even abort async compaction.
		 */
		 /* 由于需要扫描很多页框，所以这里做个检查，执行时间过长则睡眠，一般是32个1024页框休眠一下，异步的情况还需要判断运行进程是否需要被调度 */
		if (!(low_pfn % (SWAP_CLUSTER_MAX * pageblock_nr_pages))
						&& compact_should_abort(cc))
			break;
        
         /* 获取第一个页框，需要检查是否属于此zone */
		page = pageblock_pfn_to_page(block_start_pfn, block_end_pfn,
									zone);
		if (!page)//如果page为空NULL怎continue 检测下一个pageblock
			continue;

		/* If isolation recently failed, do not retry */
        /* 获取页框的PB_migrate_skip标志，如果设置了则跳过这个1024个页框 */
		if (!isolation_suitable(cc, page))
			continue;

		/*
		 * For async compaction, also only scan in MOVABLE blocks.
		 * Async compaction is optimistic to see if the minimum amount
		 * of work satisfies the allocation.
		 */
		 /* 以异步直接压缩模式运行时，如果page迁移类型与pageblock请求的迁移类型不同，则continue跳过该pageblock。
		  * 但是，在同步（migrate_sync *）方法或手动或kcompactd请求的情况下，它总是返回true以尝试无条件地隔离相应的块。
		  */
		if (!suitable_migration_source(cc, page))
			continue;

		/* Perform the isolation */
        /* 执行完隔离，将low_pfn到end_pfn中正在使用的页框从zone->lru中取出来，返回的是扫描到的可移动页的页框号
         * 而UNMOVABLE类型的页框是不会处于lru链表中的，所以所有不在lru链表中的页都会被跳过
         * 返回的是扫描到的最后的页
         */
		low_pfn = isolate_migratepages_block(cc, low_pfn,
						block_end_pfn, isolate_mode);

		if (!low_pfn || cc->contended)
			return ISOLATE_ABORT;

		/*
		 * Either we isolated something and proceed with migration. Or
		 * we failed and compact_zone should decide if we should
		 * continue or not.
		 */
		/* 跳出，说明这里如果成功只会扫描一个pageblock */
		break;
	}

	/* Record where migration scanner will be restarted. */
    /* 更新可移动页框的页框号 */
	cc->migrate_pfn = low_pfn;
    /* 判断pageblock隔离的状态，是成功还是失败 */
	return cc->nr_migratepages ? ISOLATE_SUCCESS : ISOLATE_NONE;
}

/*
 * order == -1 is expected when compacting via
 * /proc/sys/vm/compact_memory
 */
static inline bool is_via_compact_memory(int order)
{
	return order == -1;
}

static enum compact_result __compact_finished(struct zone *zone,
						struct compact_control *cc)
{
	unsigned int order;
    /* 获取zone的移动类型 */
	const int migratetype = cc->migratetype;

	if (cc->contended || fatal_signal_pending(current))
		return COMPACT_CONTENDED;

	/* Compaction run completes if the migrate and free scanner meet */
    /* 当cc->free_pfn <= cc->migrate_pfn空闲扫描于可移动页面扫描相遇则说明zone碎片扫描压缩完成 */
	if (compact_scanners_met(cc)) {
		/* Let the next compaction start anew. */
        /* 重置压缩扫描起始地址于结束地址的位置 */
		reset_cached_positions(zone);

		/*
		 * Mark that the PG_migrate_skip information should be cleared
		 * by kswapd when it goes to sleep. kcompactd does not set the
		 * flag itself as the decision to be clear should be directly
		 * based on an allocation request.
		 */
		/* 如果是直接压缩模式则设置compact_blockskip_flush = true，清除PG_migrate_skip的skip属性 */
		if (cc->direct_compaction)
			zone->compact_blockskip_flush = true;
        /*
         * 如果whole_zone = 1说明zone是从头开始扫描，扫描zone整个区域 return COMPACT_COMPLETE，表示zone扫描完成
         * 如果whole_zone = 0说明zone是从局部开始扫描的，也就是在zone的更新的free_page或者是migrate_page当中扫描
         * 也就是也就是局部的pageblock的扫描，return COMPACT_PARTIAL_SKIPPED表示跳过该pageblock，扫描下一个pageblock
         */
		if (cc->whole_zone)
			return COMPACT_COMPLETE;
		else
			return COMPACT_PARTIAL_SKIPPED;
	}
    /* 执行压缩时，将返回COMPACT_CONTINUE以强制压缩整个块，这个于手动模式有关
     * echo 1> /proc/sys/vm/compact_memory
     */
	if (is_via_compact_memory(cc->order))
		return COMPACT_CONTINUE;
    /* 如果扫描完成，则进入判断当中，做进一步判断验证 */
	if (cc->finishing_block) {
		/*
		 * We have finished the pageblock, but better check again that
		 * we really succeeded.
		 */
		/* 再次检查迁移扫描程序与pageblock是否对齐，如果对齐则说明页面压缩已经完成重置cc->finishing_block = false
		 * 如果没有对齐则，并返回COMPACT_CONTINUE以继续扫描进行zone的页面扫描压缩操作
		 */ 
		if (IS_ALIGNED(cc->migrate_pfn, pageblock_nr_pages))
			cc->finishing_block = false;
		else
			return COMPACT_CONTINUE;
	}

	/* Direct compactor: Is a suitable page free? */
    /*
     * 从当前order开始扫描，order -> MAX_ORDER进行，
     */
	for (order = cc->order; order < MAX_ORDER; order++) {
        /* 根据order获取free_area    */
		struct free_area *area = &zone->free_area[order];
		bool can_steal;

		/* Job done if page is free of the right migratetype */
        /* 如果该area->free_list[migratetype])不为NULL，不为空则COMPACT_SUCCESS压缩扫描成功 */
		if (!list_empty(&area->free_list[migratetype]))
			return COMPACT_SUCCESS;
        /* 如果定义了CONFIG_CMA如果移动类型为MIGRATE_MOVABLE可移动类型，且area->free_list[MIGRATE_CMA])不为空则return                COMPACT_SUCCESS */
#ifdef CONFIG_CMA
		/* MIGRATE_MOVABLE can fallback on MIGRATE_CMA */
		if (migratetype == MIGRATE_MOVABLE &&
			!list_empty(&area->free_list[MIGRATE_CMA]))
			return COMPACT_SUCCESS;
#endif
		/*
		 * Job done if allocation would steal freepages from
		 * other migratetype buddy lists.
		 */
		/* 如果area->free_list[migratetype]以及area->free_list[MIGRATE_CMA])均为空则取对应的migratetype的fallback当中寻找合适可用的page
		 * 判断是否能够完成页面的压缩。
		 */
		if (find_suitable_fallback(area, order, migratetype,
						true, &can_steal) != -1) {

			/* movable pages are OK in any pageblock */
            /* 如果可移动类型为MIGRATE_MOVABLE则直接return COMPACT_SUCESS
             * 说明只要是可以移动的page都可用作页面压缩功能。
             */
			if (migratetype == MIGRATE_MOVABLE)
				return COMPACT_SUCCESS;

			/*
			 * We are stealing for a non-movable allocation. Make
			 * sure we finish compacting the current pageblock
			 * first so it is as free as possible and we won't
			 * have to steal another one soon. This only applies
			 * to sync compaction, as async compaction operates
			 * on pageblocks of the same migratetype.
			 */
			 /* 如果正在执行aync异步压缩，或者如果迁移扫描程序已完成一页代码块，则返回COMPACT_SUCCESS */
			if (cc->mode == MIGRATE_ASYNC ||
					IS_ALIGNED(cc->migrate_pfn,
							pageblock_nr_pages)) {
				return COMPACT_SUCCESS;
			}
            /* 如果fallback当中没有找到合适可用的page则设置cc->finishing_block = true;return COMPACT_CONTINUE zone还需要继续扫描，
             * skip到下一个pageblock或者是下一个zone
             */
			cc->finishing_block = true;
			return COMPACT_CONTINUE;
		}
	}
    /* 如果从order ->   max_order都没有找到可用的page用作直接的页面迁移压缩则return COMPACT_NO_SUITABLE_PAGE表明没有可用的页面用于压缩                           */
	return COMPACT_NO_SUITABLE_PAGE;
}

static enum compact_result compact_finished(struct zone *zone,
			struct compact_control *cc)
{
	int ret;

	ret = __compact_finished(zone, cc);
	trace_mm_compaction_finished(zone, cc->order, ret);
	if (ret == COMPACT_NO_SUITABLE_PAGE)
		ret = COMPACT_CONTINUE;

	return ret;
}

/*
 * compaction_suitable: Is this suitable to run compaction on this zone now?
 * Returns
 *   COMPACT_SKIPPED  - If there are too few free pages for compaction
 *   COMPACT_SUCCESS  - If the allocation would succeed without compaction
 *   COMPACT_CONTINUE - If compaction should run now
 */
static enum compact_result __compaction_suitable(struct zone *zone, int order,
					unsigned int alloc_flags,
					int classzone_idx,
					unsigned long wmark_target)
{
	unsigned long watermark;
    /*
     * 如果echo 1 > /proc/sys/vm/compact_memory手动请求释放强制进行内存碎片整理则return COMPACT_CONTINUE
     */
	if (is_via_compact_memory(order))
		return COMPACT_CONTINUE;
    /* 获取请求内存压缩zone区域的watermask内存水印，也就是zone zone_watermark[min]  最小内存界限  */
	watermark = zone->watermark[alloc_flags & ALLOC_WMARK_MASK];
	/*
	 * If watermarks for high-order allocation are already met, there
	 * should be no need for compaction at all.
	 */
	/*
	 * 如果zone的freepage > watermask + zone->lowmem_reserve[classzone_idx]保留区域则说明该区域可以进行内存分配
	 * 所以直接return compact_success 结束内存压缩整理，负责说明zone当中空闲的内存不足以满足一次性分配2^order个page,
	 * 继续向下做内存碎片整理
     */
	if (zone_watermark_ok(zone, order, watermark, classzone_idx,
								alloc_flags))
		return COMPACT_SUCCESS;

	/*
	 * Watermarks for order-0 must be met for compaction to be able to
	 * isolate free pages for migration targets. This means that the
	 * watermark and alloc_flags have to match, or be more pessimistic than
	 * the check in __isolate_free_page(). We don't use the direct
	 * compactor's alloc_flags, as they are not relevant for freepage
	 * isolation. We however do use the direct compactor's classzone_idx to
	 * skip over zones where lowmem reserves would prevent allocation even
	 * if compaction succeeds.
	 * For costly orders, we require low watermark instead of min for
	 * compaction to proceed to increase its chances.
	 * ALLOC_CMA is used, as pages in CMA pageblocks are considered
	 * suitable migration targets
	 */
	/*
	 * 如果order > PAGE_ALLOC_COSTLY_ORDER则watermask = low_wmark_pages(zone) 
	 * 如果order <= PAGE_ALLOC_COSTLY_ORDER则watermask =  min_wmark_pages(zone)
	 * 同时这个基础上watermask = watermask + 2*2^order 作为zone的最低水位赋值给zone_watermark[min]
     */
	watermark = (order > PAGE_ALLOC_COSTLY_ORDER) ?
				low_wmark_pages(zone) : min_wmark_pages(zone);
	watermark += compact_gap(order);
    /*
     * 使用ALLOC_CMA从CMA当中获取内存page，如果zone freepages < min + z->lowmem_reserve[classzone_idx] return false
     * 则说明zone的空闲页面不足，无法满足order的页面内存分配请求，说明zone当中根本就没有足够的page 所以就不在这个zone当中做内存的整理直接skip跳过就可以了
     * 如果return true则说明存在足够的内存可供使用则return compact_continue 继续扫描这个zone，
     */
	if (!__zone_watermark_ok(zone, 0, watermark, classzone_idx,
						ALLOC_CMA, wmark_target))
		return COMPACT_SKIPPED;

	return COMPACT_CONTINUE;
}
/* 判断该zone是否可以做内存碎片压缩整理 */
enum compact_result compaction_suitable(struct zone *zone, int order,
					unsigned int alloc_flags,
					int classzone_idx)
{
	enum compact_result ret;
	int fragindex;
    /*
     * 根据watermask判断zone中离散的page是否满足2^order的内存分配请求，如果满足则继续对zone进行内存的compact整理zone的内存碎片
     * 说明该zone时可以做内存碎片的压缩整理的。
     */
	ret = __compaction_suitable(zone, order, alloc_flags, classzone_idx,
				    zone_page_state(zone, NR_FREE_PAGES));
	/*
	 * fragmentation index determines if allocation failures are due to
	 * low memory or external fragmentation
	 *
	 * index of -1000 would imply allocations might succeed depending on
	 * watermarks, but we already failed the high-order watermark check
	 * index towards 0 implies failure is due to lack of memory
	 * index towards 1000 implies failure is due to fragmentation
	 *
	 * Only compact if a failure would be due to fragmentation. Also
	 * ignore fragindex for non-costly orders where the alternative to
	 * a successful reclaim/compaction is OOM. Fragindex and the
	 * vm.extfrag_threshold sysctl is meant as a heuristic to prevent
	 * excessive compaction for costly orders, but it should not be at the
	 * expense of system stability.
	 */
	/* 如果return返回值为COMPACT_CONTINUE，且order        > PAGE_ALLOC_COSTLY_ORDER(3)则进入一下判断当中 */
	if (ret == COMPACT_CONTINUE && (order > PAGE_ALLOC_COSTLY_ORDER)) {
        /* 
         * 为了确定zone区域是否执行压缩，找到所请求区域zone和顺序的碎片系数。
         * 如果碎片系数值返回-1000，则存在要分配的页面，因此不需要压缩。
         * 在其他情况下，该值在0到500的范围内，并且如果它小于sysctl_extfrag_threshold，则直接return COMPACT_NOT_SUITABLE_ZONE不执行压缩
         */
		fragindex = fragmentation_index(zone, order);
		if (fragindex >= 0 && fragindex <= sysctl_extfrag_threshold)
			ret = COMPACT_NOT_SUITABLE_ZONE;
	}

	trace_mm_compaction_suitable(zone, order, ret);
	if (ret == COMPACT_NOT_SUITABLE_ZONE)
		ret = COMPACT_SKIPPED;

	return ret;
}

bool compaction_zonelist_suitable(struct alloc_context *ac, int order,
		int alloc_flags)
{
	struct zone *zone;
	struct zoneref *z;

	/*
	 * Make sure at least one zone would pass __compaction_suitable if we continue
	 * retrying the reclaim.
	 */
	for_each_zone_zonelist_nodemask(zone, z, ac->zonelist, ac->high_zoneidx,
					ac->nodemask) {
		unsigned long available;
		enum compact_result compact_result;

		/*
		 * Do not consider all the reclaimable memory because we do not
		 * want to trash just for a single high order allocation which
		 * is even not guaranteed to appear even if __compaction_suitable
		 * is happy about the watermark check.
		 */
		available = zone_reclaimable_pages(zone) / order;
		available += zone_page_state_snapshot(zone, NR_FREE_PAGES);
		compact_result = __compaction_suitable(zone, order, alloc_flags,
				ac_classzone_idx(ac), available);
		if (compact_result != COMPACT_SKIPPED)
			return true;
	}

	return false;
}

static enum compact_result compact_zone(struct zone *zone, struct compact_control *cc)
{
	enum compact_result ret;
     /* 管理区开始页框号 */
	unsigned long start_pfn = zone->zone_start_pfn;
     /* 管理区结束页框号 */
	unsigned long end_pfn = zone_end_pfn(zone);
     /* 
     * 同步还是异步 
     * 同步为true 1 ，异步为false 0
     * 轻同步和同步都是同步
     */ 
	const bool sync = cc->mode != MIGRATE_ASYNC;

	/*
	 * These counters track activities during zone compaction.  Initialize
	 * them before compacting a new zone.
	 */
	cc->total_migrate_scanned = 0;
	cc->total_free_scanned = 0;
	cc->nr_migratepages = 0;
	cc->nr_freepages = 0;
    /* 
     * 初始化freepages空闲页面列表
     * 初始化migratepages可移动页面列表
     */
	INIT_LIST_HEAD(&cc->freepages);
	INIT_LIST_HEAD(&cc->migratepages);
    /* 获取可进行移动的页框类型(__GFP_RECLAIMABLE、__GFP_MOVABLE) */
	cc->migratetype = gfpflags_to_migratetype(cc->gfp_mask);
    /* 根据传入的cc->order判断此次整理是否能够进行，主要是因为整理需要部分内存，这里面会判断内存是否足够 */
	ret = compaction_suitable(zone, cc->order, cc->alloc_flags,
							cc->classzone_idx);
	/* Compaction is likely to fail */
    /* zone碎片足够compact_sucess,或者是内存数量不足以进行内存碎片整理skip */
	if (ret == COMPACT_SUCCESS || ret == COMPACT_SKIPPED)
		return ret;

	/* huh, compaction_suitable is returning something unexpected */
    /* 可以进行内存碎片整理 */
	VM_BUG_ON(ret != COMPACT_CONTINUE);

	/*
	 * Clear pageblock skip if there were failures recently and compaction
	 * is about to be retried after being deferred.
	 */
	 /*
	  * zone的order < zone->compact_order_failed则return false此时不会执行__reset_isolation_suitable，该zone还是可以在做碎片整理推迟
	  * 如果zone的内存碎片整理推迟次数compact_considered超过了最大的推迟阈值COMPACT_MAX_DEFER_SHIFT则return true，该zone必须接受碎片整理
	  * 执行__reset_isolation_suitable(zone);
      */
     /*
      * 关于__reset_isolation_suitable(zone)分析：
      * 表明该zone当中的pageblock的skip属性被清空，必须接受碎片整理，应该是于kswap有关
      */
	if (compaction_restarting(zone, cc->order))
		__reset_isolation_suitable(zone);

	/*
	 * Setup to move all movable pages to the end of the zone. Used cached
	 * information on where the scanners should start (unless we explicitly
	 * want to compact the whole zone), but check that it is initialised
	 * by ensuring the values are within zone boundaries.
	 */
	/* 通过whole_zone判断zone是否需要全部接受扫描，从zone的头部到尾部全部接受扫描
	 * 如果whole_zone = 1 则migrate_pfn从start_pfn， free_pfn从end_pfn从尾部开始，向中间扫描，一般是zone第一次接受内存碎片扫描整理才会如此
	 * 根据compact_cached_migrate_pfn从此处开始，从最新也就是skip的位置扫描
	 */
	if (cc->whole_zone) {
		cc->migrate_pfn = start_pfn;
		cc->free_pfn = pageblock_start_pfn(end_pfn - 1);
	} else {
        /* 将可移动页框扫描起始页框号设为zone->compact_cached_migrate_pfn[sync/async] */
		cc->migrate_pfn = zone->compact_cached_migrate_pfn[sync];
        /* 空闲页框扫描起始页框号设置为zone->compact_cached_free_pfn */
		cc->free_pfn = zone->compact_cached_free_pfn;
        /* 检查cc->free_pfn，如果空闲页框扫描起始页框不在zone的范围内，则将空闲页框扫描起始页框设置为zone的最后一个页框
         * 并且也会将zone->compact_cached_free_pfn设置为zone的最后一个页框
         */
		if (cc->free_pfn < start_pfn || cc->free_pfn >= end_pfn) {
			cc->free_pfn = pageblock_start_pfn(end_pfn - 1);
			zone->compact_cached_free_pfn = cc->free_pfn;
		}
        /* 检查cc->migrate_pfn，如果可移动页框扫描起始页框不在zone的范围内，则将可移动页框扫描起始页框设置为zone的第一个页框
         * 并且也会将zone->compact_cached_free_pfn设置为zone的第一个页框
         */
		if (cc->migrate_pfn < start_pfn || cc->migrate_pfn >= end_pfn) {
			cc->migrate_pfn = start_pfn;
			zone->compact_cached_migrate_pfn[0] = cc->migrate_pfn;
			zone->compact_cached_migrate_pfn[1] = cc->migrate_pfn;
		}
        /* 如果cc->migrate_pfn == start_pfn 则设置cc->whole_zone = true;zone在下次扫描中就必须从头开始扫描 */
		if (cc->migrate_pfn == start_pfn)
			cc->whole_zone = true;
	}

	cc->last_migrated_pfn = 0;

	trace_mm_compaction_begin(start_pfn, cc->migrate_pfn,
				cc->free_pfn, end_pfn, sync);
    /* 将处于pagevec中的页都放回原本所属的lru中， 这一步非常重要，对内存的合并压缩就是对lru的操作 */
	migrate_prep_local();
    
    /* 判断是否结束本次内存碎片整理 
     * 1.可移动页框扫描的位置是否已经超过了空闲页框扫描的位置，超过则结束整理，并且会重置zone->compact_cached_free_pfn和zone->compact_cached_migrate_pfn
     * 2.判断zone的空闲页框数量是否达到标准，如果没达到zone的low阀值标准则继续
     * 3.判断伙伴系统中是否有比order值大的空闲连续页框块，有则结束整理
     * 如果是管理员写入到/proc/sys/vm/compact_memory进行强制内存碎片整理的情况，则判断条件只有第1条
     */
	while ((ret = compact_finished(zone, cc)) == COMPACT_CONTINUE) {
		int err;
         /* 将可移动页(MOVABLE和CMA和RECLAIMABLE)从zone->lru隔离出来，存到cc->migratepages这个链表，一个一个pageblock进行扫描，当一个pageblock扫描成功获取到可移动页后就返回
         * 一次扫描最多32*1024个页框
         */
        /* 异步不处理RECLAIMABLE页，完成migrate pages的隔离迁移 */
		switch (isolate_migratepages(zone, cc)) {
		case ISOLATE_ABORT:
			ret = COMPACT_CONTENDED;
             /* 失败，把这些页放回到lru或者原来的地方 */
			putback_movable_pages(&cc->migratepages);
			cc->nr_migratepages = 0;
			goto out;
		case ISOLATE_NONE:
			/*
			 * We haven't isolated and migrated anything, but
			 * there might still be unflushed migrations from
			 * previous cc->order aligned block.
			 */
			goto check_drain;
		case ISOLATE_SUCCESS:
			;
		}
        /* 将隔离出来的页进行迁移，如果到这里，cc->migratepages中最多也只有一个pageblock的页框数量，并且这些页框都是可移动的 
         * 空闲页框会在compaction_alloc中获取
         * 也就是把隔离出来的一个pageblock中可移动页进行移动
         */
		err = migrate_pages(&cc->migratepages, compaction_alloc,
				compaction_free, (unsigned long)cc, cc->mode,
				MR_COMPACTION);

		trace_mm_compaction_migratepages(cc->nr_migratepages, err,
							&cc->migratepages);

		/* All pages were either migrated or will be released */
        /* 设置所有可移动页框为0 */                
		cc->nr_migratepages = 0;
		if (err) {
             /* 将剩余的可移动页框返回原来的位置 */
			putback_movable_pages(&cc->migratepages);
			/*
			 * migrate_pages() may return -ENOMEM when scanners meet
			 * and we want compact_finished() to detect it
			 */
			/* 如果err = ENOMEM,并且cc->free_pfn    > cc->migrate_pfn则直接跳转到out     */
			if (err == -ENOMEM && !compact_scanners_met(cc)) {
				ret = COMPACT_CONTENDED;
				goto out;
			}
			/*
			 * We failed to migrate at least one page in the current
			 * order-aligned block, so skip the rest of it.
			 */
			/*
			 * 如果请求直接压缩时，且为异步移动模式时，当前正在处理的迁移块将被跳过
             */
			if (cc->direct_compaction &&
						(cc->mode == MIGRATE_ASYNC)) {
				cc->migrate_pfn = block_end_pfn(
						cc->migrate_pfn - 1, cc->order);
				/* Draining pcplists is useless in this case */
				cc->last_migrated_pfn = 0;

			}
		}

check_drain:
		/*
		 * Has the migration scanner moved away from the previous
		 * cc->order aligned block where we migrated from? If yes,
		 * flush the pages that were freed, so that they can merge and
		 * compact_finished() can detect immediately if allocation
		 * would succeed.
		 */
		/* 如果order > 0并且最后一个迁移pfn在当前pageblock时存在的则进入判断当中                    */
		if (cc->order > 0 && cc->last_migrated_pfn) {
			int cpu;
            /* 更新该pageblock start_pfn扫描地址 */
			unsigned long current_block_start =
				block_start_pfn(cc->migrate_pfn, cc->order);
            /* 如果最后一个可移动的page的页框地址小于当前页框的地址则进入判断当中
             * 则当清空作为lru缓存的pagevecs时，合并的可能性会增加，并且compact_finished（）可以立即检测到是否没有分配成功。
             */
			if (cc->last_migrated_pfn < current_block_start) {
				cpu = get_cpu();
				lru_add_drain_cpu(cpu);
				drain_local_pages(zone);
				put_cpu();
				/* No more flushing until we migrate again */
				cc->last_migrated_pfn = 0;
			}
		}

	}

out:
	/*
	 * Release free pages and update where the free scanner should restart,
	 * so we don't leave any returned pages behind in the next attempt.
	 */
	/*
	 * 如果nr_freepages > 0 则将freepages剩余的空闲页面通过调用__free_page放回到buddy system当中
     */
	if (cc->nr_freepages > 0) {
         /* 将剩余的空闲页框放回伙伴系统 */
		unsigned long free_pfn = release_freepages(&cc->freepages);
        /* 释放完成后设置freepages = 0      */
		cc->nr_freepages = 0;
		VM_BUG_ON(free_pfn == 0);
		/* The cached pfn is always the first in a pageblock */
        /* 更新空闲页面的起始地址 */
		free_pfn = pageblock_start_pfn(free_pfn);
		/*
		 * Only go back, not forward. The cached pfn might have been
		 * already reset to zone end in compact_finished()
		 */
		/* 如果free_page >  compact_cached_free_pfn最后也给空闲页面页框地址，
		 * 则将free_pages设置给compact_cached_free_pfn跟新最后的空闲页框地址
 		 */
		if (free_pfn > zone->compact_cached_free_pfn)
			zone->compact_cached_free_pfn = free_pfn;
	}

	count_compact_events(COMPACTMIGRATE_SCANNED, cc->total_migrate_scanned);
	count_compact_events(COMPACTFREE_SCANNED, cc->total_free_scanned);

	trace_mm_compaction_end(start_pfn, cc->migrate_pfn,
				cc->free_pfn, end_pfn, sync, ret);

	return ret;
}

static enum compact_result compact_zone_order(struct zone *zone, int order,
		gfp_t gfp_mask, enum compact_priority prio,
		unsigned int alloc_flags, int classzone_idx)
{
	enum compact_result ret;
    /* 对compact_control结构体进行初始化操作 */
	struct compact_control cc = {
		.order = order,
		.gfp_mask = gfp_mask,
		.zone = zone,
		.mode = (prio == COMPACT_PRIO_ASYNC) ?
					MIGRATE_ASYNC :	MIGRATE_SYNC_LIGHT,
		.alloc_flags = alloc_flags,
		.classzone_idx = classzone_idx,
		.direct_compaction = true,
		.whole_zone = (prio == MIN_COMPACT_PRIORITY),
		.ignore_skip_hint = (prio == MIN_COMPACT_PRIORITY),
		.ignore_block_suitable = (prio == MIN_COMPACT_PRIORITY)
	};
    /* 内存碎片整理 */
	ret = compact_zone(zone, &cc);

	VM_BUG_ON(!list_empty(&cc.freepages));
	VM_BUG_ON(!list_empty(&cc.migratepages));

	return ret;
}

int sysctl_extfrag_threshold = 500;

/**
 * try_to_compact_pages - Direct compact to satisfy a high-order allocation
 * @gfp_mask: The GFP mask of the current allocation
 * @order: The order of the current allocation
 * @alloc_flags: The allocation flags of the current allocation
 * @ac: The context of current allocation
 * @mode: The migration mode for async, sync light, or sync migration
 *
 * This is the main entry point for direct page compaction.
 */
enum compact_result try_to_compact_pages(gfp_t gfp_mask, unsigned int order,
		unsigned int alloc_flags, const struct alloc_context *ac,
		enum compact_priority prio)
{
    /* 表示可以使用磁盘的IO操作 */
	int may_perform_io = gfp_mask & __GFP_IO;
	struct zoneref *z;
	struct zone *zone;
	enum compact_result rc = COMPACT_SKIPPED;

	/*
	 * Check if the GFP flags allow compaction - GFP_NOIO is really
	 * tricky context because the migration might require IO
	 */
	 /* 如果不能使用磁盘IO，则跳过本次整理，因为不使用IO有可能导致死锁 */
	if (!may_perform_io)
		return COMPACT_SKIPPED;

	trace_mm_compaction_try_to_compact_pages(order, gfp_mask, prio);

	/* Compact each zone in the list */
    /* 遍历zonelist当中的每一个zone 是否能够整理足够的空间供内存分配使用 */
	for_each_zone_zonelist_nodemask(zone, z, ac->zonelist, ac->high_zoneidx,
								ac->nodemask) {
		enum compact_result status;
        /* 
         * 如果prio优先级大于MIN_COMPACT_PRIORITY，且order < zone->compact_order_failed时是不需要continue跳过的该zone
         * zone->compact_considered是否小于1UL << zone->compact_defer_shift
         * 小于则推迟，并且zone->compact_considered++，也就是这个函数会主动去推迟此管理区的内存碎片整理
         * 本次请求的order值小于之前失败时的order值，那这次整理必须要进行
         * zone->compact_considered和zone->compact_defer_shift会只有在内存碎片整理完成后，从此zone获取到了连续的1 << order个页框的情况下会重置为0
         */
		if (prio > MIN_COMPACT_PRIORITY
					&& compaction_deferred(zone, order)) {
			rc = max_t(enum compact_result, COMPACT_DEFERRED, rc);
			continue;
		}
        /* 对遍历到的zone进行内存碎片整理 */
		status = compact_zone_order(zone, order, gfp_mask, prio,
					alloc_flags, ac_classzone_idx(ac));
		rc = max(status, rc);
        /* 如果内存整理成功则进入compaction_defer_reset函数当中，完成
         * zone->compact_considered = 0;
		 * zone->compact_defer_shift = 0;重置操作
		 */
		/* The allocation should succeed, stop compacting */
		if (status == COMPACT_SUCCESS) {
			/*
			 * We think the allocation will succeed in this zone,
			 * but it is not certain, hence the false. The caller
			 * will repeat this with true if allocation indeed
			 * succeeds in this zone.
			 */
			/*
			 * 内存整理成功但是alloc_success = false 内存分配失败则更新zone的延迟的相关变量
			 * 此处是应为zone满足了内存整理的需求，但是并不明确内存是否能够分配成功所以就设置了alloc_success = false
             */
			compaction_defer_reset(zone, order, false);

			break;
		}
        /* 
         * 如果prio优先级不是异步整理操作，同时status 状态为COMPACT_COMPLETE、COMPACT_PARTIAL_SKIPPED
         * 代表compact_zone_order扫描完成或者是skip跳过该zone则进入判断当中执行defer_compaction更新zone推迟相关变量
         */
		if (prio != COMPACT_PRIO_ASYNC && (status == COMPACT_COMPLETE ||
					status == COMPACT_PARTIAL_SKIPPED))
			/*
			 * We think that allocation won't succeed in this zone
			 * so we defer compaction there. If it ends up
			 * succeeding after all, it will be reset.
			 */
			/* 提高内存碎片整理计数器的阀值，zone的内存碎片整理计数器阀值 ，也就是只有同步整理会增加推迟计数器的阀值
             * 重置zone->compact_considered = 0
             * 如果zone->compact_defer_shift < COMPACT_MAX_DEFER_SHIFT，那么zone->compact_defer_shift++
             * 如果order < zone->compact_order_failed，那么zone->compact_order_failed = order
             */
			defer_compaction(zone, order);

		/*
		 * We might have stopped compacting due to need_resched() in
		 * async compaction, or due to a fatal signal detected. In that
		 * case do not try further zones
		 */
		if ((prio == COMPACT_PRIO_ASYNC && need_resched())
					|| fatal_signal_pending(current))
			break;
	}

	return rc;
}


/* Compact all zones within a node */
static void compact_node(int nid)
{
	pg_data_t *pgdat = NODE_DATA(nid);
	int zoneid;
	struct zone *zone;
	struct compact_control cc = {
		.order = -1,
		.mode = MIGRATE_SYNC,
		.ignore_skip_hint = true,
		.whole_zone = true,
		.gfp_mask = GFP_KERNEL,
	};


	for (zoneid = 0; zoneid < MAX_NR_ZONES; zoneid++) {

		zone = &pgdat->node_zones[zoneid];
		if (!populated_zone(zone))
			continue;

		cc.zone = zone;

		compact_zone(zone, &cc);

		VM_BUG_ON(!list_empty(&cc.freepages));
		VM_BUG_ON(!list_empty(&cc.migratepages));
	}
}

/* Compact all nodes in the system */
static void compact_nodes(void)
{
	int nid;

	/* Flush pending updates to the LRU lists */
	lru_add_drain_all();

	for_each_online_node(nid)
		compact_node(nid);
}

/* The written value is actually unused, all memory is compacted */
int sysctl_compact_memory;

/*
 * This is the entry point for compacting all nodes via
 * /proc/sys/vm/compact_memory
 */
int sysctl_compaction_handler(struct ctl_table *table, int write,
			void __user *buffer, size_t *length, loff_t *ppos)
{
	if (write)
		compact_nodes();

	return 0;
}

int sysctl_extfrag_handler(struct ctl_table *table, int write,
			void __user *buffer, size_t *length, loff_t *ppos)
{
	proc_dointvec_minmax(table, write, buffer, length, ppos);

	return 0;
}

#if defined(CONFIG_SYSFS) && defined(CONFIG_NUMA)
static ssize_t sysfs_compact_node(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	int nid = dev->id;

	if (nid >= 0 && nid < nr_node_ids && node_online(nid)) {
		/* Flush pending updates to the LRU lists */
		lru_add_drain_all();

		compact_node(nid);
	}

	return count;
}
static DEVICE_ATTR(compact, S_IWUSR, NULL, sysfs_compact_node);

int compaction_register_node(struct node *node)
{
	return device_create_file(&node->dev, &dev_attr_compact);
}

void compaction_unregister_node(struct node *node)
{
	return device_remove_file(&node->dev, &dev_attr_compact);
}
#endif /* CONFIG_SYSFS && CONFIG_NUMA */

static inline bool kcompactd_work_requested(pg_data_t *pgdat)
{
	return pgdat->kcompactd_max_order > 0 || kthread_should_stop();
}

static bool kcompactd_node_suitable(pg_data_t *pgdat)
{
	int zoneid;
	struct zone *zone;
	enum zone_type classzone_idx = pgdat->kcompactd_classzone_idx;

	for (zoneid = 0; zoneid <= classzone_idx; zoneid++) {
		zone = &pgdat->node_zones[zoneid];

		if (!populated_zone(zone))
			continue;

		if (compaction_suitable(zone, pgdat->kcompactd_max_order, 0,
					classzone_idx) == COMPACT_CONTINUE)
			return true;
	}

	return false;
}

static void kcompactd_do_work(pg_data_t *pgdat)
{
	/*
	 * With no special task, compact all zones so that a page of requested
	 * order is allocatable.
	 */
	int zoneid;
	struct zone *zone;
	struct compact_control cc = {
		.order = pgdat->kcompactd_max_order,
		.classzone_idx = pgdat->kcompactd_classzone_idx,
		.mode = MIGRATE_SYNC_LIGHT,
		.ignore_skip_hint = true,
		.gfp_mask = GFP_KERNEL,

	};
	trace_mm_compaction_kcompactd_wake(pgdat->node_id, cc.order,
							cc.classzone_idx);
	count_compact_event(KCOMPACTD_WAKE);

	for (zoneid = 0; zoneid <= cc.classzone_idx; zoneid++) {
		int status;

		zone = &pgdat->node_zones[zoneid];
		if (!populated_zone(zone))
			continue;

		if (compaction_deferred(zone, cc.order))
			continue;

		if (compaction_suitable(zone, cc.order, 0, zoneid) !=
							COMPACT_CONTINUE)
			continue;

		if (kthread_should_stop())
			return;

		cc.zone = zone;
		status = compact_zone(zone, &cc);

		if (status == COMPACT_SUCCESS) {
			compaction_defer_reset(zone, cc.order, false);
		} else if (status == COMPACT_PARTIAL_SKIPPED || status == COMPACT_COMPLETE) {
			/*
			 * We use sync migration mode here, so we defer like
			 * sync direct compaction does.
			 */
			defer_compaction(zone, cc.order);
		}

		count_compact_events(KCOMPACTD_MIGRATE_SCANNED,
				     cc.total_migrate_scanned);
		count_compact_events(KCOMPACTD_FREE_SCANNED,
				     cc.total_free_scanned);

		VM_BUG_ON(!list_empty(&cc.freepages));
		VM_BUG_ON(!list_empty(&cc.migratepages));
	}

	/*
	 * Regardless of success, we are done until woken up next. But remember
	 * the requested order/classzone_idx in case it was higher/tighter than
	 * our current ones
	 */
	if (pgdat->kcompactd_max_order <= cc.order)
		pgdat->kcompactd_max_order = 0;
	if (pgdat->kcompactd_classzone_idx >= cc.classzone_idx)
		pgdat->kcompactd_classzone_idx = pgdat->nr_zones - 1;
}

void wakeup_kcompactd(pg_data_t *pgdat, int order, int classzone_idx)
{
	if (!order)
		return;

	if (pgdat->kcompactd_max_order < order)
		pgdat->kcompactd_max_order = order;

	if (pgdat->kcompactd_classzone_idx > classzone_idx)
		pgdat->kcompactd_classzone_idx = classzone_idx;

	/*
	 * Pairs with implicit barrier in wait_event_freezable()
	 * such that wakeups are not missed.
	 */
	if (!wq_has_sleeper(&pgdat->kcompactd_wait))
		return;

	if (!kcompactd_node_suitable(pgdat))
		return;

	trace_mm_compaction_wakeup_kcompactd(pgdat->node_id, order,
							classzone_idx);
	wake_up_interruptible(&pgdat->kcompactd_wait);
}

/*
 * The background compaction daemon, started as a kernel thread
 * from the init process.
 */
static int kcompactd(void *p)
{
	pg_data_t *pgdat = (pg_data_t*)p;
	struct task_struct *tsk = current;

	const struct cpumask *cpumask = cpumask_of_node(pgdat->node_id);

	if (!cpumask_empty(cpumask))
		set_cpus_allowed_ptr(tsk, cpumask);

	set_freezable();

	pgdat->kcompactd_max_order = 0;
	pgdat->kcompactd_classzone_idx = pgdat->nr_zones - 1;

	while (!kthread_should_stop()) {
		trace_mm_compaction_kcompactd_sleep(pgdat->node_id);
		wait_event_freezable(pgdat->kcompactd_wait,
				kcompactd_work_requested(pgdat));

		kcompactd_do_work(pgdat);
	}

	return 0;
}

/*
 * This kcompactd start function will be called by init and node-hot-add.
 * On node-hot-add, kcompactd will moved to proper cpus if cpus are hot-added.
 */
int kcompactd_run(int nid)
{
	pg_data_t *pgdat = NODE_DATA(nid);
	int ret = 0;

	if (pgdat->kcompactd)
		return 0;

	pgdat->kcompactd = kthread_run(kcompactd, pgdat, "kcompactd%d", nid);
	if (IS_ERR(pgdat->kcompactd)) {
		pr_err("Failed to start kcompactd on node %d\n", nid);
		ret = PTR_ERR(pgdat->kcompactd);
		pgdat->kcompactd = NULL;
	}
	return ret;
}

/*
 * Called by memory hotplug when all memory in a node is offlined. Caller must
 * hold mem_hotplug_begin/end().
 */
void kcompactd_stop(int nid)
{
	struct task_struct *kcompactd = NODE_DATA(nid)->kcompactd;

	if (kcompactd) {
		kthread_stop(kcompactd);
		NODE_DATA(nid)->kcompactd = NULL;
	}
}

/*
 * It's optimal to keep kcompactd on the same CPUs as their memory, but
 * not required for correctness. So if the last cpu in a node goes
 * away, we get changed to run anywhere: as the first one comes back,
 * restore their cpu bindings.
 */
static int kcompactd_cpu_online(unsigned int cpu)
{
	int nid;

	for_each_node_state(nid, N_MEMORY) {
		pg_data_t *pgdat = NODE_DATA(nid);
		const struct cpumask *mask;

		mask = cpumask_of_node(pgdat->node_id);

		if (cpumask_any_and(cpu_online_mask, mask) < nr_cpu_ids)
			/* One of our CPUs online: restore mask */
			set_cpus_allowed_ptr(pgdat->kcompactd, mask);
	}
	return 0;
}

static int __init kcompactd_init(void)
{
	int nid;
	int ret;

	ret = cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN,
					"mm/compaction:online",
					kcompactd_cpu_online, NULL);
	if (ret < 0) {
		pr_err("kcompactd: failed to register hotplug callbacks.\n");
		return ret;
	}

	for_each_node_state(nid, N_MEMORY)
		kcompactd_run(nid);
	return 0;
}
subsys_initcall(kcompactd_init)

#endif /* CONFIG_COMPACTION */
