/* gOS Kernel — Kernel Main Entry Declarations */

#ifndef GOS_KMAIN_H
#define GOS_KMAIN_H

#include "gOS/types.h"

void kernel_early_init(uintptr_t mb_info_addr);
NORETURN void kernel_main(void);

#endif /* GOS_KMAIN_H */
