/* gOS Kernel — Local APIC (LAPIC) and I/O APIC
 * Reference: https://wiki.osdev.org/APIC
 *            https://wiki.osdev.org/IOAPIC
 */

#ifndef GOS_APIC_H
#define GOS_APIC_H

#include "gOS/types.h"

/* LAPIC register offsets (from base, MMIO) */
#define LAPIC_ID        0x020
#define LAPIC_VERSION   0x030
#define LAPIC_TPR       0x080
#define LAPIC_APR       0x090
#define LAPIC_PPR       0x0A0
#define LAPIC_EOI       0x0B0
#define LAPIC_RRD       0x0C0
#define LAPIC_LDR       0x0D0
#define LAPIC_DFR       0x0E0
#define LAPIC_SIVR      0x0F0
#define LAPIC_ISR_BASE  0x100
#define LAPIC_TMR_BASE  0x180
#define LAPIC_IRR_BASE  0x200
#define LAPIC_ESR       0x280
#define LAPIC_ICR_LO    0x300
#define LAPIC_ICR_HI    0x310
#define LAPIC_LVT_TIMER 0x320
#define LAPIC_LVT_THERM 0x330
#define LAPIC_LVT_PERF  0x340
#define LAPIC_LVT_LINT0 0x350
#define LAPIC_LVT_LINT1 0x360
#define LAPIC_LVT_ERR   0x370
#define LAPIC_INIT_CNT  0x380
#define LAPIC_CURR_CNT  0x390
#define LAPIC_DIV_CFG   0x3E0

/* IOAPIC register offsets */
#define IOAPIC_ID       0x00
#define IOAPIC_VERSION  0x01
#define IOAPIC_ARB      0x02
#define IOAPIC_REDTBL   0x10

/* LVT flags */
#define LAPIC_LVT_MASKED    (1U << 16)
#define LAPIC_LVT_PERIODIC  (1U << 17)
#define LAPIC_LVT_TSC_DEAD  (1U << 18)

/* SIVR flags */
#define LAPIC_SVR_ENABLE    (1U << 8)

void lapic_init(void);
void lapic_eoi(void);
uint32_t lapic_read(uint32_t reg);
void lapic_write(uint32_t reg, uint32_t val);
uint32_t lapic_get_id(void);
void lapic_send_ipi(uint32_t dest, uint32_t vector);
void lapic_timer_calibrate(void);
void lapic_timer_start(uint32_t vector, uint32_t period_us);
void lapic_timer_stop(void);

void ioapic_init(void);
void ioapic_set_irq(uint8_t irq, uint8_t vector, uint32_t dest_apic_id);
uint32_t ioapic_read(uint32_t reg);
void ioapic_write(uint32_t reg, uint32_t val);

#endif /* GOS_APIC_H */
