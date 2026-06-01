/* gOS Kernel — VGA Text Mode Terminal Implementation
 * Reference: https://wiki.osdev.org/VGA_Hardware
 */

#include "drivers/vga/vga.h"
#include "gOS/klog.h"
#include "lib/string.h"

static size_t g_vga_row = 0;
static size_t g_vga_col = 0;
static uint8_t g_vga_color = (VGA_DEFAULT_BG << 4) | VGA_DEFAULT_FG;

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void vga_init(void) {
    klog_info("Initializing VGA text mode terminal");
    g_vga_row = 0;
    g_vga_col = 0;
    g_vga_color = (VGA_DEFAULT_BG << 4) | VGA_DEFAULT_FG;
    vga_clear();
    vga_move_cursor();
    klog_info("VGA terminal ready (%dx%d)", VGA_WIDTH, VGA_HEIGHT);
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', g_vga_color);
        }
    }
    g_vga_row = 0;
    g_vga_col = 0;
    vga_move_cursor();
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    g_vga_color = (bg << 4) | (fg & 0x0F);
}

void vga_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', g_vga_color);
    }
    g_vga_row = VGA_HEIGHT - 1;
}

void vga_move_cursor(void) {
    uint16_t pos = (uint16_t)(g_vga_row * VGA_WIDTH + g_vga_col);
    outb(VGA_PORT_CTRL, 0x0F);
    outb(VGA_PORT_DATA, (uint8_t)(pos & 0xFF));
    outb(VGA_PORT_CTRL, 0x0E);
    outb(VGA_PORT_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_set_cursor(size_t x, size_t y) {
    g_vga_col = x;
    g_vga_row = y;
    vga_move_cursor();
}

void vga_get_cursor(size_t *x, size_t *y) {
    if (x) *x = g_vga_col;
    if (y) *y = g_vga_row;
}

void vga_putchar(char c) {
    if (c == '\n') {
        g_vga_col = 0;
        g_vga_row++;
        if (g_vga_row >= VGA_HEIGHT) {
            vga_scroll();
        }
        vga_move_cursor();
        return;
    }
    if (c == '\r') {
        g_vga_col = 0;
        vga_move_cursor();
        return;
    }
    if (c == '\t') {
        size_t next_tab = (g_vga_col + 8) & ~7;
        while (g_vga_col < next_tab && g_vga_col < VGA_WIDTH) {
            VGA_MEMORY[g_vga_row * VGA_WIDTH + g_vga_col] = vga_entry(' ', g_vga_color);
            g_vga_col++;
        }
        vga_move_cursor();
        return;
    }
    if (c == '\b') {
        if (g_vga_col > 0) {
            g_vga_col--;
            VGA_MEMORY[g_vga_row * VGA_WIDTH + g_vga_col] = vga_entry(' ', g_vga_color);
        }
        vga_move_cursor();
        return;
    }

    if (g_vga_col >= VGA_WIDTH) {
        g_vga_col = 0;
        g_vga_row++;
        if (g_vga_row >= VGA_HEIGHT) {
            vga_scroll();
        }
    }

    VGA_MEMORY[g_vga_row * VGA_WIDTH + g_vga_col] = vga_entry(c, g_vga_color);
    g_vga_col++;
    vga_move_cursor();
}

void vga_puts(const char *str) {
    if (!str) return;
    while (*str) {
        vga_putchar(*str++);
    }
}

void vga_puthex(uint64_t val) {
    vga_puts("0x");
    for (int i = 15; i >= 0; i--) {
        uint8_t nibble = (val >> (i * 4)) & 0xF;
        vga_putchar((nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10));
    }
}

void vga_putdec(uint64_t val) {
    if (val == 0) {
        vga_putchar('0');
        return;
    }
    char buf[32];
    int i = 0;
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (i > 0) {
        vga_putchar(buf[--i]);
    }
}
