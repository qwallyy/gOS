/* gOS Kernel — Hash Function Unit Tests */

#include "lib/hash.h"
#include "lib/assert.h"
#include "lib/string.h"

static void test_djb2(void) {
    uint32_t h1 = djb2_hash("hello");
    uint32_t h2 = djb2_hash("hello");
    assert(h1 == h2);

    uint32_t h3 = djb2_hash("world");
    assert(h1 != h3);
}

static void test_fnv1a(void) {
    uint32_t h1 = fnv1a_hash("hello", 5);
    uint32_t h2 = fnv1a_hash("hello", 5);
    assert(h1 == h2);

    uint32_t h3 = fnv1a_hash("world", 5);
    assert(h1 != h3);
}

static void test_fnv1a_empty(void) {
    uint32_t h = fnv1a_hash("", 0);
    assert(h == 2166136261U);
}

void run_hash_tests(void) {
    test_djb2();
    test_fnv1a();
    test_fnv1a_empty();
}
