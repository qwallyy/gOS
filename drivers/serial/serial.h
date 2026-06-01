/* gOS Kernel — Serial Port (UART 16550A) Driver
 * Reference: https://wiki.osdev.org/Serial_Ports
 */

#ifndef GOS_SERIAL_H
#define GOS_SERIAL_H

#include "gOS/types.h"

/* COM port base addresses */
#define SERIAL_COM1     0x3F8
#define SERIAL_COM2     0x2F8
#define SERIAL_COM3     0x3E8
#define SERIAL_COM4     0x2E8

/* UART register offsets (from base) */
#define UART_DATA       0
#define UART_IER        1
#define UART_IIR        2
#define UART_FCR        2
#define UART_LCR        3
#define UART_MCR        4
#define UART_LSR        5
#define UART_MSR        6
#define UART_SCRATCH    7

/* Divisor latch access */
#define UART_DLAB       0x80

/* Line Status Register bits */
#define UART_LSR_DR     0x01  /* Data ready */
#define UART_LSR_OE     0x02  /* Overrun error */
#define UART_LSR_PE     0x04  /* Parity error */
#define UART_LSR_FE     0x08  /* Framing error */
#define UART_LSR_BI     0x10  /* Break indicator */
#define UART_LSR_THRE   0x20  /* Transmitter holding register empty */
#define UART_LSR_TEMT   0x40  /* Transmitter empty */
#define UART_LSR_ERR    0x80  /* Error in RCVR FIFO */

/* Interrupt Enable Register bits */
#define UART_IER_RDI    0x01  /* Enable received data available interrupt */
#define UART_IER_THRI   0x02  /* Enable transmitter holding register empty interrupt */
#define UART_IER_RLSI   0x04  /* Enable receiver line status interrupt */
#define UART_IER_MSI    0x08  /* Enable modem status interrupt */

/* FIFO Control Register */
#define UART_FCR_ENABLE     0x01
#define UART_FCR_CLEAR_RX   0x02
#define UART_FCR_CLEAR_TX   0x04
#define UART_FCR_DMA_MODE   0x08
#define UART_FCR_TRIGGER_1  0x00
#define UART_FCR_TRIGGER_4  0x40
#define UART_FCR_TRIGGER_8  0x80
#define UART_FCR_TRIGGER_14 0xC0

/* Modem Control Register */
#define UART_MCR_DTR    0x01
#define UART_MCR_RTS    0x02
#define UART_MCR_OUT1   0x04
#define UART_MCR_OUT2   0x08  /* Must be set to enable interrupts */
#define UART_MCR_LOOP   0x10

void serial_init(void);
bool serial_is_initialized(void);
void serial_putc(char c);
void serial_puts(const char *str);
void serial_puthex(uint64_t val);
void serial_putdec(uint64_t val);
char serial_getc(void);
bool serial_ready_to_read(void);
bool serial_ready_to_write(void);
void serial_handle_irq(int port);
void serial_enable_interrupts(int port);
void serial_disable_interrupts(int port);

#endif /* GOS_SERIAL_H */
