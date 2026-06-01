/* gOS Kernel — TTY Abstraction Implementation */

#include "lib/tty.h"
#include "drivers/serial/serial.h"
#include "drivers/vga/vga.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

static tty_t g_ttys[TTY_MAX_TTYS];
static int g_active_tty = 0;

static void tty_default_write(const char *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        serial_putc(data[i]);
        vga_putchar(data[i]);
    }
}

static int tty_default_read(char *buf, size_t len) {
    (void)buf; (void)len;
    return 0; /* No blocking read in kernel stub */
}

void tty_init(void) {
    klog_info("TTY subsystem initialized");
    for (int i = 0; i < TTY_MAX_TTYS; i++) {
        g_ttys[i].id = i;
        g_ttys[i].active = (i == 0);
        g_ttys[i].write = tty_default_write;
        g_ttys[i].read = tty_default_read;
    }
    g_active_tty = 0;
}

void tty_set_active(int tty_id) {
    if (tty_id < 0 || tty_id >= TTY_MAX_TTYS) return;
    g_ttys[g_active_tty].active = false;
    g_active_tty = tty_id;
    g_ttys[g_active_tty].active = true;
    klog_info("TTY switched to %d", tty_id);
}

tty_t *tty_get_active(void) {
    return &g_ttys[g_active_tty];
}

void tty_write(const char *data, size_t len) {
    tty_t *tty = tty_get_active();
    if (tty && tty->write) {
        tty->write(data, len);
    }
}

void tty_puts(const char *str) {
    if (!str) return;
    tty_write(str, strlen(str));
}

int tty_read(char *buf, size_t len) {
    tty_t *tty = tty_get_active();
    if (tty && tty->read) {
        return tty->read(buf, len);
    }
    return -1;
}

void tty_switch(int tty_id) {
    tty_set_active(tty_id);
}
