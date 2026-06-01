/* gOS Kernel — Simple Hash Functions */

#ifndef GOS_HASH_H
#define GOS_HASH_H

#include "gOS/types.h"

uint32_t djb2_hash(const char *str);
uint32_t fnv1a_hash(const void *data, size_t len);
uint32_t fnv1a_hash32(const void *data, size_t len);

#endif /* GOS_HASH_H */
