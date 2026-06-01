/* gOS Kernel — AHCI (SATA) Driver
 * Reference: https://wiki.osdev.org/AHCI
 */

#ifndef GOS_AHCI_H
#define GOS_AHCI_H

#include "gOS/types.h"
#include "drivers/pci/pci.h"

#define AHCI_HBA_GHC        0x04
#define AHCI_HBA_GHC_AE     (1U << 31)
#define AHCI_HBA_GHC_IE     (1U << 1)
#define AHCI_HBA_GHC_HR     (1U << 0)

#define AHCI_HBA_CAP        0x00
#define AHCI_HBA_PI         0x0C

#define AHCI_PORT_CMD       0x18
#define AHCI_PORT_CMD_ST    (1U << 0)
#define AHCI_PORT_CMD_FRE   (1U << 4)
#define AHCI_PORT_CMD_FR    (1U << 14)
#define AHCI_PORT_CMD_CR    (1U << 15)

#define AHCI_PORT_TFD       0x20
#define AHCI_PORT_TFD_BSY   (1U << 7)
#define AHCI_PORT_TFD_DRQ   (1U << 3)

#define AHCI_PORT_SIG_ATA   0x00000101
#define AHCI_PORT_SIG_ATAPI 0xEB140101
#define AHCI_PORT_SIG_SEMB  0xC33C0101
#define AHCI_PORT_SIG_PM    0x96690101

#define AHCI_CMD_LIST_SIZE  32
#define AHCI_FIS_SIZE       256
#define AHCI_CMD_TABLE_SIZE 2048

void ahci_init(void);
bool ahci_probe_port(const pci_device_t *pci_dev);

#endif /* GOS_AHCI_H */
