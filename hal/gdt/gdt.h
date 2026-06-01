/* gOS Kernel — Global Descriptor Table (GDT)
 * Reference: https://wiki.osdev.org/GDT
 *            https://wiki.osdev.org/GDT_Tutorial
 *
 * The GDT in x86-64 long mode is simplified — the base and limit
 * fields are mostly ignored. Segments are flat (base=0, limit=max).
 * The GDT is still required for:
 *   - Code/data segment selectors (CPL, RPL)
 *   - TSS descriptor (required for Ring 0 -> Ring 3 transitions)
 *   - Task switching via hardware task switching (legacy, but TSS needed for IST)
 */

#ifndef GOS_GDT_H
#define GOS_GDT_H

#include "gOS/types.h"

/* GDT segment selector values (indices * 8) */
#define GDT_NULL         0x00
#define GDT_KERNEL_CODE  0x08
#define GDT_KERNEL_DATA  0x10
#define GDT_USER_CODE32  0x18
#define GDT_USER_DATA    0x20
#define GDT_USER_CODE64  0x28
#define GDT_TSS_LOW      0x30
#define GDT_TSS_HIGH     0x38

/* Maximum number of GDT entries */
#define GDT_MAX_ENTRIES  9

/* GDT entry (8 bytes) — packed to match hardware layout */
typedef struct gdt_entry {
    uint16_t limit_low;      /* Limit bits 0-15 */
    uint16_t base_low;       /* Base bits 0-15 */
    uint8_t  base_middle;    /* Base bits 16-23 */
    uint8_t  access;         /* Access byte */
    uint8_t  granularity;    /* Granularity byte + limit bits 16-19 */
    uint8_t  base_high;      /* Base bits 24-31 */
} PACKED gdt_entry_t;

/* GDT pointer structure for LGDT */
typedef struct gdt_ptr {
    uint16_t limit;          /* Size of GDT minus 1 */
    uint64_t base;           /* Linear address of GDT */
} PACKED gdt_ptr_t;

/* TSS descriptor in 64-bit mode is a 16-byte entry spanning two GDT slots.
 * The base is 64-bit, limit is 32-bit (but mostly ignored in 64-bit mode).
 */
typedef struct tss_descriptor {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid1;
    uint8_t  access;
    uint8_t  limit_high_flags;
    uint8_t  base_mid2;
    uint32_t base_high;
    uint32_t reserved;
} PACKED tss_descriptor_t;

/* System segment type flags for TSS (busy bit must be clear on load) */
#define GDT_ACCESS_PRESENT      0x80
#define GDT_ACCESS_RING0        0x00
#define GDT_ACCESS_RING3        0x60
#define GDT_ACCESS_TYPE_TSS_AVAIL 0x09  /* 64-bit available TSS */
#define GDT_ACCESS_TYPE_TSS_BUSY  0x0B  /* 64-bit busy TSS */

/* Granularity flags */
#define GDT_GRAN_4K             0x80
#define GDT_GRAN_32BIT          0x40
#define GDT_GRAN_64BIT          0x20

/* Initialize the GDT with a flat memory model */
void gdt_init(void);

/* Set a GDT entry at the given index */
void gdt_set_entry(int index, uint32_t base, uint32_t limit,
                   uint8_t access, uint8_t granularity);

/* Set the TSS descriptor in the GDT */
void gdt_set_tss(int index, uintptr_t tss_addr, uint32_t limit);

/* Reload segment registers after GDT changes */
void gdt_reload_segments(void);

/* Load the TSS into the Task Register (TR) */
void gdt_load_tss(uint16_t selector);

/* Get the currently loaded GDT pointer (for debugging) */
void gdt_get_current(gdt_ptr_t *out);

/* Dump GDT contents to serial log (for debugging) */
void gdt_dump(void);

/* The in-memory GDT array (extern for debugging) */
extern gdt_entry_t g_gdt[GDT_MAX_ENTRIES];
extern gdt_ptr_t   g_gdt_ptr;

#endif /* GOS_GDT_H */
