/* gOS Kernel — FAT32 Filesystem Implementation
 * Reference: https://wiki.osdev.org/FAT
 */

#include "fs/fat32/fat32.h"
#include "fs/block/block.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

static fat32_bpb_t g_fat32_bpb;
static uint32_t g_fat32_partition_lba = 0;
static uint32_t g_fat32_fat_start = 0;
static uint32_t g_fat32_data_start = 0;
static uint32_t g_fat32_sectors_per_cluster = 0;
static uint32_t g_fat32_total_clusters = 0;

static bool fat32_read_sector(uint32_t lba, void *buf) {
    return block_read(g_fat32_partition_lba + lba, 1, buf);
}

static uint32_t fat32_cluster_to_lba(uint32_t cluster) {
    return g_fat32_data_start + (cluster - 2) * g_fat32_sectors_per_cluster;
}

static uint32_t __attribute__((used)) fat32_read_fat_entry(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = g_fat32_fat_start + (fat_offset / FAT32_SECTOR_SIZE);
    uint32_t ent_offset = fat_offset % FAT32_SECTOR_SIZE;

    uint8_t buf[FAT32_SECTOR_SIZE];
    if (!fat32_read_sector(fat_sector, buf)) return 0x0FFFFFF8;

    uint32_t *fat_entry = (uint32_t *)&buf[ent_offset];
    uint32_t next = *fat_entry & 0x0FFFFFFF;
    return (next >= 0x0FFFFFF8) ? 0x0FFFFFF8 : next;
}

static fat32_dir_entry_t * __attribute__((used)) fat32_read_dir(uint32_t cluster, size_t *count) {
    uint32_t sectors = g_fat32_sectors_per_cluster;
    uint8_t *buf = kmalloc(sectors * FAT32_SECTOR_SIZE);
    if (!buf) return NULL;

    uint32_t lba = fat32_cluster_to_lba(cluster);
    for (uint32_t i = 0; i < sectors; i++) {
        if (!fat32_read_sector(lba + i, buf + i * FAT32_SECTOR_SIZE)) {
            kfree(buf);
            return NULL;
        }
    }

    /* Count entries */
    size_t n = 0;
    fat32_dir_entry_t *entries = (fat32_dir_entry_t *)buf;
    for (size_t i = 0; i < (sectors * FAT32_SECTOR_SIZE) / sizeof(fat32_dir_entry_t); i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        n++;
    }
    if (count) *count = n;
    return entries;
}

/* VFS ops stubs */
static vfs_node_t *fat32_vfs_lookup(vfs_node_t *parent, const char *name) {
    (void)parent; (void)name;
    klog_debug("fat32_lookup: %s", name);
    return NULL;
}

static int fat32_vfs_read(vfs_node_t *node, uint64_t offset, void *buf, size_t count) {
    (void)node; (void)offset; (void)buf; (void)count;
    return 0;
}

static int fat32_vfs_write(vfs_node_t *node, uint64_t offset, const void *buf, size_t count) {
    (void)node; (void)offset; (void)buf; (void)count;
    return 0;
}

static int fat32_vfs_readdir(vfs_node_t *node, uint64_t offset, void *buf, size_t count) {
    (void)node; (void)offset; (void)buf; (void)count;
    return 0;
}

static vfs_ops_t g_fat32_vfs_ops = {
    .lookup = fat32_vfs_lookup,
    .read = fat32_vfs_read,
    .write = fat32_vfs_write,
    .readdir = fat32_vfs_readdir,
    .create = NULL,
    .mkdir = NULL,
    .remove = NULL,
};

void fat32_init(void) {
    klog_info("FAT32 driver initialized (not mounted)");
}

bool fat32_mount(uint32_t partition_lba, vfs_ops_t **ops_out) {
    g_fat32_partition_lba = partition_lba;

    uint8_t sector[FAT32_SECTOR_SIZE];
    if (!fat32_read_sector(0, sector)) {
        klog_err("FAT32: failed to read boot sector");
        return false;
    }

    memcpy(&g_fat32_bpb, sector, sizeof(g_fat32_bpb));

    if (g_fat32_bpb.bytes_per_sector != FAT32_SECTOR_SIZE) {
        klog_err("FAT32: unsupported sector size %u", g_fat32_bpb.bytes_per_sector);
        return false;
    }

    g_fat32_sectors_per_cluster = g_fat32_bpb.sectors_per_cluster;
    g_fat32_fat_start = g_fat32_bpb.reserved_sectors;
    g_fat32_data_start = g_fat32_fat_start + (g_fat32_bpb.num_fats * g_fat32_bpb.fat_size_32);
    g_fat32_total_clusters = (g_fat32_bpb.total_sectors_32 - g_fat32_data_start) / g_fat32_sectors_per_cluster;

    klog_info("FAT32 mounted at LBA %u", partition_lba);
    klog_info("  Sectors/Cluster: %u, FAT start: %u, Data start: %u",
              g_fat32_sectors_per_cluster, g_fat32_fat_start, g_fat32_data_start);
    klog_info("  Root cluster: %u, Total clusters: %u",
              g_fat32_bpb.root_cluster, g_fat32_total_clusters);

    if (ops_out) *ops_out = &g_fat32_vfs_ops;
    return true;
}
