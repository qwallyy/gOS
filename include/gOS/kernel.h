/* gOS Kernel — Core Kernel Definitions
 * Reference: https://wiki.osdev.org/Bare_Bones
 *
 * Central header for kernel-wide constants, version info, and
 * multiboot2 tag parsing structures.
 */

#ifndef GOS_KERNEL_H
#define GOS_KERNEL_H

#include "gOS/types.h"

/* gOS version */
#define GOS_VERSION_MAJOR   0
#define GOS_VERSION_MINOR   1
#define GOS_VERSION_PATCH   0
#define GOS_VERSION_STRING  "0.1.0-dev"

/* Kernel virtual memory base (higher half) */
#define KERNEL_VBASE        0xFFFFFFFF80000000ULL

/* Convert between physical and virtual addresses */
#define PHYS_TO_VIRT(p)     ((void *)((uintptr_t)(p) + KERNEL_VBASE))
#define VIRT_TO_PHYS(v)     ((uintptr_t)(v) - KERNEL_VBASE)

/* Page size and related constants */
#define PAGE_SIZE           4096
#define PAGE_SHIFT          12
#define PAGE_MASK           (PAGE_SIZE - 1)

/* Number of page table entries per table */
#define PT_ENTRIES          512

/* Page flags for x86-64 PML4/PDPT/PD/PT entries */
#define PAGE_PRESENT        (1ULL << 0)
#define PAGE_WRITABLE       (1ULL << 1)
#define PAGE_USER           (1ULL << 2)
#define PAGE_WRITETHROUGH   (1ULL << 3)
#define PAGE_NOCACHE        (1ULL << 4)
#define PAGE_ACCESSED       (1ULL << 5)
#define PAGE_DIRTY          (1ULL << 6)
#define PAGE_HUGE           (1ULL << 7)
#define PAGE_GLOBAL         (1ULL << 8)
#define PAGE_NX             (1ULL << 63)

/* Multiboot2 magic and alignment */
#define MULTIBOOT2_MAGIC    0xE85250D6
#define MULTIBOOT2_ARCH     0  /* i386 (but we switch to x86-64) */

/* Multiboot2 tag types */
#define MB2_TAG_END         0
#define MB2_TAG_CMDLINE     1
#define MB2_TAG_BOOTLOADER  2
#define MB2_TAG_MODULE      3
#define MB2_TAG_BASIC_MEM   4
#define MB2_TAG_BIOS_BOOT   5
#define MB2_TAG_MMAP        6
#define MB2_TAG_VBE         7
#define MB2_TAG_FRAMEBUFFER 8
#define MB2_TAG_ELF_SECTIONS 9
#define MB2_TAG_APM         10
#define MB2_TAG_EFI_32      11
#define MB2_TAG_EFI_64      12
#define MB2_TAG_SMBIOS      13
#define MB2_TAG_ACPI_OLD    14
#define MB2_TAG_ACPI_NEW    15
#define MB2_TAG_NETWORK     16
#define MB2_TAG_EFI_MMAP    17
#define MB2_TAG_EFI_BS      18
#define MB2_TAG_EFI_IMAGE   19
#define MB2_TAG_EFI_LOAD    20
#define MB2_TAG_LOAD_BASE   21

/* Multiboot2 memory map entry types */
#define MB2_MMAP_AVAILABLE  1
#define MB2_MMAP_RESERVED   2
#define MB2_MMAP_ACPI_RECLAIM 3
#define MB2_MMAP_ACPI_NVS   4
#define MB2_MMAP_BAD        5

/* Multiboot2 header structure */
typedef struct multiboot2_header {
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
} multiboot2_header_t;

/* Multiboot2 tag header */
typedef struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
} multiboot2_tag_t;

/* Multiboot2 memory map tag */
typedef struct multiboot2_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} multiboot2_tag_mmap_t;

/* Multiboot2 memory map entry */
typedef struct multiboot2_mmap_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} multiboot2_mmap_entry_t;

/* Multiboot2 framebuffer tag */
typedef struct multiboot2_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
} multiboot2_tag_framebuffer_t;

/* Multiboot2 module tag */
typedef struct multiboot2_tag_module {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char     cmdline[];
} multiboot2_tag_module_t;

/* Multiboot2 basic memory info */
typedef struct multiboot2_tag_basic_meminfo {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
} multiboot2_tag_basic_meminfo_t;

/* Kernel info structure populated from multiboot2 */
typedef struct kernel_info {
    /* Memory map */
    multiboot2_mmap_entry_t *mmap;
    uint32_t                 mmap_entries;
    uint32_t                 mmap_entry_size;

    /* Framebuffer */
    uintptr_t                framebuffer_addr;
    uint32_t                 framebuffer_pitch;
    uint32_t                 framebuffer_width;
    uint32_t                 framebuffer_height;
    uint8_t                  framebuffer_bpp;
    uint8_t                  framebuffer_type;

    /* Basic memory */
    uint32_t                 mem_lower_kb;
    uint32_t                 mem_upper_kb;

    /* Command line */
    const char              *cmdline;

    /* Bootloader name */
    const char              *bootloader_name;

    /* Modules */
    multiboot2_tag_module_t *modules;
    uint32_t                 module_count;

    /* ACPI tables */
    void                    *acpi_rsdp_old;
    void                    *acpi_rsdp_new;

    /* Total detected RAM in bytes */
    uint64_t                 total_ram;
} kernel_info_t;

/* Global kernel info (populated early in boot) */
extern kernel_info_t g_kernel_info;

/* Early boot parsing */
void parse_multiboot2_info(uintptr_t info_addr);

/* Kernel entry point declarations */
void kernel_early_init(void);
void kernel_main(void);

/* Architecture initialization */
void arch_init(void);

#endif /* GOS_KERNEL_H */
