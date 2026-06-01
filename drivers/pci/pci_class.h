/* gOS Kernel — PCI Class Code Definitions
 * Reference: https://wiki.osdev.org/PCI
 */

#ifndef GOS_PCI_CLASS_H
#define GOS_PCI_CLASS_H

#include "gOS/types.h"

#define PCI_CLASS_OLD               0x00
#define PCI_CLASS_STORAGE           0x01
#define PCI_CLASS_NETWORK           0x02
#define PCI_CLASS_DISPLAY           0x03
#define PCI_CLASS_MULTIMEDIA        0x04
#define PCI_CLASS_MEMORY            0x05
#define PCI_CLASS_BRIDGE            0x06
#define PCI_CLASS_COMM              0x07
#define PCI_CLASS_SYSTEM            0x08
#define PCI_CLASS_INPUT             0x09
#define PCI_CLASS_DOCKING           0x0A
#define PCI_CLASS_PROCESSOR         0x0B
#define PCI_CLASS_SERIAL            0x0C
#define PCI_CLASS_WIRELESS          0x0D
#define PCI_CLASS_INTELLIGENT       0x0E
#define PCI_CLASS_SATELLITE         0x0F
#define PCI_CLASS_ENCRYPTION        0x10
#define PCI_CLASS_SIGNAL            0x11
#define PCI_CLASS_ACCELERATOR       0x12
#define PCI_CLASS_NON_ESSENTIAL     0x13
#define PCI_CLASS_UNDEFINED         0xFF

/* Storage subclasses */
#define PCI_SUBCLASS_IDE            0x01
#define PCI_SUBCLASS_FLOPPY         0x02
#define PCI_SUBCLASS_IPI            0x03
#define PCI_SUBCLASS_RAID           0x04
#define PCI_SUBCLASS_ATA            0x05
#define PCI_SUBCLASS_SATA           0x06
#define PCI_SUBCLASS_SAS            0x07
#define PCI_SUBCLASS_NVME           0x08
#define PCI_SUBCLASS_OTHER_STORAGE  0x80

/* Serial bus subclasses */
#define PCI_SUBCLASS_FIREWIRE       0x00
#define PCI_SUBCLASS_ACCESS         0x01
#define PCI_SUBCLASS_SSA            0x02
#define PCI_SUBCLASS_USB            0x03
#define PCI_SUBCLASS_FIBRE          0x04
#define PCI_SUBCLASS_SMBUS          0x05
#define PCI_SUBCLASS_INFINIBAND     0x06
#define PCI_SUBCLASS_IPMI           0x07
#define PCI_SUBCLASS_SERCOS         0x08
#define PCI_SUBCLASS_CANBUS         0x09

const char *pci_class_name(uint8_t class_code);
const char *pci_subclass_name(uint8_t class_code, uint8_t subclass);

#endif /* GOS_PCI_CLASS_H */
