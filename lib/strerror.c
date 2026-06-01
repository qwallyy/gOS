/* gOS Kernel — strerror Implementation */

#include "lib/string.h"
#include "errno/errno.h"

const char *strerror(int errnum) {
    return errno_to_string(errnum);
}
