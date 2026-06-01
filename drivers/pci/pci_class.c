/* gOS Kernel — PCI Class Name Lookup */

#include "drivers/pci/pci_class.h"

const char *pci_class_name(uint8_t class_code) {
    switch (class_code) {
        case PCI_CLASS_OLD:         return "Old Device";
        case PCI_CLASS_STORAGE:     return "Mass Storage";
        case PCI_CLASS_NETWORK:     return "Network";
        case PCI_CLASS_DISPLAY:     return "Display";
        case PCI_CLASS_MULTIMEDIA: return "Multimedia";
        case PCI_CLASS_MEMORY:      return "Memory";
        case PCI_CLASS_BRIDGE:      return "Bridge";
        case PCI_CLASS_COMM:        return "Communications";
        case PCI_CLASS_SYSTEM:      return "System";
        case PCI_CLASS_INPUT:       return "Input";
        case PCI_CLASS_SERIAL:      return "Serial";
        case PCI_CLASS_WIRELESS:    return "Wireless";
        case PCI_CLASS_ENCRYPTION:  return "Encryption";
        default:                    return "Unknown";
    }
}

const char *pci_subclass_name(uint8_t class_code, uint8_t subclass) {
    if (class_code == PCI_CLASS_STORAGE) {
        switch (subclass) {
            case PCI_SUBCLASS_IDE:      return "IDE";
            case PCI_SUBCLASS_SATA:     return "SATA";
            case PCI_SUBCLASS_NVME:     return "NVMe";
            default:                    return "Other Storage";
        }
    }
    if (class_code == PCI_CLASS_SERIAL) {
        switch (subclass) {
            case PCI_SUBCLASS_USB:      return "USB";
            case PCI_SUBCLASS_FIREWIRE: return "FireWire";
            default:                    return "Other Serial";
        }
    }
    if (class_code == PCI_CLASS_NETWORK) {
        return "Ethernet/Network";
    }
    return "Unknown";
}
