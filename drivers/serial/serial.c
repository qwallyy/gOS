/* gOS Kernel — Serial Port (UART 16550A) Implementation
 * Reference: https://wiki.osdev.org/Serial_Ports
 */

#include "drivers/serial/serial.h"
#include "gOS/klog.h"
#include "lib/string.h"

static bool g_serial_ready = false;

static bool serial_port_exists(uint16_t port) {
    /* Write test pattern to scratch register */
    outb(port + UART_SCRATCH, 0xAB);
    io_wait();
    return inb(port + UART_SCRATCH) == 0xAB;
}

void serial_init(void) {
    klog_info("Initializing serial port COM1 (0x%x)", SERIAL_COM1);

    if (!serial_port_exists(SERIAL_COM1)) {
        klog_warn("COM1 not detected, serial output may not work");
        return;
    }

    /* Disable interrupts during init */
    outb(SERIAL_COM1 + UART_IER, 0x00);

    /* Enable DLAB to set baud rate */
    outb(SERIAL_COM1 + UART_LCR, UART_DLAB);

    /* Divisor = 1 for 115200 baud (low=1, high=0) */
    outb(SERIAL_COM1 + UART_DATA, 0x01);
    outb(SERIAL_COM1 + UART_IER, 0x00);

    /* 8 bits, no parity, one stop bit, clear DLAB */
    outb(SERIAL_COM1 + UART_LCR, 0x03);

    /* Enable FIFO, clear buffers, 14-byte threshold */
    outb(SERIAL_COM1 + UART_FCR,
         UART_FCR_ENABLE | UART_FCR_CLEAR_RX | UART_FCR_CLEAR_TX | UART_FCR_TRIGGER_14);

    /* Set OUT2 to enable interrupts (but keep them masked in IER for now) */
    outb(SERIAL_COM1 + UART_MCR, UART_MCR_DTR | UART_MCR_RTS | UART_MCR_OUT2);

    /* Verify by reading IIR */
    uint8_t iir = inb(SERIAL_COM1 + UART_IIR);
    if ((iir & 0xC0) == 0xC0) {
        klog_info("COM1 FIFO enabled (16550A detected)");
    } else {
        klog_info("COM1: no FIFO (8250/16450 or 16550 without FIFO)");
    }

    g_serial_ready = true;
    klog_info("Serial initialization complete");
}

bool serial_is_initialized(void) {
    return g_serial_ready;
}

bool serial_ready_to_write(void) {
    return (inb(SERIAL_COM1 + UART_LSR) & UART_LSR_THRE) != 0;
}

bool serial_ready_to_read(void) {
    return (inb(SERIAL_COM1 + UART_LSR) & UART_LSR_DR) != 0;
}

void serial_putc(char c) {
    if (!g_serial_ready) return;
    while (!serial_ready_to_write()) {
        pause();
    }
    outb(SERIAL_COM1 + UART_DATA, (uint8_t)c);
}

void serial_puts(const char *str) {
    if (!str) return;
    while (*str) {
        if (*str == '\n') serial_putc('\r');
        serial_putc(*str++);
    }
}

void serial_puthex(uint64_t val) {
    serial_puts("0x");
    for (int i = 15; i >= 0; i--) {
        uint8_t nibble = (val >> (i * 4)) & 0xF;
        serial_putc((nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10));
    }
}

void serial_putdec(uint64_t val) {
    char buf[32];
    int i = 0;
    if (val == 0) {
        serial_putc('0');
        return;
    }
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (i > 0) {
        serial_putc(buf[--i]);
    }
}

char serial_getc(void) {
    if (!g_serial_ready) return 0;
    while (!serial_ready_to_read()) {
        pause();
    }
    return (char)inb(SERIAL_COM1 + UART_DATA);
}

void serial_handle_irq(int port) {
    (void)port;
    /* Read IIR to determine interrupt cause */
    uint8_t iir = inb(SERIAL_COM1 + UART_IIR);
    if ((iir & 0x01) == 0) {
        /* Interrupt pending */
        uint8_t type = (iir >> 1) & 0x07;
        if (type == 2) {
            /* Transmitter empty */
        } else if (type == 4) {
            /* Data available */
            while (serial_ready_to_read()) {
                (void)serial_getc();
            }
        }
    }
}

void serial_enable_interrupts(int port) {
    (void)port;
    outb(SERIAL_COM1 + UART_IER, UART_IER_RDI | UART_IER_THRI);
}

void serial_disable_interrupts(int port) {
    (void)port;
    outb(SERIAL_COM1 + UART_IER, 0x00);
}
