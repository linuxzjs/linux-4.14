/*
 * Linux VM pressure
 *
 * Copyright 2012 Linaro Ltd.
 *		  Anton Vorontsov <anton.vorontsov@linaro.org>
 *
 * Based on ideas from Andrew Morton, David Rientjes, KOSAKI Motohiro,
 * Leonid Moiseichuk, Mel Gorman, Minchan Kim and Pekka Enberg.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/cgroup.h>
#include <linux/fs.h>
#include <linux/log2.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/vmstat.h>
#include <linux/eventfd.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/printk.h>
#include <linux/vmpressure.h>

/*
 * The window size (vmpressure_win) is the number of scanned pages before
 * we try to analyze scanned/reclaimed ratio. So the window is used as a
 * rate-limit tunable for the "low" level notification, and also for
 * averaging the ratio for medium/critical levels. Using small window
 * sizes can cause lot of false positives, but too big window size will
 * delay the notifications.
 *
 * As the vmscan reclaimer logic works with chunks which are multiple of
 * SWAP_CLUSTER_MAX, it makes sense to use it for the window size as well.
 *
 * TODO: Make the window size depend on machine size, as we do for vmstat
 * thresholds. Currently we set it to 512 pages (2MB for 4KB pages).
 */
static const unsigned long vmpressure_win = SWAP_CLUSTER_MAX * 16;

/*
 * These thresholds are used when we account memory pressure through
 * scanned/reclaimed ratio. The current values were chosen empirically. In
 * essence, they are percents: the higher the value, the more number
 * unsuccessful reclaims there were.
 */
static const unsigned int vmpressure_level_med = 60;
static const unsigned int vmpressure_level_critical = 95;

/*
 * When there are too little pages left to scan, vmpressure() may miss the
 * critical pressure as number of pages will be less than "window size".
 * However, in that case the vmscan priority will raise fast as the
 * reclaimer will try to scan LRUs more deeply.
 *
 * The vmscan logic considers these special priorities:
 *
 * prio == DEF_PRIORITY (12): reclaimer starts with that value
 * prio <= DEF_PRIORITY - 2 : kswapd becomes somewhat overwhelmed
 * prio == 0                : close to OOM, kernel scans every page in an lru
 *
 * Any value in this range is acceptable for this tunable (i.e. from 12 to
 * 0). Current value for the vmpressure_level_critical_prio is chosen
 * empirically, but the number, in essence, means that we consider
 * critical level when scanning depth is ~10% of the lru size (vmscan
 * scans 'lru_size >> prio' pages, so it is actually 12.5%, or one
 * eights).
 */
static const unsigned int vmpressure_level_critical_prio = ilog2(100 / 10);

static struct vmpressure *work_to_vmpressure(struct work_struct *work)
{
	return container_of(work, struct vmpressure, work);
}

static struct vmpressure *vmpressure_parent(struct vmpressure *vmpr)
{
	struct cgroup_subsys_state *css = vmpressure_to_css(vmpr);
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

	memcg = parent_mem_cgroup(memcg);
	if (!memcg)
		return NULL;
	return memcg_to_vmpressure(memcg);
}

enum vmpressure_levels {
	VMPRESSURE_LOW = 0,
	VMPRESSURE_MEDIUM,
	VMPRESSURE_CRITICAL,
	VMPRESSURE_NUM_LEVELS,
};

enum vmpressure_modes {
	VMPRESSURE_NO_PASSTHROUGH = 0,
	VMPRESSURE_HIERARCHY,
	VMPRESSURE_LOCAL,
	VMPRESSURE_NUM_MODES,
};
/* low level正常回收，medium level就是开始swaping，critical就是内存严重不足启动OOM释放内存 */
static const char * const vmpressure_str_levels[] = {
	[VMPRESSURE_LOW] = "low",
	[VMPRESSURE_MEDIUM] = "medium",
	[VMPRESSURE_CRITICAL] = "critical",
};

