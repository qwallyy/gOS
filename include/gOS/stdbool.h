/* gOS Kernel — stdbool.h replacement for freestanding builds */

#ifndef GOS_STDBOOL_H
#define GOS_STDBOOL_H

#ifndef __cplusplus

#define bool    _Bool
#define true    1
#define false   0

#endif /* __cplusplus */

#define __bool_true_false_are_defined 1

#endif /* GOS_STDBOOL_H */
