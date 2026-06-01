/* gOS Kernel — PS/2 Controller
 * Reference: https://wiki.osdev.org/%228042%22_PS/2_Controller
 */

#ifndef GOS_PS2_H
#define GOS_PS2_H

#include "gOS/types.h"

#define PS2_DATA_PORT       0x60
#define PS2_STATUS_PORT     0x64
#define PS2_COMMAND_PORT    0x64

#define PS2_STATUS_OUTBUF   0x01
#define PS2_STATUS_INBUF    0x02
#define PS2_STATUS_SYSTEM   0x04
#define PS2_STATUS_CMDDATA  0x08
#define PS2_STATUS_KEYLOCK  0x10
#define PS2_STATUS_AUXBUF   0x20
#define PS2_STATUS_TIMEOUT  0x40
#define PS2_STATUS_PARITY   0x80

#define PS2_CMD_READ_CFG    0x20
#define PS2_CMD_WRITE_CFG   0x60
#define PS2_CMD_DISABLE_P1  0xAD
#define PS2_CMD_ENABLE_P1   0xAE
#define PS2_CMD_DISABLE_P2  0xA7
#define PS2_CMD_ENABLE_P2   0xA8
#define PS2_CMD_TEST_CTRL   0xAA
#define PS2_CMD_TEST_P1     0xAB
#define PS2_CMD_TEST_P2     0xA9
#define PS2_CMD_WRITE_P2    0xD4

#define PS2_CFG_IRQ1        0x01
#define PS2_CFG_IRQ2        0x02
#define PS2_CFG_SYSFLAG     0x04
#define PS2_CFG_DISABLE_P1  0x10
#define PS2_CFG_DISABLE_P2  0x20
#define PS2_CFG_XLAT        0x40

void ps2_init(void);
bool ps2_self_test(void);
bool ps2_has_second_port(void);
uint8_t ps2_read_data(void);
void ps2_write_data(uint8_t val);
void ps2_send_command(uint8_t cmd);
uint8_t ps2_read_config(void);
void ps2_write_config(uint8_t cfg);

#endif /* GOS_PS2_H */