static const char * const vmpressure_str_modes[] = {
	[VMPRESSURE_NO_PASSTHROUGH] = "default",
	[VMPRESSURE_HIERARCHY] = "hierarchy",
	[VMPRESSURE_LOCAL] = "local",
};
/* 根据压力的值，得出内存回收的压力等级 */
static enum vmpressure_levels vmpressure_level(unsigned long pressure)
{
	if (pressure >= vmpressure_level_critical)
		return VMPRESSURE_CRITICAL;
	else if (pressure >= vmpressure_level_med)
		return VMPRESSURE_MEDIUM;
	return VMPRESSURE_LOW;
}

static enum vmpressure_levels vmpressure_calc_level(unsigned long scanned,
						    unsigned long reclaimed)
{
	unsigned long scale = scanned + reclaimed;
	unsigned long pressure = 0;

	/*
	 * reclaimed can be greater than scanned for things such as reclaimed
	 * slab pages. shrink_node() just adds reclaimed pages without a
	 * related increment to scanned pages.
	 */
	/* 如果回收的内存大于扫描的内存数量说明存在足够的内存足够的，正常的内存回收即可，所以此时直接goto                               out此时内存回收的压力为0 */
	if (reclaimed >= scanned)
		goto out;
	/*
	 * We calculate the ratio (in percents) of how many pages were
	 * scanned vs. reclaimed in a given time frame (window). Note that
	 * time is in VM reclaimer's "ticks", i.e. number of pages
	 * scanned. This makes it possible to set desired reaction time
	 * and serves as a ratelimit.
	 */
	/* 完成内存压力计算 */
	pressure = scale - (reclaimed * scale / scanned);
	pressure = pressure * 100 / scale;

out:
	pr_debug("%s: %3lu  (s: %lu  r: %lu)\n", __func__, pressure,
		 scanned, reclaimed);
    /* 根据压力的值返回内存回收压力等级 */
	return vmpressure_level(pressure);
}

struct vmpressure_event {
	struct eventfd_ctx *efd;
	enum vmpressure_levels level;
	enum vmpressure_modes mode;
	struct list_head node;
};

static bool vmpressure_event(struct vmpressure *vmpr,
			     const enum vmpressure_levels level,
			     bool ancestor, bool signalled)
{
	struct vmpressure_event *ev;
	bool ret = false;

	mutex_lock(&vmpr->events_lock);
	list_for_each_entry(ev, &vmpr->events, node) {
		if (ancestor && ev->mode == VMPRESSURE_LOCAL)
			continue;
		if (signalled && ev->mode == VMPRESSURE_NO_PASSTHROUGH)
			continue;
		if (level < ev->level)
			continue;
		eventfd_signal(ev->efd, 1);
		ret = true;
	}
	mutex_unlock(&vmpr->events_lock);

	return ret;
}

static void vmpressure_work_fn(struct work_struct *work)
{
    /* 获取vmpr工作队列的值 */
	struct vmpressure *vmpr = work_to_vmpressure(work);
	unsigned long scanned;
	unsigned long reclaimed;
	enum vmpressure_levels level;
	bool ancestor = false;
	bool signalled = false;

	spin_lock(&vmpr->sr_lock);
	/*
	 * Several contexts might be calling vmpressure(), so it is
	 * possible that the work was rescheduled again before the old
	 * work context cleared the counters. In that case we will run
	 * just after the old work returns, but then scanned might be zero
	 * here. No need for any locks here since we don't care if
	 * vmpr->reclaimed is in sync.
	 */
	/* 将tree_scanned赋值给scanned，获取扫描page的页数，如果扫描页数为0则直接return */
	scanned = vmpr->tree_scanned;
	if (!scanned) {
		spin_unlock(&vmpr->sr_lock);
		return;
	}
    /* 将tree_reclaimed赋值给reclaimed获取tree回收页面的个数 */
	reclaimed = vmpr->tree_reclaimed;
    /* 设置tree_scanned、tree_reclaimed为0，对这两个变量进行初始化操作 */
	vmpr->tree_scanned = 0;
	vmpr->tree_reclaimed = 0;
	spin_unlock(&vmpr->sr_lock);
    /* 计算tree 内存回收的压力等级 */
	level = vmpressure_calc_level(scanned, reclaimed);
    /* 遍历由parent组成的memcg的vmpressure值到最大值，并将事件通知满足条件的vmpressure侦听器 */
	do {
		if (vmpressure_event(vmpr, level, ancestor, signalled))
			signalled = true;
		ancestor = true;
	} while ((vmpr = vmpressure_parent(vmpr)));
}

