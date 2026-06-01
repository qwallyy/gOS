/* gOS Kernel — I/O Port Assembly Wrapper Declarations */

#ifndef GOS_IO_H
#define GOS_IO_H

#include "gOS/types.h"

uint8_t  io_inb(uint16_t port);
uint16_t io_inw(uint16_t port);
uint32_t io_inl(uint16_t port);
void     io_outb(uint16_t port, uint8_t val);
void     io_outw(uint16_t port, uint16_t val);
void     io_outl(uint16_t port, uint32_t val);
void     io_wait(void);

#endif /* GOS_IO_H */
