/* gOS Kernel — VGA Palette Implementation
 * Reference: https://wiki.osdev.org/VGA_Hardware
 */

#include "drivers/vga/vga_palette.h"
#include "gOS/klog.h"

/* Default 16-color EGA palette (6-bit DAC values) */
static const vga_color_t g_default_palette[16] = {
    {0, 0, 0},       /* Black */
    {0, 0, 42},      /* Blue */
    {0, 42, 0},      /* Green */
    {0, 42, 42},     /* Cyan */
    {42, 0, 0},      /* Red */
    {42, 0, 42},     /* Magenta */
    {42, 21, 0},     /* Brown */
    {42, 42, 42},    /* Light Gray */
    {21, 21, 21},    /* Dark Gray */
    {21, 21, 63},    /* Light Blue */
    {21, 63, 21},    /* Light Green */
    {21, 63, 63},    /* Light Cyan */
    {63, 21, 21},    /* Light Red */
    {63, 21, 63},    /* Light Magenta */
    {63, 63, 21},    /* Yellow */
    {63, 63, 63},    /* White */
};

void vga_set_palette_entry(uint8_t index, const vga_color_t *color) {
    if (!color) return;
    outb(VGA_DAC_MASK, 0xFF);
    outb(VGA_DAC_WRITE, index);
    outb(VGA_DAC_DATA, color->r);
    outb(VGA_DAC_DATA, color->g);
    outb(VGA_DAC_DATA, color->b);
}

void vga_get_palette_entry(uint8_t index, vga_color_t *color) {
    if (!color) return;
    outb(VGA_DAC_READ, index);
    color->r = inb(VGA_DAC_DATA);
    color->g = inb(VGA_DAC_DATA);
    color->b = inb(VGA_DAC_DATA);
}

void vga_set_palette(const vga_color_t *palette, uint8_t start, uint8_t count) {
    if (!palette) return;
    for (uint8_t i = 0; i < count; i++) {
        vga_set_palette_entry(start + i, &palette[i]);
    }
}

void vga_load_default_palette(void) {
    klog_info("VGA: loading default 16-color EGA palette");
    vga_set_palette(g_default_palette, 0, 16);
}
