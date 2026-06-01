/* gOS Kernel — TSS Assembly Helper Declarations */

#ifndef GOS_TSS_ASM_H
#define GOS_TSS_ASM_H

#include "gOS/types.h"

void tss_flush(uint16_t tss_sel);

#endif /* GOS_TSS_ASM_H */
