/* gOS Kernel — MSR Assembly Wrappers Declarations */

#ifndef GOS_MSR_ASM_H
#define GOS_MSR_ASM_H

#include "gOS/types.h"

uint64_t read_msr(uint32_t msr);
void write_msr(uint32_t msr, uint64_t val);
uint64_t read_tsc(void);
uint64_t read_rdtscp(void);

#endif /* GOS_MSR_ASM_H */
