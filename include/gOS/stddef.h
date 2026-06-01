/* gOS Kernel — stddef.h replacement for freestanding builds */

#ifndef GOS_STDDEF_H
#define GOS_STDDEF_H

#define NULL    ((void *)0)

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __SIZE_TYPE__    size_t;
typedef __SIZE_TYPE__    uintptr_t;

typedef long double max_align_t;

#define offsetof(type, member) __builtin_offsetof(type, member)

#endif /* GOS_STDDEF_H */
