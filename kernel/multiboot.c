/* gOS Kernel — Multiboot2 Info Parser
 * Reference: https://wiki.osdev.org/Multiboot
 */

#include "gOS/kernel.h"
#include "gOS/klog.h"
#include "mm/heap/heap.h"
#include "lib/string.h"

kernel_info_t g_kernel_info;

static multiboot2_tag_t *mb2_first_tag(uintptr_t info_addr) {
    /* First tag starts after the fixed header (8 bytes) */
    return (multiboot2_tag_t *)(info_addr + 8);
}

static multiboot2_tag_t *mb2_next_tag(multiboot2_tag_t *tag) {
    uintptr_t next = ((uintptr_t)tag + ((tag->size + 7) & ~7));
    return (multiboot2_tag_t *)next;
}

void parse_multiboot2_info(uintptr_t info_addr) {
    multiboot2_tag_t *tag = mb2_first_tag(info_addr);

    g_kernel_info.mmap = NULL;
    g_kernel_info.mmap_entries = 0;
    g_kernel_info.total_ram = 0;
    g_kernel_info.cmdline = NULL;
    g_kernel_info.bootloader_name = NULL;
    g_kernel_info.acpi_rsdp_old = NULL;
    g_kernel_info.acpi_rsdp_new = NULL;

    while (tag->type != MB2_TAG_END) {
        switch (tag->type) {
            case MB2_TAG_CMDLINE: {
                multiboot2_tag_module_t *mod = (multiboot2_tag_module_t *)tag;
                g_kernel_info.cmdline = (const char *)mod->cmdline;
                break;
            }
            case MB2_TAG_BOOTLOADER: {
                multiboot2_tag_module_t *mod = (multiboot2_tag_module_t *)tag;
                g_kernel_info.bootloader_name = (const char *)mod->cmdline;
                break;
            }
            case MB2_TAG_BASIC_MEM: {
                multiboot2_tag_basic_meminfo_t *mem = (multiboot2_tag_basic_meminfo_t *)tag;
                g_kernel_info.mem_lower_kb = mem->mem_lower;
                g_kernel_info.mem_upper_kb = mem->mem_upper;
                g_kernel_info.total_ram += (uint64_t)mem->mem_upper * 1024;
                break;
            }
            case MB2_TAG_MMAP: {
                multiboot2_tag_mmap_t *mmap = (multiboot2_tag_mmap_t *)tag;
                g_kernel_info.mmap = (multiboot2_mmap_entry_t *)((uintptr_t)mmap + 16);
                g_kernel_info.mmap_entries = (mmap->size - 16) / mmap->entry_size;
                g_kernel_info.mmap_entry_size = mmap->entry_size;
                break;
            }
            case MB2_TAG_FRAMEBUFFER: {
                multiboot2_tag_framebuffer_t *fb = (multiboot2_tag_framebuffer_t *)tag;
                g_kernel_info.framebuffer_addr = fb->framebuffer_addr;
                g_kernel_info.framebuffer_pitch = fb->framebuffer_pitch;
                g_kernel_info.framebuffer_width = fb->framebuffer_width;
                g_kernel_info.framebuffer_height = fb->framebuffer_height;
                g_kernel_info.framebuffer_bpp = fb->framebuffer_bpp;
                g_kernel_info.framebuffer_type = fb->framebuffer_type;
                break;
            }
            case MB2_TAG_ACPI_OLD:
                g_kernel_info.acpi_rsdp_old = (void *)((uintptr_t)tag + 8);
                break;
            case MB2_TAG_ACPI_NEW:
                g_kernel_info.acpi_rsdp_new = (void *)((uintptr_t)tag + 8);
                break;
            default:
                break;
        }
        tag = mb2_next_tag(tag);
    }

    klog_info("Multiboot2 parsed:");
    if (g_kernel_info.bootloader_name) {
        klog_info("  Bootloader: %s", g_kernel_info.bootloader_name);
    }
    if (g_kernel_info.cmdline) {
        klog_info("  Command line: %s", g_kernel_info.cmdline);
    }
    klog_info("  Lower mem: %u KB, Upper mem: %u KB",
              g_kernel_info.mem_lower_kb, g_kernel_info.mem_upper_kb);
    klog_info("  Total RAM: %llu MiB",
              (unsigned long long)(g_kernel_info.total_ram / (1024 * 1024)));
    if (g_kernel_info.mmap) {
        klog_info("  Memory map: %u entries", g_kernel_info.mmap_entries);
        for (uint32_t i = 0; i < g_kernel_info.mmap_entries; i++) {
            multiboot2_mmap_entry_t *e = (multiboot2_mmap_entry_t *)
                ((uintptr_t)g_kernel_info.mmap + i * g_kernel_info.mmap_entry_size);
            const char *type_str = "unknown";
            switch (e->type) {
                case MB2_MMAP_AVAILABLE: type_str = "available"; break;
                case MB2_MMAP_RESERVED: type_str = "reserved"; break;
                case MB2_MMAP_ACPI_RECLAIM: type_str = "ACPI reclaim"; break;
                case MB2_MMAP_ACPI_NVS: type_str = "ACPI NVS"; break;
                case MB2_MMAP_BAD: type_str = "bad"; break;
            }
            klog_info("    [%u] 0x%016llx - 0x%016llx (%s)",
                      i,
                      (unsigned long long)e->base_addr,
                      (unsigned long long)(e->base_addr + e->length),
                      type_str);
        }
    }
}
