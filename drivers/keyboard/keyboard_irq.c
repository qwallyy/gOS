/* gOS Kernel — Keyboard IRQ Handler (separate compilation unit)
 * Reference: https://wiki.osdev.org/PS/2_Keyboard
 */

#include "drivers/keyboard/keyboard.h"
#include "drivers/pic/pic.h"

void keyboard_handle_irq0(void *regs);

void keyboard_handle_irq0(void *regs) {
    keyboard_handle_irq(regs);
    pic_send_eoi(1);
}
