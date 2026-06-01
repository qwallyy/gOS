/* gOS Kernel — DMA (Direct Memory Access) Controller
 * Reference: https://wiki.osdev.org/DMA
 */

#ifndef GOS_DMA_H
#define GOS_DMA_H

#include "gOS/types.h"

/* 8237 DMA Controller ports */
#define DMA_CHAN0_ADDR      0x00
#define DMA_CHAN0_COUNT     0x01
#define DMA_CHAN1_ADDR      0x02
#define DMA_CHAN1_COUNT     0x03
#define DMA_CHAN2_ADDR      0x04
#define DMA_CHAN2_COUNT     0x05
#define DMA_CHAN3_ADDR      0x06
#define DMA_CHAN3_COUNT     0x07
#define DMA_STATUS          0x08
#define DMA_COMMAND         0x08
#define DMA_REQUEST         0x09
#define DMA_MASK_CHAN       0x0A
#define DMA_MODE            0x0B
#define DMA_CLEAR_FLIPFLOP  0x0C
#define DMA_INTERMEDIATE    0x0D
#define DMA_MASTER_CLEAR    0x0D
#define DMA_MASK_CHAN_CLR   0x0E
#define DMA_MASK_ALL        0x0F

/* Page registers for each channel */
#define DMA_PAGE_CHAN0      0x87
#define DMA_PAGE_CHAN1      0x83
#define DMA_PAGE_CHAN2      0x81
#define DMA_PAGE_CHAN3      0x82
#define DMA_PAGE_CHAN4      0x8F
#define DMA_PAGE_CHAN5      0x8B
#define DMA_PAGE_CHAN6      0x89
#define DMA_PAGE_CHAN7      0x8A

void dma_init(void);
void dma_set_mode(uint8_t channel, uint8_t mode);
void dma_set_address(uint8_t channel, uint16_t address);
void dma_set_count(uint8_t channel, uint16_t count);
void dma_mask_channel(uint8_t channel);
void dma_unmask_channel(uint8_t channel);

#endif /* GOS_DMA_H */
