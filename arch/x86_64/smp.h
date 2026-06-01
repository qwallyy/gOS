/* gOS Kernel — Symmetric Multiprocessing (SMP)
 * Reference: https://wiki.osdev.org/SMP
 */

#ifndef GOS_SMP_H
#define GOS_SMP_H

#include "gOS/types.h"

#define SMP_MAX_CPUS    32

void smp_init(void);
bool smp_detect(void);
uint32_t smp_get_cpu_count(void);
void smp_start_cpu(uint32_t apic_id, uintptr_t entry);
void smp_send_ipi(uint32_t dest_apic_id, uint32_t vector);
void smp_broadcast_ipi(uint32_t vector);

extern uint32_t g_smp_cpu_count;
extern uint32_t g_smp_bsp_apic_id;

#endif /* GOS_SMP_H */
