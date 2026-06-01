/* gOS Kernel — Assertions */

#ifndef GOS_ASSERT_H
#define GOS_ASSERT_H

#include "gOS/kernel.h"

#define assert(x) do { if (!(x)) kernel_panic(__FILE__, __LINE__, "Assertion failed: " #x); } while (0)

#endif /* GOS_ASSERT_H */
