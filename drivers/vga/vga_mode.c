/* gOS Kernel — VGA Mode Switching Implementation
 * Reference: https://wiki.osdev.org/VGA_Hardware
 */

#include "drivers/vga/vga_mode.h"
#include "gOS/klog.h"

static vga_mode_t g_current_mode = {80, 25, 16, 0xB8000, true};

void vga_set_mode(uint8_t mode) {
    (void)mode;
    klog_warn("vga_set_mode: INT 0x10 not available in long mode, use VBE or framebuffer");
}

bool vga_set_text_mode(void) {
    klog_warn("vga_set_text_mode: requires BIOS, not available in long mode");
    g_current_mode.width = 80;
    g_current_mode.height = 25;
    g_current_mode.bpp = 16;
    g_current_mode.framebuffer = 0xB8000;
    g_current_mode.text_mode = true;
    return true;
}

bool vga_set_graphics_mode(uint16_t width, uint16_t height, uint8_t bpp) {
    (void)width; (void)height; (void)bpp;
    klog_warn("vga_set_graphics_mode: requires VBE or direct framebuffer");
    return false;
}

void vga_get_mode(vga_mode_t *mode) {
    if (mode) *mode = g_current_mode;
}
