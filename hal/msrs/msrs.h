/* gOS Kernel — Model Specific Registers (MSRs)
 * Reference: https://wiki.osdev.org/Model_Specific_Registers
 */

#ifndef GOS_MSRS_H
#define GOS_MSRS_H

#include "gOS/types.h"

/* Common MSRs */
#define MSR_IA32_EFER           0xC0000080  /* Extended Feature Enable Register */
#define MSR_IA32_STAR           0xC0000081  /* SYSENTER/SYSCALL segments */
#define MSR_IA32_LSTAR          0xC0000082  /* Long mode SYSCALL target RIP */
#define MSR_IA32_CSTAR          0xC0000083  /* Compatibility mode SYSCALL target RIP */
#define MSR_IA32_FMASK          0xC0000084  /* SYSCALL RFLAGS mask */
#define MSR_IA32_FS_BASE        0xC0000100  /* FS segment base */
#define MSR_IA32_GS_BASE        0xC0000101  /* GS segment base */
#define MSR_IA32_KERNEL_GS_BASE 0xC0000102  /* SwapGS target */
#define MSR_IA32_TSC_AUX        0xC0000103  /* TSC auxiliary (per-CPU ID) */

#define MSR_IA32_APIC_BASE      0x1B         /* Local APIC base */
#define MSR_IA32_PAT            0x277        /* Page Attribute Table */
#define MSR_IA32_MISC_ENABLE    0x1A0        /* Misc enable */

/* EFER bits */
#define EFER_SCE    (1ULL << 0)   /* SYSCALL enable */
#define EFER_LME    (1ULL << 8)   /* Long mode enable */
#define EFER_LMA    (1ULL << 10)  /* Long mode active */
#define EFER_NXE    (1ULL << 11) /* NX (No-Execute) enable */

void msrs_init(void);
uint64_t msr_read(uint32_t msr);
void msr_write(uint32_t msr, uint64_t val);

#endif /* GOS_MSRS_H */
