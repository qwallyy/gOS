/* gOS Kernel — IDT Assembly Helper Declarations */

#ifndef GOS_IDT_ASM_H
#define GOS_IDT_ASM_H

#include "gOS/types.h"

void idt_load(uint64_t base, uint16_t limit);
void irq_disable(void);
void irq_enable(void);
void irq_halt(void);

#endif /* GOS_IDT_ASM_H */
