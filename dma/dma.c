/* gOS Kernel — DMA Controller Implementation
 * Reference: https://wiki.osdev.org/DMA
 */

#include "dma/dma.h"
#include "gOS/klog.h"

static const uint8_t g_dma_page_regs[] = {
    DMA_PAGE_CHAN0, DMA_PAGE_CHAN1, DMA_PAGE_CHAN2, DMA_PAGE_CHAN3,
    DMA_PAGE_CHAN4, DMA_PAGE_CHAN5, DMA_PAGE_CHAN6, DMA_PAGE_CHAN7
};

static const uint8_t g_dma_addr_regs[] = {
    DMA_CHAN0_ADDR, DMA_CHAN1_ADDR, DMA_CHAN2_ADDR, DMA_CHAN3_ADDR,
    0xC0, 0xC4, 0xC8, 0xCC  /* Channels 4-7 on second controller */
};

static const uint8_t g_dma_count_regs[] = {
    DMA_CHAN0_COUNT, DMA_CHAN1_COUNT, DMA_CHAN2_COUNT, DMA_CHAN3_COUNT,
    0xC2, 0xC6, 0xCA, 0xCE
};

void dma_init(void) {
    klog_info("DMA controller initialized (legacy 8237)");
}

void dma_set_mode(uint8_t channel, uint8_t mode) {
    outb(DMA_MASK_CHAN, 0x04 | channel); /* Mask channel */
    outb(DMA_CLEAR_FLIPFLOP, 0x00);
    outb(DMA_MODE, mode | channel);
    outb(DMA_MASK_CHAN, channel); /* Unmask */
}

void dma_set_address(uint8_t channel, uint16_t address) {
    if (channel > 7) return;
    outb(DMA_CLEAR_FLIPFLOP, 0x00);
    outb(g_dma_addr_regs[channel], address & 0xFF);
    outb(g_dma_addr_regs[channel], (address >> 8) & 0xFF);
    if (channel < 4) {
        outb(g_dma_page_regs[channel], (address >> 16) & 0xFF);
    }
}

void dma_set_count(uint8_t channel, uint16_t count) {
    if (channel > 7) return;
    outb(DMA_CLEAR_FLIPFLOP, 0x00);
    outb(g_dma_count_regs[channel], count & 0xFF);
    outb(g_dma_count_regs[channel], (count >> 8) & 0xFF);
}

void dma_mask_channel(uint8_t channel) {
    outb(DMA_MASK_CHAN, 0x04 | (channel & 0x03));
}

void dma_unmask_channel(uint8_t channel) {
    outb(DMA_MASK_CHAN, channel & 0x03);
}
