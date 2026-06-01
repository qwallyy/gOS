/* gOS Kernel — Per-CPU Data Implementation
 * Reference: https://wiki.osdev.org/SMP
 */

#include "arch/x86_64/percpu.h"
#include "proc/proc.h"
#include "gOS/klog.h"
#include "lib/string.h"

percpu_data_t g_percpu[MAX_CPUS];

void percpu_init(uint32_t cpu_id) {
    if (cpu_id >= MAX_CPUS) {
        klog_err("percpu_init: invalid CPU ID %u", cpu_id);
        return;
    }
    memset(&g_percpu[cpu_id], 0, sizeof(percpu_data_t));
    g_percpu[cpu_id].cpu_id = cpu_id;
    klog_info("Per-CPU data initialized for CPU %u", cpu_id);
}

void percpu_init_all(void) {
    for (uint32_t i = 0; i < MAX_CPUS; i++) {
        percpu_init(i);
    }
}
