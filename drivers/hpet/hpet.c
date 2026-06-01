/* gOS Kernel — HPET Implementation
 * Reference: https://wiki.osdev.org/HPET
 */

#include "drivers/hpet/hpet.h"
#include "drivers/pci/pci.h"
#include "acpi/acpi.h"
#include "mm/vmm/vmm.h"
#include "gOS/klog.h"

static uint64_t g_hpet_base = 0;
static uint64_t g_hpet_period_fs = 0; /* femtoseconds per tick */

void hpet_init(void) {
    klog_info("HPET driver initializing");

    /* HPET is typically found via ACPI */
    /* Check if ACPI has an HPET table */
    extern acpi_sdt_header_t *acpi_find_table(const char *);
    acpi_sdt_header_t *hpet = acpi_find_table("HPET");
    if (!hpet) {
        klog_warn("HPET: no HPET table in ACPI");
        return;
    }

    /* HPET table structure: base address at offset 44 (ACPI 2.0) */
    uint64_t *base_ptr = (uint64_t *)((uintptr_t)hpet + 44);
    g_hpet_base = *base_ptr;
    if (!g_hpet_base) {
        klog_warn("HPET: base address is zero");
        return;
    }

    /* Map HPET registers */
    uint64_t aligned = g_hpet_base & ~0xFFFULL;
    vmm_map_page(aligned + KERNEL_VBASE, aligned,
                 PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE);
    g_hpet_base = aligned + KERNEL_VBASE;

    uint64_t cap_id = mmio_read64(g_hpet_base + HPET_GENERAL_CAP);
    g_hpet_period_fs = (cap_id >> 32) & 0xFFFFFFFF;

    uint16_t rev_id = cap_id & 0xFFFF;
    uint8_t num_timers = ((cap_id >> 8) & 0x1F) + 1;

    klog_info("HPET: rev=%u, timers=%u, period=%llu fs",
              rev_id, num_timers, (unsigned long long)g_hpet_period_fs);

    /* Enable HPET overall counter */
    uint64_t config = mmio_read64(g_hpet_base + HPET_GENERAL_CONFIG);
    config |= 1; /* Enable main counter */
    mmio_write64(g_hpet_base + HPET_GENERAL_CONFIG, config);
}

uint64_t hpet_get_counter(void) {
    if (!g_hpet_base) return 0;
    return mmio_read64(g_hpet_base + HPET_MAIN_COUNTER);
}

uint64_t hpet_get_frequency(void) {
    if (g_hpet_period_fs == 0) return 0;
    return 1000000000000000ULL / g_hpet_period_fs; /* Hz */
}

void hpet_sleep_us(uint64_t us) {
    if (!g_hpet_base || g_hpet_period_fs == 0) return;
    uint64_t freq = hpet_get_frequency();
    if (freq == 0) return;
    uint64_t ticks = (us * freq) / 1000000;
    uint64_t start = hpet_get_counter();
    while ((hpet_get_counter() - start) < ticks) {
        pause();
    }
}

bool hpet_is_available(void) {
    return g_hpet_base != 0;
}
