/* gOS Kernel — Known PCI Vendor/Device IDs
 * Reference: https://wiki.osdev.org/PCI
 */

#ifndef GOS_PCI_IDS_H
#define GOS_PCI_IDS_H

/* Intel */
#define PCI_VENDOR_INTEL            0x8086
#define PCI_DEVICE_INTEL_E1000      0x100E
#define PCI_DEVICE_INTEL_E1000_2  0x10EA
#define PCI_DEVICE_INTEL_UHCI       0x7020
#define PCI_DEVICE_INTEL_EHCI       0x268C
#define PCI_DEVICE_INTEL_XHCI       0x8C31
#define PCI_DEVICE_INTEL_AHCI       0x2829

/* Realtek */
#define PCI_VENDOR_REALTEK          0x10EC
#define PCI_DEVICE_REALTEK_RTL8139  0x8139
#define PCI_DEVICE_REALTEK_RTL8169 0x8168

/* AMD */
#define PCI_VENDOR_AMD              0x1022
#define PCI_VENDOR_ATI              0x1002

/* NVIDIA */
#define PCI_VENDOR_NVIDIA           0x10DE

/* QEMU */
#define PCI_VENDOR_QEMU             0x1234
#define PCI_DEVICE_QEMU_VGA         0x1111
#define PCI_DEVICE_QEMU_BALLOON     0x1112

/* VirtIO */
#define PCI_VENDOR_VIRTIO           0x1AF4
#define PCI_DEVICE_VIRTIO_NET       0x1000
#define PCI_DEVICE_VIRTIO_BLOCK     0x1001
#define PCI_DEVICE_VIRTIO_CONSOLE   0x1003

/* VMware */
#define PCI_VENDOR_VMWARE           0x15AD

/* Broadcom */
#define PCI_VENDOR_BROADCOM         0x14E4

/* Atheros */
#define PCI_VENDOR_ATHEROS          0x168C

#endif /* GOS_PCI_IDS_H */
