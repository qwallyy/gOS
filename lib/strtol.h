/* gOS Kernel — strtol Family Declarations */

#ifndef GOS_STRTOL_H
#define GOS_STRTOL_H

#include "gOS/types.h"

long     strtol(const char *str, char **endptr, int base);
long long strtoll(const char *str, char **endptr, int base);
unsigned long strtoul(const char *str, char **endptr, int base);
unsigned long long strtoull(const char *str, char **endptr, int base);

#endif /* GOS_STRTOL_H */
