/* gOS Kernel — Virtual Memory Manager Implementation
 * Reference: https://wiki.osdev.org/Paging
 */

#include "mm/vmm/vmm.h"
#include "mm/pmm/pmm.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

static pml4_t *g_vmm_kernel_pml4 = NULL;

static uintptr_t vmm_virt_to_index(uintptr_t virt, int level) {
    /* level 3 = PML4, 2 = PDPT, 1 = PD, 0 = PT */
    int shift = 12 + (level * 9);
    return (virt >> shift) & 0x1FF;
}

static pt_entry_t *vmm_get_or_create_table(pt_entry_t *parent, uintptr_t index, uint64_t flags) {
    if (parent[index] & VMM_PRESENT) {
        return (pt_entry_t *)((parent[index] & ~0xFFF) + KERNEL_VBASE);
    }
    void *page = pmm_alloc_page();
    if (!page) {
        klog_emerg("VMM: Out of memory allocating page table");
        cli(); hlt();
        for (;;) { }
    }
    parent[index] = ((uintptr_t)page) | flags;
    pt_entry_t *table = (pt_entry_t *)((uintptr_t)page + KERNEL_VBASE);
    for (int i = 0; i < PT_ENTRIES; i++) {
        table[i] = 0;
    }
    return table;
}

void vmm_init(void) {
    klog_info("Initializing VMM (paging)");

    /* Allocate a new PML4 for the kernel */
    g_vmm_kernel_pml4 = (pml4_t *)((uintptr_t)pmm_alloc_page() + KERNEL_VBASE);
    for (int i = 0; i < PT_ENTRIES; i++) {
        g_vmm_kernel_pml4->entries[i] = 0;
    }

    /* Identity map first 1GB (for early boot structures) */
    vmm_map_range(0, 0, 0x40000, VMM_PRESENT | VMM_WRITABLE);

    /* Higher half kernel mapping */
    vmm_map_range(KERNEL_VBASE, 0, 0x40000, VMM_PRESENT | VMM_WRITABLE);

    /* Switch to new PML4 */
    vmm_switch_pml4(g_vmm_kernel_pml4);

    klog_info("VMM initialized with new kernel PML4 at 0x%016llx",
              (unsigned long long)VIRT_TO_PHYS((uintptr_t)g_vmm_kernel_pml4));
}

void vmm_map_page(uintptr_t virt, uintptr_t phys, uint64_t flags) {
    if (!g_vmm_kernel_pml4) return;

    uintptr_t pml4i = vmm_virt_to_index(virt, 3);
    uintptr_t pdpti = vmm_virt_to_index(virt, 2);
    uintptr_t pdi   = vmm_virt_to_index(virt, 1);
    uintptr_t pti   = vmm_virt_to_index(virt, 0);

    pt_entry_t *pdpt = vmm_get_or_create_table(g_vmm_kernel_pml4->entries, pml4i,
                                               VMM_PRESENT | VMM_WRITABLE);
    pt_entry_t *pd   = vmm_get_or_create_table(pdpt, pdpti,
                                               VMM_PRESENT | VMM_WRITABLE);
    pt_entry_t *pt   = vmm_get_or_create_table(pd, pdi,
                                               VMM_PRESENT | VMM_WRITABLE);

    pt[pti] = (phys & ~0xFFF) | (flags & 0xFFF) | VMM_PRESENT;
    vmm_invalidate_page(virt);
}

void vmm_unmap_page(uintptr_t virt) {
    if (!g_vmm_kernel_pml4) return;

    pt_entry_t *pte = vmm_get_pte(virt);
    if (pte && (*pte & VMM_PRESENT)) {
        *pte = 0;
        vmm_invalidate_page(virt);
    }
}

bool vmm_is_mapped(uintptr_t virt) {
    pt_entry_t *pte = vmm_get_pte(virt);
    return (pte && (*pte & VMM_PRESENT)) != 0;
}

uintptr_t vmm_get_phys(uintptr_t virt) {
    pt_entry_t *pte = vmm_get_pte(virt);
    if (!pte || !(*pte & VMM_PRESENT)) return 0;
    return (*pte & ~0xFFF) | (virt & 0xFFF);
}

pt_entry_t *vmm_get_pte(uintptr_t virt) {
    if (!g_vmm_kernel_pml4) return NULL;

    uintptr_t pml4i = vmm_virt_to_index(virt, 3);
    if (!(g_vmm_kernel_pml4->entries[pml4i] & VMM_PRESENT)) return NULL;

    pt_entry_t *pdpt = (pt_entry_t *)((g_vmm_kernel_pml4->entries[pml4i] & ~0xFFF) + KERNEL_VBASE);
    uintptr_t pdpti = vmm_virt_to_index(virt, 2);
    if (!(pdpt[pdpti] & VMM_PRESENT)) return NULL;

    pt_entry_t *pd = (pt_entry_t *)((pdpt[pdpti] & ~0xFFF) + KERNEL_VBASE);
    uintptr_t pdi = vmm_virt_to_index(virt, 1);
    if (!(pd[pdi] & VMM_PRESENT)) return NULL;

    pt_entry_t *pt = (pt_entry_t *)((pd[pdi] & ~0xFFF) + KERNEL_VBASE);
    uintptr_t pti = vmm_virt_to_index(virt, 0);
    return &pt[pti];
}

pml4_t *vmm_get_pml4(void) {
    return g_vmm_kernel_pml4;
}

void vmm_switch_pml4(pml4_t *pml4) {
    write_cr3(VIRT_TO_PHYS((uintptr_t)pml4));
}

void vmm_invalidate_page(uintptr_t virt) {
    invlpg(virt);
}

void vmm_invalidate_all(void) {
    write_cr3(read_cr3());
}
