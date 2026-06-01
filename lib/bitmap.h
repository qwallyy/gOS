/* gOS Kernel — Bitmap Utilities */

#ifndef GOS_BITMAP_H
#define GOS_BITMAP_H

#include "gOS/types.h"

static inline bool bitmap_get(const uint8_t *bitmap, size_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

static inline void bitmap_set(uint8_t *bitmap, size_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear(uint8_t *bitmap, size_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline void bitmap_toggle(uint8_t *bitmap, size_t bit) {
    bitmap[bit / 8] ^= (1 << (bit % 8));
}

static inline size_t bitmap_find_first_set(const uint8_t *bitmap, size_t bits) {
    for (size_t i = 0; i < bits; i++) {
        if (bitmap_get(bitmap, i)) return i;
    }
    return bits; /* Not found */
}

static inline size_t bitmap_find_first_clear(const uint8_t *bitmap, size_t bits) {
    for (size_t i = 0; i < bits; i++) {
        if (!bitmap_get(bitmap, i)) return i;
    }
    return bits; /* Not found */
}

static inline void bitmap_set_range(uint8_t *bitmap, size_t start, size_t count) {
    for (size_t i = 0; i < count; i++) {
        bitmap_set(bitmap, start + i);
    }
}

static inline void bitmap_clear_range(uint8_t *bitmap, size_t start, size_t count) {
    for (size_t i = 0; i < count; i++) {
        bitmap_clear(bitmap, start + i);
    }
}

static inline size_t bitmap_count_set(const uint8_t *bitmap, size_t bits) {
    size_t count = 0;
    for (size_t i = 0; i < bits; i++) {
        if (bitmap_get(bitmap, i)) count++;
    }
    return count;
}

#endif /* GOS_BITMAP_H */
