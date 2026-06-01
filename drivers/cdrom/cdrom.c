/* gOS Kernel — ATAPI CD-ROM Implementation
 * Reference: https://wiki.osdev.org/ATAPI
 */

#include "drivers/cdrom/cdrom.h"
#include "drivers/ata/ata.h"
#include "gOS/klog.h"

#define ATA_REG_FEATURES    0x01
#define ATA_REG_SECCOUNT    0x02
#define ATA_REG_LBA0        0x03
#define ATA_REG_LBA1        0x04
#define ATA_REG_LBA2        0x05
#define ATA_REG_HDDEVSEL    0x06
#define ATA_REG_COMMAND     0x07
#define ATA_REG_STATUS      0x07
#define ATA_CMD_PACKET      0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1

static uint16_t g_cdrom_base = ATA_PRIMARY_BUS;

void cdrom_init(void) {
    klog_info("ATAPI CD-ROM driver initializing");
    /* Detection would happen during ATA init by checking signature */
}

bool cdrom_detect(uint8_t drive) {
    (void)drive;
    klog_warn("cdrom_detect: stub");
    return false;
}

static bool __attribute__((used)) atapi_wait_ready(void) {
    for (int i = 0; i < 1000000; i++) {
        uint8_t status = inb(g_cdrom_base + ATA_REG_STATUS);
        if (!(status & ATA_STATUS_BSY)) {
            if (status & ATA_STATUS_DRQ) return true;
            if (status & ATA_STATUS_ERR) return false;
        }
        io_wait();
    }
    return false;
}

bool cdrom_read_sectors(uint8_t drive, uint32_t lba, uint32_t count, void *buf) {
    (void)drive; (void)lba; (void)count; (void)buf;
    klog_warn("cdrom_read_sectors: stub (ATAPI packet commands not implemented)");
    return false;
}

bool cdrom_get_capacity(uint8_t drive, atapi_read_capacity_t *cap) {
    (void)drive; (void)cap;
    klog_warn("cdrom_get_capacity: stub");
    return false;
}
