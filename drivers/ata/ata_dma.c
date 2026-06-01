/* gOS Kernel — ATA DMA Mode Implementation (stub)
 * Reference: https://wiki.osdev.org/ATA_DMA
 */

#include "drivers/ata/ata_dma.h"
#include "gOS/klog.h"

void ata_dma_init(void) {
    klog_info("ATA DMA initialized (stub, falls back to PIO)");
}

bool ata_dma_read(uint64_t lba, uint32_t count, void *buf) {
    (void)lba; (void)count; (void)buf;
    klog_warn("ata_dma_read: not implemented, use ata_read_sectors()");
    return false;
}

bool ata_dma_write(uint64_t lba, uint32_t count, const void *buf) {
    (void)lba; (void)count; (void)buf;
    klog_warn("ata_dma_write: not implemented, use ata_write_sectors()");
    return false;
}
