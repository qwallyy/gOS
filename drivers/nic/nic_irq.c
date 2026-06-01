/* gOS Kernel — NIC IRQ Handler Stub
 * Reference: https://wiki.osdev.org/Network_Stack
 */

#include "drivers/nic/nic.h"
#include "drivers/pic/pic.h"
#include "gOS/klog.h"

void nic_handle_irq(void *regs);

void nic_handle_irq(void *regs) {
    UNUSED(regs);
    klog_debug("NIC IRQ fired");
    pic_send_eoi(11); /* NIC typically on IRQ11 */
}
