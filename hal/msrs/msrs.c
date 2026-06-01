/* gOS Kernel — Model Specific Registers (MSRs)
 * Reference: https://wiki.osdev.org/Model_Specific_Registers
 */

#include "hal/msrs/msrs.h"
#include "gOS/klog.h"

void msrs_init(void) {
    klog_info("Initializing MSRs");

    /* Enable SYSCALL/SYSRET via EFER.SCE */
    uint64_t efer = rdmsr(MSR_IA32_EFER);
    if (!(efer & EFER_SCE)) {
        efer |= EFER_SCE;
        wrmsr(MSR_IA32_EFER, efer);
        klog_info("EFER.SCE enabled");
    }

    /* Verify long mode is active */
    if (efer & EFER_LMA) {
        klog_info("Long Mode Active (LMA=1)");
    }

    /* Enable NX bit if available */
    uint32_t eax, ebx, ecx, edx;
    cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx);
    if (edx & (1U << 20)) {
        efer |= EFER_NXE;
        wrmsr(MSR_IA32_EFER, efer);
        klog_info("NX (No-Execute) bit enabled");
    }
}

uint64_t msr_read(uint32_t msr) {
    return rdmsr(msr);
}

void msr_write(uint32_t msr, uint64_t val) {
    wrmsr(msr, val);
}
