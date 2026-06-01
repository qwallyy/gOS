/* gOS Kernel — VGA Mode Switching
 * Reference: https://wiki.osdev.org/VGA_Hardware
 */

#ifndef GOS_VGA_MODE_H
#define GOS_VGA_MODE_H

#include "gOS/types.h"

typedef struct vga_mode {
    uint16_t width;
    uint16_t height;
    uint8_t  bpp;
    uint32_t framebuffer;
    bool     text_mode;
} vga_mode_t;

#define VGA_MODE_TEXT_80x25     0x03
#define VGA_MODE_TEXT_80x50     0x55
#define VGA_MODE_GRAPH_320x200  0x13
#define VGA_MODE_GRAPH_640x480  0x12

void vga_set_mode(uint8_t mode);
bool vga_set_text_mode(void);
bool vga_set_graphics_mode(uint16_t width, uint16_t height, uint8_t bpp);
void vga_get_mode(vga_mode_t *mode);

#endif /* GOS_VGA_MODE_H */
