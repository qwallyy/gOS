/* gOS Kernel — Memory Stress Test
 * Allocates and frees many objects rapidly.
 */

#include "mm/pmm/pmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/assert.h"

static void test_pmm_stress(void) {
    klog_info("[STRESS] PMM stress test: allocating 256 pages");
    void *pages[256];
    for (int i = 0; i < 256; i++) {
        pages[i] = pmm_alloc_page();
        assert(pages[i] != NULL);
    }
    for (int i = 0; i < 256; i++) {
        pmm_free_page(pages[i]);
    }
    klog_info("[STRESS] PMM stress PASSED");
}

static void test_heap_stress(void) {
    klog_info("[STRESS] Heap stress test: 512 allocations");
    void *ptrs[512];
    for (int i = 0; i < 512; i++) {
        ptrs[i] = kmalloc((i % 16 + 1) * 64);
        assert(ptrs[i] != NULL);
    }
    for (int i = 0; i < 512; i++) {
        kfree(ptrs[i]);
    }
    klog_info("[STRESS] Heap stress PASSED");
}

static void test_heap_fragmentation(void) {
    klog_info("[STRESS] Heap fragmentation test");
    void *a = kmalloc(1024);
    void *b = kmalloc(1024);
    void *c = kmalloc(1024);
    kfree(b); /* Create hole */
    void *d = kmalloc(512); /* Should fit in hole */
    assert(d != NULL);
    kfree(a);
    kfree(c);
    kfree(d);
    klog_info("[STRESS] Fragmentation test PASSED");
}

void run_memory_stress_tests(void) {
    klog_info("================================");
    klog_info("  Memory Stress Tests");
    klog_info("================================");

    test_pmm_stress();
    test_heap_stress();
    test_heap_fragmentation();

    klog_info("================================");
    klog_info("  Memory Stress PASSED");
    klog_info("================================");
}
