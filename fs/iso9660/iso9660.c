/* gOS Kernel — ISO 9660 Filesystem Implementation
 * Reference: https://wiki.osdev.org/ISO_9660
 */

#include "fs/iso9660/iso9660.h"
#include "fs/block/block.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

static uint64_t g_iso9660_start = 0;
static iso9660_primary_t g_iso9660_primary;

void iso9660_init(void) {
    klog_info("ISO 9660 driver initialized");
}

bool iso9660_probe(uint64_t start_lba) {
    g_iso9660_start = start_lba;

    uint8_t sector[ISO_SECTOR_SIZE];
    if (!block_read(start_lba + (ISO_VOLUME_DESC_OFF / ISO_SECTOR_SIZE), 1, sector)) {
        return false;
    }

    iso9660_volume_desc_t *vd = (iso9660_volume_desc_t *)sector;
    if (vd->type != ISO_VD_PRIMARY || strncmp(vd->id, "CD001", 5) != 0) {
        klog_warn("ISO 9660: not a valid ISO 9660 volume");
        return false;
    }

    memcpy(&g_iso9660_primary, vd, sizeof(iso9660_primary_t));

    klog_info("ISO 9660: volume '%s'", g_iso9660_primary.volume_id);
    klog_info("  Block size: %u, Volume size: %u blocks",
              g_iso9660_primary.logical_block_size_le,
              g_iso9660_primary.volume_space_size_le);
    return true;
}

void iso9660_list_root(void) {
    if (!g_iso9660_primary.volume_id[0]) {
        klog_warn("ISO 9660: not mounted");
        return;
    }

    iso9660_dir_entry_t *root = (iso9660_dir_entry_t *)g_iso9660_primary.root_dir;
    uint32_t lba = root->extent_lba_le;
    uint32_t len = root->data_length_le;

    uint8_t *buf = kmalloc(len);
    if (!buf) return;

    uint32_t sectors = (len + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
    for (uint32_t i = 0; i < sectors; i++) {
        block_read(g_iso9660_start + lba + i, 1, buf + i * ISO_SECTOR_SIZE);
    }

    klog_info("ISO 9660 root directory:");
    uint8_t *ptr = buf;
    while (ptr < buf + len) {
        iso9660_dir_entry_t *e = (iso9660_dir_entry_t *)ptr;
        if (e->length == 0) break;
        if (e->name_len == 1 && (e->name[0] == 0 || e->name[0] == 1)) {
            /* Skip . and .. entries */
        } else {
            char name[32];
            size_t n = e->name_len < 31 ? e->name_len : 31;
            memcpy(name, e->name, n);
            name[n] = '\0';
            const char *type = (e->file_flags & ISO_FLAG_DIRECTORY) ? "dir" : "file";
            klog_info("  %-4s %s (%u bytes)", type, name, e->data_length_le);
        }
        ptr += e->length;
    }
    kfree(buf);
}
