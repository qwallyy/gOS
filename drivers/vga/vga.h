/* gOS Kernel — VGA Text Mode Terminal
 * Reference: https://wiki.osdev.org/VGA_Hardware
 *            https://wiki.osdev.org/Printing_To_Screen
 */

#ifndef GOS_VGA_H
#define GOS_VGA_H

#include "gOS/types.h"

/* VGA text mode memory address */
#define VGA_MEMORY      ((volatile uint16_t *)0xFFFFFFFF800B8000ULL)
#define VGA_PORT_CTRL   0x3D4
#define VGA_PORT_DATA   0x3D5

/* Screen dimensions (standard 80x25 text mode) */
#define VGA_WIDTH       80
#define VGA_HEIGHT      25

/* VGA color attributes */
#define VGA_BLACK       0
#define VGA_BLUE        1
#define VGA_GREEN       2
#define VGA_CYAN        3
#define VGA_RED         4
#define VGA_MAGENTA     5
#define VGA_BROWN       6
#define VGA_LIGHT_GREY  7
#define VGA_DARK_GREY   8
#define VGA_LIGHT_BLUE  9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN  11
#define VGA_LIGHT_RED   12
#define VGA_LIGHT_MAGENTA 13
#define VGA_LIGHT_BROWN 14
#define VGA_WHITE       15

/* Default terminal colors */
#define VGA_DEFAULT_FG  VGA_LIGHT_GREY
#define VGA_DEFAULT_BG  VGA_BLACK

void vga_init(void);
void vga_clear(void);
void vga_putchar(char c);
void vga_puts(const char *str);
void vga_puthex(uint64_t val);
void vga_putdec(uint64_t val);
void vga_set_color(uint8_t fg, uint8_t bg);
void vga_set_cursor(size_t x, size_t y);
void vga_get_cursor(size_t *x, size_t *y);
void vga_scroll(void);
void vga_move_cursor(void);

#endif /* GOS_VGA_H */
