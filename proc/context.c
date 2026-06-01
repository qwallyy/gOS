/* gOS Kernel — Context Switch (Assembly)
 * Reference: https://wiki.osdev.org/Context_Switching
 *
 * This file contains the low-level assembly for saving and restoring
 * CPU context during a process switch. It is called by the scheduler.
 */

#include "gOS/types.h"

/*
 * void context_switch(uint64_t *old_rsp, uint64_t new_rsp);
 *
 * If old_rsp is not NULL, saves the current CPU context at the address
 * pointed to by old_rsp, then loads the context from new_rsp.
 *
 * The stack layout expected (pushed by assembly ISR stubs or scheduler):
 *   [top]  RAX, RBX, RCX, RDX, RSI, RDI, RBP, R8-R15  (15 registers)
 *   [bot]  RIP (return address)
 */

__asm__ (
".global context_switch\n"
".type context_switch, @function\n"
"context_switch:\n"
"    pushq %rbx\n"
"    pushq %r12\n"
"    pushq %r13\n"
"    pushq %r14\n"
"    pushq %r15\n"
"    pushq %rbp\n"
"    movq %rsp, (%rdi)\n"     /* Save current RSP to *old_rsp */
"    movq %rsi, %rsp\n"       /* Load new RSP */
"    popq %rbp\n"
"    popq %r15\n"
"    popq %r14\n"
"    popq %r13\n"
"    popq %r12\n"
"    popq %rbx\n"
"    retq\n"
);
