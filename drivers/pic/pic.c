/* gOS Kernel — 8259 PIC Implementation
 * Reference: https://wiki.osdev.org/8259_PIC
 */

#include "drivers/pic/pic.h"
#include "gOS/klog.h"

/* Cascade IRQ (IRQ2 on PIC1 is wired to PIC2) */
#define PIC1_CASCADE_IRQ    2

void pic_init(void) {
    klog_info("Initializing 8259 PIC (remap IRQs to vectors 32-47)");

    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    /* Start initialization sequence (cascade mode) */
    outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();

    /* ICW2: Vector offsets */
    outb(PIC1_DATA, PIC1_OFFSET);
    io_wait();
    outb(PIC2_DATA, PIC2_OFFSET);
    io_wait();

    /* ICW3: Tell PIC1 that PIC2 is at IRQ2 (bit 2), tell PIC2 its cascade identity */
    outb(PIC1_DATA, 1 << PIC1_CASCADE_IRQ);
    io_wait();
    outb(PIC2_DATA, PIC1_CASCADE_IRQ);
    io_wait();

    /* ICW4: 8086 mode, auto EOI disabled, normal nested mode */
    outb(PIC1_DATA, PIC_ICW4_8086);
    io_wait();
    outb(PIC2_DATA, PIC_ICW4_8086);
    io_wait();

    /* Restore saved masks */
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

    klog_info("PIC remap complete: IRQ0-7 -> 0x%02x, IRQ8-15 -> 0x%02x",
              PIC1_OFFSET, PIC2_OFFSET);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_mask_irq(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t mask = inb(port);
    mask |= (1 << (irq & 7));
    outb(port, mask);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t mask = inb(port);
    mask &= ~(1 << (irq & 7));
    outb(port, mask);
}

void pic_disable(void) {
    /* Mask all IRQs on both PICs */
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
    klog_info("PIC disabled (all IRQs masked)");
}

uint16_t pic_get_irr(void) {
    outb(PIC1_COMMAND, PIC_READ_IRR);
    outb(PIC2_COMMAND, PIC_READ_IRR);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t pic_get_isr(void) {
    outb(PIC1_COMMAND, PIC_READ_ISR);
    outb(PIC2_COMMAND, PIC_READ_ISR);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}
