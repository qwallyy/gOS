/* gOS Kernel — ISR Exception Handlers
 * Reference: https://wiki.osdev.org/Exceptions
 *            https://wiki.osdev.org/ISR
 *
 * These functions are called from the common assembly ISR entry point
 * after the full CPU context has been saved on the stack.
 */

#include "hal/idt/idt.h"
#include "mm/vmm/vmm.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"
#include "proc/proc.h"

/* Page Fault error code bits */
#define PF_ERR_PRESENT      (1ULL << 0)  /* Page was present */
#define PF_ERR_WRITE        (1ULL << 1)  /* Access was a write */
#define PF_ERR_USER         (1ULL << 2)  /* Access from user mode */
#define PF_ERR_RESERVED     (1ULL << 3)  /* Reserved bit violation */
#define PF_ERR_INST_FETCH   (1ULL << 4)  /* Instruction fetch */
#define PF_ERR_PK           (1ULL << 5)  /* Protection key violation */

/* NMI lock — prevent recursive NMI handling */
static volatile uint64_t g_nmi_lock = 0;

void isr_divide_error(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_emerg("#DE: Divide Error at RIP");
    cli(); hlt();
    for (;;) { }
}

void isr_debug(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_debug("#DB: Debug exception");
    /* Debug exceptions are often benign (single-step) */
}

void isr_nmi(void *registers_ptr) {
    UNUSED(registers_ptr);

    /* NMI is non-maskable. Use a lock to prevent recursion. */
    if (atomic_fetch_add64((volatile uint64_t *)&g_nmi_lock, 1) != 0) {
        /* Another NMI is already being handled — wait */
        while (g_nmi_lock > 1) { pause(); }
    }

    klog_emerg("#NMI: Non-Maskable Interrupt!");
    klog_emerg("  This typically indicates hardware failure, parity error, or watchdog timeout.");

    /* Try to identify the cause by reading NMI status ports */
    uint8_t nmi_status = inb(0x70); /* CMOS/RTC port also used for NMI status */
    klog_emerg("  NMI status port 0x70 = 0x%02x", nmi_status);

    g_nmi_lock = 0;

    /* NMI is critical — halt to prevent further damage */
    cli(); hlt();
    for (;;) { }
}

void isr_breakpoint(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_info("#BP: Breakpoint hit (INT 3)");
    /* Breakpoints are typically benign in a debugger context */
}

void isr_overflow(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("#OF: Overflow exception");
}

void isr_bound_range(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("#BR: Bound Range Exceeded");
}

void isr_invalid_opcode(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_emerg("#UD: Invalid Opcode");
    cli(); hlt();
    for (;;) { }
}

void isr_device_not_available(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("#NM: Device Not Available (FPU not initialized?)");
}

void isr_double_fault(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_emerg("#DF: DOUBLE FAULT — error code = 0x%llx", (unsigned long long)error_code);
    klog_emerg("  This means a fault occurred while handling another fault.");
    klog_emerg("  Common causes: stack overflow, corrupt IDT/GDT, recursive page fault.");

    /* Double fault with IST2 gives us a fresh stack, but we still halt */
    cli(); hlt();
    for (;;) { }
}

void isr_invalid_tss(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_emerg("#TS: Invalid TSS — error code = 0x%llx (selector index=%llu)",
               (unsigned long long)error_code, (unsigned long long)(error_code >> 3));
    cli(); hlt();
    for (;;) { }
}

void isr_segment_not_present(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_emerg("#NP: Segment Not Present — error code = 0x%llx (selector index=%llu)",
               (unsigned long long)error_code, (unsigned long long)(error_code >> 3));
    cli(); hlt();
    for (;;) { }
}

void isr_stack_segment(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_emerg("#SS: Stack-Segment Fault — error code = 0x%llx", (unsigned long long)error_code);
    cli(); hlt();
    for (;;) { }
}

void isr_general_protection(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_emerg("#GP: General Protection Fault — error code = 0x%llx", (unsigned long long)error_code);

    /* Decode the error code */
    if (error_code & 1) {
        klog_emerg("  External event (NMI/INTR during task switch)");
    }
    if (error_code & 2) {
        klog_emerg("  IDT descriptor (vector %llu)", (unsigned long long)((error_code >> 3) & 0x1FF));
    } else if (error_code & 4) {
        klog_emerg("  LDT selector");
    } else {
        klog_emerg("  GDT selector index %llu", (unsigned long long)(error_code >> 3));
    }

    cli(); hlt();
    for (;;) { }
}

void isr_page_fault(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);

    /* CR2 holds the linear address that caused the page fault */
    uint64_t fault_addr = read_cr2();

    klog_emerg("#PF: Page Fault at address 0x%016llx", (unsigned long long)fault_addr);
    klog_emerg("  Error code: 0x%llx", (unsigned long long)error_code);

    if (error_code & PF_ERR_PRESENT) {
        klog_emerg("  - Page was present (protection violation)");
    } else {
        klog_emerg("  - Page was NOT present");
    }
    if (error_code & PF_ERR_WRITE) {
        klog_emerg("  - Caused by a WRITE");
    } else {
        klog_emerg("  - Caused by a READ");
    }
    if (error_code & PF_ERR_USER) {
        klog_emerg("  - In USER mode (CPL=3)");
    } else {
        klog_emerg("  - In KERNEL mode (CPL=0)");
    }
    if (error_code & PF_ERR_RESERVED) {
        klog_emerg("  - Reserved bit violation in page table entry!");
    }
    if (error_code & PF_ERR_INST_FETCH) {
        klog_emerg("  - Instruction fetch fault (NX bit violation)");
    }

    /* Attempt to handle the fault if it's in a mapped region */
    if (!(error_code & PF_ERR_PRESENT)) {
        /* Page not present — could be demand paging or stack growth */
        /* In a full implementation, we would try to allocate a page here */
        klog_emerg("  Page fault handler: no automatic page allocation in current build.");
    }

    cli(); hlt();
    for (;;) { }
}

void isr_fpu_error(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("#MF: x87 FPU Floating-Point Error");
}

void isr_alignment_check(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_err("#AC: Alignment Check — error code = 0x%llx", (unsigned long long)error_code);
}

void isr_machine_check(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_emerg("#MC: MACHINE CHECK! Hardware failure detected.");

    /* Read Machine Check MSRs if available */
    uint64_t mcg_status = rdmsr(0x17A); /* MCG_STATUS */
    klog_emerg("  MCG_STATUS = 0x%016llx", (unsigned long long)mcg_status);

    cli(); hlt();
    for (;;) { }
}

void isr_simd_exception(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("#XF: SIMD Floating-Point Exception");
}

void isr_virtualization(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("#VE: Virtualization Exception");
}

void isr_control_protection(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_err("#CP: Control Protection Exception — error code = 0x%llx", (unsigned long long)error_code);
}

void isr_hypervisor_injection(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("#HV: Hypervisor Injection Exception");
}

void isr_vmm_communication(void *registers_ptr, uint64_t error_code) {
    UNUSED(registers_ptr);
    klog_err("#VC: VMM Communication Exception — error code = 0x%llx", (unsigned long long)error_code);
}

void isr_security(void *registers_ptr) {
    UNUSED(registers_ptr);
    klog_err("Security exception");
}
