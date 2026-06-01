/* gOS Kernel — SYSCALL Assembly Declarations */

#ifndef GOS_SYSCALL_ASM_H
#define GOS_SYSCALL_ASM_H

#include "gOS/types.h"

void syscall_entry_asm(void);
void syscall_enable(void);
void syscall_disable(void);

#endif /* GOS_SYSCALL_ASM_H */
