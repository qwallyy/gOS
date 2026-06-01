/* gOS Kernel — PMM Unit Tests */

#include "mm/pmm/pmm.h"
#include "lib/assert.h"
#include "lib/string.h"

static void test_pmm_alloc_free(void) {
    void *p1 = pmm_alloc_page();
    assert(p1 != NULL);
    assert((uintptr_t)p1 % 4096 == 0);

    void *p2 = pmm_alloc_page();
    assert(p2 != NULL);
    assert(p2 != p1);

    pmm_free_page(p1);
    pmm_free_page(p2);
}

static void test_pmm_alloc_pages(void) {
    void *p = pmm_alloc_pages(4);
    assert(p != NULL);
    assert((uintptr_t)p % 4096 == 0);
    pmm_free_pages(p, 4);
}

static void test_pmm_stats(void) {
    uintptr_t total = pmm_get_total_memory();
    uintptr_t free = pmm_get_free_memory();
    assert(total > 0);
    assert(free <= total);
}

void run_pmm_tests(void) {
    test_pmm_alloc_free();
    test_pmm_alloc_pages();
    test_pmm_stats();
}
