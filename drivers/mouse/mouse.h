/* gOS Kernel — PS/2 Mouse Driver
 * Reference: https://wiki.osdev.org/PS/2_Mouse
 */

#ifndef GOS_MOUSE_H
#define GOS_MOUSE_H

#include "gOS/types.h"

typedef struct mouse_state {
    int32_t x;
    int32_t y;
    int8_t  dx;
    int8_t  dy;
    uint8_t buttons;
    bool    present;
} mouse_state_t;

#define MOUSE_BTN_LEFT      0x01
#define MOUSE_BTN_RIGHT     0x02
#define MOUSE_BTN_MIDDLE    0x04

void mouse_init(void);
void mouse_handle_irq(void *regs);
bool mouse_get_state(mouse_state_t *state);
bool mouse_is_present(void);

extern mouse_state_t g_mouse_state;

#endif /* GOS_MOUSE_H */
