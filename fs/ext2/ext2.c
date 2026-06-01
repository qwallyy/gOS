/* gOS Kernel — ext2 Filesystem Implementation (stub)
 * Reference: https://wiki.osdev.org/Ext2
 */

#include "fs/ext2/ext2.h"
#include "fs/block/block.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

static ext2_superblock_t g_ext2_sb;

void ext2_init(void) {
    klog_info("ext2 driver initialized (stub)");
}

bool ext2_mount(uint32_t partition_lba, vfs_ops_t **ops_out) {
    uint8_t sector[512];
    if (!block_read(partition_lba + 2, 1, sector)) {
        klog_err("ext2: failed to read superblock");
        return false;
    }

    memcpy(&g_ext2_sb, sector, sizeof(ext2_superblock_t));
    if (g_ext2_sb.magic != EXT2_MAGIC) {
        klog_warn("ext2: invalid magic 0x%04x", g_ext2_sb.magic);
        return false;
    }

    klog_info("ext2 mounted at LBA %u", partition_lba);
    klog_info("  Inodes: %u, Blocks: %u, Block size: %u bytes",
              g_ext2_sb.inodes_count, g_ext2_sb.blocks_count,
              1024 << g_ext2_sb.log_block_size);

    if (ops_out) {
        /* No VFS ops registered in stub */
        *ops_out = NULL;
    }
    return true;
}
