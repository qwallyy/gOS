/* gOS Kernel — AHCI Implementation
 * Reference: https://wiki.osdev.org/AHCI
 */

#include "drivers/ahci/ahci.h"
#include "mm/vmm/vmm.h"
#include "mm/pmm/pmm.h"
#include "gOS/klog.h"

void ahci_init(void) {
    klog_info("AHCI driver initializing");

    const pci_device_t *ahci_dev = pci_find_by_class(0x01, 0x06);
    if (!ahci_dev) {
        klog_warn("AHCI: no SATA AHCI controller found");
        return;
    }

    klog_info("AHCI: found controller at %02x:%02x.%x",
              ahci_dev->bus, ahci_dev->device, ahci_dev->function);

    uint64_t bar5 = pci_get_bar_address(ahci_dev, 5);
    if (!bar5) {
        klog_warn("AHCI: BAR5 is empty");
        return;
    }

    /* Map BAR5 if MMIO */
    if (pci_is_mmio_bar(ahci_dev, 5)) {
        /* Map 4KB for HBA registers */
        vmm_map_page(bar5, bar5, PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE);
        klog_info("AHCI: HBA mapped at 0x%016llx", (unsigned long long)bar5);
    }

    uint32_t cap = mmio_read32(bar5 + AHCI_HBA_CAP);
    uint32_t ports = mmio_read32(bar5 + AHCI_HBA_PI);
    klog_info("AHCI: CAP=0x%08x, PI=0x%08x, ports=%u",
              cap, ports, (cap & 0x1F) + 1);

    /* Enable AHCI mode */
    uint32_t ghc = mmio_read32(bar5 + AHCI_HBA_GHC);
    if (!(ghc & AHCI_HBA_GHC_AE)) {
        mmio_write32(bar5 + AHCI_HBA_GHC, ghc | AHCI_HBA_GHC_AE);
        klog_info("AHCI: global host control enabled");
    }
}

bool ahci_probe_port(const pci_device_t *pci_dev) {
    (void)pci_dev;
    klog_warn("AHCI port probing: not yet implemented");
    return false;
}
