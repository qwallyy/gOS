/* gOS Kernel — Process Table Dumper */

#include "proc/proc.h"
#include "gOS/klog.h"

void proc_dump_all(void) {
    klog_info("Process table:");
    for (size_t i = 0; i < PROC_MAX_PROCESSES; i++) {
        process_t *p = &g_process_table[i];
        if (p->state == PROC_STATE_UNUSED) continue;
        const char *state_str = "unknown";
        switch (p->state) {
            case PROC_STATE_READY:    state_str = "ready"; break;
            case PROC_STATE_RUNNING:  state_str = "running"; break;
            case PROC_STATE_BLOCKED:  state_str = "blocked"; break;
            case PROC_STATE_ZOMBIE:   state_str = "zombie"; break;
            default: break;
        }
        klog_info("  [PID %u] '%s' state=%s",
                  p->pid, p->name, state_str);
    }
}
