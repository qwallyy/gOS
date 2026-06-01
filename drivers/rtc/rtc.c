/* gOS Kernel — RTC Implementation
 * Reference: https://wiki.osdev.org/RTC
 */

#include "drivers/rtc/rtc.h"
#include "drivers/pic/pic.h"
#include "gOS/klog.h"

#define RTC_INDEX_PORT  0x70
#define RTC_DATA_PORT   0x71

#define RTC_SECOND      0x00
#define RTC_MINUTE      0x02
#define RTC_HOUR        0x04
#define RTC_DAY         0x07
#define RTC_MONTH       0x08
#define RTC_YEAR        0x09
#define RTC_CENTURY     0x32
#define RTC_STATUS_A    0x0A
#define RTC_STATUS_B    0x0B
#define RTC_STATUS_C    0x0C

#define RTC_STATUS_B_24HOUR  0x02
#define RTC_STATUS_B_BINARY  0x04
#define RTC_STATUS_B_PIE     0x40  /* Periodic interrupt enable */

static uint8_t rtc_read_reg(uint8_t reg) {
    outb(RTC_INDEX_PORT, reg);
    io_wait();
    return inb(RTC_DATA_PORT);
}

static void __attribute__((used)) rtc_write_reg(uint8_t reg, uint8_t val) {
    outb(RTC_INDEX_PORT, reg);
    io_wait();
    outb(RTC_DATA_PORT, val);
}

static uint8_t bcd_to_binary(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rtc_init(void) {
    klog_info("Initializing RTC");

    /* Read status B to determine format */
    uint8_t status_b = rtc_read_reg(RTC_STATUS_B);

    /* Enable update-ended interrupt if needed (optional) */
    (void)status_b;

    /* Disable NMI while reading registers */
    uint8_t century = rtc_read_reg(RTC_CENTURY);

    rtc_time_t t;
    rtc_read_time(&t);
    klog_info("RTC time: %04u-%02u-%02u %02u:%02u:%02u",
              t.year, t.month, t.day, t.hour, t.minute, t.second);

    (void)century;
    pic_unmask_irq(8); /* RTC uses IRQ8 */
}

void rtc_read_time(rtc_time_t *time) {
    if (!time) return;

    uint8_t status_b = rtc_read_reg(RTC_STATUS_B);
    bool is_binary = (status_b & RTC_STATUS_B_BINARY) != 0;
    bool is_24h    = (status_b & RTC_STATUS_B_24HOUR) != 0;

    /* Wait for update not in progress */
    while (rtc_read_reg(RTC_STATUS_A) & 0x80) {
        io_wait();
    }

    uint8_t second = rtc_read_reg(RTC_SECOND);
    uint8_t minute = rtc_read_reg(RTC_MINUTE);
    uint8_t hour   = rtc_read_reg(RTC_HOUR);
    uint8_t day    = rtc_read_reg(RTC_DAY);
    uint8_t month  = rtc_read_reg(RTC_MONTH);
    uint8_t year   = rtc_read_reg(RTC_YEAR);

    if (!is_binary) {
        second = bcd_to_binary(second);
        minute = bcd_to_binary(minute);
        hour   = bcd_to_binary(hour);
        day    = bcd_to_binary(day);
        month  = bcd_to_binary(month);
        year   = bcd_to_binary(year);
    }

    if (!is_24h && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    time->second = second;
    time->minute = minute;
    time->hour = hour;
    time->day = day;
    time->month = month;
    time->year = 2000 + year; /* Assume 21st century */
    time->century = 20;
}

uint64_t rtc_get_timestamp(void) {
    rtc_time_t t;
    rtc_read_time(&t);
    /* Simple timestamp: seconds since year 2000 (approximate) */
    uint64_t days = (t.year - 2000) * 365 + (t.year - 2000) / 4;
    static const uint8_t days_in_month[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    for (int m = 1; m < t.month; m++) {
        days += days_in_month[m];
    }
    days += t.day - 1;
    return days * 86400ULL + t.hour * 3600ULL + t.minute * 60ULL + t.second;
}

void rtc_handle_irq(void *regs) {
    UNUSED(regs);
    /* Read status C to acknowledge the interrupt */
    rtc_read_reg(RTC_STATUS_C);
}
