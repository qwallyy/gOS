/* gOS Kernel — Panic Handler Declarations */

#ifndef GOS_PANIC_H
#define GOS_PANIC_H

#include "gOS/types.h"

NORETURN void kernel_panic(const char *file, int line, const char *reason);
NORETURN void kernel_panic_msg(const char *msg);

#endif /* GOS_PANIC_H */
