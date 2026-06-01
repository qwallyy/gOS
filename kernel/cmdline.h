/* gOS Kernel — Command Line Parser Declarations */

#ifndef GOS_CMDLINE_H
#define GOS_CMDLINE_H

#include "gOS/types.h"

void cmdline_parse(const char *cmdline);
bool cmdline_has_flag(const char *flag);
const char *cmdline_get_value(const char *key);

#endif /* GOS_CMDLINE_H */
