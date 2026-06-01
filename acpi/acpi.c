/* gOS Kernel — ACPI Implementation
 * Reference: https://wiki.osdev.org/ACPI
 */

#include "acpi/acpi.h"
#include "gOS/klog.h"
#include "mm/vmm/vmm.h"
#include "lib/string.h"

static acpi_rsdp_t *g_rsdp = NULL;
static uint64_t g_rsdt_phys = 0;

void acpi_init(void) {
    klog_info("ACPI: initializing");

    if (!acpi_find_rsdp(&g_rsdp)) {
        klog_warn("ACPI: RSDP not found");
        return;
    }

    klog_info("ACPI: RSDP found at %p", (void *)g_rsdp);
    klog_info("ACPI: OEM='%s' Revision=%u",
              g_rsdp->oem_id, g_rsdp->revision);

    if (g_rsdp->revision >= 2 && g_rsdp->xsdt_addr) {
        g_rsdt_phys = g_rsdp->xsdt_addr;
        klog_info("ACPI: Using XSDT at 0x%016llx", (unsigned long long)g_rsdt_phys);
    } else {
        g_rsdt_phys = g_rsdp->rsdt_addr;
        klog_info("ACPI: Using RSDT at 0x%016llx", (unsigned long long)g_rsdt_phys);
    }

    /* Map the RSDT/XSDT page if needed */
    if (g_rsdt_phys < 0x100000) {
        /* Already identity mapped */
    } else {
        vmm_map_page(g_rsdt_phys, g_rsdt_phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE);
    }

    acpi_dump_tables();
}

bool acpi_find_rsdp(acpi_rsdp_t **rsdp) {
    /* Search in the EBDA (first 1KB at 0x80000) and BIOS area (0xE0000-0xFFFFF) */
    for (uintptr_t addr = 0x80000; addr < 0xA0000; addr += 16) {
        if (memcmp((void *)addr, ACPI_RSDP_SIGNATURE, 8) == 0) {
            *rsdp = (acpi_rsdp_t *)addr;
            return true;
        }
    }
    for (uintptr_t addr = 0xE0000; addr < 0x100000; addr += 16) {
        if (memcmp((void *)addr, ACPI_RSDP_SIGNATURE, 8) == 0) {
            *rsdp = (acpi_rsdp_t *)addr;
            return true;
        }
    }
    return false;
}

acpi_sdt_header_t *acpi_find_table(const char *signature) {
    if (!g_rsdt_phys) return NULL;

    acpi_sdt_header_t *rsdt = (acpi_sdt_header_t *)g_rsdt_phys;
    uint32_t num_entries = (rsdt->length - sizeof(acpi_sdt_header_t)) / 4;
    uint32_t *entries = (uint32_t *)((uintptr_t)rsdt + sizeof(acpi_sdt_header_t));

    for (uint32_t i = 0; i < num_entries; i++) {
        uint32_t table_phys = entries[i];
        if (!table_phys) continue;
        acpi_sdt_header_t *hdr = (acpi_sdt_header_t *)(uintptr_t)table_phys;
        if (memcmp(hdr->signature, signature, 4) == 0) {
            return hdr;
        }
    }
    return NULL;
}

void acpi_dump_tables(void) {
    if (!g_rsdt_phys) return;

    acpi_sdt_header_t *rsdt = (acpi_sdt_header_t *)g_rsdt_phys;
    uint32_t num_entries = (rsdt->length - sizeof(acpi_sdt_header_t)) / 4;
    uint32_t *entries = (uint32_t *)((uintptr_t)rsdt + sizeof(acpi_sdt_header_t));

    klog_info("ACPI tables (%u entries):", num_entries);
    for (uint32_t i = 0; i < num_entries; i++) {
        uint32_t table_phys = entries[i];
        if (!table_phys) continue;
        acpi_sdt_header_t *hdr = (acpi_sdt_header_t *)(uintptr_t)table_phys;
        klog_info("  %c%c%c%c at 0x%08x len=%u rev=%u",
                  hdr->signature[0], hdr->signature[1],
                  hdr->signature[2], hdr->signature[3],
                  table_phys, hdr->length, hdr->revision);
    }
}
