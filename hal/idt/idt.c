/* gOS Kernel — Interrupt Descriptor Table (IDT) Implementation
 * Reference: https://wiki.osdev.org/IDT
 *            https://wiki.osdev.org/ISR
 */

#include "hal/idt/idt.h"
#include "hal/gdt/gdt.h"
#include "drivers/pic/pic.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

/* In-memory IDT array */
ALIGN(16) static idt_entry_t g_idt[IDT_ENTRIES];
static idt_ptr_t g_idt_ptr;

/* Registered IRQ handlers */
static irq_handler_t g_irq_handlers[16] = { NULL };

/* Exception names for diagnostics */
const char *g_exception_names[IDT_ENTRIES] = {
    [0]  = "Divide Error (#DE)",
    [1]  = "Debug (#DB)",
    [2]  = "Non-Maskable Interrupt (#NMI)",
    [3]  = "Breakpoint (#BP)",
    [4]  = "Overflow (#OF)",
    [5]  = "Bound Range Exceeded (#BR)",
    [6]  = "Invalid Opcode (#UD)",
    [7]  = "Device Not Available (#NM)",
    [8]  = "Double Fault (#DF)",
    [9]  = "Coprocessor Segment Overrun",
    [10] = "Invalid TSS (#TS)",
    [11] = "Segment Not Present (#NP)",
    [12] = "Stack-Segment Fault (#SS)",
    [13] = "General Protection Fault (#GP)",
    [14] = "Page Fault (#PF)",
    [15] = "Reserved",
    [16] = "x87 FPU Floating-Point Error (#MF)",
    [17] = "Alignment Check (#AC)",
    [18] = "Machine Check (#MC)",
    [19] = "SIMD Floating-Point Exception (#XF)",
    [20] = "Virtualization Exception (#VE)",
    [21] = "Control Protection Exception (#CP)",
    [22] = "Reserved",
    [23] = "Reserved",
    [24] = "Reserved",
    [25] = "Reserved",
    [26] = "Reserved",
    [27] = "Reserved",
    [28] = "Reserved",
    [29] = "Hypervisor Injection Exception (#HV)",
    [30] = "VMM Communication Exception (#VC)",
    [31] = "Reserved",
};

/* Default exception handler for unhandled vectors */
static void isr_default(void *regs, uint64_t vector) {
    klog_emerg("Unhandled exception #%llu", (unsigned long long)vector);
    klog_emerg("  Name: %s", g_exception_names[vector] ? g_exception_names[vector] : "Unknown");

    /* On unhandled exception, dump registers and halt */
    if (regs) {
        /* registers_t is built on stack by isr_common_entry */
        uint64_t *r = (uint64_t *)regs;
        klog_emerg("  RAX=0x%016llx RBX=0x%016llx RCX=0x%016llx RDX=0x%016llx",
                   (unsigned long long)r[0], (unsigned long long)r[1],
                   (unsigned long long)r[2], (unsigned long long)r[3]);
    }

    /* Triple fault if we return — better to halt here */
    cli();
    hlt();
    for (;;) { }
}

void idt_init(void) {
    klog_info("Initializing IDT (%d entries)", IDT_ENTRIES);

    /* Clear all IDT entries */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        g_idt[i].offset_low    = 0;
        g_idt[i].selector      = GDT_KERNEL_CODE;
        g_idt[i].ist           = 0;
        g_idt[i].type_attr     = 0;
        g_idt[i].offset_mid    = 0;
        g_idt[i].offset_high   = 0;
        g_idt[i].reserved      = 0;
    }

    /* Install CPU exception handlers (vectors 0-31) */
    for (int i = 0; i < 32; i++) {
        if (isr_stub_table[i]) {
            uint8_t ist = 0;

            /* Use IST for critical exceptions that may happen on a corrupt stack:
             *   IST1 = NMI (#2)
             *   IST2 = Double Fault (#8)
             *   IST3 = Machine Check (#18)
             */
            if (i == 2)  ist = 1;  /* NMI */
            if (i == 8)  ist = 2;  /* Double Fault */
            if (i == 18) ist = 3;  /* Machine Check */

            idt_set_gate((uint8_t)i, isr_stub_table[i], GDT_KERNEL_CODE, ist, IDT_GATE_INTERRUPT);
        }
    }

    /* Install IRQ stubs (vectors 32-47) */
    for (int i = 0; i < 16; i++) {
        if (irq_stub_table[i]) {
            idt_set_gate((uint8_t)(i + 32), irq_stub_table[i], GDT_KERNEL_CODE, 0, IDT_GATE_INTERRUPT);
        }
    }

    /* Load the IDT pointer */
    g_idt_ptr.limit = sizeof(g_idt) - 1;
    g_idt_ptr.base  = (uint64_t)&g_idt[0];

    lidt(&g_idt_ptr);

    klog_info("IDT loaded at 0x%016llx", (unsigned long long)g_idt_ptr.base);
}

