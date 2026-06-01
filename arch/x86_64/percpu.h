/* gOS Kernel — Per-CPU Variables
 * Reference: https://wiki.osdev.org/SMP
 */

#ifndef GOS_PERCPU_H
#define GOS_PERCPU_H

#include "gOS/types.h"
#include "proc/proc.h"

#define MAX_CPUS        32

typedef struct percpu_data {
    uint32_t cpu_id;
    uint32_t lapic_id;
    uint64_t syscall_count;
    uint64_t irq_count;
    uint64_t timer_ticks;
    process_t *current_proc;
    void *irq_stack;
} percpu_data_t;

extern percpu_data_t g_percpu[MAX_CPUS];

static inline percpu_data_t *percpu_get(void) {
    /* Use GS base for per-CPU data in SMP mode */
    /* For now, return CPU 0 */
    return &g_percpu[0];
}

static inline uint32_t percpu_id(void) {
    return percpu_get()->cpu_id;
}

void percpu_init(uint32_t cpu_id);
void percpu_init_all(void);

#endif /* GOS_PERCPU_H */
