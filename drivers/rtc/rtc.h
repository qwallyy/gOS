/* gOS Kernel — Real-Time Clock (RTC)
 * Reference: https://wiki.osdev.org/RTC
 */

#ifndef GOS_RTC_H
#define GOS_RTC_H

#include "gOS/types.h"

typedef struct rtc_time {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t century;
} rtc_time_t;

void rtc_init(void);
void rtc_read_time(rtc_time_t *time);
uint64_t rtc_get_timestamp(void);
void rtc_handle_irq(void *regs);

#endif /* GOS_RTC_H */
