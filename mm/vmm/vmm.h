/* gOS Kernel — Virtual Memory Manager (Paging)
 * Reference: https://wiki.osdev.org/Paging
 */

#ifndef GOS_VMM_H
#define GOS_VMM_H

#include "gOS/types.h"
#include "gOS/kernel.h"

/* Page table entry flags */
#define VMM_PRESENT     PAGE_PRESENT
#define VMM_WRITABLE    PAGE_WRITABLE
#define VMM_USER        PAGE_USER
#define VMM_NOCACHE     PAGE_NOCACHE
#define VMM_HUGE        PAGE_HUGE
#define VMM_GLOBAL      PAGE_GLOBAL
#define VMM_NX          PAGE_NX

/* Page table entry type */
typedef uint64_t pt_entry_t;

/* Paging structures */
typedef struct page_table {
    pt_entry_t entries[PT_ENTRIES];
} ALIGN(4096) page_table_t;

typedef struct page_directory {
    pt_entry_t entries[PT_ENTRIES];
} ALIGN(4096) page_directory_t;

typedef struct pdpt {
    pt_entry_t entries[PT_ENTRIES];
} ALIGN(4096) pdpt_t;

typedef struct pml4 {
    pt_entry_t entries[PT_ENTRIES];
} ALIGN(4096) pml4_t;

void vmm_init(void);
void vmm_map_page(uintptr_t virt, uintptr_t phys, uint64_t flags);
void vmm_unmap_page(uintptr_t virt);
bool vmm_is_mapped(uintptr_t virt);
uintptr_t vmm_get_phys(uintptr_t virt);
void vmm_map_range(uintptr_t virt_start, uintptr_t phys_start, size_t pages, uint64_t flags);
void vmm_unmap_range(uintptr_t virt_start, size_t pages);
pt_entry_t *vmm_get_pte(uintptr_t virt);
pml4_t *vmm_get_pml4(void);
void vmm_switch_pml4(pml4_t *pml4);
void vmm_invalidate_page(uintptr_t virt);
void vmm_invalidate_all(void);

#endif /* GOS_VMM_H */
