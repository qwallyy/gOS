/* gOS Kernel — Intel E1000 NIC Driver
 * Reference: https://wiki.osdev.org/Intel_8254x
 */

#ifndef GOS_E1000_H
#define GOS_E1000_H

#include "gOS/types.h"

#define E1000_VENDOR_ID     0x8086
#define E1000_DEVICE_ID     0x100E  /* QEMU e1000 default */

/* E1000 register offsets */
#define E1000_CTRL      0x0000
#define E1000_STATUS    0x0008
#define E1000_EERD      0x0014
#define E1000_CTRL_EXT  0x0018
#define E1000_MDIC      0x0020
#define E1000_FCAL      0x0028
#define E1000_FCAH      0x002C
#define E1000_FCT       0x0030
#define E1000_VET       0x0038
#define E1000_ICR       0x00C0
#define E1000_ITR       0x00C4
#define E1000_ICS       0x00C8
#define E1000_IMS       0x00D0
#define E1000_IMC       0x00D8
#define E1000_RCTL      0x0100
#define E1000_RDTR1     0x2820
#define E1000_RDBAL1    0x2900
#define E1000_RDBAH1    0x2904
#define E1000_RDLEN1    0x2908
#define E1000_RDH1      0x2910
#define E1000_RDT1      0x2918
#define E1000_RDBAL     0x2800
#define E1000_RDBAH     0x2804
#define E1000_RDLEN     0x2808
#define E1000_RDH       0x2810
#define E1000_RDT       0x2818
#define E1000_RDTR      0x2820
#define E1000_TCTL      0x0400
#define E1000_TDBAL     0x3800
#define E1000_TDBAH     0x3804
#define E1000_TDLEN     0x3808
#define E1000_TDH       0x3810
#define E1000_TDT       0x3818
#define E1000_MTA       0x5200
#define E1000_RAL       0x5400
#define E1000_RAH       0x5404

/* RCTL bits */
#define RCTL_EN         (1U << 1)
#define RCTL_SBP        (1U << 2)
#define RCTL_UPE        (1U << 3)
#define RCTL_MPE        (1U << 4)
#define RCTL_LPE        (1U << 5)
#define RCTL_BAM        (1U << 15)
#define RCTL_BSIZE_256  (3U << 16)
#define RCTL_BSIZE_512  (2U << 16)
#define RCTL_BSIZE_1024 (1U << 16)
#define RCTL_BSIZE_2048 (0U << 16)
#define RCTL_BSEX       (1U << 25)
#define RCTL_SECRC      (1U << 26)

/* TCTL bits */
#define TCTL_EN         (1U << 1)
#define TCTL_PSP        (1U << 3)
#define TCTL_CT_SHIFT   4
#define TCTL_COLD_SHIFT 12

/* TX descriptor */
typedef struct e1000_tx_desc {
    uint64_t addr;
    uint16_t length;
    uint8_t  cso;
    uint8_t  cmd;
    uint8_t  status;
    uint8_t  css;
    uint16_t special;
} PACKED e1000_tx_desc_t;

/* RX descriptor */
typedef struct e1000_rx_desc {
    uint64_t addr;
    uint16_t length;
    uint16_t checksum;
    uint8_t  status;
    uint8_t  errors;
    uint16_t special;
} PACKED e1000_rx_desc_t;

extern uint64_t g_e1000_base;

static inline uint32_t e1000_read_reg(uint64_t base, uint32_t reg) {
    return mmio_read32(base + reg);
}

static inline void e1000_write_reg(uint64_t base, uint32_t reg, uint32_t val) {
    mmio_write32(base + reg, val);
}

void e1000_init(void);
bool e1000_probe(void);

#endif /* GOS_E1000_H */
