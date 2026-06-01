/* gOS Kernel — VMM High-Level Mapping Implementation */

#include "mm/vmm/vmm_map.h"
#include "mm/vmm/vmm.h"
#include "mm/pmm/pmm.h"
#include "lib/math.h"
#include "gOS/klog.h"

void vmm_map_range(uintptr_t virt, uintptr_t phys, size_t count, uint64_t flags) {
    for (size_t i = 0; i < count; i++) {
        vmm_map_page(virt + i * PAGE_SIZE, phys + i * PAGE_SIZE, flags);
    }
}

void vmm_unmap_range(uintptr_t virt, size_t count) {
    for (size_t i = 0; i < count; i++) {
        vmm_unmap_page(virt + i * PAGE_SIZE);
    }
}

void vmm_map_identity(uintptr_t start, uintptr_t end, uint64_t flags) {
    start = ALIGN_DOWN(start, PAGE_SIZE);
    end = ALIGN_UP(end, PAGE_SIZE);
    for (uintptr_t addr = start; addr < end; addr += PAGE_SIZE) {
        vmm_map_page(addr, addr, flags);
    }
}

void vmm_map_kernel_text(void) {
    extern char _start[];
    extern char _etext[];
    uintptr_t start = (uintptr_t)_start;
    uintptr_t end = (uintptr_t)_etext;
    start = ALIGN_DOWN(start, PAGE_SIZE);
    end = ALIGN_UP(end, PAGE_SIZE);
    for (uintptr_t addr = start; addr < end; addr += PAGE_SIZE) {
        vmm_map_page(addr, addr - KERNEL_VBASE,
                     PAGE_PRESENT | PAGE_NX);
    }
}

void vmm_map_kernel_data(void) {
    extern char _edata[];
    extern char _end[];
    uintptr_t start = (uintptr_t)_edata;
    uintptr_t end = (uintptr_t)_end;
    start = ALIGN_DOWN(start, PAGE_SIZE);
    end = ALIGN_UP(end, PAGE_SIZE);
    for (uintptr_t addr = start; addr < end; addr += PAGE_SIZE) {
        vmm_map_page(addr, addr - KERNEL_VBASE,
                     PAGE_PRESENT | PAGE_WRITABLE);
    }
}

bool vmm_protect_page(uintptr_t virt, uint64_t new_flags) {
    uintptr_t phys = vmm_get_phys(virt);
    if (!phys) return false;
    vmm_unmap_page(virt);
    vmm_map_page(virt, phys, new_flags);
    return true;
}
