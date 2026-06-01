/* gOS Kernel — Shared Memory Implementation
 * Reference: https://wiki.osdev.org/Shared_Memory
 */

#include "ipc/sharedmem.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

static shm_region_t g_shm_regions[SHM_MAX_REGIONS];

void shm_init(void) {
    klog_info("Shared memory subsystem initialized");
    for (int i = 0; i < SHM_MAX_REGIONS; i++) {
        g_shm_regions[i].valid = false;
    }
}

int shm_create(const char *name, size_t size, void **out_addr) {
    if (!name || !out_addr || size == 0) return -EINVAL;

    /* Check if already exists */
    for (int i = 0; i < SHM_MAX_REGIONS; i++) {
        if (g_shm_regions[i].valid && strcmp(g_shm_regions[i].name, name) == 0) {
            *out_addr = g_shm_regions[i].addr;
            g_shm_regions[i].refcount++;
            return 0;
        }
    }

    /* Allocate new region */
    for (int i = 0; i < SHM_MAX_REGIONS; i++) {
        if (!g_shm_regions[i].valid) {
            size_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
            void *phys = pmm_alloc_pages(pages);
            if (!phys) return -ENOMEM;

            uintptr_t virt = 0xFFFFFFFF80C00000ULL + i * 0x100000; /* Fixed per-region */
            vmm_map_range(virt, (uintptr_t)phys, pages,
                          PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);

            strncpy(g_shm_regions[i].name, name, SHM_NAME_MAX - 1);
            g_shm_regions[i].name[SHM_NAME_MAX - 1] = '\0';
            g_shm_regions[i].addr = (void *)virt;
            g_shm_regions[i].size = pages * PAGE_SIZE;
            g_shm_regions[i].refcount = 1;
            g_shm_regions[i].valid = true;
            *out_addr = g_shm_regions[i].addr;
            klog_info("SHM created '%s': %p size=%zu", name, *out_addr, g_shm_regions[i].size);
            return 0;
        }
    }
    return -ENOSPC;
}

int shm_open(const char *name, void **out_addr) {
    if (!name || !out_addr) return -EINVAL;
    for (int i = 0; i < SHM_MAX_REGIONS; i++) {
        if (g_shm_regions[i].valid && strcmp(g_shm_regions[i].name, name) == 0) {
            *out_addr = g_shm_regions[i].addr;
            g_shm_regions[i].refcount++;
            return 0;
        }
    }
    return -ENOENT;
}

int shm_close(const char *name) {
    for (int i = 0; i < SHM_MAX_REGIONS; i++) {
        if (g_shm_regions[i].valid && strcmp(g_shm_regions[i].name, name) == 0) {
            g_shm_regions[i].refcount--;
            if (g_shm_regions[i].refcount <= 0) {
                g_shm_regions[i].valid = false;
                klog_info("SHM destroyed '%s'", name);
            }
            return 0;
        }
    }
    return -ENOENT;
}

void shm_list(void) {
    klog_info("Shared memory regions:");
    for (int i = 0; i < SHM_MAX_REGIONS; i++) {
        if (!g_shm_regions[i].valid) continue;
        klog_info("  [%d] '%s' addr=%p size=%zu refcount=%d",
                  i, g_shm_regions[i].name, g_shm_regions[i].addr,
                  g_shm_regions[i].size, g_shm_regions[i].refcount);
    }
}
