/* gOS Kernel — PCI Bus Enumeration Implementation
 * Reference: https://wiki.osdev.org/PCI
 */

#include "drivers/pci/pci.h"
#include "gOS/klog.h"

pci_device_t g_pci_devices[64];
size_t g_pci_device_count = 0;

uint32_t pci_read_config(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t addr = (uint32_t)(((uint32_t)bus << 16) |
                               ((uint32_t)dev << 11) |
                               ((uint32_t)func << 8) |
                               (offset & 0xFC) |
                               0x80000000);
    outl(PCI_CONFIG_ADDR, addr);
    io_wait();
    return inl(PCI_CONFIG_DATA);
}

void pci_write_config(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t val) {
    uint32_t addr = (uint32_t)(((uint32_t)bus << 16) |
                               ((uint32_t)dev << 11) |
                               ((uint32_t)func << 8) |
                               (offset & 0xFC) |
                               0x80000000);
    outl(PCI_CONFIG_ADDR, addr);
    io_wait();
    outl(PCI_CONFIG_DATA, val);
}

bool pci_probe_device(uint8_t bus, uint8_t dev, uint8_t func, pci_device_t *out) {
    uint32_t vendor_device = pci_read_config(bus, dev, func, PCI_VENDOR_ID);
    uint16_t vendor = vendor_device & 0xFFFF;

    if (vendor == PCI_VENDOR_INVALID || vendor == 0) {
        return false;
    }

    if (out) {
        out->bus = bus;
        out->device = dev;
        out->function = func;
        out->vendor_id = vendor;
        out->device_id = (vendor_device >> 16) & 0xFFFF;

        uint32_t class_rev = pci_read_config(bus, dev, func, PCI_REVISION_ID);
        out->revision = class_rev & 0xFF;
        out->prog_if = (class_rev >> 8) & 0xFF;
        out->subclass = (class_rev >> 16) & 0xFF;
        out->class_code = (class_rev >> 24) & 0xFF;

        uint32_t header = pci_read_config(bus, dev, func, PCI_HEADER_TYPE);
        out->header_type = header & 0x7F;

        uint32_t interrupt = pci_read_config(bus, dev, func, PCI_INTERRUPT_LINE);
        out->interrupt_line = interrupt & 0xFF;
        out->interrupt_pin = (interrupt >> 8) & 0xFF;

        for (int i = 0; i < 6; i++) {
            out->bar[i] = pci_read_config(bus, dev, func, PCI_BAR0 + i * 4);
        }
    }
    return true;
}

void pci_enumerate(void) {
    g_pci_device_count = 0;

    for (uint16_t bus = 0; bus < PCI_MAX_BUSES && g_pci_device_count < 64; bus++) {
        for (uint8_t dev = 0; dev < PCI_MAX_DEVICES && g_pci_device_count < 64; dev++) {
            if (!pci_probe_device((uint8_t)bus, dev, 0, &g_pci_devices[g_pci_device_count])) {
                continue;
            }
            g_pci_device_count++;

            /* Check for multi-function devices */
            uint32_t header = pci_read_config((uint8_t)bus, dev, 0, PCI_HEADER_TYPE);
            if ((header & 0x80) != 0) {
                for (uint8_t func = 1; func < PCI_MAX_FUNCTIONS && g_pci_device_count < 64; func++) {
                    if (pci_probe_device((uint8_t)bus, dev, func, &g_pci_devices[g_pci_device_count])) {
                        g_pci_device_count++;
                    }
                }
            }
        }
    }
}

void pci_init(void) {
    klog_info("Initializing PCI bus enumeration");
    pci_enumerate();
    klog_info("PCI: %zu devices found", g_pci_device_count);
    for (size_t i = 0; i < g_pci_device_count; i++) {
        klog_info("  [%02x:%02x.%x] Vendor=0x%04x Device=0x%04x Class=0x%02x Sub=0x%02x",
                  g_pci_devices[i].bus, g_pci_devices[i].device, g_pci_devices[i].function,
                  g_pci_devices[i].vendor_id, g_pci_devices[i].device_id,
                  g_pci_devices[i].class_code, g_pci_devices[i].subclass);
    }
}

const pci_device_t *pci_find_device(uint16_t vendor, uint16_t device) {
    for (size_t i = 0; i < g_pci_device_count; i++) {
        if (g_pci_devices[i].vendor_id == vendor && g_pci_devices[i].device_id == device) {
            return &g_pci_devices[i];
        }
    }
    return NULL;
}

const pci_device_t *pci_find_by_class(uint8_t class_code, uint8_t subclass) {
    for (size_t i = 0; i < g_pci_device_count; i++) {
        if (g_pci_devices[i].class_code == class_code && g_pci_devices[i].subclass == subclass) {
            return &g_pci_devices[i];
        }
    }
    return NULL;
}

uint64_t pci_get_bar_address(const pci_device_t *pdev, int bar) {
    if (!pdev || bar < 0 || bar > 5) return 0;
    uint32_t bar_val = pdev->bar[bar];
    if ((bar_val & 0x01) == 0) {
        /* Memory-mapped BAR */
        if ((bar_val & 0x06) == 0x04) {
            /* 64-bit BAR — read high part from next BAR */
            uint32_t bar_high = (bar + 1 < 6) ? pdev->bar[bar + 1] : 0;
            return ((uint64_t)bar_high << 32) | (bar_val & ~0x0F);
        }
        return bar_val & ~0x0F;
    }
    /* I/O BAR */
    return bar_val & ~0x03;
}

bool pci_is_mmio_bar(const pci_device_t *pdev, int bar) {
    if (!pdev || bar < 0 || bar > 5) return false;
    return (pdev->bar[bar] & 0x01) == 0;
}

void pci_dump_device(const pci_device_t *dev) {
    if (!dev) return;
    klog_debug("PCI Device %02x:%02x.%x", dev->bus, dev->device, dev->function);
    klog_debug("  Vendor: 0x%04x  Device: 0x%04x", dev->vendor_id, dev->device_id);
    klog_debug("  Class: 0x%02x  Subclass: 0x%02x  ProgIF: 0x%02x",
               dev->class_code, dev->subclass, dev->prog_if);
    for (int i = 0; i < 6; i++) {
        if (dev->bar[i]) {
            uint64_t addr = pci_get_bar_address(dev, i);
            bool mmio = pci_is_mmio_bar(dev, i);
            klog_debug("  BAR%d: 0x%08x -> %s 0x%016llx",
                       i, dev->bar[i], mmio ? "MMIO" : "IO", (unsigned long long)addr);
        }
    }
}

void pci_dump_devices(void) {
    for (size_t i = 0; i < g_pci_device_count; i++) {
        pci_dump_device(&g_pci_devices[i]);
    }
}
