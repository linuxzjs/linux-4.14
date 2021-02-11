// SPDX-License-Identifier: GPL-2.0
#include <linux/mm.h>
#include <linux/rmap.h>
#include <linux/hugetlb.h>
#include <linux/swap.h>
#include <linux/swapops.h>

#include "internal.h"

static inline bool not_found(struct page_vma_mapped_walk *pvmw)
{
	page_vma_mapped_walk_done(pvmw);
	return false;
}

static bool map_pte(struct page_vma_mapped_walk *pvmw)
{
    /* 使用pvmw->pmd条目和pvmw->address来获取pte条目 */
	pvmw->pte = pte_offset_map(pvmw->pmd, pvmw->address);
    /* 如果flags设置了PVMW_SYNC则不进入一下判断，如果没有则进入判断当中 */
	if (!(pvmw->flags & PVMW_SYNC)) {
        /* 如果设置了迁移属性则进入判断，通过is_swap_pte判断是否为swap类型，如果不是swap type则直接return false */
		if (pvmw->flags & PVMW_MIGRATION) {
			if (!is_swap_pte(*pvmw->pte))
				return false;
		} else {
			/*
			 * We get here when we are trying to unmap a private
			 * device page from the process address space. Such
			 * page is not CPU accessible and thus is mapped as
			 * a special swap entry, nonetheless it still does
			 * count as a valid regular mapping for the page (and
			 * is accounted as such in page maps count).
			 *
			 * So handle this special case as if it was a normal
			 * page mapping ie lock CPU page table and returns
			 * true.
			 *
			 * For more details on device private memory see HMM
			 * (include/linux/hmm.h or mm/hmm.c).
			 */
			 /* 如果是swap type类型进入判断当中 */
			if (is_swap_pte(*pvmw->pte)) {
				swp_entry_t entry;

				/* Handle un-addressable ZONE_DEVICE memory */
                 /* 将pte转化为entry */
				entry = pte_to_swp_entry(*pvmw->pte);
                 /* 如果该entry不是设备专用条目，则return false */
				if (!is_device_private_entry(entry))
					return false;
			} else if (!pte_present(*pvmw->pte))//如果该pte已经被使用则直接return false
				return false;
		}
	}
    /* 获取ptl锁并返回true */
	pvmw->ptl = pte_lockptr(pvmw->vma->vm_mm, pvmw->pmd);
	spin_lock(pvmw->ptl);
	return true;
}

/**
 * check_pte - check if @pvmw->page is mapped at the @pvmw->pte
 *
 * page_vma_mapped_walk() found a place where @pvmw->page is *potentially*
 * mapped. check_pte() has to validate this.
 *
 * @pvmw->pte may point to empty PTE, swap PTE or PTE pointing to arbitrary
 * page.
 *
 * If PVMW_MIGRATION flag is set, returns true if @pvmw->pte contains migration
 * entry that points to @pvmw->page or any subpage in case of THP.
 *
 * If PVMW_MIGRATION flag is not set, returns true if @pvmw->pte points to
 * @pvmw->page or any subpage in case of THP.
 *
 * Otherwise, return false.
 *
 */
static bool check_pte(struct page_vma_mapped_walk *pvmw)
{
	unsigned long pfn;
    /* 判断pvmw->flags是否设置迁移类型，如果设置则进入当中 */
	if (pvmw->flags & PVMW_MIGRATION) {
		swp_entry_t entry;
        /* 判断pte是否为swap   类型，如果不是则直接return false */
		if (!is_swap_pte(*pvmw->pte))
			return false;
        /* 将pte转化为pte entry入口*/
		entry = pte_to_swp_entry(*pvmw->pte);
        /* 判断该entry是否是可迁移类型，如果不是则直接return false */
		if (!is_migration_entry(entry))
			return false;
        /* 将entry转换成pfn */
		pfn = migration_entry_to_pfn(entry);
	} else if (is_swap_pte(*pvmw->pte)) {
        /* 判断是否为swap pte如果是则进入该判断语句 */
		swp_entry_t entry;

		/* Handle un-addressable ZONE_DEVICE memory */
        /* 将pte转化为entry */
		entry = pte_to_swp_entry(*pvmw->pte);
        /* 如果该entry不是设备专用条目，则return false */
		if (!is_device_private_entry(entry))
			return false;
        /* 将entry转化成pte,这里如果直接判断pte这样岂不是缩小了中间过程吗？提高效率 */
		pfn = device_private_entry_to_pfn(entry);
	} else {
		if (!pte_present(*pvmw->pte))//如果该pte已经被使用则直接return false
			return false;
        /* 将pte转化为pfn页表项 */
		pfn = pte_pfn(*pvmw->pte);
	}

	if (pfn < page_to_pfn(pvmw->page))
		return false;

	/* THP can be referenced by any subpage */
    /* 判断该page是否在巨型页的范围之内，如果不在则直接return false */
	if (pfn - page_to_pfn(pvmw->page) >= hpage_nr_pages(pvmw->page))
		return false;

	return true;
}

