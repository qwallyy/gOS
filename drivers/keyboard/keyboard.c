/* gOS Kernel — PS/2 Keyboard Implementation
 * Reference: https://wiki.osdev.org/PS/2_Keyboard
 */

#include "drivers/keyboard/keyboard.h"
#include "drivers/pic/pic.h"
#include "gOS/klog.h"

#define PS2_DATA_PORT       0x60
#define PS2_STATUS_PORT     0x64
#define PS2_COMMAND_PORT    0x64

#define PS2_STATUS_OUTBUF   0x01
#define PS2_STATUS_INBUF    0x02

/* Scancode set 1 US layout translation table (non-shifted) */
static const char g_scancode_to_ascii[128] = {
    0,  0, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0,
    /* F1-F10 */ 0,0,0,0,0,0,0,0,0,0,
    /* NumLock etc */ 0,0,0,0,0,0,0,0,0,0,
    /* Home, Up, etc */ 0,0,0,0,0,0,0,0,0,0,0,0,
};

static volatile uint8_t g_key_buffer[KEYBOARD_BUFFER_SIZE];
static volatile size_t g_key_head = 0;
static volatile size_t g_key_tail = 0;
static volatile bool g_key_shift = false;
static volatile bool g_key_ctrl = false;
static volatile bool g_key_alt = false;

static bool __attribute__((used)) ps2_wait_output(void) {
    for (int i = 0; i < 100000; i++) {
        if (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTBUF) {
            return true;
        }
    }
    return false;
}

void keyboard_init(void) {
    klog_info("Initializing PS/2 keyboard");

    /* Wait for output buffer, then read any pending data */
    while (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTBUF) {
        (void)inb(PS2_DATA_PORT);
    }

    /* Enable keyboard interrupt (IRQ1) */
    pic_unmask_irq(1);

    klog_info("Keyboard initialized, IRQ1 unmasked");
}

void keyboard_handle_irq(void *regs) {
    UNUSED(regs);

    uint8_t scancode = inb(PS2_DATA_PORT);

    /* Break code (key release): bit 7 set in scancode set 1 */
    if (scancode & 0x80) {
        uint8_t key = scancode & 0x7F;
        if (key == 0x2A || key == 0x36) {
            g_key_shift = false;
        }
        if (key == 0x1D) {
            g_key_ctrl = false;
        }
        if (key == 0x38) {
            g_key_alt = false;
        }
        return;
    }

    /* Make code (key press) */
    if (scancode == 0x2A || scancode == 0x36) {
        g_key_shift = true;
        return;
    }
    if (scancode == 0x1D) {
        g_key_ctrl = true;
        return;
    }
    if (scancode == 0x38) {
        g_key_alt = true;
        return;
    }

    /* Translate to ASCII and buffer */
    if (scancode < 128) {
        char c = g_scancode_to_ascii[scancode];
        if (c) {
            if (g_key_shift) {
                /* Simple shift conversion for letters */
                if (c >= 'a' && c <= 'z') {
                    c = c - 'a' + 'A';
                } else if (c >= '0' && c <= '9') {
                    const char shifted[] = ")!@#$%^&*(";
                    c = shifted[c - '0'];
                }
            }
            if (g_key_ctrl && c >= 'a' && c <= 'z') {
                c = c - 'a' + 1; /* Control characters */
            }

            size_t next = (g_key_head + 1) % KEYBOARD_BUFFER_SIZE;
            if (next != g_key_tail) {
                g_key_buffer[g_key_head] = (uint8_t)c;
                g_key_head = next;
            }
        }
    }
}

bool keyboard_has_input(void) {
    return g_key_head != g_key_tail;
}

char keyboard_get_char(void) {
    while (g_key_head == g_key_tail) {
        hlt();
    }
    char c = (char)g_key_buffer[g_key_tail];
    g_key_tail = (g_key_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

uint8_t keyboard_get_scancode(void) {
    while (g_key_head == g_key_tail) {
        hlt();
    }
    uint8_t sc = g_key_buffer[g_key_tail];
    g_key_tail = (g_key_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return sc;
}

bool keyboard_is_key_pressed(uint8_t scancode) {
    (void)scancode;
    return false; /* Full key state tracking not implemented in minimal version */
}
