/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __MM_CMA_H__
#define __MM_CMA_H__

struct cma {
	unsigned long   base_pfn;   //CMA区域物理地址的起始页帧号
	unsigned long   count;      //CMA area区域总的page页数
	unsigned long   *bitmap;    //CMA bitmap位图，用于描述page的分配情况
	/* 直观的翻译Order of pages represented by one bit，可知order_per_bit描述的是每个bitmap位图中每个bit的物理页面的order值，其中页面数为2^order_per_bit值 */
	unsigned int order_per_bit; /* Order of pages represented by one bit */
	struct mutex    lock;
#ifdef CONFIG_CMA_DEBUGFS
	struct hlist_head mem_head;
	spinlock_t mem_head_lock;
#endif
	const char *name;
};

extern struct cma cma_areas[MAX_CMA_AREAS];
extern unsigned cma_area_count;

static inline unsigned long cma_bitmap_maxno(struct cma *cma)
{
	return cma->count >> cma->order_per_bit;
}

#endif
