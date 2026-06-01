/* gOS Kernel — Integer to String Conversion */

#ifndef GOS_ITOA_H
#define GOS_ITOA_H

#include "gOS/types.h"

char *itoa(int value, char *str, int base);
char *utoa(unsigned int value, char *str, int base);
char *lltoa(long long value, char *str, int base);
char *ulltoa(unsigned long long value, char *str, int base);

#endif /* GOS_ITOA_H */
