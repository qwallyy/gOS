/* gOS Kernel — Unified Timer Abstraction
 * Wraps PIT, HPET, and LAPIC timer sources.
 */

#ifndef GOS_TIMER_H
#define GOS_TIMER_H

#include "gOS/types.h"

typedef enum timer_source {
    TIMER_SOURCE_PIT = 0,
    TIMER_SOURCE_HPET,
    TIMER_SOURCE_LAPIC,
    TIMER_SOURCE_RTC,
} timer_source_t;

void timer_init(void);
void timer_set_source(timer_source_t source);
timer_source_t timer_get_source(void);
uint64_t timer_get_ticks(void);
uint64_t timer_get_frequency(void);
void timer_sleep_ms(uint64_t ms);
void timer_sleep_us(uint64_t us);
void timer_register_callback(void (*callback)(void));
void timer_unregister_callback(void);

#endif /* GOS_TIMER_H */
