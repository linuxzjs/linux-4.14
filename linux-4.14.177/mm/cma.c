/*
 * Contiguous Memory Allocator
 *
 * Copyright (c) 2010-2011 by Samsung Electronics.
 * Copyright IBM Corporation, 2013
 * Copyright LG Electronics Inc., 2014
 * Written by:
 *	Marek Szyprowski <m.szyprowski@samsung.com>
 *	Michal Nazarewicz <mina86@mina86.com>
 *	Aneesh Kumar K.V <aneesh.kumar@linux.vnet.ibm.com>
 *	Joonsoo Kim <iamjoonsoo.kim@lge.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License or (at your optional) any later version of the license.
 */

#define pr_fmt(fmt) "cma: " fmt

#ifdef CONFIG_CMA_DEBUG
#ifndef DEBUG
#  define DEBUG
#endif
#endif
#define CREATE_TRACE_POINTS

#include <linux/memblock.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/mutex.h>
#include <linux/sizes.h>
#include <linux/slab.h>
#include <linux/log2.h>
#include <linux/cma.h>
#include <linux/highmem.h>
#include <linux/io.h>
#include <trace/events/cma.h>

#include "cma.h"

struct cma cma_areas[MAX_CMA_AREAS];
unsigned cma_area_count;
static DEFINE_MUTEX(cma_mutex);

/* 根据cma base_pfn页框号，获取cma起始物理地址 */
phys_addr_t cma_get_base(const struct cma *cma)
{                                
	return PFN_PHYS(cma->base_pfn);
}

/* 计算cma count总的页面数计算整个cma area区域总内存大小 */
unsigned long cma_get_size(const struct cma *cma)
{
	return cma->count << PAGE_SHIFT;
}

/* 获取cma当前区域名称，如果没有名字默认undefined */
const char *cma_get_name(const struct cma *cma)
{
	return cma->name ? cma->name : "(undefined)";
} 

static unsigned long cma_bitmap_aligned_mask(const struct cma *cma,
					     unsigned int align_order)
{
    /* 如果align_order <= order_per_bit则直接return 0 */
	if (align_order <= cma->order_per_bit)
		return 0;
    /* 
     * 当align_order > order_per_bit时，需要通过这种方式对齐，
     * 因为我们要分配连续的物理页面所以必须在分配中对齐使用，同时cma也用在DMA当中
     * 由于 DMA 硬件起始地址限制，这通常需要对齐，如果不对其就无法分配需要的地址空间
     * 基于这两点必须要使用cma_bitmap_aligned_mask做对齐操作
     */
	return (1UL << (align_order - cma->order_per_bit)) - 1;
}

/*
 * Find the offset of the base PFN from the specified align_order.
 * The value returned is represented in order_per_bits.
 */
static unsigned long cma_bitmap_aligned_offset(const struct cma *cma,
					       unsigned int align_order)
{
    /* 这里相当于是起始地址的对齐，将base_pfn对齐到2^align_order然后再根据order_per_bit
     * 得到对应的bitmap offset偏移
     */
	return (cma->base_pfn & ((1UL << align_order) - 1))
		>> cma->order_per_bit;
}

static unsigned long cma_bitmap_pages_to_bits(const struct cma *cma,
					      unsigned long pages)
{
    /* pages这里代表的是页的个数，通过2^order_per_bit对齐，获取对应bitmap位图所需要的页面数 */
	return ALIGN(pages, 1UL << cma->order_per_bit) >> cma->order_per_bit;
}

static void cma_clear_bitmap(struct cma *cma, unsigned long pfn,
			     unsigned int count)
{
	unsigned long bitmap_no, bitmap_count;
    /* pfn - base_pfn获取到需要clean的内存是多少个页框，再除以/2^order获得位图的起始地址 */
	bitmap_no = (pfn - cma->base_pfn) >> cma->order_per_bit;
    /* 根据对齐获取了本次需要clean的位图数 */
	bitmap_count = cma_bitmap_pages_to_bits(cma, count);

	mutex_lock(&cma->lock);
    /* 从位图中的bitmap_no位置开始，按bitmap_count位数清零 */
	bitmap_clear(cma->bitmap, bitmap_no, bitmap_count);
	mutex_unlock(&cma->lock);
}

