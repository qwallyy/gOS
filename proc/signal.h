/* gOS Kernel — Signals
 * Reference: https://wiki.osdev.org/Signals
 */

#ifndef GOS_SIGNAL_H
#define GOS_SIGNAL_H

#include "gOS/types.h"
#include "proc/proc.h"

#define SIGKILL     9
#define SIGSEGV     11
#define SIGTERM     15
#define SIGSTOP     17
#define SIGCONT     18
#define SIGCHLD     20
#define SIGUSR1     30
#define SIGUSR2     31

typedef void (*signal_handler_t)(int);

typedef struct signal_state {
    uint64_t pending;
    uint64_t blocked;
    signal_handler_t handlers[64];
} signal_state_t;

void signal_init(void);
void signal_send(process_t *proc, int sig);
void signal_deliver_pending(void);
signal_handler_t signal_set_handler(int sig, signal_handler_t handler);
void signal_block(int sig);
void signal_unblock(int sig);

#endif /* GOS_SIGNAL_H */
