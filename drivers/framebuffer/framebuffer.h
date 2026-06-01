/* gOS Kernel — Linear Framebuffer Driver
 * Reference: https://wiki.osdev.org/Framebuffer
 */

#ifndef GOS_FRAMEBUFFER_H
#define GOS_FRAMEBUFFER_H

#include "gOS/types.h"

typedef struct framebuffer {
    uintptr_t addr;
    uint32_t  width;
    uint32_t  height;
    uint32_t  pitch;
    uint8_t   bpp;
    uint8_t   type; /* 0 = indexed, 1 = RGB, 2 = EGA text */
    uint8_t   red_pos;
    uint8_t   red_size;
    uint8_t   green_pos;
    uint8_t   green_size;
    uint8_t   blue_pos;
    uint8_t   blue_size;
} framebuffer_t;

void framebuffer_init(void);
void framebuffer_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void framebuffer_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void framebuffer_clear(uint32_t color);
void framebuffer_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);
void framebuffer_draw_string(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg);
bool framebuffer_is_available(void);

extern framebuffer_t g_framebuffer;

#endif /* GOS_FRAMEBUFFER_H */
