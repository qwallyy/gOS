/* gOS Kernel — Hash Function Implementations */

#include "lib/hash.h"

uint32_t djb2_hash(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

uint32_t fnv1a_hash(const void *data, size_t len) {
    const uint8_t *bytes = data;
    uint32_t hash = 2166136261U;
    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= 16777619U;
    }
    return hash;
}

uint32_t fnv1a_hash32(const void *data, size_t len) {
    return fnv1a_hash(data, len);
}
