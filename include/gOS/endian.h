/* gOS Kernel — Endianness Helpers
 * Reference: https://wiki.osdev.org/Byte_Order
 */

#ifndef GOS_ENDIAN_H
#define GOS_ENDIAN_H

#include "gOS/types.h"

#define LITTLE_ENDIAN   1234
#define BIG_ENDIAN      4321
#define BYTE_ORDER      LITTLE_ENDIAN

static inline uint16_t bswap16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

static inline uint32_t bswap32(uint32_t x) {
    return ((x >> 24) & 0xFF)       |
           ((x >> 8)  & 0xFF00)     |
           ((x << 8)  & 0xFF0000)   |
           ((x << 24) & 0xFF000000);
}

static inline uint64_t bswap64(uint64_t x) {
    return ((x >> 56) & 0xFFULL)             |
           ((x >> 40) & 0xFF00ULL)           |
           ((x >> 24) & 0xFF0000ULL)         |
           ((x >> 8)  & 0xFF000000ULL)       |
           ((x << 8)  & 0xFF00000000ULL)     |
           ((x << 24) & 0xFF0000000000ULL)   |
           ((x << 40) & 0xFF000000000000ULL) |
           ((x << 56) & 0xFF00000000000000ULL);
}

#if BYTE_ORDER == LITTLE_ENDIAN
    #define cpu_to_le16(x) (x)
    #define cpu_to_le32(x) (x)
    #define cpu_to_le64(x) (x)
    #define le16_to_cpu(x) (x)
    #define le32_to_cpu(x) (x)
    #define le64_to_cpu(x) (x)
    #define cpu_to_be16(x) bswap16(x)
    #define cpu_to_be32(x) bswap32(x)
    #define cpu_to_be64(x) bswap64(x)
    #define be16_to_cpu(x) bswap16(x)
    #define be32_to_cpu(x) bswap32(x)
    #define be64_to_cpu(x) bswap64(x)
#else
    #define cpu_to_le16(x) bswap16(x)
    #define cpu_to_le32(x) bswap32(x)
    #define cpu_to_le64(x) bswap64(x)
    #define le16_to_cpu(x) bswap16(x)
    #define le32_to_cpu(x) bswap32(x)
    #define le64_to_cpu(x) bswap64(x)
    #define cpu_to_be16(x) (x)
    #define cpu_to_be32(x) (x)
    #define cpu_to_be64(x) (x)
    #define be16_to_cpu(x) (x)
    #define be32_to_cpu(x) (x)
    #define be64_to_cpu(x) (x)
#endif

#endif /* GOS_ENDIAN_H */