void idt_set_gate(uint16_t vector, void *handler, uint16_t selector,
                  uint8_t ist, uint8_t type_attr) {
    if (vector >= IDT_ENTRIES || handler == NULL) {
        klog_err("idt_set_gate: invalid vector %u or NULL handler", vector);
        return;
    }

    uint64_t handler_addr = (uint64_t)handler;

    g_idt[vector].offset_low    = (uint16_t)(handler_addr & 0xFFFF);
    g_idt[vector].selector      = selector;
    g_idt[vector].ist           = (ist & 0x07); /* IST is bits 0-2 */
    g_idt[vector].type_attr     = type_attr;
    g_idt[vector].offset_mid    = (uint16_t)((handler_addr >> 16) & 0xFFFF);
    g_idt[vector].offset_high   = (uint32_t)((handler_addr >> 32) & 0xFFFFFFFF);
    g_idt[vector].reserved      = 0;
}

void idt_get_current(idt_ptr_t *out) {
    if (!out) return;
    __asm__ volatile ("sidt %0" : "=m"(*out));
}

void idt_dump(void) {
    idt_ptr_t current;
    idt_get_current(&current);

    klog_debug("IDT dump: base=0x%016llx, limit=%u",
               (unsigned long long)current.base, current.limit);

    for (int i = 0; i < 48; i++) {
        uint64_t offset = ((uint64_t)g_idt[i].offset_high << 32) |
                          ((uint64_t)g_idt[i].offset_mid << 16) |
                          (uint64_t)g_idt[i].offset_low;
        if (offset == 0) continue;

        klog_debug("  [%3d] handler=0x%016llx sel=0x%04x ist=%u type=0x%02x",
                   i, (unsigned long long)offset,
                   g_idt[i].selector, g_idt[i].ist, g_idt[i].type_attr);
    }
}

/* -------------------------------------------------------------------------
 * IRQ Handler Registration
 * ------------------------------------------------------------------------- */

void irq_register_handler(uint8_t irq, irq_handler_t handler) {
    if (irq > 15) {
        klog_err("irq_register_handler: invalid IRQ %u", irq);
        return;
    }
    g_irq_handlers[irq] = handler;
    klog_debug("IRQ handler registered for IRQ%u", irq);
}

void irq_unregister_handler(uint8_t irq) {
    if (irq > 15) return;
    g_irq_handlers[irq] = NULL;
}

void irq_send_eoi(uint8_t irq) {
    pic_send_eoi(irq);
}

/* -------------------------------------------------------------------------
 * Common ISR Handler (called from assembly)
 * ------------------------------------------------------------------------- */

typedef struct registers {
    uint64_t gs, fs, es, ds;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip, cs, rflags, rsp, ss;
} registers_t;

void isr_common_handler(void *registers_ptr) {
    registers_t *regs = (registers_t *)registers_ptr;
    uint64_t vector = regs->vector;

    if (vector < 32) {
        /* CPU Exception */
        switch (vector) {
            case 0:
                isr_divide_error(registers_ptr);
                break;
            case 1:
                isr_debug(registers_ptr);
                break;
            case 2:
                isr_nmi(registers_ptr);
                break;
            case 3:
                isr_breakpoint(registers_ptr);
                break;
            case 4:
                isr_overflow(registers_ptr);
                break;
            case 5:
                isr_bound_range(registers_ptr);
                break;
            case 6:
                isr_invalid_opcode(registers_ptr);
                break;
            case 7:
                isr_device_not_available(registers_ptr);
                break;
            case 8:
                isr_double_fault(registers_ptr, regs->error_code);
                break;
            case 10:
                isr_invalid_tss(registers_ptr, regs->error_code);
                break;
            case 11:
                isr_segment_not_present(registers_ptr, regs->error_code);
                break;
            case 12:
                isr_stack_segment(registers_ptr, regs->error_code);
                break;
            case 13:
                isr_general_protection(registers_ptr, regs->error_code);
                break;
            case 14:
                isr_page_fault(registers_ptr, regs->error_code);
                break;
            case 16:
                isr_fpu_error(registers_ptr);
                break;
            case 17:
                isr_alignment_check(registers_ptr, regs->error_code);
                break;
            case 18:
                isr_machine_check(registers_ptr);
                break;
            case 19:
                isr_simd_exception(registers_ptr);
                break;
            case 20:
                isr_virtualization(registers_ptr);
                break;
            case 21:
                isr_control_protection(registers_ptr, regs->error_code);
                break;
            case 29:
                isr_hypervisor_injection(registers_ptr);
                break;
            case 30:
                isr_vmm_communication(registers_ptr, regs->error_code);
                break;
            default:
                isr_default(registers_ptr, vector);
                break;
        }
    } else if (vector >= 32 && vector < 48) {
        /* IRQ (remapped PIC) */
        uint8_t irq = (uint8_t)(vector - 32);
        if (g_irq_handlers[irq]) {
            g_irq_handlers[irq](registers_ptr);
        } else {
            klog_warn("Unhandled IRQ%u (vector %llu)", irq, (unsigned long long)vector);
        }
        pic_send_eoi(irq);
    } else if (vector == 80 || vector == 128) {
        /* System call entry points (legacy INT-based) */
        /* Handled by syscall_dispatch() in sys/syscall.c */
        extern void syscall_dispatch_legacy(void *registers_ptr);
        syscall_dispatch_legacy(registers_ptr);
    } else {
        klog_warn("Unhandled interrupt vector %llu", (unsigned long long)vector);
    }
}
