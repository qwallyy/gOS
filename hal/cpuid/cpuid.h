/* gOS Kernel — CPUID Feature Detection
 * Reference: https://wiki.osdev.org/CPUID
 *
 * Provides wrappers around the CPUID instruction and structures
 * to cache and query CPU capabilities (APIC, MSR, PAE, NX, SSE, etc.).
 */

#ifndef GOS_CPUID_H
#define GOS_CPUID_H

#include "gOS/types.h"

/* CPU feature flags from CPUID.1.EDX */
#define CPUID_FEATURE_FPU       (1U << 0)
#define CPUID_FEATURE_VME       (1U << 1)
#define CPUID_FEATURE_DE        (1U << 2)
#define CPUID_FEATURE_PSE       (1U << 3)
#define CPUID_FEATURE_TSC       (1U << 4)
#define CPUID_FEATURE_MSR       (1U << 5)
#define CPUID_FEATURE_PAE       (1U << 6)
#define CPUID_FEATURE_MCE       (1U << 7)
#define CPUID_FEATURE_CX8       (1U << 8)
#define CPUID_FEATURE_APIC      (1U << 9)
#define CPUID_FEATURE_SEP       (1U << 11)
#define CPUID_FEATURE_MTRR      (1U << 12)
#define CPUID_FEATURE_PGE       (1U << 13)
#define CPUID_FEATURE_MCA       (1U << 14)
#define CPUID_FEATURE_CMOV      (1U << 15)
#define CPUID_FEATURE_PAT       (1U << 16)
#define CPUID_FEATURE_PSE36     (1U << 17)
#define CPUID_FEATURE_PSN       (1U << 18)
#define CPUID_FEATURE_CLFLUSH   (1U << 19)
#define CPUID_FEATURE_DS        (1U << 21)
#define CPUID_FEATURE_ACPI      (1U << 22)
#define CPUID_FEATURE_MMX       (1U << 23)
#define CPUID_FEATURE_FXSR      (1U << 24)
#define CPUID_FEATURE_SSE       (1U << 25)
#define CPUID_FEATURE_SSE2      (1U << 26)
#define CPUID_FEATURE_SS        (1U << 27)
#define CPUID_FEATURE_HT        (1U << 28)
#define CPUID_FEATURE_TM        (1U << 29)
#define CPUID_FEATURE_IA64      (1U << 30)
#define CPUID_FEATURE_PBE       (1U << 31)

/* CPU feature flags from CPUID.1.ECX */
#define CPUID_FEATURE_SSE3      (1U << 0)
#define CPUID_FEATURE_PCLMULQDQ (1U << 1)
#define CPUID_FEATURE_DTES64    (1U << 2)
#define CPUID_FEATURE_MONITOR   (1U << 3)
#define CPUID_FEATURE_DS_CPL    (1U << 4)
#define CPUID_FEATURE_VMX       (1U << 5)
#define CPUID_FEATURE_SMX       (1U << 6)
#define CPUID_FEATURE_EST       (1U << 7)
#define CPUID_FEATURE_TM2       (1U << 8)
#define CPUID_FEATURE_SSSE3     (1U << 9)
#define CPUID_FEATURE_CID       (1U << 10)
#define CPUID_FEATURE_FMA       (1U << 12)
#define CPUID_FEATURE_CX16      (1U << 13)
#define CPUID_FEATURE_XTPR      (1U << 14)
#define CPUID_FEATURE_PDCM      (1U << 15)
#define CPUID_FEATURE_PCID      (1U << 17)
#define CPUID_FEATURE_DCA       (1U << 18)
#define CPUID_FEATURE_SSE41     (1U << 19)
#define CPUID_FEATURE_SSE42     (1U << 20)
#define CPUID_FEATURE_X2APIC    (1U << 21)
#define CPUID_FEATURE_MOVBE     (1U << 22)
#define CPUID_FEATURE_POPCNT    (1U << 23)
#define CPUID_FEATURE_AES       (1U << 25)
#define CPUID_FEATURE_XSAVE     (1U << 26)
#define CPUID_FEATURE_OSXSAVE   (1U << 27)
#define CPUID_FEATURE_AVX       (1U << 28)
#define CPUID_FEATURE_F16C      (1U << 29)
#define CPUID_FEATURE_RDRAND    (1U << 30)
#define CPUID_FEATURE_HYPERVISOR (1U << 31)

/* Extended feature flags from CPUID.80000001.EDX */
#define CPUID_EXT_FEATURE_NX      (1U << 20)
#define CPUID_EXT_FEATURE_MMXEXT  (1U << 22)
#define CPUID_EXT_FEATURE_FFXSR   (1U << 25)
#define CPUID_EXT_FEATURE_RDTSCP  (1U << 27)
#define CPUID_EXT_FEATURE_LM      (1U << 29)  /* Long Mode (AMD64) */
#define CPUID_EXT_FEATURE_3DNOWEXT (1U << 30)
#define CPUID_EXT_FEATURE_3DNOW   (1U << 31)

/* CPU information cache */
typedef struct cpu_info {
    char     vendor[13];          /* CPU vendor string + null terminator */
    char     brand[49];           /* CPU brand string + null terminator */
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    uint32_t features_edx;
    uint32_t features_ecx;
    uint32_t ext_features_edx;
    uint32_t ext_features_ecx;
    uint32_t max_standard_leaf;
    uint32_t max_extended_leaf;
    uint8_t  apic_id;
    uint8_t  logical_cpus;
    uint8_t  physical_cores;
    uint32_t cache_line_size;
    uint32_t maxphyaddr;          /* Maximum physical address bits */
    uint32_t maxlinaddr;          /* Maximum linear address bits */
    bool     has_apic;
    bool     has_x2apic;
    bool     has_msr;
    bool     has_pae;
    bool     has_nx;
    bool     has_sse;
    bool     has_sse2;
    bool     has_long_mode;
} cpu_info_t;

/* Global CPU info (populated during boot) */
extern cpu_info_t g_cpu_info;

/* Detect and cache CPU capabilities */
void cpuid_init(void);

/* Query if a specific feature is present */
bool cpu_has_feature(uint32_t feature);
bool cpu_has_ext_feature(uint32_t feature);

/* Get the Local APIC base physical address from MSR */
uintptr_t cpu_get_lapic_base(void);

/* Get the number of local APIC IDs (for SMP detection) */
uint32_t cpu_get_apic_count(void);

/* Print CPU info to kernel log */
void cpuid_dump(void);

#endif /* GOS_CPUID_H */
