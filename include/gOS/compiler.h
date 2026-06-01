/* gOS Kernel — Compiler Intrinsics and Attributes
 * Reference: GCC documentation for __attribute__ extensions
 */

#ifndef GOS_COMPILER_H
#define GOS_COMPILER_H

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

/* Unused parameter suppression */
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

/* Section placement */
#ifndef SECTION
#define SECTION(x) __attribute__((section(x)))
#endif

/* Used marker (prevent linker stripping) */
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

/* Pure function (no side effects, depends only on args) */
#ifndef PURE
#define PURE __attribute__((pure))
#endif

/* Const function (no side effects, no global state) */
#ifndef CONST
#define CONST __attribute__((const))
#endif

/* Deprecated */
#ifndef DEPRECATED
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#endif

/* Constructor (runs before main) */
#ifndef CONSTRUCTOR
#define CONSTRUCTOR __attribute__((constructor))
#endif

/* Destructor */
#ifndef DESTRUCTOR
#define DESTRUCTOR __attribute__((destructor))
#endif

/* Memory barrier / compiler fence */
#ifndef barrier
#define barrier() __asm__ volatile ("" ::: "memory")
#endif

/* Static assertion at compile time */
#ifndef STATIC_ASSERT
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#endif

/* Branch prediction hints */
#ifndef LIKELY
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#endif

/* Offset of field in structure */
#ifndef OFFSETOF
#define OFFSETOF(type, member) __builtin_offsetof(type, member)
#endif

/* Array size */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* Container of member pointer */
#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - OFFSETOF(type, member)))
#endif

/* Minimum / maximum */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Clamp value to range */
#ifndef CLAMP
#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))
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

/* Next power of two */
#ifndef NEXT_POW2
#define NEXT_POW2(x) \
    ((x) <= 1 ? 1 : 1ULL << (64 - __builtin_clzll((x) - 1)))
#endif

/* Bit manipulation */
#ifndef BIT
#define BIT(n) (1ULL << (n))
#endif

#ifndef BIT_SET
#define BIT_SET(x, n)   ((x) |= BIT(n))
#endif

#ifndef BIT_CLEAR
#define BIT_CLEAR(x, n) ((x) &= ~BIT(n))
#endif

#ifndef BIT_TEST
#define BIT_TEST(x, n)  (((x) & BIT(n)) != 0)
#endif

#endif /* GOS_COMPILER_H */
