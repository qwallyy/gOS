/* gOS Kernel — Task State Segment (TSS)
 * Reference: https://wiki.osdev.org/TSS
 *            https://wiki.osdev.org/TSS_Tutorial
 *
 * In x86-64 long mode, the TSS is not used for hardware task switching
 * (that mechanism is deprecated). However, it is still required for:
 *   - Storing the Ring 0 stack pointer (RSP0) for interrupts from Ring 3
 *   - Interrupt Stack Table (IST) entries for NMI, double fault, etc.
 *   - I/O permission bitmap base address
 */

#ifndef GOS_TSS_H
#define GOS_TSS_H

#include "gOS/types.h"

/* 64-bit TSS structure (104 bytes minimum) */
/* Reference: Intel SDM Vol. 3A, Chapter 7 "Task Management" */
typedef struct tss64 {
    uint32_t reserved0;      /* 0x00 */
    uint64_t rsp0;           /* 0x04 — Stack pointer for Ring 0 */
    uint64_t rsp1;           /* 0x0C — Stack pointer for Ring 1 */
    uint64_t rsp2;           /* 0x14 — Stack pointer for Ring 2 */
    uint64_t reserved1;      /* 0x1C */
    uint64_t ist1;           /* 0x24 — IST entry 1 */
    uint64_t ist2;           /* 0x2C — IST entry 2 */
    uint64_t ist3;           /* 0x34 — IST entry 3 */
    uint64_t ist4;           /* 0x3C — IST entry 4 */
    uint64_t ist5;           /* 0x44 — IST entry 5 */
    uint64_t ist6;           /* 0x4C — IST entry 6 */
    uint64_t ist7;           /* 0x54 — IST entry 7 */
    uint64_t reserved2;      /* 0x5C */
    uint16_t reserved3;      /* 0x64 */
    uint16_t iomap_base;     /* 0x66 — I/O Map Base Address */
} PACKED tss64_t;

/* Size of the 64-bit TSS */
#define TSS64_SIZE  104

/* IST index assignments:
 * IST1 = NMI handler stack
 * IST2 = Double Fault (#DF) handler stack
 * IST3 = Machine Check (#MC) handler stack
 * IST4-7 = Reserved for future use
 */
#define IST_INDEX_NMI       1
#define IST_INDEX_DF      2
#define IST_INDEX_MC      3

/* Initialize the TSS and install it in the GDT */
void tss_init(void);

/* Set the Ring 0 stack pointer (used when entering from Ring 3) */
void tss_set_rsp0(uintptr_t rsp);

/* Set an IST entry by index (1-7) */
void tss_set_ist(int index, uintptr_t stack_top);

/* Get the physical address of the TSS structure (for GDT installation) */
uintptr_t tss_get_address(void);

/* Get the TSS size (for limit field in GDT) */
uint32_t tss_get_size(void);

/* Dump TSS contents for debugging */
void tss_dump(void);

#endif /* GOS_TSS_H */
