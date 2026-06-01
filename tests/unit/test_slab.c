/* gOS Kernel — Slab Allocator Unit Tests */

#include "lib/slab.h"
#include "lib/assert.h"

static void test_slab_create_alloc_free(void) {
    slab_cache_t *cache = slab_cache_create("test_cache", 64, 8, NULL, NULL);
    assert(cache != NULL);

    void *p1 = slab_alloc(cache);
    void *p2 = slab_alloc(cache);
    assert(p1 != NULL);
    assert(p2 != NULL);
    assert(p1 != p2);

    slab_free(cache, p1);
    slab_free(cache, p2);

    /* Reallocation should reuse */
    void *p3 = slab_alloc(cache);
    assert(p3 != NULL);
    slab_free(cache, p3);

    slab_cache_destroy(cache);
}

static void test_slab_multiple_allocs(void) {
    slab_cache_t *cache = slab_cache_create("multi_cache", 32, 8, NULL, NULL);
    assert(cache != NULL);

    void *ptrs[32];
    for (int i = 0; i < 32; i++) {
        ptrs[i] = slab_alloc(cache);
        assert(ptrs[i] != NULL);
    }
    for (int i = 0; i < 32; i++) {
        slab_free(cache, ptrs[i]);
    }
    slab_cache_destroy(cache);
}

void run_slab_tests(void) {
    test_slab_create_alloc_free();
    test_slab_multiple_allocs();
}
