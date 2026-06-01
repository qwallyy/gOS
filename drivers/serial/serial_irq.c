/* gOS Kernel — Serial IRQ Handler (separate compilation unit)
 * Reference: https://wiki.osdev.org/Serial_Ports
 */

#include "drivers/serial/serial.h"
#include "drivers/pic/pic.h"

void serial_handle_irq0(void *regs);

void serial_handle_irq0(void *regs) {
    UNUSED(regs);
    serial_handle_irq(0);
    pic_send_eoi(4);
}
