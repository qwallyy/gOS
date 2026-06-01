/* gOS Kernel — Kernel Heap Implementation
 * Reference: https://wiki.osdev.org/Heap
 *
 * Simple bump allocator with free list for released blocks.
 * Suitable for kernel early development; can be replaced with
 * a slab allocator or buddy allocator later.
 */

#include "mm/heap/heap.h"
#include "mm/vmm/vmm.h"
#include "mm/pmm/pmm.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

#define HEAP_INITIAL_SIZE   (2 * 1024 * 1024)  /* 2MB */
#define HEAP_MAX_SIZE       (64 * 1024 * 1024) /* 64MB */
#define HEAP_BLOCK_ALIGN    16

#define HEAP_MAGIC_FREE     0xDEADBEEFCAFEBAB0ULL
#define HEAP_MAGIC_USED     0xCAFEBABEDEADBEEFULL

typedef struct heap_block {
    uint64_t magic;
    size_t size;
    struct heap_block *next;
    struct heap_block *prev;
} heap_block_t;

static uintptr_t g_heap_start = 0;
static uintptr_t g_heap_end = 0;
static uintptr_t g_heap_max = 0;
static heap_block_t *g_heap_free_list = NULL;

static void *heap_extend(size_t pages) {
    for (size_t i = 0; i < pages; i++) {
        void *phys = pmm_alloc_page();
        if (!phys) return NULL;
        vmm_map_page(g_heap_end + i * PAGE_SIZE, (uintptr_t)phys,
                     VMM_PRESENT | VMM_WRITABLE);
    }
    void *result = (void *)g_heap_end;
    g_heap_end += pages * PAGE_SIZE;
    return result;
}

static heap_block_t *heap_coalesce(heap_block_t *block) {
    if (block->next && (uintptr_t)block + sizeof(heap_block_t) + block->size == (uintptr_t)block->next) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }
    if (block->prev && (uintptr_t)block->prev + sizeof(heap_block_t) + block->prev->size == (uintptr_t)block) {
        block->prev->size += sizeof(heap_block_t) + block->size;
        block->prev->next = block->next;
        if (block->next) block->next->prev = block->prev;
        return block->prev;
    }
    return block;
}

void heap_init(void) {
    klog_info("Initializing kernel heap");

    g_heap_start = 0xFFFFFFFF80400000ULL; /* 4MB above kernel base */
    g_heap_end = g_heap_start;
    g_heap_max = g_heap_start + HEAP_MAX_SIZE;

    /* Allocate initial heap space */
    if (!heap_extend(HEAP_INITIAL_SIZE / PAGE_SIZE)) {
        klog_emerg("Heap: failed to allocate initial pages");
        cli(); hlt();
        for (;;) { }
    }

    /* Create initial free block covering all allocated space */
    g_heap_free_list = (heap_block_t *)g_heap_start;
    g_heap_free_list->magic = HEAP_MAGIC_FREE;
    g_heap_free_list->size = HEAP_INITIAL_SIZE - sizeof(heap_block_t);
    g_heap_free_list->next = NULL;
    g_heap_free_list->prev = NULL;

    klog_info("Heap initialized at 0x%016llx, size=%zu KiB",
              (unsigned long long)g_heap_start, HEAP_INITIAL_SIZE / 1024);
}