static int __init cma_activate_area(struct cma *cma)
{
    /* 根据cma_bitmap_maxno(cma)计算出cma区域有多少个bitmap位图，
     * 同时计算出这个cma area bitmap占用了多少个long类型的数据结构
     * 最后再乘以sizeof(long)计算出整个cma area bitmap的大小，这里本质上有一个sizeof(long)对齐的操作
     */
	int bitmap_size = BITS_TO_LONGS(cma_bitmap_maxno(cma)) * sizeof(long);
    /* 初始化cma area区域物理地址起始地址页框号             */
	unsigned long base_pfn = cma->base_pfn, pfn = base_pfn;
    /* 计算出cma  区域有多少个pageblock，pageblock_order = 10, 这样一个pageblock = 4M */
	unsigned i = cma->count >> pageblock_order;
	struct zone *zone;
    /* 给cma area区域的所需要的bitmap位图空间分配地址 */
	cma->bitmap = kzalloc(bitmap_size, GFP_KERNEL);
    /* 内存分配失败直接return       */
	if (!cma->bitmap) {
		cma->count = 0;
		return -ENOMEM;
	}

	WARN_ON_ONCE(!pfn_valid(pfn));
    /* 通过pfn_to_page将cma  空间的页帧号转换成page对应的，再通过page_zone判断出当前page所属的zone区域 */
	zone = page_zone(pfn_to_page(pfn));
    /* 以pageblock为单位，遍历整个cma area区域，将cma area区域当中的每个pageblock导入到buddy system当中，供分配使用 */
	do {
		unsigned j;
        /*
         * 这个循环很有意思，通过设置j = pageblock_nr_pages,采用自减的方式当j < 0时结束循环，此时pfn就从原来的cma->base_pfn 
         * 变成了pfn = cma->base_pfn + pageblock_nr_pages,最后将pfn赋值给base_pfn，成功的进入到cma area区域中下一个pageblock
         */
		base_pfn = pfn;
		for (j = pageblock_nr_pages; j; --j, pfn++) {
			WARN_ON_ONCE(!pfn_valid(pfn));
			/*
			 * alloc_contig_range requires the pfn range
			 * specified to be in the same zone. Make this
			 * simple by forcing the entire CMA resv range
			 * to be in the same zone.
			 */
			 /* 判断当前pfn所在的zone与之前获取zone区域是否相同，如果相同继续这个循环，如果不同则直接goto not_in_zone结束整个初始化过程 */
			if (page_zone(pfn_to_page(pfn)) != zone)
				goto not_in_zone;
		}
        /* 将该pageblock导入到伙伴系统，并且将migrate type设定为MIGRATE_CMA */
		init_cma_reserved_pageblock(pfn_to_page(base_pfn));
	} while (--i);

	mutex_init(&cma->lock);

#ifdef CONFIG_CMA_DEBUGFS
	INIT_HLIST_HEAD(&cma->mem_head);
	spin_lock_init(&cma->mem_head_lock);
#endif

	return 0;

not_in_zone:
	pr_err("CMA area %s could not be activated\n", cma->name);
	kfree(cma->bitmap);
	cma->count = 0;
	return -EINVAL;
}

static int __init cma_init_reserved_areas(void)
{
	int i;
    /* 遍历cma_area_count区域，将cma area进行初始化，并添加到buddy system系统当中 */
	for (i = 0; i < cma_area_count; i++) {
		int ret = cma_activate_area(&cma_areas[i]);

		if (ret)
			return ret;
	}

	return 0;
}
core_initcall(cma_init_reserved_areas);

/**
 * cma_init_reserved_mem() - create custom contiguous area from reserved memory
 * @base: Base address of the reserved area
 * @size: Size of the reserved area (in bytes),
 * @order_per_bit: Order of pages represented by one bit on bitmap.
 * @res_cma: Pointer to store the created cma region.
 *
 * This function creates custom contiguous area from already reserved memory.
 */
int __init cma_init_reserved_mem(phys_addr_t base, phys_addr_t size,
				 unsigned int order_per_bit,
				 const char *name,
				 struct cma **res_cma)
{
	struct cma *cma;
	phys_addr_t alignment;

	/* Sanity checks */
	if (cma_area_count == ARRAY_SIZE(cma_areas)) {
		pr_err("Not enough slots for CMA reserved regions!\n");
		return -ENOSPC;
	}

	if (!size || !memblock_is_region_reserved(base, size))
		return -EINVAL;

	/* ensure minimal alignment required by mm core */
	alignment = PAGE_SIZE <<
			max_t(unsigned long, MAX_ORDER - 1, pageblock_order);

