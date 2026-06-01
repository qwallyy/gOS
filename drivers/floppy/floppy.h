/* gOS Kernel — Floppy Disk Driver (legacy)
 * Reference: https://wiki.osdev.org/Floppy_Disk_Controller
 */

#ifndef GOS_FLOPPY_H
#define GOS_FLOPPY_H

#include "gOS/types.h"

#define FLOPPY_PRIMARY      0x3F0
#define FLOPPY_SECONDARY    0x370

#define FLOPPY_DOR          0x02  /* Digital Output Register */
#define FLOPPY_MSR          0x04  /* Main Status Register */
#define FLOPPY_FIFO         0x05  /* Data FIFO */
#define FLOPPY_DIR          0x07  /* Digital Input Register */
#define FLOPPY_CCR          0x07  /* Configuration Control Register */

#define FLOPPY_CMD_READ_TRACK    0x02
#define FLOPPY_CMD_SPECIFY       0x03
#define FLOPPY_CMD_SENSE_STATUS  0x04
#define FLOPPY_CMD_WRITE_DATA    0x05
#define FLOPPY_CMD_READ_DATA     0x06
#define FLOPPY_CMD_RECALIBRATE  0x07
#define FLOPPY_CMD_SENSE_INT     0x08
#define FLOPPY_CMD_SEEK          0x0F

#define FLOPPY_STATUS_BSY        0x10
#define FLOPPY_STATUS_DIO        0x40
#define FLOPPY_STATUS_MRQ        0x80

void floppy_init(void);
bool floppy_detect(void);
bool floppy_read_sector(uint8_t head, uint8_t track, uint8_t sector, void *buf);
bool floppy_write_sector(uint8_t head, uint8_t track, uint8_t sector, const void *buf);

#endif /* GOS_FLOPPY_H */
