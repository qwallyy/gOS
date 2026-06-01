/* gOS Kernel — ATA DMA Mode (UDMA) Definitions
 * Reference: https://wiki.osdev.org/ATA_DMA
 */

#ifndef GOS_ATA_DMA_H
#define GOS_ATA_DMA_H

#include "gOS/types.h"

#define ATA_DMA_CMD_START   0x01
#define ATA_DMA_CMD_READ    0x08
#define ATA_DMA_CMD_WRITE   0x00

#define ATA_DMA_STATUS_ERR  0x01
#define ATA_DMA_STATUS_INT  0x04
#define ATA_DMA_STATUS_DMA  0x01

void ata_dma_init(void);
bool ata_dma_read(uint64_t lba, uint32_t count, void *buf);
bool ata_dma_write(uint64_t lba, uint32_t count, const void *buf);

#endif /* GOS_ATA_DMA_H */
