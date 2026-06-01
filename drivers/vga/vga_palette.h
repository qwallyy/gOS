/* gOS Kernel — VGA Palette Operations
 * Reference: https://wiki.osdev.org/VGA_Hardware
 */

#ifndef GOS_VGA_PALETTE_H
#define GOS_VGA_PALETTE_H

#include "gOS/types.h"

#define VGA_DAC_WRITE       0x3C8
#define VGA_DAC_READ        0x3C7
#define VGA_DAC_DATA        0x3C9
#define VGA_DAC_MASK        0x3C6

#define VGA_PALETTE_SIZE    256

typedef struct vga_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} vga_color_t;

void vga_set_palette_entry(uint8_t index, const vga_color_t *color);
void vga_get_palette_entry(uint8_t index, vga_color_t *color);
void vga_set_palette(const vga_color_t *palette, uint8_t start, uint8_t count);
void vga_load_default_palette(void);

#endif /* GOS_VGA_PALETTE_H */
