/* gOS Kernel — Initial RAM Disk Implementation
 * Reference: https://wiki.osdev.org/Initrd
 *
 * Simple tar-like initrd format:
 *   [header: magic, num_files]
 *   [file_header: name, offset, size] x num_files
 *   [raw file data]
 */

#include "kernel/initrd.h"
#include "gOS/klog.h"
#include "lib/string.h"

static initrd_file_t g_initrd_files[INITRD_MAX_FILES];
static uintptr_t g_initrd_base = 0;
static uintptr_t g_initrd_data_start = 0;
static bool g_initrd_present = false;

void initrd_init(uintptr_t start, uintptr_t end) {
    (void)end;
    g_initrd_base = start;

    if (!start) {
        klog_info("Initrd: not present");
        return;
    }

    initrd_header_t *hdr = (initrd_header_t *)start;
    if (hdr->magic != INITRD_MAGIC) {
        klog_warn("Initrd: invalid magic 0x%08x", hdr->magic);
        return;
    }

    uint32_t num_files = hdr->num_files;
    if (num_files > INITRD_MAX_FILES) {
        klog_warn("Initrd: too many files (%u), truncating to %d",
                  num_files, INITRD_MAX_FILES);
        num_files = INITRD_MAX_FILES;
    }

    /* File headers follow the main header */
    initrd_file_t *files = (initrd_file_t *)(start + sizeof(initrd_header_t));
    for (uint32_t i = 0; i < num_files; i++) {
        memcpy(&g_initrd_files[i], &files[i], sizeof(initrd_file_t));
        g_initrd_files[i].valid = true;
    }

    g_initrd_data_start = start + sizeof(initrd_header_t) +
                          num_files * sizeof(initrd_file_t);
    g_initrd_present = true;

    klog_info("Initrd: %u files loaded from 0x%016llx",
              num_files, (unsigned long long)start);
}

bool initrd_is_present(void) {
    return g_initrd_present;
}

const initrd_file_t *initrd_find(const char *name) {
    if (!name) return NULL;
    for (int i = 0; i < INITRD_MAX_FILES; i++) {
        if (g_initrd_files[i].valid && strcmp(g_initrd_files[i].name, name) == 0) {
            return &g_initrd_files[i];
        }
    }
    return NULL;
}

size_t initrd_read(const initrd_file_t *file, void *buf, size_t offset, size_t len) {
    if (!file || !buf || !file->valid) return 0;
    if (offset >= file->size) return 0;
    if (offset + len > file->size) {
        len = file->size - offset;
    }
    memcpy(buf, (const void *)(g_initrd_data_start + file->offset + offset), len);
    return len;
}

void initrd_list(void) {
    if (!g_initrd_present) {
        klog_info("Initrd: not present");
        return;
    }
    klog_info("Initrd files:");
    for (int i = 0; i < INITRD_MAX_FILES; i++) {
        if (g_initrd_files[i].valid) {
            klog_info("  %s (%u bytes)", g_initrd_files[i].name, g_initrd_files[i].size);
        }
    }
}

uintptr_t initrd_get_base(void) {
    return g_initrd_base;
}

size_t initrd_get_size(void) {
    if (!g_initrd_present) return 0;
    /* Approximate: data start to wherever end is */
    return 0; /* Not tracked in this simple format */
}
