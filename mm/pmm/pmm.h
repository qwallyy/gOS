/* gOS Kernel — Physical Memory Manager (Bitmap-based)
 * Reference: https://wiki.osdev.org/Detecting_Memory_(x86)
 *            https://wiki.osdev.org/Page_Frame_Allocation
 */

#ifndef GOS_PMM_H
#define GOS_PMM_H

#include "gOS/types.h"

#define PMM_PAGE_SIZE       4096
#define PMM_PAGE_ALIGN(x)   (((x) + PMM_PAGE_SIZE - 1) & ~(PMM_PAGE_SIZE - 1))
#define PMM_PAGE_ALIGN_DOWN(x) ((x) & ~(PMM_PAGE_SIZE - 1))

void pmm_init(void);
void pmm_mark_used(uintptr_t phys_addr, size_t pages);
void pmm_mark_free(uintptr_t phys_addr, size_t pages);
void *pmm_alloc_page(void);
void *pmm_alloc_pages(size_t count);
void pmm_free_page(void *page);
void pmm_free_pages(void *pages, size_t count);
uintptr_t pmm_get_total_memory(void);
uintptr_t pmm_get_used_memory(void);
uintptr_t pmm_get_free_memory(void);
void pmm_dump_stats(void);

#endif /* GOS_PMM_H */
