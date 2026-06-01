/* gOS Kernel — Block Device Layer Implementation */

#include "fs/block/block.h"
#include "gOS/klog.h"
#include "lib/string.h"

#define MAX_BLOCK_DEVICES 8

static block_device_t g_block_devices[MAX_BLOCK_DEVICES];
static int g_block_device_count = 0;
static block_device_t *g_default_block = NULL;

void block_init(void) {
    klog_info("Block device layer initialized");
    for (int i = 0; i < MAX_BLOCK_DEVICES; i++) {
        g_block_devices[i].name[0] = '\0';
    }
}

bool block_register(const char *name, uint64_t sectors, uint32_t sector_size,
                    block_read_fn read, block_write_fn write, void *private) {
    if (g_block_device_count >= MAX_BLOCK_DEVICES) {
        klog_err("Block: max devices reached");
        return false;
    }
    block_device_t *dev = &g_block_devices[g_block_device_count++];
    strncpy(dev->name, name, 31);
    dev->name[31] = '\0';
    dev->total_sectors = sectors;
    dev->sector_size = sector_size;
    dev->read = read;
    dev->write = write;
    dev->private_data = private;
    if (!g_default_block) g_default_block = dev;
    klog_info("Block device '%s' registered: %llu sectors x %u bytes",
              name, (unsigned long long)sectors, sector_size);
    return true;
}

bool block_read(uint64_t lba, uint32_t count, void *buf) {
    if (!g_default_block || !g_default_block->read) {
        klog_err("Block: no default block device");
        return false;
    }
    return g_default_block->read(lba, count, buf);
}

bool block_write(uint64_t lba, uint32_t count, const void *buf) {
    if (!g_default_block || !g_default_block->write) {
        klog_err("Block: no default block device");
        return false;
    }
    return g_default_block->write(lba, count, buf);
}
