/* gOS Kernel — CRC32 Checksum */

#ifndef GOS_CRC32_H
#define GOS_CRC32_H

#include "gOS/types.h"

uint32_t crc32(const void *data, size_t len);
uint32_t crc32_update(uint32_t crc, const void *data, size_t len);

#endif /* GOS_CRC32_H */
