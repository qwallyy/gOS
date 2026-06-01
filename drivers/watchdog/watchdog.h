/* gOS Kernel — Software Watchdog
 * Reference: https://wiki.osdev.org/Watchdog
 */

#ifndef GOS_WATCHDOG_H
#define GOS_WATCHDOG_H

#include "gOS/types.h"

#define WATCHDOG_TIMEOUT_MS     5000

void watchdog_init(void);
void watchdog_feed(void);
void watchdog_enable(void);
void watchdog_disable(void);
bool watchdog_is_enabled(void);
void watchdog_set_timeout(uint32_t ms);

#endif /* GOS_WATCHDOG_H */
