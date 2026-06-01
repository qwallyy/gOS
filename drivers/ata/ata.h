/* gOS Kernel — ATA PIO Mode Driver
 * Reference: https://wiki.osdev.org/ATA_PIO_Mode
 */

#ifndef GOS_ATA_H
#define GOS_ATA_H

#include "gOS/types.h"

#define ATA_PRIMARY_BUS     0x1F0
#define ATA_SECONDARY_BUS   0x170

#define ATA_REG_DATA        0x00
#define ATA_REG_ERROR       0x01
#define ATA_REG_FEATURES    0x01
#define ATA_REG_SECCOUNT    0x02
#define ATA_REG_LBA0        0x03
#define ATA_REG_LBA1        0x04
#define ATA_REG_LBA2        0x05
#define ATA_REG_HDDEVSEL    0x06
#define ATA_REG_COMMAND     0x07
#define ATA_REG_STATUS      0x07
#define ATA_REG_ALTSTATUS   0x206
#define ATA_REG_CONTROL     0x206

#define ATA_CMD_READ_PIO    0x20
#define ATA_CMD_WRITE_PIO   0x30
#define ATA_CMD_IDENTIFY    0xEC
#define ATA_CMD_CACHE_FLUSH 0xE7

#define ATA_STATUS_ERR      0x01
#define ATA_STATUS_DRQ      0x08
#define ATA_STATUS_SRV      0x10
#define ATA_STATUS_DF       0x20
#define ATA_STATUS_RDY      0x40
#define ATA_STATUS_BSY      0x80

#define ATA_DRIVE_MASTER    0xA0
#define ATA_DRIVE_SLAVE     0xB0

#define ATA_SECTOR_SIZE     512

typedef struct ata_device {
    uint16_t base_port;
    uint8_t  drive;
    bool     present;
    uint64_t total_sectors;
    char     model[41];
} ata_device_t;

void ata_init(void);
bool ata_read_sectors(uint64_t lba, uint32_t count, void *buf);
bool ata_write_sectors(uint64_t lba, uint32_t count, const void *buf);
bool ata_identify(void);

extern ata_device_t g_ata_primary_master;
extern ata_device_t g_ata_primary_slave;

#endif /* GOS_ATA_H */
