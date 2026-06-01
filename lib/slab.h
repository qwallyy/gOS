/* gOS Kernel — Slab Allocator
 * Reference: https://wiki.osdev.org/Slab_Allocator
 *            Linux kernel slab allocator pattern
 */

#ifndef GOS_SLAB_H
#define GOS_SLAB_H

#include "gOS/types.h"

typedef struct slab {
    struct slab *next;
    void *free_list;
    uint8_t *memory;
    size_t obj_size;
    size_t obj_count;
    size_t used;
} slab_t;

typedef struct slab_cache {
    const char *name;
    size_t obj_size;
    size_t align;
    slab_t *slabs_full;
    slab_t *slabs_partial;
    slab_t *slabs_free;
    void (*ctor)(void *);
    void (*dtor)(void *);
} slab_cache_t;

void slab_init(void);
slab_cache_t *slab_cache_create(const char *name, size_t obj_size, size_t align,
                                 void (*ctor)(void *), void (*dtor)(void *));
void slab_cache_destroy(slab_cache_t *cache);
void *slab_alloc(slab_cache_t *cache);
void slab_free(slab_cache_t *cache, void *obj);
void slab_cache_info(const slab_cache_t *cache);

#endif /* GOS_SLAB_H */
