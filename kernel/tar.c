/* gOS Kernel — TAR Archive Parser Implementation
 * Reference: https://wiki.osdev.org/Tar
 */

#include "kernel/tar.h"
#include "gOS/klog.h"
#include "lib/string.h"

static uintptr_t g_tar_start = 0;
static size_t g_tar_size = 0;

static size_t tar_octal_to_size(const char *str, size_t len) {
    size_t val = 0;
    for (size_t i = 0; i < len && str[i] >= '0' && str[i] <= '7'; i++) {
        val = val * 8 + (str[i] - '0');
    }
    return val;
}

void tar_init(uintptr_t start, size_t size) {
    g_tar_start = start;
    g_tar_size = size;
    klog_info("TAR archive at 0x%016llx, size=%zu",
              (unsigned long long)start, size);
}

bool tar_is_valid(const tar_header_t *header) {
    if (!header) return false;
    return memcmp(header->magic, TAR_MAGIC, 5) == 0;
}

size_t tar_get_size(const tar_header_t *header) {
    if (!header) return 0;
    return tar_octal_to_size(header->size, sizeof(header->size));
}

const tar_header_t *tar_find(const char *name) {
    if (!g_tar_start || !name) return NULL;
    uintptr_t ptr = g_tar_start;
    uintptr_t end = g_tar_start + g_tar_size;

    while (ptr + sizeof(tar_header_t) <= end) {
        const tar_header_t *hdr = (const tar_header_t *)ptr;
        if (hdr->name[0] == '\0') break;
        if (tar_is_valid(hdr) && strcmp(hdr->name, name) == 0) {
            return hdr;
        }
        size_t file_size = tar_get_size(hdr);
        size_t blocks = (file_size + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE;
        ptr += TAR_BLOCK_SIZE * (1 + blocks);
    }
    return NULL;
}

void tar_list(void) {
    if (!g_tar_start) {
        klog_info("TAR: no archive loaded");
        return;
    }
    klog_info("TAR archive contents:");
    uintptr_t ptr = g_tar_start;
    uintptr_t end = g_tar_start + g_tar_size;

    while (ptr + sizeof(tar_header_t) <= end) {
        const tar_header_t *hdr = (const tar_header_t *)ptr;
        if (hdr->name[0] == '\0') break;
        if (tar_is_valid(hdr)) {
            size_t sz = tar_get_size(hdr);
            const char *type = "?";
            switch (hdr->typeflag) {
                case TAR_TYPE_REGULAR: type = "file"; break;
                case TAR_TYPE_DIR:     type = "dir";  break;
                case TAR_TYPE_SYMLINK: type = "sym";  break;
            }
            klog_info("  %-10s %s (%zu bytes)", type, hdr->name, sz);
        }
        size_t file_size = tar_get_size(hdr);
        size_t blocks = (file_size + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE;
        ptr += TAR_BLOCK_SIZE * (1 + blocks);
    }
}
