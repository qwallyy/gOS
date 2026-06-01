/* gOS Kernel — Interrupt Descriptor Table (IDT)
 * Reference: https://wiki.osdev.org/IDT
 *            https://wiki.osdev.org/ISR
 *            https://wiki.osdev.org/Exceptions
 *
 * The IDT contains 256 gate descriptors. Each descriptor points to an
 * ISR stub that saves the full CPU context and calls a C handler.
 * Gate types:
 *   - 0x8E = 64-bit Interrupt Gate (auto cli on entry, sti on iret)
 *   - 0x8F = 64-bit Trap Gate     (does NOT auto cli)
 */

#ifndef GOS_IDT_H
#define GOS_IDT_H

#include "gOS/types.h"

/* Number of IDT entries (256 vectors) */
#define IDT_ENTRIES  256

/* IDT gate type constants */
#define IDT_GATE_INTERRUPT  0x8E  /* 64-bit interrupt gate, DPL=0, present */
#define IDT_GATE_TRAP       0x8F  /* 64-bit trap gate, DPL=0, present */
#define IDT_GATE_INTERRUPT_USER 0xEE /* 64-bit interrupt gate, DPL=3, present */

/* IDT entry (16 bytes in 64-bit mode) */
/* Reference: https://wiki.osdev.org/IDT */
typedef struct idt_entry {
    uint16_t offset_low;     /* Bits 0-15 of handler address */
    uint16_t selector;       /* Code segment selector (GDT_KERNEL_CODE = 0x08) */
    uint8_t  ist;            /* IST index (0 = none, 1-7 = use IST) */
    uint8_t  type_attr;      /* Gate type and attributes */
    uint16_t offset_mid;     /* Bits 16-31 of handler address */
    uint32_t offset_high;    /* Bits 32-63 of handler address */
    uint32_t reserved;       /* Reserved, must be zero */
} PACKED idt_entry_t;

/* IDT pointer for LIDT */
typedef struct idt_ptr {
    uint16_t limit;          /* Size of IDT minus 1 */
    uint64_t base;           /* Linear address of IDT */
} PACKED idt_ptr_t;

/* CPU exception names (for diagnostic output) */
extern const char *g_exception_names[IDT_ENTRIES];

/* Initialize the IDT — sets all entries to a default handler, then
 * installs specific handlers for CPU exceptions and IRQs. */
void idt_init(void);

/* Set an IDT gate at the given vector */
void idt_set_gate(uint16_t vector, void *handler, uint16_t selector,
                  uint8_t ist, uint8_t type_attr);

/* Install the IDT pointer (calls LIDT) */
void idt_load(void);

/* Get current IDT pointer (for debugging) */
void idt_get_current(idt_ptr_t *out);

/* Dump IDT contents to serial log */
void idt_dump(void);

/* The ISR stubs declared in assembly */
extern void *isr_stub_table[IDT_ENTRIES];
extern void *irq_stub_table[16];

/* -------------------------------------------------------------------------
 * C ISR Handlers — declared here, defined in idt_isr.c
 * ------------------------------------------------------------------------- */

/* Common handler called by all assembly stubs */
void isr_common_handler(void *registers_ptr);

/* Specific exception handlers */
void isr_page_fault(void *registers_ptr, uint64_t error_code);
void isr_general_protection(void *registers_ptr, uint64_t error_code);
void isr_double_fault(void *registers_ptr, uint64_t error_code);
void isr_divide_error(void *registers_ptr);
void isr_debug(void *registers_ptr);
void isr_nmi(void *registers_ptr);
void isr_breakpoint(void *registers_ptr);
void isr_overflow(void *registers_ptr);
void isr_bound_range(void *registers_ptr);
void isr_invalid_opcode(void *registers_ptr);
void isr_device_not_available(void *registers_ptr);
void isr_invalid_tss(void *registers_ptr, uint64_t error_code);
void isr_segment_not_present(void *registers_ptr, uint64_t error_code);
void isr_stack_segment(void *registers_ptr, uint64_t error_code);
void isr_fpu_error(void *registers_ptr);
void isr_alignment_check(void *registers_ptr, uint64_t error_code);
void isr_machine_check(void *registers_ptr);
void isr_simd_exception(void *registers_ptr);
void isr_virtualization(void *registers_ptr);
void isr_control_protection(void *registers_ptr, uint64_t error_code);
void isr_hypervisor_injection(void *registers_ptr);
void isr_vmm_communication(void *registers_ptr, uint64_t error_code);
void isr_security(void *registers_ptr);

/* IRQ handlers (installed by driver code, referenced here) */
typedef void (*irq_handler_t)(void *registers_ptr);

/* Register an IRQ handler for the given IRQ number (0-15) */
void irq_register_handler(uint8_t irq, irq_handler_t handler);

/* Unregister an IRQ handler */
void irq_unregister_handler(uint8_t irq);

/* Send End-of-Interrupt to the PIC */
void irq_send_eoi(uint8_t irq);

#endif /* GOS_IDT_H */
