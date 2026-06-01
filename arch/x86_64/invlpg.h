/* gOS Kernel — TLB/Cache Operation Declarations */

#ifndef GOS_INVLPG_H
#define GOS_INVLPG_H

#include "gOS/types.h"

void invlpg_page(uintptr_t addr);
void flush_tlb(void);
void flush_tlb_entry(uintptr_t addr);
void wbinvd_cache(void);

#endif /* GOS_INVLPG_H */
