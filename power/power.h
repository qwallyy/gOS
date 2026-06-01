/* gOS Kernel — Power Management
 * Reference: https://wiki.osdev.org/Shutdown
 *            https://wiki.osdev.org/Reboot
 */

#ifndef GOS_POWER_H
#define GOS_POWER_H

#include "gOS/types.h"

#define POWER_OFF_ACPI      1
#define POWER_OFF_APM       2
#define POWER_OFF_VIRTUAL   3
#define POWER_OFF_TRIPLE    4

void power_init(void);
void power_shutdown(void);
void power_reboot(void);
void power_halt(void);
bool power_can_shutdown(void);
bool power_can_reboot(void);

#endif /* GOS_POWER_H */
