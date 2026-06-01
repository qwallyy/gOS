/* gOS Kernel — Floppy Disk Controller Implementation
 * Reference: https://wiki.osdev.org/Floppy_Disk_Controller
 */

#include "drivers/floppy/floppy.h"
#include "drivers/pic/pic.h"
#include "gOS/klog.h"

static uint16_t g_floppy_base = FLOPPY_PRIMARY;
static volatile bool g_floppy_irq_fired = false;

static void floppy_wait_msr(void) {
    for (int i = 0; i < 100000; i++) {
        uint8_t msr = inb(g_floppy_base + FLOPPY_MSR);
        if ((msr & FLOPPY_STATUS_MRQ) && !(msr & FLOPPY_STATUS_DIO)) {
            return;
        }
        io_wait();
    }
}

static void floppy_send_cmd(uint8_t cmd) {
    floppy_wait_msr();
    outb(g_floppy_base + FLOPPY_FIFO, cmd);
    io_wait();
}

static uint8_t floppy_read_data(void) {
    for (int i = 0; i < 100000; i++) {
        if (inb(g_floppy_base + FLOPPY_MSR) & FLOPPY_STATUS_MRQ) {
            return inb(g_floppy_base + FLOPPY_FIFO);
        }
        io_wait();
    }
    return 0;
}

void floppy_init(void) {
    klog_info("Floppy disk controller initializing");

    /* Reset controller */
    outb(g_floppy_base + FLOPPY_DOR, 0x00);
    io_wait();
    outb(g_floppy_base + FLOPPY_DOR, 0x0C);
    io_wait();

    /* Wait for IRQ (not yet hooked, so just poll) */
    for (int i = 0; i < 1000000; i++) {
        if (inb(g_floppy_base + FLOPPY_DIR) & 0x80) break;
        io_wait();
    }

    /* Sense interrupt status */
    floppy_send_cmd(FLOPPY_CMD_SENSE_INT);
    (void)floppy_read_data();
    (void)floppy_read_data();

    /* Configure */
    floppy_send_cmd(FLOPPY_CMD_SPECIFY);
    outb(g_floppy_base + FLOPPY_FIFO, 0xDF); /* SRT=3ms, HUT=240ms */
    outb(g_floppy_base + FLOPPY_FIFO, 0x02); /* HLT=16ms, ND=0 */
    io_wait();

    klog_info("Floppy controller initialized (stub, no IRQ handler)");
}

bool floppy_detect(void) {
    /* Read CMOS equipment byte */
    outb(0x70, 0x10);
    io_wait();
    uint8_t equip = inb(0x71);
    bool present = (equip & 0xF0) != 0;
    if (present) {
        klog_info("Floppy drive detected via CMOS");
    }
    return present;
}

bool floppy_read_sector(uint8_t head, uint8_t track, uint8_t sector, void *buf) {
    (void)head; (void)track; (void)sector; (void)buf;
    klog_warn("floppy_read_sector: not implemented");
    return false;
}

bool floppy_write_sector(uint8_t head, uint8_t track, uint8_t sector, const void *buf) {
    (void)head; (void)track; (void)sector; (void)buf;
    klog_warn("floppy_write_sector: not implemented");
    return false;
}
