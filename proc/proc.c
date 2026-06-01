/* gOS Kernel — Process Management Implementation
 * Reference: https://wiki.osdev.org/Processes_and_Threads
 */

#include "proc/proc.h"
#include "proc/scheduler.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "mm/heap/heap.h"
#include "hal/gdt/gdt.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"
#include "lib/string.h"

process_t g_process_table[PROC_MAX_PROCESSES];
process_t *g_current_proc = NULL;
uint32_t g_next_pid = 1;

void proc_init(void) {
    klog_info("Initializing process manager");
    for (int i = 0; i < PROC_MAX_PROCESSES; i++) {
        g_process_table[i].state = PROC_STATE_UNUSED;
        g_process_table[i].pid = 0;
        g_process_table[i].next = NULL;
        g_process_table[i].prev = NULL;
    }

    /* Create idle process (PID 0) */
    process_t *idle = &g_process_table[0];
    idle->pid = 0;
    idle->ppid = 0;
    strcpy(idle->name, "idle");
    idle->state = PROC_STATE_READY;
    idle->rsp = read_rsp();
    idle->rip = 0;
    idle->cr3 = read_cr3();
    idle->stack = NULL;
    g_current_proc = idle;

    klog_info("Process manager initialized, idle proc PID=0");
}

process_t *proc_create(const char *name, void (*entry)(void)) {
    int slot = -1;
    for (int i = 1; i < PROC_MAX_PROCESSES; i++) {
        if (g_process_table[i].state == PROC_STATE_UNUSED) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        klog_err("proc_create: no free process slots");
        return NULL;
    }

    process_t *proc = &g_process_table[slot];
    proc->pid = g_next_pid++;
    proc->ppid = g_current_proc ? g_current_proc->pid : 0;
    strncpy(proc->name, name, PROC_NAME_MAX - 1);
    proc->name[PROC_NAME_MAX - 1] = '\0';
    proc->state = PROC_STATE_READY;

    /* Allocate kernel stack */
    proc->stack = (uint64_t *)kmalloc(PROC_STACK_SIZE);
    if (!proc->stack) {
        klog_err("proc_create: failed to allocate stack for PID %u", proc->pid);
        proc->state = PROC_STATE_UNUSED;
        return NULL;
    }

    /* Initialize stack for context switch */
    uint64_t *sp = (uint64_t *)((uintptr_t)proc->stack + PROC_STACK_SIZE);
    /* Push dummy values for registers that will be popped by context switch */
    sp -= 15; /* Space for r15-r8, rbp, rdi, rsi, rdx, rcx, rbx, rax */
    for (int i = 0; i < 15; i++) sp[i] = 0;

    /* Entry point pushed as "return address" */
    sp--;
    *sp = (uint64_t)entry;

    proc->rsp = (uint64_t)sp;
    proc->rip = (uint64_t)entry;
    proc->cr3 = read_cr3(); /* Use kernel page table for now */
    proc->wake_time = 0;
    proc->exit_code = 0;
    proc->next = NULL;
    proc->prev = NULL;

    scheduler_add(proc);
    klog_info("Created process '%s' PID=%u entry=%p", proc->name, proc->pid, (void *)(uintptr_t)entry);
    return proc;
}

void proc_destroy(process_t *proc) {
    if (!proc || proc->pid == 0) return;
    klog_info("Destroying process '%s' PID=%u", proc->name, proc->pid);

    scheduler_remove(proc);

    if (proc->stack) {
        kfree(proc->stack);
        proc->stack = NULL;
    }

    proc->state = PROC_STATE_UNUSED;
    proc->pid = 0;
}

void proc_yield(void) {
    scheduler_yield();
}

process_t *proc_get_current(void) {
    return g_current_proc;
}

process_t *proc_get_by_pid(uint32_t pid) {
    for (int i = 0; i < PROC_MAX_PROCESSES; i++) {
        if (g_process_table[i].pid == pid && g_process_table[i].state != PROC_STATE_UNUSED) {
            return &g_process_table[i];
        }
    }
    return NULL;
}

void proc_exit(int code) {
    process_t *proc = g_current_proc;
    if (!proc || proc->pid == 0) {
        klog_err("proc_exit: idle process tried to exit!");
        return;
    }
    klog_info("Process '%s' PID=%u exiting with code %d", proc->name, proc->pid, code);
    proc->exit_code = code;
    proc->state = PROC_STATE_ZOMBIE;
    scheduler_yield();
}

void proc_sleep(uint64_t ms) {
    process_t *proc = g_current_proc;
    if (!proc || proc->pid == 0) return;
    extern uint64_t pit_get_ticks(void);
    extern uint32_t g_pit_hz;
    proc->wake_time = pit_get_ticks() + (ms * g_pit_hz / 1000);
    proc->state = PROC_STATE_BLOCKED;
    scheduler_yield();
}

