/* gOS Kernel — Common Definitions and Macros */

#ifndef GOS_CDEFS_H
#define GOS_CDEFS_H

/* Alignment */
#ifndef ALIGN
#define ALIGN(x) __attribute__((aligned(x)))
#endif

/* Packed structures */
#ifndef PACKED
#define PACKED __attribute__((packed))
#endif

/* No return */
#ifndef NORETURN
#define NORETURN __attribute__((noreturn))
#endif

/* Unused parameter */
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

/* Section placement */
#ifndef SECTION
#define SECTION(x) __attribute__((section(x)))
#endif

/* Used marker */
#ifndef USED
#define USED __attribute__((used))
#endif

/* Weak symbol */
#ifndef WEAK
#define WEAK __attribute__((weak))
#endif

/* Always inline */
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif

/* No inline */
#ifndef NOINLINE
#define NOINLINE __attribute__((noinline))
#endif

/* Memory barrier */
#ifndef barrier
#define barrier() __asm__ volatile ("" ::: "memory")
#endif

/* Branch prediction */
#ifndef LIKELY
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#endif

/* Offset of */
#ifndef OFFSETOF
#define OFFSETOF(type, member) __builtin_offsetof(type, member)
#endif

/* Array size */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* Container of */
#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - OFFSETOF(type, member)))
#endif

/* Min / Max */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Round up/down */
#ifndef ROUND_UP
#define ROUND_UP(x, y)   ((((x) + (y) - 1) / (y)) * (y))
#endif
#ifndef ROUND_DOWN
#define ROUND_DOWN(x, y) (((x) / (y)) * (y))
#endif

/* Is power of two */
#ifndef IS_POW2
#define IS_POW2(x) (((x) & ((x) - 1)) == 0)
#endif

/* Bit macros */
#ifndef BIT
#define BIT(n) (1ULL << (n))
#endif

#endif /* GOS_CDEFS_H */
