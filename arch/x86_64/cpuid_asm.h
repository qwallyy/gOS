/* gOS Kernel — CPUID Assembly Wrappers Declarations */

#ifndef GOS_CPUID_ASM_H
#define GOS_CPUID_ASM_H

#include "gOS/types.h"

void cpuid_get_vendor(char *buf);
void cpuid_get_brand(char *buf);
void cpuid_get_features(uint32_t *edx, uint32_t *ecx);

#endif /* GOS_CPUID_ASM_H */
