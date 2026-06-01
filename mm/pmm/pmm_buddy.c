/* gOS Kernel — Buddy Allocator Implementation (stub)
 * Reference: https://wiki.osdev.org/Page_Frame_Allocation
 */

#include "mm/pmm/pmm_buddy.h"
#include "gOS/klog.h"

void buddy_init(uintptr_t base, size_t total_pages) {
    (void)base; (void)total_pages;
    klog_info("Buddy allocator initialized (stub)");
}

void *buddy_alloc(size_t order) {
    (void)order;
    klog_warn("buddy_alloc: stub, use pmm_alloc_page()");
    return NULL;
}

void buddy_free(void *page, size_t order) {
    (void)page; (void)order;
    klog_warn("buddy_free: stub, use pmm_free_page()");
}

size_t buddy_get_free_pages(void) {
    return 0;
}

void buddy_dump(void) {
    klog_info("Buddy allocator: stub, no data");
}
