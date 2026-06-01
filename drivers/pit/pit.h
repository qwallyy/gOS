/* gOS Kernel — Programmable Interval Timer (PIT)
 * Reference: https://wiki.osdev.org/Programmable_Interval_Timer
 */

#ifndef GOS_PIT_H
#define GOS_PIT_H

#include "gOS/types.h"

/* PIT I/O ports */
#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42
#define PIT_COMMAND     0x43

/* PIT command byte */
#define PIT_SELECT_CH0  0x00
#define PIT_SELECT_CH1  0x40
#define PIT_SELECT_CH2  0x80
#define PIT_SELECT_READBACK 0xC0
#define PIT_ACCESS_LATCH  0x00
#define PIT_ACCESS_LOBYTE 0x10
#define PIT_ACCESS_HIBYTE 0x20
#define PIT_ACCESS_LOHI   0x30
#define PIT_MODE_ONESHOT  0x02
#define PIT_MODE_RATEGEN  0x04
#define PIT_MODE_SQUARE   0x06
#define PIT_MODE_SWSTROBE 0x08
#define PIT_MODE_HWSTROBE 0x0A
#define PIT_MODE_RATEGEN2 0x0C
#define PIT_MODE_SQUARE2  0x0E
#define PIT_BCD_BINARY    0x00
#define PIT_BCD_BCD       0x01

/* PIT oscillator frequency (fixed) */
#define PIT_FREQUENCY     1193182

/* Target PIT frequency (100 Hz = 10ms tick) */
#define PIT_HZ            100

/* System tick counter (incremented by IRQ0) */
extern volatile uint64_t g_pit_ticks;
extern uint32_t g_pit_hz;

void pit_init(void);
void pit_set_frequency(uint32_t hz);
void pit_handle_tick(void *regs);
void pit_sleep_ms(uint32_t ms);
uint64_t pit_get_ticks(void);
void pit_wait(uint32_t ms);

#endif /* GOS_PIT_H */
