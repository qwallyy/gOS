/* gOS Kernel — Process Management
 * Reference: https://wiki.osdev.org/Processes_and_Threads
 */

#ifndef GOS_PROC_H
#define GOS_PROC_H

#include "gOS/types.h"

#define PROC_MAX_PROCESSES      256
#define PROC_NAME_MAX           64
#define PROC_STACK_SIZE         (16 * 1024)  /* 16KB per process */

#define PROC_STATE_UNUSED       0
#define PROC_STATE_READY        1
#define PROC_STATE_RUNNING      2
#define PROC_STATE_BLOCKED      3
#define PROC_STATE_ZOMBIE       4

typedef struct process {
    uint32_t pid;
    uint32_t ppid;
    char name[PROC_NAME_MAX];
    int state;
    uint64_t rsp;           /* Kernel stack pointer */
    uint64_t rip;           /* Instruction pointer */
    uint64_t cr3;           /* Page table base */
    uint64_t *stack;        /* Kernel stack base */
    uint64_t wake_time;     /* For sleep/wait */
    int exit_code;
    struct process *next;   /* Scheduler linked list */
    struct process *prev;
} process_t;

void proc_init(void);
process_t *proc_create(const char *name, void (*entry)(void));
void proc_destroy(process_t *proc);
void proc_yield(void);
process_t *proc_get_current(void);
process_t *proc_get_by_pid(uint32_t pid);
void proc_exit(int code);
void proc_sleep(uint64_t ms);
void proc_dump_all(void);

extern process_t g_process_table[PROC_MAX_PROCESSES];
extern process_t *g_current_proc;
extern uint32_t g_next_pid;

#endif /* GOS_PROC_H */
