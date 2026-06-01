/* gOS Kernel — CPUID Feature Detection
 * Reference: https://wiki.osdev.org/CPUID
 */

#include "hal/cpuid/cpuid.h"
#include "gOS/klog.h"

cpu_info_t g_cpu_info;

void cpuid_init(void) {
    uint32_t eax, ebx, ecx, edx;

    klog_info("Detecting CPU capabilities via CPUID");
    for (size_t i = 0; i < sizeof(g_cpu_info); i++)
        ((uint8_t *)&g_cpu_info)[i] = 0;

    cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    g_cpu_info.max_standard_leaf = eax;
    *(uint32_t *)&g_cpu_info.vendor[0] = ebx;
    *(uint32_t *)&g_cpu_info.vendor[4] = edx;
    *(uint32_t *)&g_cpu_info.vendor[8] = ecx;
    g_cpu_info.vendor[12] = '\0';

    if (g_cpu_info.max_standard_leaf >= 1) {
        cpuid(1, 0, &eax, &ebx, &ecx, &edx);
        g_cpu_info.stepping = eax & 0xF;
        g_cpu_info.model    = (eax >> 4) & 0xF;
        g_cpu_info.family   = (eax >> 8) & 0xF;
        if (g_cpu_info.family == 0x0F)
            g_cpu_info.family += (eax >> 20) & 0xFF;
        if (g_cpu_info.family == 0x06 || g_cpu_info.family == 0x0F)
            g_cpu_info.model |= ((eax >> 16) & 0x0F) << 4;
        g_cpu_info.features_edx = edx;
        g_cpu_info.features_ecx = ecx;
        g_cpu_info.apic_id = (ebx >> 24) & 0xFF;
        g_cpu_info.logical_cpus = (ebx >> 16) & 0xFF;
        g_cpu_info.cache_line_size = ((ebx >> 8) & 0xFF) * 8;
        g_cpu_info.has_apic = (edx & CPUID_FEATURE_APIC) != 0;
        g_cpu_info.has_msr  = (edx & CPUID_FEATURE_MSR) != 0;
        g_cpu_info.has_pae  = (edx & CPUID_FEATURE_PAE) != 0;
        g_cpu_info.has_sse  = (edx & CPUID_FEATURE_SSE) != 0;
        g_cpu_info.has_sse2 = (edx & CPUID_FEATURE_SSE2) != 0;
        g_cpu_info.has_x2apic = (ecx & CPUID_FEATURE_X2APIC) != 0;
    }

    cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx);
    g_cpu_info.max_extended_leaf = eax;

    if (g_cpu_info.max_extended_leaf >= 0x80000001) {
        cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx);
        g_cpu_info.ext_features_edx = edx;
        g_cpu_info.ext_features_ecx = ecx;
        g_cpu_info.has_long_mode = (edx & CPUID_EXT_FEATURE_LM) != 0;
        g_cpu_info.has_nx = (edx & CPUID_EXT_FEATURE_NX) != 0;
    }

    if (g_cpu_info.max_extended_leaf >= 0x80000004) {
        for (int i = 0; i < 3; i++) {
            cpuid(0x80000002 + i, 0, &eax, &ebx, &ecx, &edx);
            *(uint32_t *)&g_cpu_info.brand[i * 16 + 0]  = eax;
            *(uint32_t *)&g_cpu_info.brand[i * 16 + 4]  = ebx;
            *(uint32_t *)&g_cpu_info.brand[i * 16 + 8]  = ecx;
            *(uint32_t *)&g_cpu_info.brand[i * 16 + 12] = edx;
        }
        g_cpu_info.brand[48] = '\0';
    }

    if (g_cpu_info.max_standard_leaf >= 0x0D) {
        cpuid(0x0D, 0, &eax, &ebx, &ecx, &edx);
        g_cpu_info.maxphyaddr = (eax >> 0) & 0xFF;
        g_cpu_info.maxlinaddr = (eax >> 8) & 0xFF;
    } else {
        g_cpu_info.maxphyaddr = 36;
        g_cpu_info.maxlinaddr = 48;
    }

    klog_info("CPU: %s", g_cpu_info.vendor);
    klog_info("Brand: %s", g_cpu_info.brand[0] ? g_cpu_info.brand : "N/A");
    klog_info("Family: %u, Model: %u, Stepping: %u",
              g_cpu_info.family, g_cpu_info.model, g_cpu_info.stepping);
    klog_info("Features: APIC=%d MSR=%d PAE=%d SSE=%d SSE2=%d LM=%d NX=%d",
              g_cpu_info.has_apic, g_cpu_info.has_msr, g_cpu_info.has_pae,
              g_cpu_info.has_sse, g_cpu_info.has_sse2,
              g_cpu_info.has_long_mode, g_cpu_info.has_nx);
}

bool cpu_has_feature(uint32_t feature) {
    return (g_cpu_info.features_edx & feature) != 0;
}

bool cpu_has_ext_feature(uint32_t feature) {
    return (g_cpu_info.ext_features_edx & feature) != 0;
}

uintptr_t cpu_get_lapic_base(void) {
    if (!g_cpu_info.has_msr) return 0;
    /* IA32_APIC_BASE MSR = 0x1B */
    return (rdmsr(0x1B) & 0xFFFFFFFFFFFFF000ULL);
}

uint32_t cpu_get_apic_count(void) {
    return g_cpu_info.logical_cpus;
}

void cpuid_dump(void) {
    klog_debug("CPUID dump:");
    klog_debug("  Vendor: %s", g_cpu_info.vendor);
    klog_debug("  Brand:  %s", g_cpu_info.brand);
    klog_debug("  Max standard leaf: 0x%x", g_cpu_info.max_standard_leaf);
    klog_debug("  Max extended leaf: 0x%x", g_cpu_info.max_extended_leaf);
    klog_debug("  Features EDX: 0x%08x", g_cpu_info.features_edx);
    klog_debug("  Features ECX: 0x%08x", g_cpu_info.features_ecx);
    klog_debug("  Ext Features EDX: 0x%08x", g_cpu_info.ext_features_edx);
    klog_debug("  Ext Features ECX: 0x%08x", g_cpu_info.ext_features_ecx);
    klog_debug("  APIC ID: %u, Logical CPUs: %u",
               g_cpu_info.apic_id, g_cpu_info.logical_cpus);
}
