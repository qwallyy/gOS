/* gOS Kernel — Signal Implementation
 * Reference: https://wiki.osdev.org/Signals
 */

#include "proc/signal.h"
#include "proc/proc.h"
#include "gOS/klog.h"

static const char *g_signal_names[] = {
    [SIGKILL] = "SIGKILL",
    [SIGSEGV] = "SIGSEGV",
    [SIGTERM] = "SIGTERM",
    [SIGSTOP] = "SIGSTOP",
    [SIGCONT] = "SIGCONT",
    [SIGCHLD] = "SIGCHLD",
    [SIGUSR1] = "SIGUSR1",
    [SIGUSR2] = "SIGUSR2",
};

void signal_init(void) {
    klog_info("Signal subsystem initialized");
}

void signal_send(process_t *proc, int sig) {
    if (!proc || sig < 0 || sig >= 64) return;
    if (proc->state == PROC_STATE_UNUSED) return;

    const char *name = (sig < (int)(sizeof(g_signal_names)/sizeof(g_signal_names[0])))
                        ? g_signal_names[sig] : "UNKNOWN";
    klog_info("Signal %s (%d) sent to PID %u", name ? name : "?", sig, proc->pid);

    /* For stub: just terminate on fatal signals */
    if (sig == SIGKILL || sig == SIGSEGV || sig == SIGTERM) {
        proc_exit(sig);
    }
}

void signal_deliver_pending(void) {
    /* Check current process for pending signals */
    process_t *current = proc_get_current();
    if (!current) return;
    /* Stub: no real signal delivery yet */
}

signal_handler_t signal_set_handler(int sig, signal_handler_t handler) {
    (void)sig; (void)handler;
    klog_warn("signal_set_handler: stub");
    return NULL;
}

void signal_block(int sig) {
    (void)sig;
    klog_warn("signal_block: stub");
}

void signal_unblock(int sig) {
    (void)sig;
    klog_warn("signal_unblock: stub");
}
