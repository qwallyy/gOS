/* gOS Kernel — stdatomic.h replacement (minimal)
 * For full atomics, use the atomic_* wrappers in types.h
 */

#ifndef GOS_STDATOMIC_H
#define GOS_STDATOMIC_H

#include "gOS/types.h"

typenum memory_order {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

#define atomic_load_explicit(ptr, order) \
    __atomic_load_n(ptr, order)

#define atomic_store_explicit(ptr, val, order) \
    __atomic_store_n(ptr, val, order)

#define atomic_fetch_add_explicit(ptr, val, order) \
    __atomic_fetch_add(ptr, val, order)

#define atomic_fetch_sub_explicit(ptr, val, order) \
    __atomic_fetch_sub(ptr, val, order)

#define atomic_compare_exchange_strong_explicit(ptr, expected, desired, succ, fail) \
    __atomic_compare_exchange_n(ptr, expected, desired, false, succ, fail)

#define atomic_thread_fence(order) \
    __atomic_thread_fence(order)

#define ATOMIC_VAR_INIT(value) (value)
#define atomic_init(ptr, value) atomic_store_explicit(ptr, value, memory_order_relaxed)

#endif /* GOS_STDATOMIC_H */
