/* gOS Kernel — ATA PIO Mode Implementation
 * Reference: https://wiki.osdev.org/ATA_PIO_Mode
 */

#include "drivers/ata/ata.h"
#include "fs/block/block.h"
#include "gOS/klog.h"
#include "lib/string.h"

ata_device_t g_ata_primary_master = { ATA_PRIMARY_BUS, ATA_DRIVE_MASTER, false, 0, "" };
ata_device_t g_ata_primary_slave  = { ATA_PRIMARY_BUS, ATA_DRIVE_SLAVE,  false, 0, "" };

static uint8_t ata_read_status(ata_device_t *dev) {
    return inb(dev->base_port + ATA_REG_STATUS);
}

static bool ata_wait_bsy(ata_device_t *dev) {
    for (int i = 0; i < 1000000; i++) {
        if (!(ata_read_status(dev) & ATA_STATUS_BSY)) {
            return true;
        }
        io_wait();
    }
    return false;
}

static bool ata_wait_drq(ata_device_t *dev) {
    for (int i = 0; i < 1000000; i++) {
        uint8_t status = ata_read_status(dev);
        if (status & ATA_STATUS_ERR) return false;
        if (status & ATA_STATUS_DRQ) return true;
        io_wait();
    }
    return false;
}

static void ata_select_drive(ata_device_t *dev) {
    outb(dev->base_port + ATA_REG_HDDEVSEL, dev->drive);
    io_wait();
}

bool ata_identify(void) {
    ata_device_t *dev = &g_ata_primary_master;
    ata_select_drive(dev);

    /* Send IDENTIFY command */
    outb(dev->base_port + ATA_REG_SECCOUNT, 0);
    outb(dev->base_port + ATA_REG_LBA0, 0);
    outb(dev->base_port + ATA_REG_LBA1, 0);
    outb(dev->base_port + ATA_REG_LBA2, 0);
    outb(dev->base_port + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    io_wait();

    if (ata_read_status(dev) == 0) {
        klog_warn("ATA primary master: no device");
        return false;
    }

    if (!ata_wait_bsy(dev)) {
        klog_warn("ATA primary master: timeout waiting for BSY");
        return false;
    }

    uint8_t lba1 = inb(dev->base_port + ATA_REG_LBA1);
    uint8_t lba2 = inb(dev->base_port + ATA_REG_LBA2);
    if (lba1 != 0 || lba2 != 0) {
        klog_warn("ATA primary master: not ATA (LBA1=%02x LBA2=%02x)", lba1, lba2);
        return false;
    }

    if (!ata_wait_drq(dev)) {
        klog_warn("ATA primary master: timeout waiting for DRQ");
        return false;
    }

    uint16_t buf[256];
    for (int i = 0; i < 256; i++) {
        buf[i] = inw(dev->base_port + ATA_REG_DATA);
    }

    dev->present = true;
    dev->total_sectors = ((uint64_t)buf[61] << 16) | buf[60];

    /* Model string is in words 27-46, byte-swapped */
    for (int i = 0; i < 20; i++) {
        dev->model[i * 2] = (char)(buf[27 + i] >> 8);
        dev->model[i * 2 + 1] = (char)(buf[27 + i] & 0xFF);
    }
    dev->model[40] = '\0';
    /* Trim trailing spaces */
    for (int i = 39; i >= 0 && dev->model[i] == ' '; i--) {
        dev->model[i] = '\0';
    }

    klog_info("ATA primary master: %s, %llu sectors (%llu MiB)",
              dev->model, (unsigned long long)dev->total_sectors,
              (unsigned long long)(dev->total_sectors * ATA_SECTOR_SIZE / (1024 * 1024)));

    /* Register as block device */
    block_register("hda", dev->total_sectors, ATA_SECTOR_SIZE,
                   ata_read_sectors, ata_write_sectors, dev);
    return true;
}

bool ata_read_sectors(uint64_t lba, uint32_t count, void *buf) {
    ata_device_t *dev = &g_ata_primary_master;
    if (!dev->present) return false;

    ata_select_drive(dev);
    outb(dev->base_port + ATA_REG_FEATURES, 0x00);
    outb(dev->base_port + ATA_REG_SECCOUNT, (uint8_t)count);
    outb(dev->base_port + ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outb(dev->base_port + ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outb(dev->base_port + ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));
    outb(dev->base_port + ATA_REG_HDDEVSEL, dev->drive | 0x40 | ((lba >> 24) & 0x0F));
    io_wait();
    outb(dev->base_port + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    io_wait();

    uint16_t *data = (uint16_t *)buf;
    for (uint32_t s = 0; s < count; s++) {
        if (!ata_wait_drq(dev)) return false;
        for (int i = 0; i < 256; i++) {
            data[s * 256 + i] = inw(dev->base_port + ATA_REG_DATA);
        }
    }
    return true;
}

bool ata_write_sectors(uint64_t lba, uint32_t count, const void *buf) {
    ata_device_t *dev = &g_ata_primary_master;
    if (!dev->present) return false;

    ata_select_drive(dev);
    outb(dev->base_port + ATA_REG_FEATURES, 0x00);
    outb(dev->base_port + ATA_REG_SECCOUNT, (uint8_t)count);
    outb(dev->base_port + ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outb(dev->base_port + ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outb(dev->base_port + ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));
    outb(dev->base_port + ATA_REG_HDDEVSEL, dev->drive | 0x40 | ((lba >> 24) & 0x0F));
    io_wait();
    outb(dev->base_port + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    io_wait();

    const uint16_t *data = (const uint16_t *)buf;
    for (uint32_t s = 0; s < count; s++) {
        if (!ata_wait_drq(dev)) return false;
        for (int i = 0; i < 256; i++) {
            outw(dev->base_port + ATA_REG_DATA, data[s * 256 + i]);
        }
    }
    outb(dev->base_port + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    io_wait();
    ata_wait_bsy(dev);
    return true;
}

void ata_init(void) {
    klog_info("Initializing ATA PIO driver");
    if (!ata_identify()) {
        klog_warn("ATA: no primary master found");
    }
}