/**
 * page_vma_mapped_walk - check if @pvmw->page is mapped in @pvmw->vma at
 * @pvmw->address
 * @pvmw: pointer to struct page_vma_mapped_walk. page, vma, address and flags
 * must be set. pmd, pte and ptl must be NULL.
 *
 * Returns true if the page is mapped in the vma. @pvmw->pmd and @pvmw->pte point
 * to relevant page table entries. @pvmw->ptl is locked. @pvmw->address is
 * adjusted if needed (for PTE-mapped THPs).
 *
 * If @pvmw->pmd is set but @pvmw->pte is not, you have found PMD-mapped page
 * (usually THP). For PTE-mapped THP, you should run page_vma_mapped_walk() in
 * a loop to find all PTEs that map the THP.
 *
 * For HugeTLB pages, @pvmw->pte is set to the relevant page table entry
 * regardless of which page table level the page is mapped at. @pvmw->pmd is
 * NULL.
 *
 * Retruns false if there are no more page table entries for the page in
 * the vma. @pvmw->ptl is unlocked and @pvmw->pte is unmapped.
 *
 * If you need to stop the walk before page_vma_mapped_walk() returned false,
 * use page_vma_mapped_walk_done(). It will do the housekeeping.
 */
bool page_vma_mapped_walk(struct page_vma_mapped_walk *pvmw)
{
	struct mm_struct *mm = pvmw->vma->vm_mm;
	struct page *page = pvmw->page;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t pmde;

	/* The only possible pmd mapping has been handled on last iteration */
    /* 判断pmd,pte是否存在如果不存在，则return not_found*/
	if (pvmw->pmd && !pvmw->pte)
		return not_found(pvmw);
    /* 如果pvmw->pte不为空，则说明该pvmw已经建立映射，则直接跳转到next_pte指向下一个pte */
	if (pvmw->pte)
		goto next_pte;
    /* 判断是否为巨型页，如果是则进入判断当中。通过huge_pte_offset获取pvmw->pte */
	if (unlikely(PageHuge(pvmw->page))) {
		/* when pud is not present, pte will be NULL */
		pvmw->pte = huge_pte_offset(mm, pvmw->address,
					    PAGE_SIZE << compound_order(page));
		if (!pvmw->pte)
			return false;

		pvmw->ptl = huge_pte_lockptr(page_hstate(page), mm, pvmw->pte);
		spin_lock(pvmw->ptl);
		if (!check_pte(pvmw))
			return not_found(pvmw);
		return true;
	}
restart:
    /* pgd->p4d->pud->pmd条目,但是如果没有条目则返回false */
	pgd = pgd_offset(mm, pvmw->address);
	if (!pgd_present(*pgd))
		return false;
	p4d = p4d_offset(pgd, pvmw->address);
	if (!p4d_present(*p4d))
		return false;
	pud = pud_offset(p4d, pvmw->address);
	if (!pud_present(*pud))
		return false;
	pvmw->pmd = pmd_offset(pud, pvmw->address);
	/*
	 * Make sure the pmd value isn't cached in a register by the
	 * compiler and used as a stale value after we've observed a
	 * subsequent update.
	 */
	/* READ_ONCE的作用与volatile相同避免读取的值是cache当中的值，而不是真正的pmd */
	pmde = READ_ONCE(*pvmw->pmd);
    /* 判断pmde是否为透明大页，或者是否为迁移pmde条目 */
	if (pmd_trans_huge(pmde) || is_pmd_migration_entry(pmde)) {
        /* 获取pte lock   */
		pvmw->ptl = pmd_lock(mm, pvmw->pmd);
        /*
         * 如果是透明大页则进入循环，如果
         * 如果pvmw设置PVMW_MIGRATION迁移标注，则return not_found
         * 如果pvmw->pmd对应的page与当前page不一致则说明映射存在问题，则直接return not_found
        */
		if (likely(pmd_trans_huge(*pvmw->pmd))) {
			if (pvmw->flags & PVMW_MIGRATION)
				return not_found(pvmw);
			if (pmd_page(*pvmw->pmd) != page)
				return not_found(pvmw);
			return true;
		} else if (!pmd_present(*pvmw->pmd)) {
            /* 
             * 如果该pvmw->pmd存在被使用，判断透明大页迁移是否支持，如果不支持则直接return not_found
             * 在ARM64架构当中CONFIG_ARCH_ENABLE_THP_MIGRATION该功能并未开启，所以此处判断失败直接return not_found
            */
			if (thp_migration_supported()) {
				if (!(pvmw->flags & PVMW_MIGRATION))
					return not_found(pvmw);
				if (is_migration_entry(pmd_to_swp_entry(*pvmw->pmd))) {
					swp_entry_t entry = pmd_to_swp_entry(*pvmw->pmd);

					if (migration_entry_to_page(entry) != page)
						return not_found(pvmw);
					return true;
				}
			}
			return not_found(pvmw);
		} else {
			/* THP pmd was split under us: handle on pte level */
            /* 如果以上三个选项都不满足则解锁ptl，同时设置ptl为空 */
			spin_unlock(pvmw->ptl);
			pvmw->ptl = NULL;
		}
	} else if (!pmd_present(pmde)) {
	    /* 如果pmd不存在则直接return false */
		return false;
	}
    /* 如果pte没有建立映射，则跳转到next_pte取查询next pte */
	if (!map_pte(pvmw))
		goto next_pte;
	while (1) {
        /* 遍历pvmw的pte页目录，严格检查是否映射了每个页面，如果确认，则返回true */
		if (check_pte(pvmw))
			return true;
next_pte:
		/* Seek to next pte only makes sense for THP */
        /* 如果不是thp或很大的页面，则page_vma_mappled_walk已完成，并且返回false */
		if (!PageTransHuge(pvmw->page) || PageHuge(pvmw->page))
			return not_found(pvmw);
		do {
            /*
             * 如果pvmw->address + PAGE_SIZE大于等于pvmw->vma->vm_end
             * 或者是pvmw->address + PAGE_SIZE大于等于实际计算出来的vm_start地址加上pageoff偏移
             * 则直接return not_found
            */
			pvmw->address += PAGE_SIZE;
			if (pvmw->address >= pvmw->vma->vm_end ||
			    pvmw->address >=
					__vma_address(pvmw->page, pvmw->vma) +
					hpage_nr_pages(pvmw->page) * PAGE_SIZE)
				return not_found(pvmw);
			/* Did we cross page table boundary? */
			if (pvmw->address % PMD_SIZE == 0) {
                /* 如果address % PMD_SIZE ==  0说明address没有跨页面区域       
                 * 则进行pte unmap操作，并检查当前ptl是否为真，如果为真则做spin_unlock解锁操作，并设置pvmw->ptl = NULL;
                 * 否则执行跳转到restart
                 */
				pte_unmap(pvmw->pte);
				if (pvmw->ptl) {
					spin_unlock(pvmw->ptl);
					pvmw->ptl = NULL;
				}
				goto restart;
			} else {
                /* pvmw->pte自加1 */
				pvmw->pte++;
			}
            /* 判断pte为空时结束do循环 */
		} while (pte_none(*pvmw->pte));
        /* 如果上面释放了ptl锁，则再次获取并重复该锁 */
		if (!pvmw->ptl) {
			pvmw->ptl = pte_lockptr(mm, pvmw->pmd);
			spin_lock(pvmw->ptl);
		}
	}
}

/**
 * page_mapped_in_vma - check whether a page is really mapped in a VMA
 * @page: the page to test
 * @vma: the VMA to test
 *
 * Returns 1 if the page is mapped into the page tables of the VMA, 0
 * if the page is not mapped into the page tables of this VMA.  Only
 * valid for normal file or anonymous VMAs.
 */
int page_mapped_in_vma(struct page *page, struct vm_area_struct *vma)
{
	struct page_vma_mapped_walk pvmw = {
		.page = page,
		.vma = vma,
		.flags = PVMW_SYNC,
	};
	unsigned long start, end;

	start = __vma_address(page, vma);
	end = start + PAGE_SIZE * (hpage_nr_pages(page) - 1);

	if (unlikely(end < vma->vm_start || start >= vma->vm_end))
		return 0;
	pvmw.address = max(start, vma->vm_start);
	if (!page_vma_mapped_walk(&pvmw))
		return 0;
	page_vma_mapped_walk_done(&pvmw);
	return 1;
}
