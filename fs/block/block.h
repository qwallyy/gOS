/* gOS Kernel — Block Device Layer
 * Reference: https://wiki.osdev.org/Block_Device
 */

#ifndef GOS_BLOCK_H
#define GOS_BLOCK_H

#include "gOS/types.h"

#define BLOCK_SECTOR_SIZE   512

typedef bool (*block_read_fn)(uint64_t lba, uint32_t count, void *buf);
typedef bool (*block_write_fn)(uint64_t lba, uint32_t count, const void *buf);

typedef struct block_device {
    char name[32];
    uint64_t total_sectors;
    uint32_t sector_size;
    block_read_fn read;
    block_write_fn write;
    void *private_data;
} block_device_t;

void block_init(void);
bool block_register(const char *name, uint64_t sectors, uint32_t sector_size,
                    block_read_fn read, block_write_fn write, void *private);
bool block_read(uint64_t lba, uint32_t count, void *buf);
bool block_write(uint64_t lba, uint32_t count, const void *buf);

#endif /* GOS_BLOCK_H */
