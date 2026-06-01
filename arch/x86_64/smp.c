/* gOS Kernel — SMP Implementation (stub)
 * Reference: https://wiki.osdev.org/SMP
 */

#include "arch/x86_64/smp.h"
#include "apic/apic.h"
#include "hal/cpuid/cpuid.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "gOS/klog.h"

uint32_t g_smp_cpu_count = 1;
uint32_t g_smp_bsp_apic_id = 0;

void smp_init(void) {
    klog_info("SMP initializing");

    if (!g_cpu_info.has_apic) {
        klog_warn("SMP: APIC not available, assuming uniprocessor");
        g_smp_cpu_count = 1;
        return;
    }

    g_smp_bsp_apic_id = lapic_get_id();
    klog_info("SMP: BSP APIC ID = %u", g_smp_bsp_apic_id);

    /* Parse ACPI MADT for APs */
    /* For now, just report what CPUID tells us */
    uint32_t logical = g_cpu_info.logical_cpus;
    if (logical > 1) {
        klog_info("SMP: CPU reports %u logical CPUs (may include HT)", logical);
    }

    klog_info("SMP: %u CPU(s) detected", g_smp_cpu_count);
}

bool smp_detect(void) {
    return g_smp_cpu_count > 1;
}

uint32_t smp_get_cpu_count(void) {
    return g_smp_cpu_count;
}

void smp_start_cpu(uint32_t apic_id, uintptr_t entry) {
    (void)apic_id; (void)entry;
    klog_warn("smp_start_cpu: not implemented");
}

void smp_send_ipi(uint32_t dest_apic_id, uint32_t vector) {
    lapic_send_ipi(dest_apic_id, vector);
}

void smp_broadcast_ipi(uint32_t vector) {
    /* Send to all excluding self */
    lapic_send_ipi(0xFF, vector); /* 0xFF = broadcast to all */
}
