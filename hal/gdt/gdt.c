/* gOS Kernel — Global Descriptor Table (GDT) Implementation
 * Reference: https://wiki.osdev.org/GDT
 *            https://wiki.osdev.org/GDT_Tutorial
 */

#include "hal/gdt/gdt.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

/* In-memory GDT — must be page-aligned for performance, but not strictly required */
ALIGN(16) gdt_entry_t g_gdt[GDT_MAX_ENTRIES];
gdt_ptr_t g_gdt_ptr;

/* Forward declaration for TSS init */
extern void tss_init(void);
extern void tss_set_rsp0(uintptr_t rsp);

void gdt_init(void) {
    klog_info("Initializing GDT (flat memory model)");

    /* Clear the GDT */
    for (int i = 0; i < GDT_MAX_ENTRIES; i++) {
        g_gdt[i].limit_low    = 0;
        g_gdt[i].base_low     = 0;
        g_gdt[i].base_middle  = 0;
        g_gdt[i].access       = 0;
        g_gdt[i].granularity  = 0;
        g_gdt[i].base_high    = 0;
    }

    /* 0x00: Null descriptor — required by Intel architecture */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* 0x08: 64-bit kernel code segment
     * Access: Present | Ring 0 | Code | Readable
     * Granularity: 64-bit long mode active
     */
    gdt_set_entry(1, 0, 0,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | 0x0A,
                  GDT_GRAN_64BIT);

    /* 0x10: 64-bit kernel data segment
     * Access: Present | Ring 0 | Data | Writable
     */
    gdt_set_entry(2, 0, 0,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | 0x02,
                  GDT_GRAN_64BIT);

    /* 0x18: 32-bit user code segment (compatibility mode for user tasks) */
    gdt_set_entry(3, 0, 0xFFFFFFFF,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | 0x0A,
                  GDT_GRAN_4K | GDT_GRAN_32BIT);

    /* 0x20: 64-bit user data segment */
    gdt_set_entry(4, 0, 0,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | 0x02,
                  GDT_GRAN_64BIT);

    /* 0x28: 64-bit user code segment */
    gdt_set_entry(5, 0, 0,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | 0x0A,
                  GDT_GRAN_64BIT);

    /* 0x30 + 0x38: TSS descriptor (16 bytes, two entries)
     * Initialized later by tss_init() after we have a stack.
     */
    gdt_set_entry(6, 0, 0, 0, 0);
    gdt_set_entry(7, 0, 0, 0, 0);

    /* Load the GDT pointer */
    g_gdt_ptr.limit = sizeof(g_gdt) - 1;
    g_gdt_ptr.base  = (uint64_t)&g_gdt[0];

    lgdt(&g_gdt_ptr);

    /* Reload segment registers to use the new GDT */
    gdt_reload_segments();

    /* Initialize TSS and load it into TR */
    tss_init();

    klog_info("GDT initialized successfully");
}

void gdt_set_entry(int index, uint32_t base, uint32_t limit,
                   uint8_t access, uint8_t granularity) {
    if (index < 0 || index >= GDT_MAX_ENTRIES) {
        klog_err("gdt_set_entry: index %d out of range", index);
        return;
    }

    g_gdt[index].limit_low    = (uint16_t)(limit & 0xFFFF);
    g_gdt[index].base_low     = (uint16_t)(base & 0xFFFF);
    g_gdt[index].base_middle  = (uint8_t)((base >> 16) & 0xFF);
    g_gdt[index].access       = access;
    g_gdt[index].granularity  = (uint8_t)(((limit >> 16) & 0x0F) | (granularity & 0xF0));
    g_gdt[index].base_high    = (uint8_t)((base >> 24) & 0xFF);
}

void gdt_set_tss(int index, uintptr_t tss_addr, uint32_t limit) {
    if (index < 0 || index >= GDT_MAX_ENTRIES - 1) {
        klog_err("gdt_set_tss: index %d out of range", index);
        return;
    }

    /* TSS descriptor is a 16-byte System Segment in long mode */
    tss_descriptor_t *desc = (tss_descriptor_t *)&g_gdt[index];

    desc->limit_low       = (uint16_t)(limit & 0xFFFF);
    desc->base_low        = (uint16_t)(tss_addr & 0xFFFF);
    desc->base_mid1       = (uint8_t)((tss_addr >> 16) & 0xFF);
    desc->access          = GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_TYPE_TSS_AVAIL;
    desc->limit_high_flags = (uint8_t)(((limit >> 16) & 0x0F));
    desc->base_mid2       = (uint8_t)((tss_addr >> 24) & 0xFF);
    desc->base_high       = (uint32_t)((tss_addr >> 32) & 0xFFFFFFFF);
    desc->reserved        = 0;

    /* Clear the next entry (it is consumed by the 16-byte TSS descriptor) */
    gdt_set_entry(index + 1, 0, 0, 0, 0);
}


void gdt_load_tss(uint16_t selector) {
    ltr(selector);
}

void gdt_get_current(gdt_ptr_t *out) {
    if (!out) return;
    __asm__ volatile ("sgdt %0" : "=m"(*out));
}

void gdt_dump(void) {
    gdt_ptr_t current;
    gdt_get_current(&current);

    klog_debug("GDT dump: base=0x%016llx, limit=%u",
               (unsigned long long)current.base, current.limit);

    for (int i = 0; i < GDT_MAX_ENTRIES; i++) {
        uint32_t base = g_gdt[i].base_low |
                        ((uint32_t)g_gdt[i].base_middle << 16) |
                        ((uint32_t)g_gdt[i].base_high << 24);
        uint32_t limit = g_gdt[i].limit_low |
                         ((uint32_t)(g_gdt[i].granularity & 0x0F) << 16);
        uint8_t access = g_gdt[i].access;

        if (access == 0 && i > 0) continue; /* Skip unused entries after first */

        klog_debug("  [%d] base=0x%08x limit=0x%05x access=0x%02x gran=0x%02x",
                   i, base, limit, access, g_gdt[i].granularity);
    }
}
