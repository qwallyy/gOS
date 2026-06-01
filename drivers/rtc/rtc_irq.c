/* gOS Kernel — RTC IRQ Handler (separate compilation unit)
 * Reference: https://wiki.osdev.org/RTC
 */

#include "drivers/rtc/rtc.h"
#include "drivers/pic/pic.h"

void rtc_handle_irq0(void *regs);

void rtc_handle_irq0(void *regs) {
    rtc_handle_irq(regs);
    pic_send_eoi(8);
}
