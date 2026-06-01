/* gOS Kernel — System Calls
 * Reference: https://wiki.osdev.org/System_Calls
 */

#ifndef GOS_SYSCALL_H
#define GOS_SYSCALL_H

#include "gOS/types.h"

#define SYSCALL_MAX     32

#define SYS_EXIT        0
#define SYS_FORK        1
#define SYS_READ        2
#define SYS_WRITE       3
#define SYS_OPEN        4
#define SYS_CLOSE       5
#define SYS_SLEEP       6
#define SYS_GETPID      7
#define SYS_YIELD       8
#define SYS_MALLOC      9
#define SYS_FREE        10

typedef uint64_t (*syscall_handler_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

void syscall_init(void);
void syscall_register(uint64_t num, syscall_handler_t handler);
void syscall_dispatch_legacy(void *registers_ptr);

#endif /* GOS_SYSCALL_H */
