/* gOS Kernel — Memory Integration Test
 * Verifies PMM -> VMM -> heap chain works end-to-end.
 */

#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/assert.h"

static void test_alloc_chain(void) {
    /* Allocate physical page */
    void *phys = pmm_alloc_page();
    assert(phys != NULL);

    /* Map it at a known virtual address */
    uintptr_t virt = 0xFFFFFFFF81000000ULL;
    vmm_map_page(virt, (uintptr_t)phys, PAGE_PRESENT | PAGE_WRITABLE);
    assert(vmm_is_mapped(virt));

    /* Verify physical address round-trips */
    uintptr_t phys_back = vmm_get_phys(virt);
    assert((phys_back & ~0xFFF) == (uintptr_t)phys);

    /* Write through virtual address */
    volatile uint32_t *ptr = (volatile uint32_t *)virt;
    *ptr = 0xDEADBEEF;
    assert(*ptr == 0xDEADBEEF);

    /* Unmap */
    vmm_unmap_page(virt);
    assert(!vmm_is_mapped(virt));

    pmm_free_page(phys);
    klog_info("[MEM_TEST] Alloc chain PASSED");
}

static void test_heap_stress(void) {
    void *ptrs[128];
    for (int i = 0; i < 128; i++) {
        ptrs[i] = kmalloc(1024);
        assert(ptrs[i] != NULL);
    }
    for (int i = 0; i < 128; i++) {
        kfree(ptrs[i]);
    }
    klog_info("[MEM_TEST] Heap stress PASSED");
}

void run_memory_integration_tests(void) {
    klog_info("================================");
    klog_info("  Memory Integration Tests");
    klog_info("================================");

    test_alloc_chain();
    test_heap_stress();

    klog_info("================================");
    klog_info("  Memory Tests PASSED");
    klog_info("================================");
}
