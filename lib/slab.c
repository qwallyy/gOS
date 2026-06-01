/* gOS Kernel — Slab Allocator Implementation
 * Reference: https://wiki.osdev.org/Slab_Allocator
 */

#include "lib/slab.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

#define SLAB_PAGE_COUNT     1
#define SLAB_PAGE_SIZE      4096
#define SLAB_TOTAL_SIZE     (SLAB_PAGE_COUNT * SLAB_PAGE_SIZE)

void slab_init(void) {
    klog_info("Slab allocator initialized");
}

static slab_t *slab_create(size_t obj_size) {
    slab_t *slab = (slab_t *)pmm_alloc_page();
    if (!slab) return NULL;

    /* Map the slab structure and its page */
    slab = (slab_t *)((uintptr_t)slab + KERNEL_VBASE);

    size_t header_size = sizeof(slab_t);
    size_t avail = SLAB_TOTAL_SIZE - header_size;
    size_t count = avail / obj_size;

    slab->memory = (uint8_t *)((uintptr_t)slab + header_size);
    slab->obj_size = obj_size;
    slab->obj_count = count;
    slab->used = 0;
    slab->next = NULL;

    /* Build free list */
    void **current = (void **)&slab->free_list;
    for (size_t i = 0; i < count; i++) {
        *current = slab->memory + i * obj_size;
        current = (void **)(*current);
    }
    *current = NULL;

    return slab;
}

slab_cache_t *slab_cache_create(const char *name, size_t obj_size, size_t align,
                                void (*ctor)(void *), void (*dtor)(void *)) {
    (void)align;
    slab_cache_t *cache = kmalloc(sizeof(slab_cache_t));
    if (!cache) return NULL;

    cache->name = name;
    cache->obj_size = (obj_size < sizeof(void *)) ? sizeof(void *) : obj_size;
    cache->align = align;
    cache->slabs_full = NULL;
    cache->slabs_partial = NULL;
    cache->slabs_free = NULL;
    cache->ctor = ctor;
    cache->dtor = dtor;

    /* Create initial slab */
    cache->slabs_free = slab_create(cache->obj_size);
    if (!cache->slabs_free) {
        kfree(cache);
        return NULL;
    }

    klog_info("Slab cache '%s' created: obj_size=%zu", name, cache->obj_size);
    return cache;
}

void slab_cache_destroy(slab_cache_t *cache) {
    if (!cache) return;
    /* In a full implementation, we would unmap and free all slab pages */
    klog_info("Slab cache '%s' destroyed", cache->name);
    kfree(cache);
}

void *slab_alloc(slab_cache_t *cache) {
    if (!cache) return NULL;

    /* Try partial slabs first */
    slab_t *slab = cache->slabs_partial;
    if (!slab) {
        slab = cache->slabs_free;
        if (!slab) {
            /* Allocate new slab */
            slab = slab_create(cache->obj_size);
            if (!slab) return NULL;
            cache->slabs_free = slab;
        }
        cache->slabs_free = slab->next;
        slab->next = cache->slabs_partial;
        cache->slabs_partial = slab;
    }

    void *obj = slab->free_list;
    if (obj) {
        slab->free_list = *(void **)obj;
        slab->used++;
        if (cache->ctor) cache->ctor(obj);

        if (slab->used >= slab->obj_count) {
            /* Move to full list */
            cache->slabs_partial = slab->next;
            slab->next = cache->slabs_full;
            cache->slabs_full = slab;
        }
    }
    return obj;
}

void slab_free(slab_cache_t *cache, void *obj) {
    if (!cache || !obj) return;

    /* Find which slab owns this object */
    slab_t *slab = cache->slabs_partial;
    slab_t *prev = NULL;
    while (slab) {
        if (obj >= (void *)slab->memory &&
            obj < (void *)(slab->memory + slab->obj_count * slab->obj_size)) {
            break;
        }
        prev = slab;
        slab = slab->next;
    }
    if (!slab) {
        slab = cache->slabs_full;
        prev = NULL;
        while (slab) {
            if (obj >= (void *)slab->memory &&
                obj < (void *)(slab->memory + slab->obj_count * slab->obj_size)) {
                break;
            }
            prev = slab;
            slab = slab->next;
        }
    }
    if (!slab) {
        klog_err("slab_free: object %p does not belong to cache '%s'", obj, cache->name);
        return;
    }

    if (cache->dtor) cache->dtor(obj);
    *(void **)obj = slab->free_list;
    slab->free_list = obj;
    slab->used--;

    /* Move from full to partial if needed */
    if (slab->used == slab->obj_count - 1) {
        /* Was full, move to partial */
        if (prev) prev->next = slab->next;
        else cache->slabs_full = slab->next;
        slab->next = cache->slabs_partial;
        cache->slabs_partial = slab;
    }
}

void slab_cache_info(const slab_cache_t *cache) {
    if (!cache) return;
    size_t total = 0, used = 0;
    slab_t *s = cache->slabs_full;
    while (s) { total += s->obj_count; used += s->used; s = s->next; }
    s = cache->slabs_partial;
    while (s) { total += s->obj_count; used += s->used; s = s->next; }
    s = cache->slabs_free;
    while (s) { total += s->obj_count; used += s->used; s = s->next; }
    klog_info("Slab cache '%s': total=%zu, used=%zu, free=%zu",
              cache->name, total, used, total - used);
}
