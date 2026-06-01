/* gOS Kernel — PS/2 Mouse Implementation
 * Reference: https://wiki.osdev.org/PS/2_Mouse
 */

#include "drivers/mouse/mouse.h"
#include "drivers/pic/pic.h"
#include "gOS/klog.h"

#define PS2_DATA_PORT   0x60
#define PS2_CMD_PORT    0x64
#define PS2_STATUS_PORT 0x64

#define PS2_STATUS_OUT  0x01
#define PS2_STATUS_IN   0x02

static uint8_t g_mouse_cycle = 0;
static int8_t  g_mouse_packet[4] = {0};
       mouse_state_t g_mouse_state = {0};

static void ps2_wait_input(void) {
    for (int i = 0; i < 100000; i++) {
        if (!(inb(PS2_STATUS_PORT) & PS2_STATUS_IN)) return;
        io_wait();
    }
}

static void ps2_wait_output(void) {
    for (int i = 0; i < 100000; i++) {
        if (inb(PS2_STATUS_PORT) & PS2_STATUS_OUT) return;
        io_wait();
    }
}

static uint8_t ps2_read_data(void) {
    ps2_wait_output();
    return inb(PS2_DATA_PORT);
}

static void ps2_write_data(uint8_t val) {
    ps2_wait_input();
    outb(PS2_DATA_PORT, val);
    io_wait();
}

static void ps2_write_cmd(uint8_t cmd) {
    ps2_wait_input();
    outb(PS2_CMD_PORT, cmd);
    io_wait();
}

void mouse_init(void) {
    klog_info("Initializing PS/2 mouse");

    /* Wait for any pending data */
    while (inb(PS2_STATUS_PORT) & PS2_STATUS_OUT) {
        (void)inb(PS2_DATA_PORT);
    }

    /* Tell the PS/2 controller we want to send a command to the mouse */
    ps2_write_cmd(0xA8); /* Enable second PS/2 port (mouse) */
    ps2_write_cmd(0x20); /* Read configuration byte */
    uint8_t status = ps2_read_data();
    status |= 0x02; /* Enable IRQ12 */
    ps2_write_cmd(0x60); /* Write configuration byte */
    ps2_write_data(status);

    /* Tell the controller we're sending a mouse command */
    ps2_write_cmd(0xD4);
    ps2_write_data(0xF6); /* Set default settings */
    ps2_read_data(); /* ACK */

    /* Enable packet streaming */
    ps2_write_cmd(0xD4);
    ps2_write_data(0xF4); /* Enable data reporting */
    ps2_read_data(); /* ACK */

    /* Enable mouse interrupt (IRQ12) */
    pic_unmask_irq(12);

    g_mouse_state.present = true;
    klog_info("Mouse initialized, IRQ12 unmasked");
}

void mouse_handle_irq(void *regs) {
    UNUSED(regs);

    uint8_t data = inb(PS2_DATA_PORT);

    switch (g_mouse_cycle) {
        case 0:
            if ((data & 0x08) == 0) {
                /* Not a valid first byte, ignore */
                return;
            }
            g_mouse_packet[0] = data;
            g_mouse_cycle = 1;
            break;
        case 1:
            g_mouse_packet[1] = data;
            g_mouse_cycle = 2;
            break;
        case 2:
            g_mouse_packet[2] = data;
            g_mouse_cycle = 0;

            /* Decode packet */
            g_mouse_state.buttons = g_mouse_packet[0] & 0x07;
            g_mouse_state.dx = g_mouse_packet[1];
            g_mouse_state.dy = g_mouse_packet[2];

            if (g_mouse_packet[0] & 0x10) {
                g_mouse_state.dx |= 0xFFFFFF00; /* Sign extend */
            }
            if (g_mouse_packet[0] & 0x20) {
                g_mouse_state.dy |= 0xFFFFFF00; /* Sign extend */
            }

            g_mouse_state.x += g_mouse_state.dx;
            g_mouse_state.y -= g_mouse_state.dy; /* Y is inverted */

            if (g_mouse_state.x < 0) g_mouse_state.x = 0;
            if (g_mouse_state.y < 0) g_mouse_state.y = 0;
            /* Clamp to screen (VGA text is 80x25, ~640x200 in pixels) */
            if (g_mouse_state.x > 639) g_mouse_state.x = 639;
            if (g_mouse_state.y > 199) g_mouse_state.y = 199;
            break;
    }
}

bool mouse_get_state(mouse_state_t *state) {
    if (!state || !g_mouse_state.present) return false;
    *state = g_mouse_state;
    return true;
}

bool mouse_is_present(void) {
    return g_mouse_state.present;
}