	/* alignment should be aligned with order_per_bit */
	if (!IS_ALIGNED(alignment >> PAGE_SHIFT, 1 << order_per_bit))
		return -EINVAL;

	if (ALIGN(base, alignment) != base || ALIGN(size, alignment) != size)
		return -EINVAL;

	/*
	 * Each reserved area must be initialised later, when more kernel
	 * subsystems (like slab allocator) are available.
	 */
	cma = &cma_areas[cma_area_count];
	if (name) {
		cma->name = name;
	} else {
		cma->name = kasprintf(GFP_KERNEL, "cma%d\n", cma_area_count);
		if (!cma->name)
			return -ENOMEM;
	}
	cma->base_pfn = PFN_DOWN(base);
	cma->count = size >> PAGE_SHIFT;
	cma->order_per_bit = order_per_bit;
	*res_cma = cma;
	cma_area_count++;
	totalcma_pages += (size / PAGE_SIZE);

	return 0;
}

/**
 * cma_declare_contiguous() - reserve custom contiguous area
 * @base: Base address of the reserved area optional, use 0 for any
 * @size: Size of the reserved area (in bytes),
 * @limit: End address of the reserved memory (optional, 0 for any).
 * @alignment: Alignment for the CMA area, should be power of 2 or zero
 * @order_per_bit: Order of pages represented by one bit on bitmap.
 * @fixed: hint about where to place the reserved area
 * @res_cma: Pointer to store the created cma region.
 *
 * This function reserves memory from early allocator. It should be
 * called by arch specific code once the early allocator (memblock or bootmem)
 * has been activated and all other subsystems have already allocated/reserved
 * memory. This function allows to create custom reserved areas.
 *
 * If @fixed is true, reserve contiguous area at exactly @base.  If false,
 * reserve in range from @base to @limit.
 */
int __init cma_declare_contiguous(phys_addr_t base,
			phys_addr_t size, phys_addr_t limit,
			phys_addr_t alignment, unsigned int order_per_bit,
			bool fixed, const char *name, struct cma **res_cma)
{
	phys_addr_t memblock_end = memblock_end_of_DRAM();
	phys_addr_t highmem_start;
	int ret = 0;

	/*
	 * We can't use __pa(high_memory) directly, since high_memory
	 * isn't a valid direct map VA, and DEBUG_VIRTUAL will (validly)
	 * complain. Find the boundary by adding one to the last valid
	 * address.
	 */
	highmem_start = __pa(high_memory - 1) + 1;
	pr_debug("%s(size %pa, base %pa, limit %pa alignment %pa)\n",
		__func__, &size, &base, &limit, &alignment);

	if (cma_area_count == ARRAY_SIZE(cma_areas)) {
		pr_err("Not enough slots for CMA reserved regions!\n");
		return -ENOSPC;
	}

	if (!size)
		return -EINVAL;

	if (alignment && !is_power_of_2(alignment))
		return -EINVAL;

	/*
	 * Sanitise input arguments.
	 * Pages both ends in CMA area could be merged into adjacent unmovable
	 * migratetype page by page allocator's buddy algorithm. In the case,
	 * you couldn't get a contiguous memory, which is not what we want.
	 */
	alignment = max(alignment,  (phys_addr_t)PAGE_SIZE <<
			  max_t(unsigned long, MAX_ORDER - 1, pageblock_order));
	if (fixed && base & (alignment - 1)) {
		ret = -EINVAL;
		pr_err("Region at %pa must be aligned to %pa bytes\n",
			&base, &alignment);
		goto err;
	}
	base = ALIGN(base, alignment);
	size = ALIGN(size, alignment);
	limit &= ~(alignment - 1);

	if (!base)
		fixed = false;

	/* size should be aligned with order_per_bit */
	if (!IS_ALIGNED(size >> PAGE_SHIFT, 1 << order_per_bit))
		return -EINVAL;

	/*
	 * If allocating at a fixed base the request region must not cross the
	 * low/high memory boundary.
	 */
	if (fixed && base < highmem_start && base + size > highmem_start) {
		ret = -EINVAL;
		pr_err("Region at %pa defined on low/high memory boundary (%pa)\n",
			&base, &highmem_start);
		goto err;
	}

	/*
	 * If the limit is unspecified or above the memblock end, its effective
	 * value will be the memblock end. Set it explicitly to simplify further
	 * checks.
	 */
	if (limit == 0 || limit > memblock_end)
		limit = memblock_end;

	if (base + size > limit) {
		ret = -EINVAL;
		pr_err("Size (%pa) of region at %pa exceeds limit (%pa)\n",
			&size, &base, &limit);
		goto err;
	}

	/* Reserve memory */
	if (fixed) {
		if (memblock_is_region_reserved(base, size) ||
		    memblock_reserve(base, size) < 0) {
			ret = -EBUSY;
			goto err;
		}
	} else {
		phys_addr_t addr = 0;

		/*
		 * All pages in the reserved area must come from the same zone.
		 * If the requested region crosses the low/high memory boundary,
		 * try allocating from high memory first and fall back to low
		 * memory in case of failure.
		 */
		if (base < highmem_start && limit > highmem_start) {
			addr = memblock_alloc_range(size, alignment,
						    highmem_start, limit,
						    MEMBLOCK_NONE);
			limit = highmem_start;
		}

		if (!addr) {
			addr = memblock_alloc_range(size, alignment, base,
						    limit,
						    MEMBLOCK_NONE);
			if (!addr) {
				ret = -ENOMEM;
				goto err;
			}
		}

		/*
		 * kmemleak scans/reads tracked objects for pointers to other
		 * objects but this address isn't mapped and accessible
		 */
		kmemleak_ignore_phys(addr);
		base = addr;
	}

	ret = cma_init_reserved_mem(base, size, order_per_bit, name, res_cma);
	if (ret)
		goto free_mem;

	pr_info("Reserved %ld MiB at %pa\n", (unsigned long)size / SZ_1M,
		&base);
	return 0;

free_mem:
	memblock_free(base, size);
err:
	pr_err("Failed to reserve %ld MiB\n", (unsigned long)size / SZ_1M);
	return ret;
}

