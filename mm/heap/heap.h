/* gOS Kernel — Kernel Heap (kmalloc/kfree)
 * Reference: https://wiki.osdev.org/Heap
 */

#ifndef GOS_HEAP_H
#define GOS_HEAP_H

#include "gOS/types.h"

void heap_init(void);
void *kmalloc(size_t size);
void *kzalloc(size_t size);
void *krealloc(void *ptr, size_t new_size);
void kfree(void *ptr);
size_t ksize(void *ptr);
void heap_dump_stats(void);

#endif /* GOS_HEAP_H */
