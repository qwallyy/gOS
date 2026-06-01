/* gOS Kernel — Round-Robin Scheduler
 * Reference: https://wiki.osdev.org/Scheduling_Algorithms
 */

#ifndef GOS_SCHEDULER_H
#define GOS_SCHEDULER_H

#include "gOS/types.h"
#include "proc/proc.h"

void scheduler_init(void);
void scheduler_add(process_t *proc);
void scheduler_remove(process_t *proc);
void scheduler_tick(void);
void scheduler_yield(void);
void scheduler_switch_to(process_t *next);
void scheduler_dump(void);

extern volatile uint64_t g_scheduler_ticks;

#endif /* GOS_SCHEDULER_H */
