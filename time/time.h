/* gOS Kernel — Timekeeping
 * Reference: https://wiki.osdev.org/RTC
 *            https://wiki.osdev.org/Programmable_Interval_Timer
 */

#ifndef GOS_TIME_H
#define GOS_TIME_H

#include "gOS/types.h"

typedef struct timespec {
    uint64_t sec;
    uint64_t nsec;
} timespec_t;

void time_init(void);
uint64_t time_get_ms(void);
uint64_t time_get_us(void);
void time_sleep_ms(uint64_t ms);
void time_get_timespec(timespec_t *ts);

#endif /* GOS_TIME_H */
