/* gOS Kernel — Unaligned Memory Access Helpers
 * Safe packed reads/writes for potentially misaligned addresses.
 */

#ifndef GOS_UNALIGNED_H
#define GOS_UNALIGNED_H

#include "gOS/types.h"

static inline uint16_t read_unaligned_u16(const void *p) {
    const uint8_t *b = p;
    return (uint16_t)b[0] | ((uint16_t)b[1] << 8);
}

static inline uint32_t read_unaligned_u32(const void *p) {
    const uint8_t *b = p;
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

static inline uint64_t read_unaligned_u64(const void *p) {
    const uint8_t *b = p;
    return (uint64_t)b[0] | ((uint64_t)b[1] << 8) |
           ((uint64_t)b[2] << 16) | ((uint64_t)b[3] << 24) |
           ((uint64_t)b[4] << 32) | ((uint64_t)b[5] << 40) |
           ((uint64_t)b[6] << 48) | ((uint64_t)b[7] << 56);
}

static inline void write_unaligned_u16(void *p, uint16_t val) {
    uint8_t *b = p;
    b[0] = val & 0xFF;
    b[1] = (val >> 8) & 0xFF;
}

static inline void write_unaligned_u32(void *p, uint32_t val) {
    uint8_t *b = p;
    b[0] = val & 0xFF;
    b[1] = (val >> 8) & 0xFF;
    b[2] = (val >> 16) & 0xFF;
    b[3] = (val >> 24) & 0xFF;
}

static inline void write_unaligned_u64(void *p, uint64_t val) {
    uint8_t *b = p;
    b[0] = val & 0xFF;
    b[1] = (val >> 8) & 0xFF;
    b[2] = (val >> 16) & 0xFF;
    b[3] = (val >> 24) & 0xFF;
    b[4] = (val >> 32) & 0xFF;
    b[5] = (val >> 40) & 0xFF;
    b[6] = (val >> 48) & 0xFF;
    b[7] = (val >> 56) & 0xFF;
}

#endif /* GOS_UNALIGNED_H */
