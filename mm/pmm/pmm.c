/* gOS Kernel — Physical Memory Manager Implementation
 * Reference: https://wiki.osdev.org/Page_Frame_Allocation
 */

#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

#define PMM_MAX_PAGES       0x1000000  /* 64GB max */
#define PMM_BITMAP_SIZE     (PMM_MAX_PAGES / 8)

static uint8_t g_pmm_bitmap[PMM_BITMAP_SIZE];
static uintptr_t g_pmm_total_pages = 0;
static uintptr_t g_pmm_used_pages = 0;
static uintptr_t g_pmm_first_free_page = 0;

static inline void pmm_set_bit(uintptr_t page) {
    g_pmm_bitmap[page / 8] |= (1 << (page % 8));
}

static inline void pmm_clear_bit(uintptr_t page) {
    g_pmm_bitmap[page / 8] &= ~(1 << (page % 8));
}

static inline bool pmm_test_bit(uintptr_t page) {
    return (g_pmm_bitmap[page / 8] & (1 << (page % 8))) != 0;
}

static inline uintptr_t phys_to_page(uintptr_t addr) {
    return addr / PMM_PAGE_SIZE;
}

void pmm_init(void) {
    /* Clear bitmap */
    for (size_t i = 0; i < PMM_BITMAP_SIZE; i++) {
        g_pmm_bitmap[i] = 0xFF; /* Mark all as used initially */
    }

    /* Parse multiboot2 memory map to mark available regions */
    /* For now, mark 1MB to 256MB as available */
    uintptr_t avail_start = 0x100000;
    uintptr_t avail_end = 0x10000000; /* 256MB */

    g_pmm_total_pages = (avail_end - avail_start) / PMM_PAGE_SIZE;

    for (uintptr_t addr = avail_start; addr < avail_end; addr += PMM_PAGE_SIZE) {
        pmm_clear_bit(phys_to_page(addr));
    }

    /* Mark kernel memory as used (0x100000 to _kernel_end virtual mapped physical) */
    extern char _kernel_end[];
    uintptr_t kernel_end_phys = (uintptr_t)_kernel_end - KERNEL_VBASE;
    pmm_mark_used(0x100000, (kernel_end_phys - 0x100000 + PMM_PAGE_SIZE - 1) / PMM_PAGE_SIZE);

    g_pmm_first_free_page = phys_to_page(avail_start);

    klog_info("PMM initialized: %llu pages total (%llu MiB), %llu used",
              (unsigned long long)g_pmm_total_pages,
              (unsigned long long)(g_pmm_total_pages * PMM_PAGE_SIZE / (1024 * 1024)),
              (unsigned long long)g_pmm_used_pages);
}

void pmm_mark_used(uintptr_t phys_addr, size_t pages) {
    uintptr_t start_page = phys_to_page(PMM_PAGE_ALIGN_DOWN(phys_addr));
    for (size_t i = 0; i < pages; i++) {
        if (!pmm_test_bit(start_page + i)) {
            pmm_set_bit(start_page + i);
            g_pmm_used_pages++;
        }
    }
}

void pmm_mark_free(uintptr_t phys_addr, size_t pages) {
    uintptr_t start_page = phys_to_page(PMM_PAGE_ALIGN_DOWN(phys_addr));
    for (size_t i = 0; i < pages; i++) {
        if (pmm_test_bit(start_page + i)) {
            pmm_clear_bit(start_page + i);
            g_pmm_used_pages--;
            if ((start_page + i) < g_pmm_first_free_page) {
                g_pmm_first_free_page = start_page + i;
            }
        }
    }
}

void *pmm_alloc_page(void) {
    for (uintptr_t page = g_pmm_first_free_page; page < g_pmm_total_pages; page++) {
        if (!pmm_test_bit(page)) {
            pmm_set_bit(page);
            g_pmm_used_pages++;
            g_pmm_first_free_page = page + 1;
            return (void *)(page * PMM_PAGE_SIZE);
        }
    }
    klog_err("PMM: Out of physical memory!");
    return NULL;
}

void *pmm_alloc_pages(size_t count) {
    if (count == 0) return NULL;
    if (count == 1) return pmm_alloc_page();

    /* Simple first-fit allocation for contiguous pages */
    for (uintptr_t page = g_pmm_first_free_page; page + count <= g_pmm_total_pages; page++) {
        bool found = true;
        for (size_t i = 0; i < count; i++) {
            if (pmm_test_bit(page + i)) {
                found = false;
                page += i; /* Skip ahead */
                break;
            }
        }
        if (found) {
            for (size_t i = 0; i < count; i++) {
                pmm_set_bit(page + i);
            }
            g_pmm_used_pages += count;
            g_pmm_first_free_page = page + count;
            return (void *)(page * PMM_PAGE_SIZE);
        }
    }
    klog_err("PMM: Failed to allocate %zu contiguous pages", count);
    return NULL;
}

void pmm_free_page(void *page) {
    if (!page) return;
    uintptr_t p = phys_to_page((uintptr_t)page);
    if (pmm_test_bit(p)) {
        pmm_clear_bit(p);
        g_pmm_used_pages--;
        if (p < g_pmm_first_free_page) {
            g_pmm_first_free_page = p;
        }
    }
}

void pmm_free_pages(void *pages, size_t count) {
    if (!pages) return;
    for (size_t i = 0; i < count; i++) {
        pmm_free_page((void *)((uintptr_t)pages + i * PMM_PAGE_SIZE));
    }
}

uintptr_t pmm_get_total_memory(void) {
    return g_pmm_total_pages * PMM_PAGE_SIZE;
}

uintptr_t pmm_get_used_memory(void) {
    return g_pmm_used_pages * PMM_PAGE_SIZE;
}

uintptr_t pmm_get_free_memory(void) {
    return (g_pmm_total_pages - g_pmm_used_pages) * PMM_PAGE_SIZE;
}

void pmm_dump_stats(void) {
    klog_info("PMM Stats: total=%llu pages, used=%llu, free=%llu",
              (unsigned long long)g_pmm_total_pages,
              (unsigned long long)g_pmm_used_pages,
              (unsigned long long)(g_pmm_total_pages - g_pmm_used_pages));
}
