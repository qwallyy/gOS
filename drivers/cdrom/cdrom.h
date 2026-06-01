/* gOS Kernel — ATAPI CD-ROM Driver
 * Reference: https://wiki.osdev.org/ATAPI
 */

#ifndef GOS_CDROM_H
#define GOS_CDROM_H

#include "gOS/types.h"

#define ATAPI_CMD_READ_CAPACITY     0x25
#define ATAPI_CMD_READ_10           0x28
#define ATAPI_CMD_REQUEST_SENSE     0x03
#define ATAPI_CMD_TEST_UNIT_READY   0x00
#define ATAPI_CMD_START_STOP_UNIT   0x1B

typedef struct atapi_read_capacity {
    uint32_t lba;
    uint32_t block_size;
} atapi_read_capacity_t;

void cdrom_init(void);
bool cdrom_detect(uint8_t drive);
bool cdrom_read_sectors(uint8_t drive, uint32_t lba, uint32_t count, void *buf);
bool cdrom_get_capacity(uint8_t drive, atapi_read_capacity_t *cap);

#endif /* GOS_CDROM_H */
