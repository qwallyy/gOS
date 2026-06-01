/* gOS Kernel — HPET (High Precision Event Timer)
 * Reference: https://wiki.osdev.org/HPET
 */

#ifndef GOS_HPET_H
#define GOS_HPET_H

#include "gOS/types.h"

#define HPET_GENERAL_CAP    0x000
#define HPET_GENERAL_CONFIG 0x010
#define HPET_GENERAL_STATUS 0x020
#define HPET_MAIN_COUNTER   0x0F0
#define HPET_TIMER0_CONFIG  0x100
#define HPET_TIMER0_COMP    0x108
#define HPET_TIMER0_FSB     0x110

#define HPET_CAP_ID_COUNTER_CLK  (1ULL << 32)
#define HPET_CAP_LEGACY_REPLACE  (1ULL << 15)

void hpet_init(void);
uint64_t hpet_get_counter(void);
uint64_t hpet_get_frequency(void);
void hpet_sleep_us(uint64_t us);
bool hpet_is_available(void);

#endif /* GOS_HPET_H */
