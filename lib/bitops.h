/* gOS Kernel — Bit Operations */

#ifndef GOS_BITOPS_H
#define GOS_BITOPS_H

#include "gOS/types.h"

static inline int clz32(uint32_t x) {
    return __builtin_clz(x);
}

static inline int clz64(uint64_t x) {
    return __builtin_clzll(x);
}

static inline int ctz32(uint32_t x) {
    return __builtin_ctz(x);
}

static inline int ctz64(uint64_t x) {
    return __builtin_ctzll(x);
}

static inline int popcount32(uint32_t x) {
    return __builtin_popcount(x);
}

static inline int popcount64(uint64_t x) {
    return __builtin_popcountll(x);
}

static inline int ffs32(uint32_t x) {
    return __builtin_ffs(x);
}

static inline int ffs64(uint64_t x) {
    return __builtin_ffsll(x);
}

static inline uint32_t rol32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static inline uint32_t ror32(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

static inline uint64_t rol64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

static inline uint64_t ror64(uint64_t x, int n) {
    return (x >> n) | (x << (64 - n));
}

static inline uint32_t swap_bytes32(uint32_t x) {
    return __builtin_bswap32(x);
}

static inline uint64_t swap_bytes64(uint64_t x) {
    return __builtin_bswap64(x);
}

/* Find first set bit, zero-indexed */
static inline int first_set_bit(uint64_t x) {
    if (x == 0) return -1;
    return ctz64(x);
}

/* Find last set bit */
static inline int last_set_bit(uint64_t x) {
    if (x == 0) return -1;
    return 63 - clz64(x);
}

/* Set bit range [lo, hi] inclusive */
static inline uint64_t bit_range_mask(int lo, int hi) {
    return ((1ULL << (hi - lo + 1)) - 1) << lo;
}

#endif /* GOS_BITOPS_H */
