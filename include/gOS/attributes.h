/* gOS Kernel — Function and Variable Attributes */

#ifndef GOS_ATTRIBUTES_H
#define GOS_ATTRIBUTES_H

#define __aligned(x)     __attribute__((aligned(x)))
#define __packed         __attribute__((packed))
#define __noreturn       __attribute__((noreturn))
#define __unused         __attribute__((unused))
#define __used           __attribute__((used))
#define __weak           __attribute__((weak))
#define __always_inline  __attribute__((always_inline)) inline
#define __noinline       __attribute__((noinline))
#define __pure           __attribute__((pure))
#define __const          __attribute__((const))
#define __deprecated(msg) __attribute__((deprecated(msg)))
#define __section(name)  __attribute__((section(name)))
#define __init           __attribute__((constructor))
#define __exit           __attribute__((destructor))
#define __hidden         __attribute__((visibility("hidden")))
#define __export         __attribute__((visibility("default")))

#endif /* GOS_ATTRIBUTES_H */
