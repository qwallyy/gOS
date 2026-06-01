/* gOS Kernel — Task State Segment (TSS) Implementation
 * Reference: https://wiki.osdev.org/TSS
 *            https://wiki.osdev.org/TSS_Tutorial
 */

#include "hal/tss/tss.h"
#include "hal/gdt/gdt.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

/* The TSS must be globally accessible for the GDT to point to it */
ALIGN(16) static tss64_t g_tss;

/* Dedicated stacks for IST entries */
#define IST_STACK_SIZE  8192

/* IST1 — NMI handler stack */
static uint8_t g_ist1_stack[IST_STACK_SIZE] ALIGN(16);

/* IST2 — Double Fault handler stack */
static uint8_t g_ist2_stack[IST_STACK_SIZE] ALIGN(16);

/* IST3 — Machine Check handler stack */
static uint8_t g_ist3_stack[IST_STACK_SIZE] ALIGN(16);

void tss_init(void) {
    klog_info("Initializing TSS");

    /* Clear the TSS structure */
    volatile uint8_t *p = (volatile uint8_t *)&g_tss;
    for (size_t i = 0; i < sizeof(g_tss); i++) {
        p[i] = 0;
    }

    /* Set the Ring 0 stack pointer to the top of the kernel stack.
     * This is used when an interrupt from Ring 3 occurs.
     * We use the current RSP as a reasonable value; scheduler will update per-task.
     */
    g_tss.rsp0 = read_rsp();

    /* Set up IST entries with dedicated exception handler stacks */
    tss_set_ist(IST_INDEX_NMI, (uintptr_t)&g_ist1_stack[IST_STACK_SIZE]);
    tss_set_ist(IST_INDEX_DF,  (uintptr_t)&g_ist2_stack[IST_STACK_SIZE]);
    tss_set_ist(IST_INDEX_MC,  (uintptr_t)&g_ist3_stack[IST_STACK_SIZE]);

    /* I/O permission bitmap offset — set to limit+1 to indicate no bitmap present */
    /* Reference: https://wiki.osdev.org/TSS */
    g_tss.iomap_base = sizeof(tss64_t);

    /* Install the TSS descriptor in the GDT at index 6 (selector 0x30) */
    gdt_set_tss(6, (uintptr_t)&g_tss, sizeof(g_tss) - 1);

    /* Reload the GDT so the TSS descriptor is visible */
    lgdt(&g_gdt_ptr);

    /* Load the Task Register with the TSS selector */
    gdt_load_tss(GDT_TSS_LOW);

    klog_info("TSS installed at selector 0x%04x, RSP0=0x%016llx",
              GDT_TSS_LOW, (unsigned long long)g_tss.rsp0);
}

void tss_set_rsp0(uintptr_t rsp) {
    g_tss.rsp0 = rsp;
}

void tss_set_ist(int index, uintptr_t stack_top) {
    if (index < 1 || index > 7) {
        klog_err("tss_set_ist: invalid IST index %d (must be 1-7)", index);
        return;
    }

    switch (index) {
        case 1: g_tss.ist1 = stack_top; break;
        case 2: g_tss.ist2 = stack_top; break;
        case 3: g_tss.ist3 = stack_top; break;
        case 4: g_tss.ist4 = stack_top; break;
        case 5: g_tss.ist5 = stack_top; break;
        case 6: g_tss.ist6 = stack_top; break;
        case 7: g_tss.ist7 = stack_top; break;
    }

    klog_debug("IST[%d] set to 0x%016llx", index, (unsigned long long)stack_top);
}

uintptr_t tss_get_address(void) {
    return (uintptr_t)&g_tss;
}

uint32_t tss_get_size(void) {
    return sizeof(g_tss);
}

void tss_dump(void) {
    klog_debug("TSS dump:");
    klog_debug("  RSP0 = 0x%016llx", (unsigned long long)g_tss.rsp0);
    klog_debug("  RSP1 = 0x%016llx", (unsigned long long)g_tss.rsp1);
    klog_debug("  RSP2 = 0x%016llx", (unsigned long long)g_tss.rsp2);
    klog_debug("  IST1 = 0x%016llx", (unsigned long long)g_tss.ist1);
    klog_debug("  IST2 = 0x%016llx", (unsigned long long)g_tss.ist2);
    klog_debug("  IST3 = 0x%016llx", (unsigned long long)g_tss.ist3);
    klog_debug("  IST4 = 0x%016llx", (unsigned long long)g_tss.ist4);
    klog_debug("  IST5 = 0x%016llx", (unsigned long long)g_tss.ist5);
    klog_debug("  IST6 = 0x%016llx", (unsigned long long)g_tss.ist6);
    klog_debug("  IST7 = 0x%016llx", (unsigned long long)g_tss.ist7);
    klog_debug("  IOMap base = 0x%04x", g_tss.iomap_base);
}
