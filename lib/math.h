/* gOS Kernel — Math Utilities */

#ifndef GOS_MATH_H
#define GOS_MATH_H

#include "gOS/types.h"

static inline int abs(int x) {
    return x < 0 ? -x : x;
}

static inline long labs(long x) {
    return x < 0 ? -x : x;
}

static inline long long llabs(long long x) {
    return x < 0 ? -x : x;
}

static inline int imax(int a, int b) {
    return a > b ? a : b;
}

static inline int imin(int a, int b) {
    return a < b ? a : b;
}

static inline long lmax(long a, long b) {
    return a > b ? a : b;
}

static inline long lmin(long a, long b) {
    return a < b ? a : b;
}

static inline uint32_t umax32(uint32_t a, uint32_t b) {
    return a > b ? a : b;
}

static inline uint32_t umin32(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

static inline uint64_t umax64(uint64_t a, uint64_t b) {
    return a > b ? a : b;
}

static inline uint64_t umin64(uint64_t a, uint64_t b) {
    return a < b ? a : b;
}

static inline int div_round_up(int a, int b) {
    return (a + b - 1) / b;
}

static inline uint64_t div64_round_up(uint64_t a, uint64_t b) {
    return (a + b - 1) / b;
}

static inline bool is_aligned(uintptr_t addr, size_t alignment) {
    return (addr & (alignment - 1)) == 0;
}

static inline uintptr_t align_up(uintptr_t addr, size_t alignment) {
    return (addr + alignment - 1) & ~(alignment - 1);
}

static inline uintptr_t align_down(uintptr_t addr, size_t alignment) {
    return addr & ~(alignment - 1);
}

#define ALIGN_UP(x, a)   align_up((uintptr_t)(x), (size_t)(a))
#define ALIGN_DOWN(x, a) align_down((uintptr_t)(x), (size_t)(a))

static inline size_t log2_ceil(size_t x) {
    if (x <= 1) return 0;
    return 64 - __builtin_clzll(x - 1);
}

static inline size_t log2_floor(size_t x) {
    if (x == 0) return 0;
    return 63 - __builtin_clzll(x);
}

#endif /* GOS_MATH_H */
