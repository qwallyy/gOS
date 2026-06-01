/* gOS Kernel — Parallel Port (LPT) Driver
 * Reference: https://wiki.osdev.org/Parallel_port
 */

#ifndef GOS_LPT_H
#define GOS_LPT_H

#include "gOS/types.h"

#define LPT1_BASE   0x378
#define LPT2_BASE   0x278
#define LPT3_BASE   0x3BC

#define LPT_DATA    0x00
#define LPT_STATUS  0x01
#define LPT_CONTROL 0x02

#define LPT_STATUS_BUSY     0x80
#define LPT_STATUS_ACK      0x40
#define LPT_STATUS_PAPER    0x20
#define LPT_STATUS_SELECT   0x10
#define LPT_STATUS_ERROR    0x08

void lpt_init(void);
bool lpt_detect(uint16_t base);
bool lpt_write_byte(uint16_t base, uint8_t data);
bool lpt_write_data(uint16_t base, const void *data, size_t len);

#endif /* GOS_LPT_H */
