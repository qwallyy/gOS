/* gOS Kernel — General Purpose Macros */

#ifndef GOS_MACRO_H
#define GOS_MACRO_H

/* Token pasting */
#define TOKEN_PASTE(a, b) a ## b
#define TOKEN_PASTE2(a, b) TOKEN_PASTE(a, b)

/* Stringify */
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

/* Unique identifier generation */
#define UNIQUE_ID(prefix) TOKEN_PASTE2(prefix, __LINE__)

/* Compile-time assert */
#define CASSERT(expr) typedef char UNIQUE_ID(_assert_)[(expr) ? 1 : -1]

/* Fallthrough annotation */
#define FALLTHROUGH __attribute__((fallthrough))

/* Non-null argument annotation */
#define NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))

/* Format check annotation */
#define FORMAT_CHECK(fmt, args) __attribute__((format(printf, fmt, args)))

/* Sentinel for variadic functions */
#define SENTINEL __attribute__((sentinel))

/* Warn unused result */
#define WUR __attribute__((warn_unused_result))

/* Access size annotation */
#define ACCESS_SIZE(size) __attribute__((access(read_write, size)))

#endif /* GOS_MACRO_H */
