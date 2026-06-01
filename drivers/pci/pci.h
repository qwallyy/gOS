/* gOS Kernel — PCI Bus Enumeration
 * Reference: https://wiki.osdev.org/PCI
 */

#ifndef GOS_PCI_H
#define GOS_PCI_H

#include "gOS/types.h"

#define PCI_CONFIG_ADDR     0xCF8
#define PCI_CONFIG_DATA     0xCFC

#define PCI_MAX_BUSES       256
#define PCI_MAX_DEVICES     32
#define PCI_MAX_FUNCTIONS   8

/* PCI config space offsets */
#define PCI_VENDOR_ID       0x00
#define PCI_DEVICE_ID       0x02
#define PCI_COMMAND         0x04
#define PCI_STATUS          0x06
#define PCI_REVISION_ID     0x08
#define PCI_PROG_IF         0x09
#define PCI_SUBCLASS        0x0A
#define PCI_CLASS           0x0B
#define PCI_CACHE_LINE      0x0C
#define PCI_LATENCY         0x0D
#define PCI_HEADER_TYPE     0x0E
#define PCI_BIST            0x0F
#define PCI_BAR0            0x10
#define PCI_BAR1            0x14
#define PCI_BAR2            0x18
#define PCI_BAR3            0x1C
#define PCI_BAR4            0x20
#define PCI_BAR5            0x24
#define PCI_SUBSYS_VENDOR   0x2C
#define PCI_SUBSYS_ID       0x2E
#define PCI_ROM_BASE        0x30
#define PCI_CAPABILITIES    0x34
#define PCI_INTERRUPT_LINE  0x3C
#define PCI_INTERRUPT_PIN   0x3D

/* Header types */
#define PCI_HEADER_NORMAL   0x00
#define PCI_HEADER_BRIDGE   0x01
#define PCI_HEADER_CARDBUS  0x02

/* Vendor IDs */
#define PCI_VENDOR_INTEL    0x8086
#define PCI_VENDOR_AMD      0x1022
#define PCI_VENDOR_NVIDIA   0x10DE
#define PCI_VENDOR_REALTEK  0x10EC
#define PCI_VENDOR_QEMU     0x1234
#define PCI_VENDOR_BOCHS    0x1234
#define PCI_VENDOR_VIRTIO   0x1AF4

#define PCI_VENDOR_INVALID  0xFFFF

typedef struct pci_device {
    uint8_t  bus;
    uint8_t  device;
    uint8_t  function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  prog_if;
    uint8_t  revision;
    uint8_t  header_type;
    uint8_t  interrupt_line;
    uint8_t  interrupt_pin;
    uint32_t bar[6];
} pci_device_t;

void pci_init(void);
uint32_t pci_read_config(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t val);
bool pci_probe_device(uint8_t bus, uint8_t dev, uint8_t func, pci_device_t *out);
void pci_enumerate(void);
const pci_device_t *pci_find_device(uint16_t vendor, uint16_t device);
const pci_device_t *pci_find_by_class(uint8_t class_code, uint8_t subclass);
uint64_t pci_get_bar_address(const pci_device_t *pdev, int bar);
bool pci_is_mmio_bar(const pci_device_t *pdev, int bar);
void pci_dump_device(const pci_device_t *dev);

extern pci_device_t g_pci_devices[64];
extern size_t g_pci_device_count;

#endif /* GOS_PCI_H */
