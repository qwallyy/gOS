/* gOS Kernel — stdnoreturn.h replacement */

#ifndef GOS_STDNORETURN_H
#define GOS_STDNORETURN_H

#ifndef __cplusplus

#define noreturn _Noreturn

#else

#define noreturn [[noreturn]]

#endif

#endif /* GOS_STDNORETURN_H */
