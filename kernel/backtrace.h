/* gOS Kernel — Stack Backtrace
 * Reference: https://wiki.osdev.org/Stack_Trace
 */

#ifndef GOS_BACKTRACE_H
#define GOS_BACKTRACE_H

#include "gOS/types.h"

#define BACKTRACE_MAX_DEPTH     32

typedef struct stack_frame {
    struct stack_frame *rbp;
    uint64_t rip;
} stack_frame_t;

void backtrace_print(void);
void backtrace_capture(uint64_t *addrs, size_t max_depth, size_t *out_depth);
void backtrace_symbol(uint64_t addr, char *buf, size_t buf_size);

#endif /* GOS_BACKTRACE_H */
