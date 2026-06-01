/* gOS Kernel — Spinlock
 * Reference: https://wiki.osdev.org/Spinlock
 */

#ifndef GOS_SPINLOCK_H
#define GOS_SPINLOCK_H

#include "gOS/types.h"

typedef struct spinlock {
    volatile uint32_t lock;
    const char *name;
} spinlock_t;

#define SPINLOCK_INIT(name) { 0, #name }

static inline void spinlock_init(spinlock_t *sl) {
    sl->lock = 0;
    sl->name = "unnamed";
}

static inline void spinlock_acquire(spinlock_t *sl) {
    while (!atomic_cmpxchg64((volatile uint64_t *)&sl->lock, 0, 1)) {
        while (sl->lock) {
            pause();
        }
    }
    barrier();
}

static inline void spinlock_release(spinlock_t *sl) {
    barrier();
    sl->lock = 0;
}

static inline bool spinlock_try_acquire(spinlock_t *sl) {
    bool acquired = atomic_cmpxchg64((volatile uint64_t *)&sl->lock, 0, 1);
    if (acquired) barrier();
    return acquired;
}

/* Interrupt-safe spinlock */
typedef struct irq_spinlock {
    spinlock_t lock;
    uint64_t irq_flags;
} irq_spinlock_t;

static inline void irq_spinlock_acquire(irq_spinlock_t *sl) {
    sl->irq_flags = read_rflags();
    cli();
    spinlock_acquire(&sl->lock);
}

static inline void irq_spinlock_release(irq_spinlock_t *sl) {
    spinlock_release(&sl->lock);
    if (sl->irq_flags & 0x200) {
        sti();
    }
}

#endif /* GOS_SPINLOCK_H */
