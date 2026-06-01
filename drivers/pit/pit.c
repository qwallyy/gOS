/* gOS Kernel — PIT Implementation
 * Reference: https://wiki.osdev.org/Programmable_Interval_Timer
 */

#include "drivers/pit/pit.h"
#include "gOS/klog.h"
#include "proc/proc.h"
#include "proc/scheduler.h"

volatile uint64_t g_pit_ticks = 0;
uint32_t g_pit_hz = PIT_HZ;
static uint32_t g_pit_divisor = 0;

void pit_init(void) {
    klog_info("Initializing PIT at %d Hz", PIT_HZ);
    pit_set_frequency(PIT_HZ);
    klog_info("PIT initialized, divisor=%u", g_pit_divisor);
}

void pit_set_frequency(uint32_t hz) {
    if (hz == 0) hz = 18; /* minimum ~18.2 Hz */
    g_pit_divisor = PIT_FREQUENCY / hz;
    if (g_pit_divisor > 65535) g_pit_divisor = 0; /* 0 = 65536 in PIT */

    uint8_t cmd = PIT_SELECT_CH0 | PIT_ACCESS_LOHI | PIT_MODE_RATEGEN | PIT_BCD_BINARY;
    outb(PIT_COMMAND, cmd);
    io_wait();
    outb(PIT_CHANNEL0, (uint8_t)(g_pit_divisor & 0xFF));
    io_wait();
    outb(PIT_CHANNEL0, (uint8_t)((g_pit_divisor >> 8) & 0xFF));
}

void pit_handle_tick(void *regs) {
    UNUSED(regs);
    g_pit_ticks++;

    /* Trigger scheduler on every tick */
    scheduler_tick();
}

uint64_t pit_get_ticks(void) {
    return g_pit_ticks;
}

void pit_sleep_ms(uint32_t ms) {
    uint64_t target = g_pit_ticks + (ms * PIT_HZ / 1000) + 1;
    while (g_pit_ticks < target) {
        hlt();
    }
}

void pit_wait(uint32_t ms) {
    pit_sleep_ms(ms);
}