#ifdef CONFIG_CMA_DEBUG
static void cma_debug_show_areas(struct cma *cma)
{
	unsigned long next_zero_bit, next_set_bit, nr_zero;
	unsigned long start = 0;
	unsigned long nr_part, nr_total = 0;
	unsigned long nbits = cma_bitmap_maxno(cma);

	mutex_lock(&cma->lock);
	pr_info("number of available pages: ");
	for (;;) {
		next_zero_bit = find_next_zero_bit(cma->bitmap, nbits, start);
		if (next_zero_bit >= nbits)
			break;
		next_set_bit = find_next_bit(cma->bitmap, nbits, next_zero_bit);
		nr_zero = next_set_bit - next_zero_bit;
		nr_part = nr_zero << cma->order_per_bit;
		pr_cont("%s%lu@%lu", nr_total ? "+" : "", nr_part,
			next_zero_bit);
		nr_total += nr_part;
		start = next_zero_bit + nr_zero;
	}
	pr_cont("=> %lu free of %lu total pages\n", nr_total, cma->count);
	mutex_unlock(&cma->lock);
}
#else
static inline void cma_debug_show_areas(struct cma *cma) { }
#endif

/**
 * cma_alloc() - allocate pages from contiguous area
 * @cma:   Contiguous memory region for which the allocation is performed.
 * @count: Requested number of pages.
 * @align: Requested alignment of pages (in PAGE_SIZE order).
 *
 * This function allocates part of contiguous memory on specific
 * contiguous memory area.
 */
/*
 * CMA分配通过统一接口cma_alloc函数，会从bitmap中先查找满足要求的连续bit，然后通过alloc_contig_range实现分配，成功后的页面会从buddy system系统当中分离出来
 */
