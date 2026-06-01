/* gOS Kernel — Round-Robin Scheduler Implementation
 * Reference: https://wiki.osdev.org/Scheduling_Algorithms
 *            https://wiki.osdev.org/Context_Switching
 */

#include "proc/scheduler.h"
#include "proc/proc.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"
#include "drivers/pit/pit.h"

volatile uint64_t g_scheduler_ticks = 0;
static process_t *g_ready_queue_head = NULL;
static process_t *g_ready_queue_tail = NULL;

void scheduler_init(void) {
    klog_info("Initializing round-robin scheduler");
    g_ready_queue_head = NULL;
    g_ready_queue_tail = NULL;
    g_scheduler_ticks = 0;
}

void scheduler_add(process_t *proc) {
    if (!proc || proc->state != PROC_STATE_READY) return;
    proc->next = NULL;
    proc->prev = g_ready_queue_tail;
    if (g_ready_queue_tail) {
        g_ready_queue_tail->next = proc;
    } else {
        g_ready_queue_head = proc;
    }
    g_ready_queue_tail = proc;
}

void scheduler_remove(process_t *proc) {
    if (!proc) return;
    if (proc->prev) proc->prev->next = proc->next;
    else g_ready_queue_head = proc->next;
    if (proc->next) proc->next->prev = proc->prev;
    else g_ready_queue_tail = proc->prev;
    proc->next = NULL;
    proc->prev = NULL;
}

void scheduler_tick(void) {
    g_scheduler_ticks++;

    /* Check for processes that need to wake up */
    for (int i = 1; i < PROC_MAX_PROCESSES; i++) {
        if (g_process_table[i].state == PROC_STATE_BLOCKED &&
            g_process_table[i].wake_time > 0 &&
            g_process_table[i].wake_time <= g_scheduler_ticks) {
            g_process_table[i].state = PROC_STATE_READY;
            g_process_table[i].wake_time = 0;
            scheduler_add(&g_process_table[i]);
        }
    }

    /* Simple round-robin: switch every tick if there are other processes */
    if (g_ready_queue_head && g_current_proc && g_current_proc->pid != 0) {
        scheduler_yield();
    }
}

void scheduler_yield(void) {
    if (!g_ready_queue_head) return;

    process_t *current = g_current_proc;
    process_t *next = g_ready_queue_head;

    /* Move current to tail if it is still ready */
    if (current && current->state == PROC_STATE_RUNNING) {
        current->state = PROC_STATE_READY;
        scheduler_remove(current);
        scheduler_add(current);
    }

    /* Get next ready process */
    next = g_ready_queue_head;
    while (next && next->state != PROC_STATE_READY) {
        process_t *to_remove = next;
        next = next->next;
        scheduler_remove(to_remove);
    }
    if (!next) {
        /* No ready processes — run idle */
        next = &g_process_table[0];
    }

    next->state = PROC_STATE_RUNNING;
    if (next != current) {
        scheduler_switch_to(next);
    }
}

/* Assembly context switch — saves current context and loads next */
extern void context_switch(uint64_t *old_rsp, uint64_t new_rsp);

void scheduler_switch_to(process_t *next) {
    if (!next || next == g_current_proc) return;

    process_t *prev = g_current_proc;
    g_current_proc = next;

    /* Update TSS RSP0 for Ring 0 entry from user mode */
    extern void tss_set_rsp0(uintptr_t rsp);
    if (next->stack) {
        tss_set_rsp0((uintptr_t)next->stack + PROC_STACK_SIZE);
    }

    if (prev && prev->state != PROC_STATE_UNUSED) {
        context_switch(&prev->rsp, next->rsp);
    } else {
        /* No previous process to save — just load new context */
        context_switch(NULL, next->rsp);
    }
}

void scheduler_dump(void) {
    klog_info("Scheduler: ticks=%llu", (unsigned long long)g_scheduler_ticks);
    klog_info("Ready queue:");
    process_t *p = g_ready_queue_head;
    while (p) {
        klog_info("  PID=%u name=%s state=%d", p->pid, p->name, p->state);
        p = p->next;
    }
}
