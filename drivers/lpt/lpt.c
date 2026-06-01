/* gOS Kernel — Parallel Port Implementation
 * Reference: https://wiki.osdev.org/Parallel_port
 */

#include "drivers/lpt/lpt.h"
#include "gOS/klog.h"

void lpt_init(void) {
    klog_info("Parallel port driver initialized (stub)");
}

bool lpt_detect(uint16_t base) {
    /* Write test pattern and read back */
    outb(base + LPT_DATA, 0x55);
    io_wait();
    uint8_t val = inb(base + LPT_DATA);
    if (val == 0x55) {
        klog_info("LPT detected at 0x%04x", base);
        return true;
    }
    return false;
}

bool lpt_write_byte(uint16_t base, uint8_t data) {
    for (int i = 0; i < 100000; i++) {
        uint8_t status = inb(base + LPT_STATUS);
        if (!(status & LPT_STATUS_BUSY)) {
            outb(base + LPT_DATA, data);
            /* Strobe pulse */
            outb(base + LPT_CONTROL, 0x0D);
            io_wait();
            outb(base + LPT_CONTROL, 0x0C);
            return true;
        }
        io_wait();
    }
    return false;
}

bool lpt_write_data(uint16_t base, const void *data, size_t len) {
    const uint8_t *bytes = data;
    for (size_t i = 0; i < len; i++) {
        if (!lpt_write_byte(base, bytes[i])) return false;
    }
    return true;
}