struct page *cma_alloc(struct cma *cma, size_t count, unsigned int align,
		       gfp_t gfp_mask)
{
	unsigned long mask, offset;
	unsigned long pfn = -1;
	unsigned long start = 0;
	unsigned long bitmap_maxno, bitmap_no, bitmap_count;
	struct page *page = NULL;
	int ret = -ENOMEM;
    /* cma,cma->count任意一个为空则说明cma没有可用的page可供分配使用则直接return */
	if (!cma || !cma->count)
		return NULL;

	pr_debug("%s(cma %p, count %zu, align %d)\n", __func__, (void *)cma,
		 count, align);

	if (!count)
		return NULL;
    /*  */
	mask = cma_bitmap_aligned_mask(cma, align);
	offset = cma_bitmap_aligned_offset(cma, align);
	bitmap_maxno = cma_bitmap_maxno(cma);
	bitmap_count = cma_bitmap_pages_to_bits(cma, count);

	if (bitmap_count > bitmap_maxno)
		return NULL;

	for (;;) {
		mutex_lock(&cma->lock);
        /* 在cma->bitmap中从start开始向后查找bitmap_count个连续bit为0的位图
         * 这里有一个很重要的mask,offset的对齐操作，bitmap_no表示的是cma area对应的bitmap数，
         * 这个bitmap_no就相当于分配内存page的起始地址，
         */
		bitmap_no = bitmap_find_next_zero_area_off(cma->bitmap,
				bitmap_maxno, start, bitmap_count, mask,
				offset);
        /* 如果查找到的bitmap_no个数大于cma area整个区域最大的bitmap_maxno，则直接break，cma内存分配失败 */
		if (bitmap_no >= bitmap_maxno) {
			mutex_unlock(&cma->lock);
			break;
		}
        /* 从bitmap_no开始连续设置bitmap_count个bit为1，代表bitmap_no ~ (bitmap_no + bitmap_count)连续的bit全部被占用 */
		bitmap_set(cma->bitmap, bitmap_no, bitmap_count);
		/*
		 * It's safe to drop the lock here. We've marked this region for
		 * our exclusive use. If the migration fails we will take the
		 * lock again and unmark it.
		 */
		mutex_unlock(&cma->lock);
        /* bitmap_no << cma->order_per_bit通过使用每个bit中的order_per_bit,计算出start ~ bitmap_no区所占用了多少个page，也就是bitmap_no*2^order_per_bit
         * 这样通过与cma->base_pfn + (bitmap_no << cma->order_per_bit)就可以得到当前bitmap_no所对应的bitmap_no_pfn，举个例子：
         * 假设cma->base_pfn = 0x200， bitmap_no = 1 order_per_bit = 1 则新的pfn = 0x200 + 2 = 0x202
         */
		pfn = cma->base_pfn + (bitmap_no << cma->order_per_bit);
		mutex_lock(&cma_mutex);
        /* 
         * 完成pfn到(pfn + count)区域的page的cma内存分配工作
         * 使用migratetype MIGRATE_CMA 根据需要分配尽可能多的页面，如果成功则返回页面结构指针
         * 该区域中所有现有的可移动页面都通过压缩过程进行迁移
         */
		ret = alloc_contig_range(pfn, pfn + count, MIGRATE_CMA,
					 gfp_mask);
		mutex_unlock(&cma_mutex);
		if (ret == 0) {
            /* 内存分配成功，将pfn转换为page break完成cma区域的内存分配 */
			page = pfn_to_page(pfn);
			break;
		}
        /* 如果分配失败，位图将再次被清除 */
		cma_clear_bitmap(cma, pfn, count);
		if (ret != -EBUSY)
			break;

		pr_debug("%s(): memory range at %p is busy, retrying\n",
			 __func__, pfn_to_page(pfn));
		/* try again with a bit different memory target */
        /* 如果bitmap_no的到bitmap_no +   bitmap_count的内存分配失败则更新start重新尝试分配 */
		start = bitmap_no + mask + 1;
	}

	trace_cma_alloc(pfn, page, count, align);

	if (ret && !(gfp_mask & __GFP_NOWARN)) {
		pr_info("%s: alloc failed, req-size: %zu pages, ret: %d\n",
			__func__, count, ret);
		cma_debug_show_areas(cma);
	}

	pr_debug("%s(): returned %p\n", __func__, page);
	return page;
}

/**
 * cma_release() - release allocated pages
 * @cma:   Contiguous memory region for which the allocation is performed.
 * @pages: Allocated pages.
 * @count: Number of allocated pages.
 *
 * This function releases memory allocated by alloc_cma().
 * It returns false when provided pages do not belong to contiguous area and
 * true otherwise.
 */
bool cma_release(struct cma *cma, const struct page *pages, unsigned int count)
{
	unsigned long pfn;

	if (!cma || !pages)
		return false;

	pr_debug("%s(page %p)\n", __func__, (void *)pages);

	pfn = page_to_pfn(pages);

	if (pfn < cma->base_pfn || pfn >= cma->base_pfn + cma->count)
		return false;

	VM_BUG_ON(pfn + count > cma->base_pfn + cma->count);

	free_contig_range(pfn, count);
	cma_clear_bitmap(cma, pfn, count);
	trace_cma_release(pfn, pages, count);

	return true;
}

int cma_for_each_area(int (*it)(struct cma *cma, void *data), void *data)
{
	int i;

	for (i = 0; i < cma_area_count; i++) {
		int ret = it(&cma_areas[i], data);

		if (ret)
			return ret;
	}

	return 0;
}