void *kmalloc(size_t size) {
    if (size == 0) return NULL;

    size = (size + HEAP_BLOCK_ALIGN - 1) & ~(HEAP_BLOCK_ALIGN - 1);

    heap_block_t *block = g_heap_free_list;
    while (block) {
        if (block->magic != HEAP_MAGIC_FREE) {
            klog_err("Heap corruption: invalid magic at %p", (void *)block);
            return NULL;
        }
        if (block->size >= size) {
            /* Split block if large enough */
            if (block->size >= size + sizeof(heap_block_t) + HEAP_BLOCK_ALIGN) {
                heap_block_t *new_block = (heap_block_t *)((uintptr_t)block + sizeof(heap_block_t) + size);
                new_block->magic = HEAP_MAGIC_FREE;
                new_block->size = block->size - sizeof(heap_block_t) - size;
                new_block->next = block->next;
                new_block->prev = block->prev;
                if (block->next) block->next->prev = new_block;
                if (block->prev) block->prev->next = new_block;
                else g_heap_free_list = new_block;
                block->size = size;
            } else {
                if (block->prev) block->prev->next = block->next;
                else g_heap_free_list = block->next;
                if (block->next) block->next->prev = block->prev;
            }
            block->magic = HEAP_MAGIC_USED;
            block->next = NULL;
            block->prev = NULL;
            return (void *)((uintptr_t)block + sizeof(heap_block_t));
        }
        block = block->next;
    }

    /* Need to extend heap */
    size_t needed = size + sizeof(heap_block_t);
    size_t pages = (needed + PAGE_SIZE - 1) / PAGE_SIZE;
    if (g_heap_end + pages * PAGE_SIZE > g_heap_max) {
        klog_err("Heap: out of memory (kmalloc %zu)", size);
        return NULL;
    }
    void *new_mem = heap_extend(pages);
    if (!new_mem) return NULL;

    heap_block_t *new_block = (heap_block_t *)new_mem;
    new_block->magic = HEAP_MAGIC_FREE;
    new_block->size = pages * PAGE_SIZE - sizeof(heap_block_t);
    new_block->next = g_heap_free_list;
    new_block->prev = NULL;
    if (g_heap_free_list) g_heap_free_list->prev = new_block;
    g_heap_free_list = new_block;

    return kmalloc(size);
}

void *kzalloc(size_t size) {
    void *ptr = kmalloc(size);
    if (ptr) {
        uint8_t *p = ptr;
        for (size_t i = 0; i < size; i++) p[i] = 0;
    }
    return ptr;
}

void *krealloc(void *ptr, size_t new_size) {
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }
    heap_block_t *block = (heap_block_t *)((uintptr_t)ptr - sizeof(heap_block_t));
    if (block->magic != HEAP_MAGIC_USED) {
        klog_err("krealloc: invalid block magic");
        return NULL;
    }
    if (block->size >= new_size) return ptr;
    void *new_ptr = kmalloc(new_size);
    if (!new_ptr) return NULL;
    uint8_t *src = ptr;
    uint8_t *dst = new_ptr;
    for (size_t i = 0; i < block->size; i++) dst[i] = src[i];
    kfree(ptr);
    return new_ptr;
}

void kfree(void *ptr) {
    if (!ptr) return;
    heap_block_t *block = (heap_block_t *)((uintptr_t)ptr - sizeof(heap_block_t));
    if (block->magic != HEAP_MAGIC_USED) {
        klog_err("kfree: invalid block magic at %p", ptr);
        return;
    }
    block->magic = HEAP_MAGIC_FREE;
    block->next = g_heap_free_list;
    block->prev = NULL;
    if (g_heap_free_list) g_heap_free_list->prev = block;
    g_heap_free_list = block;
    heap_coalesce(block);
}

size_t ksize(void *ptr) {
    if (!ptr) return 0;
    heap_block_t *block = (heap_block_t *)((uintptr_t)ptr - sizeof(heap_block_t));
    if (block->magic != HEAP_MAGIC_USED) return 0;
    return block->size;
}

void heap_dump_stats(void) {
    size_t free_total = 0;
    size_t free_count = 0;
    heap_block_t *block = g_heap_free_list;
    while (block) {
        free_total += block->size;
        free_count++;
        block = block->next;
    }
    klog_info("Heap: allocated=%llu bytes, free=%llu bytes (%zu blocks), max=%llu bytes",
              (unsigned long long)(g_heap_end - g_heap_start),
              (unsigned long long)free_total, free_count,
              (unsigned long long)HEAP_MAX_SIZE);
}
