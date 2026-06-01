/* gOS Kernel — Heap Unit Tests */

#include "mm/heap/heap.h"
#include "lib/assert.h"
#include "lib/string.h"

static void test_kmalloc_kfree(void) {
    void *p = kmalloc(64);
    assert(p != NULL);
    kfree(p);
}

static void test_kzalloc(void) {
    uint8_t *p = kzalloc(128);
    assert(p != NULL);
    for (int i = 0; i < 128; i++) {
        assert(p[i] == 0);
    }
    kfree(p);
}

static void test_krealloc(void) {
    uint8_t *p = kmalloc(32);
    assert(p != NULL);
    memset(p, 0xAB, 32);

    uint8_t *q = krealloc(p, 64);
    assert(q != NULL);
    for (int i = 0; i < 32; i++) {
        assert(q[i] == 0xAB);
    }
    kfree(q);
}

static void test_ksize(void) {
    void *p = kmalloc(100);
    assert(p != NULL);
    size_t sz = ksize(p);
    assert(sz >= 100);
    kfree(p);
}

static void test_kmalloc_multiple(void) {
    void *ptrs[16];
    for (int i = 0; i < 16; i++) {
        ptrs[i] = kmalloc(256);
        assert(ptrs[i] != NULL);
    }
    for (int i = 0; i < 16; i++) {
        kfree(ptrs[i]);
    }
}

void run_heap_tests(void) {
    test_kmalloc_kfree();
    test_kzalloc();
    test_krealloc();
    test_ksize();
    test_kmalloc_multiple();
}