/**
 * vmpressure() - Account memory pressure through scanned/reclaimed ratio
 * @gfp:	reclaimer's gfp mask
 * @memcg:	cgroup memory controller handle
 * @tree:	legacy subtree mode
 * @scanned:	number of pages scanned
 * @reclaimed:	number of pages reclaimed
 *
 * This function should be called from the vmscan reclaim path to account
 * "instantaneous" memory pressure (scanned/reclaimed ratio). The raw
 * pressure index is then further refined and averaged over time.
 *
 * If @tree is set, vmpressure is in traditional userspace reporting
 * mode: @memcg is considered the pressure root and userspace is
 * notified of the entire subtree's reclaim efficiency.
 *
 * If @tree is not set, reclaim efficiency is recorded for @memcg, and
 * only in-kernel users are notified.
 *
 * This function does not return any value.
 */
void vmpressure(gfp_t gfp, struct mem_cgroup *memcg, bool tree,
		unsigned long scanned, unsigned long reclaimed)
{
    /* 获取memcg的vmpressure信息 */
	struct vmpressure *vmpr = memcg_to_vmpressure(memcg);

	/*
	 * Here we only want to account pressure that userland is able to
	 * help us with. For example, suppose that DMA zone is under
	 * pressure; if we notify userland about that kind of pressure,
	 * then it will be mostly a waste as it will trigger unnecessary
	 * freeing of memory by userland (since userland is more likely to
	 * have HIGHMEM/MOVABLE pages instead of the DMA fallback). That
	 * is why we include only movable, highmem and FS/IO pages.
	 * Indirect reclaim (kswapd) sets sc->gfp_mask to GFP_KERNEL, so
	 * we account it too.
	 */
	/* 如果内存分配请求当中不存在任何一种：HIGHMEM高端内存, MOVABLE可移动内存，IO类型以及FS文件系统内存则直接return，不做vmpressure计算 */
	if (!(gfp & (__GFP_HIGHMEM | __GFP_MOVABLE | __GFP_IO | __GFP_FS)))
		return;

	/*
	 * If we got here with no pages scanned, then that is an indicator
	 * that reclaimer was unable to find any shrinkable LRUs at the
	 * current scanning depth. But it does not mean that we should
	 * report the critical pressure, yet. If the scanning priority
	 * (scanning depth) goes too high (deep), we will be notified
	 * through vmpressure_prio(). But so far, keep calm.
	 */
	/* 如果设置scanned    = 0则直接return结束该函数，负责向下执行完成扫描这个scannd代表page，如果为0说明memcg当中不存在任何的可以使用的page则没有必要进行后续操作
	 */
	if (!scanned)
		return;
    /* 如果tree = true则走上面的处理流程，负责如果tree           = false则进入else的处理流程      */
	if (tree) {
		spin_lock(&vmpr->sr_lock);//获取自旋锁
		/* 分两步：
         * vmpr->tree_scanned += scanned;将扫描的pages页数自加到tree_scanned
         * scanned = vmpr->tree_scanned;更新vmpr scanned
         */
		scanned = vmpr->tree_scanned += scanned;
        /* 更新tree树回收的page页数 */
		vmpr->tree_reclaimed += reclaimed;
        /* 解锁 */
		spin_unlock(&vmpr->sr_lock);
        /* 如果扫描到的page小于512个page则直接return结束该函数 */
		if (scanned < vmpressure_win)
			return;
        /* 执行vmpressure_work_fn函数完成内存压力计算 */
		schedule_work(&vmpr->work);
	} else {
		enum vmpressure_levels level;

		/* For now, no users for root-level efficiency */
        /* 记录memcg的检索效率以通知内部内核用户，如果未指定memcg，则该函数将中止 */
		if (!memcg || memcg == root_mem_cgroup)
			return;
        /* 执行过程同上 */
		spin_lock(&vmpr->sr_lock);
        /* 分两步：
         * vmpr->scanned += scanned;将扫描的pages页数自加到scanned
         * scanned = vmpr->scanned;更新vmpr scanned
         */
		scanned = vmpr->scanned += scanned;
        /* 统计回收到的页面总数 */
		reclaimed = vmpr->reclaimed += reclaimed;
        /* 如果扫描的page小于512则直接解锁并return */
		if (scanned < vmpressure_win) {
			spin_unlock(&vmpr->sr_lock);
			return;
		}
        /* 清空回收的页面数，设置vmpr—>reclaimed = 0 */
		vmpr->scanned = vmpr->reclaimed = 0;
		spin_unlock(&vmpr->sr_lock);
        /* 计算的vmpressure级别也就是内存回收的压力等级 */
		level = vmpressure_calc_level(scanned, reclaimed);
        /* 如果等级超过VMPRESSURE_LOW，则将memcg的socket_pressure设置为当前时间之后1秒钟的滴答值 */
		if (level > VMPRESSURE_LOW) {
			/*
			 * Let the socket buffer allocator know that
			 * we are having trouble reclaiming LRU pages.
			 *
			 * For hysteresis keep the pressure state
			 * asserted for a second in which subsequent
			 * pressure events can occur.
			 */
			memcg->socket_pressure = jiffies + HZ;
		}
	}
}

