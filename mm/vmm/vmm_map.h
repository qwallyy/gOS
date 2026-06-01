/* gOS Kernel — VMM High-Level Mapping Helpers */

#ifndef GOS_VMM_MAP_H
#define GOS_VMM_MAP_H

#include "gOS/types.h"

void vmm_map_range(uintptr_t virt, uintptr_t phys, size_t count, uint64_t flags);
void vmm_unmap_range(uintptr_t virt, size_t count);
void vmm_map_identity(uintptr_t start, uintptr_t end, uint64_t flags);
void vmm_map_kernel_text(void);
void vmm_map_kernel_data(void);
bool vmm_protect_page(uintptr_t virt, uint64_t new_flags);

#endif /* GOS_VMM_MAP_H */
