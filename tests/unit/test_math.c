/* gOS Kernel — Math Utility Unit Tests */

#include "lib/math.h"
#include "lib/assert.h"

static void test_abs(void) {
    assert(abs(-5) == 5);
    assert(abs(5) == 5);
    assert(abs(0) == 0);
}

static void test_min_max(void) {
    assert(imax(3, 7) == 7);
    assert(imin(3, 7) == 3);
    assert(umax64(100ULL, 50ULL) == 100ULL);
    assert(umin64(100ULL, 50ULL) == 50ULL);
}

static void test_div_round_up(void) {
    assert(div_round_up(10, 3) == 4);
    assert(div_round_up(9, 3) == 3);
    assert(div_round_up(1, 5) == 1);
}

static void test_align(void) {
    assert(align_up(0, 4096) == 0);
    assert(align_up(1, 4096) == 4096);
    assert(align_up(4096, 4096) == 4096);
    assert(align_up(4097, 4096) == 8192);
    assert(align_down(4097, 4096) == 4096);
    assert(is_aligned(4096, 4096));
    assert(!is_aligned(4097, 4096));
}

static void test_log2(void) {
    assert(log2_floor(1) == 0);
    assert(log2_floor(2) == 1);
    assert(log2_floor(4) == 2);
    assert(log2_floor(8) == 3);
    assert(log2_ceil(5) == 3);
    assert(log2_ceil(8) == 3);
}

void run_math_tests(void) {
    test_abs();
    test_min_max();
    test_div_round_up();
    test_align();
    test_log2();
}