/**
 * vmpressure_prio() - Account memory pressure through reclaimer priority level
 * @gfp:	reclaimer's gfp mask
 * @memcg:	cgroup memory controller handle
 * @prio:	reclaimer's priority
 *
 * This function should be called from the reclaim path every time when
 * the vmscan's reclaiming priority (scanning depth) changes.
 *
 * This function does not return any value.
 */
void vmpressure_prio(gfp_t gfp, struct mem_cgroup *memcg, int prio)
{
	/*
	 * We only use prio for accounting critical level. For more info
	 * see comment for vmpressure_level_critical_prio variable above.
	 */
	/* 如果prio的优先级大于vmpressure_level_critical_prio = ilog2(100 / 10);
	 * 也就是prio > 3则直接return；关于优先级需要明确的是prio越小则优先级越高
     * 优先级越低，一次扫描的页框数量就越多
     * 优先级越高，一次扫描的数量就越少
	 */
	if (prio > vmpressure_level_critical_prio)
		return;

	/*
	 * OK, the prio is below the threshold, updating vmpressure
	 * information before shrinker dives into long shrinking of long
	 * range vmscan. Passing scanned = vmpressure_win, reclaimed = 0
	 * to the vmpressure() basically means that we signal 'critical'
	 * level.
	 */
	/* 
	 * 如果prio下降到threshold阈值以下，即优先级升高，则收缩器在长时间扫描之前更新vmpressure信息
	 */
	vmpressure(gfp, memcg, true, vmpressure_win, 0);
}

static enum vmpressure_levels str_to_level(const char *arg)
{
	enum vmpressure_levels level;

	for (level = 0; level < VMPRESSURE_NUM_LEVELS; level++)
		if (!strcmp(vmpressure_str_levels[level], arg))
			return level;
	return -1;
}

static enum vmpressure_modes str_to_mode(const char *arg)
{
	enum vmpressure_modes mode;

	for (mode = 0; mode < VMPRESSURE_NUM_MODES; mode++)
		if (!strcmp(vmpressure_str_modes[mode], arg))
			return mode;
	return -1;
}

#define MAX_VMPRESSURE_ARGS_LEN	(strlen("critical") + strlen("hierarchy") + 2)

