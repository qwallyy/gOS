/* gOS Kernel — PS/2 Controller Implementation
 * Reference: https://wiki.osdev.org/%228042%22_PS/2_Controller
 */

#include "drivers/ps2/ps2.h"
#include "gOS/klog.h"

static bool g_ps2_dual_channel = false;

void ps2_init(void) {
    klog_info("PS/2 controller initializing");

    /* Step 1: Disable devices */
    ps2_send_command(PS2_CMD_DISABLE_P1);
    ps2_send_command(PS2_CMD_DISABLE_P2);

    /* Step 2: Flush output buffer */
    while (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTBUF) {
        (void)inb(PS2_DATA_PORT);
    }

    /* Step 3: Read configuration byte */
    ps2_send_command(PS2_CMD_READ_CFG);
    uint8_t config = ps2_read_data();

    /* Step 4: Check for second port */
    g_ps2_dual_channel = (config & PS2_CFG_DISABLE_P2) != 0;

    /* Step 5: Disable IRQs and translation */
    config &= ~(PS2_CFG_IRQ1 | PS2_CFG_IRQ2 | PS2_CFG_XLAT);
    ps2_send_command(PS2_CMD_WRITE_CFG);
    ps2_write_data(config);

    /* Step 6: Controller self-test */
    if (!ps2_self_test()) {
        klog_err("PS/2 controller self-test failed");
        return;
    }

    /* Step 7: Enable devices */
    ps2_send_command(PS2_CMD_ENABLE_P1);
    if (g_ps2_dual_channel) {
        ps2_send_command(PS2_CMD_ENABLE_P2);
    }

    /* Step 8: Enable IRQs in config */
    config = ps2_read_config();
    config |= PS2_CFG_IRQ1;
    if (g_ps2_dual_channel) config |= PS2_CFG_IRQ2;
    ps2_write_config(config);

    klog_info("PS/2 controller initialized (dual_channel=%d)", g_ps2_dual_channel);
}

bool ps2_self_test(void) {
    ps2_send_command(PS2_CMD_TEST_CTRL);
    uint8_t result = ps2_read_data();
    return result == 0x55;
}

bool ps2_has_second_port(void) {
    return g_ps2_dual_channel;
}

uint8_t ps2_read_data(void) {
    for (int i = 0; i < 100000; i++) {
        if (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTBUF) {
            return inb(PS2_DATA_PORT);
        }
        io_wait();
    }
    return 0;
}

void ps2_write_data(uint8_t val) {
    for (int i = 0; i < 100000; i++) {
        if (!(inb(PS2_STATUS_PORT) & PS2_STATUS_INBUF)) {
            outb(PS2_DATA_PORT, val);
            io_wait();
            return;
        }
        io_wait();
    }
}

void ps2_send_command(uint8_t cmd) {
    for (int i = 0; i < 100000; i++) {
        if (!(inb(PS2_STATUS_PORT) & PS2_STATUS_INBUF)) {
            outb(PS2_COMMAND_PORT, cmd);
            io_wait();
            return;
        }
        io_wait();
    }
}

uint8_t ps2_read_config(void) {
    ps2_send_command(PS2_CMD_READ_CFG);
    return ps2_read_data();
}

void ps2_write_config(uint8_t cfg) {
    ps2_send_command(PS2_CMD_WRITE_CFG);
    ps2_write_data(cfg);
}
