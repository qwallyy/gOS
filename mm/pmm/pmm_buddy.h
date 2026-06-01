/* gOS Kernel — Buddy Allocator (future PMM replacement)
 * Reference: https://wiki.osdev.org/Page_Frame_Allocation
 */

#ifndef GOS_PMM_BUDDY_H
#define GOS_PMM_BUDDY_H

#include "gOS/types.h"

#define BUDDY_MAX_ORDER     12  /* Up to 4MB blocks */

void buddy_init(uintptr_t base, size_t total_pages);
void *buddy_alloc(size_t order);
void buddy_free(void *page, size_t order);
size_t buddy_get_free_pages(void);
void buddy_dump(void);

#endif /* GOS_PMM_BUDDY_H */