/**
 * vmpressure_register_event() - Bind vmpressure notifications to an eventfd
 * @memcg:	memcg that is interested in vmpressure notifications
 * @eventfd:	eventfd context to link notifications with
 * @args:	event arguments (pressure level threshold, optional mode)
 *
 * This function associates eventfd context with the vmpressure
 * infrastructure, so that the notifications will be delivered to the
 * @eventfd. The @args parameter is a comma-delimited string that denotes a
 * pressure level threshold (one of vmpressure_str_levels, i.e. "low", "medium",
 * or "critical") and an optional mode (one of vmpressure_str_modes, i.e.
 * "hierarchy" or "local").
 *
 * To be used as memcg event method.
 */
int vmpressure_register_event(struct mem_cgroup *memcg,
			      struct eventfd_ctx *eventfd, const char *args)
{
	struct vmpressure *vmpr = memcg_to_vmpressure(memcg);
	struct vmpressure_event *ev;
	enum vmpressure_modes mode = VMPRESSURE_NO_PASSTHROUGH;
	enum vmpressure_levels level = -1;
	char *spec, *spec_orig;
	char *token;
	int ret = 0;

	spec_orig = spec = kzalloc(MAX_VMPRESSURE_ARGS_LEN + 1, GFP_KERNEL);
	if (!spec) {
		ret = -ENOMEM;
		goto out;
	}
	strncpy(spec, args, MAX_VMPRESSURE_ARGS_LEN);

	/* Find required level */
	token = strsep(&spec, ",");
	level = str_to_level(token);
	if (level == -1) {
		ret = -EINVAL;
		goto out;
	}

	/* Find optional mode */
	token = strsep(&spec, ",");
	if (token) {
		mode = str_to_mode(token);
		if (mode == -1) {
			ret = -EINVAL;
			goto out;
		}
	}

	ev = kzalloc(sizeof(*ev), GFP_KERNEL);
	if (!ev) {
		ret = -ENOMEM;
		goto out;
	}

	ev->efd = eventfd;
	ev->level = level;
	ev->mode = mode;

	mutex_lock(&vmpr->events_lock);
	list_add(&ev->node, &vmpr->events);
	mutex_unlock(&vmpr->events_lock);
out:
	kfree(spec_orig);
	return ret;
}

/**
 * vmpressure_unregister_event() - Unbind eventfd from vmpressure
 * @memcg:	memcg handle
 * @eventfd:	eventfd context that was used to link vmpressure with the @cg
 *
 * This function does internal manipulations to detach the @eventfd from
 * the vmpressure notifications, and then frees internal resources
 * associated with the @eventfd (but the @eventfd itself is not freed).
 *
 * To be used as memcg event method.
 */
void vmpressure_unregister_event(struct mem_cgroup *memcg,
				 struct eventfd_ctx *eventfd)
{
	struct vmpressure *vmpr = memcg_to_vmpressure(memcg);
	struct vmpressure_event *ev;

	mutex_lock(&vmpr->events_lock);
	list_for_each_entry(ev, &vmpr->events, node) {
		if (ev->efd != eventfd)
			continue;
		list_del(&ev->node);
		kfree(ev);
		break;
	}
	mutex_unlock(&vmpr->events_lock);
}

/**
 * vmpressure_init() - Initialize vmpressure control structure
 * @vmpr:	Structure to be initialized
 *
 * This function should be called on every allocated vmpressure structure
 * before any usage.
 */
void vmpressure_init(struct vmpressure *vmpr)
{
	spin_lock_init(&vmpr->sr_lock);
	mutex_init(&vmpr->events_lock);
	INIT_LIST_HEAD(&vmpr->events);
	INIT_WORK(&vmpr->work, vmpressure_work_fn);
}

/**
 * vmpressure_cleanup() - shuts down vmpressure control structure
 * @vmpr:	Structure to be cleaned up
 *
 * This function should be called before the structure in which it is
 * embedded is cleaned up.
 */
void vmpressure_cleanup(struct vmpressure *vmpr)
{
	/*
	 * Make sure there is no pending work before eventfd infrastructure
	 * goes away.
	 */
	flush_work(&vmpr->work);
}
