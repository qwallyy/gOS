/* gOS Kernel — PIT IRQ Handler (separate compilation unit)
 * Reference: https://wiki.osdev.org/Programmable_Interval_Timer
 */

#include "drivers/pit/pit.h"
#include "drivers/pic/pic.h"
#include "gOS/klog.h"
#include "proc/scheduler.h"

void pit_handle_irq0(void *regs);

void pit_handle_irq0(void *regs) {
    pit_handle_tick(regs);
    pic_send_eoi(0);
}
