/* gOS Kernel — PS/2 Keyboard Driver
 * Reference: https://wiki.osdev.org/PS/2_Keyboard
 */

#ifndef GOS_KEYBOARD_H
#define GOS_KEYBOARD_H

#include "gOS/types.h"

#define KEYBOARD_BUFFER_SIZE    256

void keyboard_init(void);
void keyboard_handle_irq(void *regs);
bool keyboard_has_input(void);
char keyboard_get_char(void);
uint8_t keyboard_get_scancode(void);
bool keyboard_is_key_pressed(uint8_t scancode);

#endif /* GOS_KEYBOARD_H */
