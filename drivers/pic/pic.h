/* gOS Kernel — 8259 Programmable Interrupt Controller (PIC)
 * Reference: https://wiki.osdev.org/8259_PIC
 *
 * The legacy PIC must be remapped so IRQs don't overlap with CPU exceptions
 * (0-31). We remap IRQ0-7 to vectors 32-39 and IRQ8-15 to vectors 40-47.
 */

#ifndef GOS_PIC_H
#define GOS_PIC_H

#include "gOS/types.h"

/* PIC I/O ports */
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

/* ICW1 bits */
#define PIC_ICW1_ICW4       0x01
#define PIC_ICW1_SINGLE     0x02
#define PIC_ICW1_INTERVAL4  0x04
#define PIC_ICW1_LEVEL      0x08
#define PIC_ICW1_INIT       0x10

/* ICW4 bits */
#define PIC_ICW4_8086       0x01
#define PIC_ICW4_AUTO       0x02
#define PIC_ICW4_BUF_SLAVE  0x08
#define PIC_ICW4_BUF_MASTER 0x0C
#define PIC_ICW4_SFNM       0x10

/* PIC remap offsets */
#define PIC1_OFFSET     32
#define PIC2_OFFSET     40

/* OCW3 bits (read ISR/IRR) */
#define PIC_READ_IRR    0x0A
#define PIC_READ_ISR    0x0B

/* End-of-Interrupt command */
#define PIC_EOI         0x20

/* Initialize and remap the PIC */
void pic_init(void);

/* Send End-of-Interrupt to the appropriate PIC */
void pic_send_eoi(uint8_t irq);

/* Set/clear IRQ mask */
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);

/* Disable the PIC entirely (when using APIC/IOAPIC) */
void pic_disable(void);

/* Read IRR or ISR register from a PIC */
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

/* IRQ-to-vector conversion */
static inline uint8_t pic_irq_to_vector(uint8_t irq) {
    return (irq < 8) ? (PIC1_OFFSET + irq) : (PIC2_OFFSET + irq);
}

#endif /* GOS_PIC_H */
