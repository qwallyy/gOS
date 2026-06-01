/* gOS Kernel — PIC IRQ Handlers
 * Reference: https://wiki.osdev.org/8259_PIC
 */

#include "drivers/pic/pic.h"
#include "drivers/pit/pit.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/serial/serial.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

/* Spurious IRQ detection */
#define PIC1_SPURIOUS_VECTOR  0x07
#define PIC2_SPURIOUS_VECTOR  0x0F

void pic_handle_irq0(void *regs) {
    /* Timer tick handled by PIT driver */
    pit_handle_tick(regs);
    pic_send_eoi(0);
}

void pic_handle_irq1(void *regs) {
    /* Keyboard interrupt */
    keyboard_handle_irq(regs);
    pic_send_eoi(1);
}

void pic_handle_irq3(void *regs) {
    UNUSED(regs);
    /* COM2 / COM4 serial */
    serial_handle_irq(2);
    pic_send_eoi(3);
}

void pic_handle_irq4(void *regs) {
    UNUSED(regs);
    /* COM1 / COM3 serial */
    serial_handle_irq(0);
    pic_send_eoi(4);
}

void pic_handle_irq7(void *regs) {
    UNUSED(regs);
    /* Spurious IRQ7 check */
    uint16_t isr = pic_get_isr();
    if (!(isr & 0x80)) {
        klog_warn("Spurious IRQ7 detected");
        return;
    }
    pic_send_eoi(7);
}

void pic_handle_irq15(void *regs) {
    UNUSED(regs);
    /* Spurious IRQ15 check */
    uint16_t isr = pic_get_isr();
    if (!(isr & 0x8000)) {
        klog_warn("Spurious IRQ15 detected");
        /* Must still EOI PIC1 for cascade */
        outb(PIC1_COMMAND, PIC_EOI);
        return;
    }
    pic_send_eoi(15);
}

/* Generic IRQ handler registration */
void pic_register_default_handlers(void) {
    extern void irq_register_handler(uint8_t irq, void (*handler)(void*));
    irq_register_handler(0, pic_handle_irq0);
    irq_register_handler(1, pic_handle_irq1);
    irq_register_handler(3, pic_handle_irq3);
    irq_register_handler(4, pic_handle_irq4);
    irq_register_handler(7, pic_handle_irq7);
    irq_register_handler(15, pic_handle_irq15);
    klog_info("Default PIC IRQ handlers registered");
}
