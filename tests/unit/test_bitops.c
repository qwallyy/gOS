/* gOS Kernel — Bit Operations Unit Tests */

#include "lib/bitops.h"
#include "lib/assert.h"

static void test_clz32(void) {
    assert(clz32(0x00000001) == 31);
    assert(clz32(0x80000000) == 0);
    assert(clz32(0x00010000) == 15);
}

static void test_ctz64(void) {
    assert(ctz64(1ULL) == 0);
    assert(ctz64(2ULL) == 1);
    assert(ctz64(0x100ULL) == 8);
}

static void test_popcount(void) {
    assert(popcount32(0) == 0);
    assert(popcount32(0xFF) == 8);
    assert(popcount32(0xAAAAAAAA) == 16);
    assert(popcount64(0xFFFFFFFFFFFFFFFFULL) == 64);
}

static void test_rol_ror(void) {
    assert(rol32(0x80000000, 1) == 1);
    assert(ror32(1, 1) == 0x80000000);
    assert(rol64(1ULL, 63) == 0x8000000000000000ULL);
}

static void test_first_last_set(void) {
    assert(first_set_bit(0x10) == 4);
    assert(last_set_bit(0x10) == 4);
    assert(first_set_bit(0x100) == 8);
    assert(last_set_bit(0x300) == 9);
    assert(first_set_bit(0) == -1);
}

static void test_swap_bytes(void) {
    assert(swap_bytes32(0x12345678) == 0x78563412);
    assert(swap_bytes64(0x0102030405060708ULL) == 0x0807060504030201ULL);
}

void run_bitops_tests(void) {
    test_clz32();
    test_ctz64();
    test_popcount();
    test_rol_ror();
    test_first_last_set();
    test_swap_bytes();
}
