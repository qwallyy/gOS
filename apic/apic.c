/* gOS Kernel — Local APIC and I/O APIC Implementation
 * Reference: https://wiki.osdev.org/APIC
 *            https://wiki.osdev.org/IOAPIC
 */

#include "apic/apic.h"
#include "hal/cpuid/cpuid.h"
#include "mm/vmm/vmm.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

static uintptr_t g_lapic_base = 0;
static uintptr_t g_ioapic_base = 0;

static inline uint32_t lapic_mmio_read(uint32_t reg) {
    return mmio_read32(g_lapic_base + reg);
}

static inline void lapic_mmio_write(uint32_t reg, uint32_t val) {
    mmio_write32(g_lapic_base + reg, val);
}

void lapic_init(void) {
    klog_info("Initializing Local APIC");

    if (!g_cpu_info.has_apic) {
        klog_warn("LAPIC: CPU does not support APIC");
        return;
    }

    /* Get LAPIC base from MSR */
    g_lapic_base = cpu_get_lapic_base();
    if (!g_lapic_base) {
        klog_warn("LAPIC: base address is zero");
        return;
    }

    /* Map LAPIC base into virtual memory (page-aligned) */
    uint64_t aligned = g_lapic_base & ~0xFFFULL;
    vmm_map_page(aligned + KERNEL_VBASE, aligned,
                 PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE);
    g_lapic_base = aligned + KERNEL_VBASE;

    /* Enable LAPIC by setting bit 8 of SVR */
    uint32_t svr = lapic_mmio_read(LAPIC_SIVR);
    svr |= LAPIC_SVR_ENABLE;
    lapic_mmio_write(LAPIC_SIVR, svr);

    klog_info("LAPIC: base=0x%016llx, ID=%u, Version=0x%08x",
              (unsigned long long)g_lapic_base,
              lapic_get_id(), lapic_mmio_read(LAPIC_VERSION));
}

void lapic_eoi(void) {
    if (g_lapic_base) {
        lapic_mmio_write(LAPIC_EOI, 0);
    }
}

uint32_t lapic_read(uint32_t reg) {
    if (!g_lapic_base) return 0;
    return lapic_mmio_read(reg);
}

void lapic_write(uint32_t reg, uint32_t val) {
    if (!g_lapic_base) return;
    lapic_mmio_write(reg, val);
}

uint32_t lapic_get_id(void) {
    return (lapic_mmio_read(LAPIC_ID) >> 24) & 0xFF;
}

void lapic_send_ipi(uint32_t dest, uint32_t vector) {
    if (!g_lapic_base) return;
    /* Write high destination first, then low command */
    lapic_mmio_write(LAPIC_ICR_HI, dest << 24);
    lapic_mmio_write(LAPIC_ICR_LO, vector | 0x4000); /* Assert, fixed delivery */
    while (lapic_mmio_read(LAPIC_ICR_LO) & (1U << 12)); /* Wait for delivery */
}

void lapic_timer_calibrate(void) {
    if (!g_lapic_base) return;
    klog_info("LAPIC timer: calibration not implemented (using PIT)");
}

void lapic_timer_start(uint32_t vector, uint32_t period_us) {
    (void)vector; (void)period_us;
    klog_warn("LAPIC timer start: stub");
}

void lapic_timer_stop(void) {
    if (!g_lapic_base) return;
    lapic_mmio_write(LAPIC_LVT_TIMER, LAPIC_LVT_MASKED);
}

/* ------------------------------------------------------------------ */
/* I/O APIC                                                           */
/* ------------------------------------------------------------------ */

static inline uint32_t ioapic_read_reg(uint32_t reg) {
    mmio_write32(g_ioapic_base, reg);
    return mmio_read32(g_ioapic_base + 0x10);
}

static inline void ioapic_write_reg(uint32_t reg, uint32_t val) {
    mmio_write32(g_ioapic_base, reg);
    mmio_write32(g_ioapic_base + 0x10, val);
}

void ioapic_init(void) {
    klog_info("Initializing I/O APIC");

    /* I/O APIC is typically found via ACPI MADT table */
    /* For now, try a common default address */
    g_ioapic_base = 0xFEC00000;
    vmm_map_page(g_ioapic_base + KERNEL_VBASE, g_ioapic_base,
                 PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE);
    g_ioapic_base += KERNEL_VBASE;

    uint32_t id_ver = ioapic_read_reg(IOAPIC_VERSION);
    uint8_t version = id_ver & 0xFF;
    uint8_t max_redir = (id_ver >> 16) & 0xFF;

    klog_info("IOAPIC: version=%u, max_redirection=%u", version, max_redir);

    /* Mask all entries initially */
    for (uint32_t i = 0; i <= max_redir; i++) {
        ioapic_write_reg(IOAPIC_REDTBL + i * 2, 0x00010000); /* Masked */
        ioapic_write_reg(IOAPIC_REDTBL + i * 2 + 1, 0);
    }
}

void ioapic_set_irq(uint8_t irq, uint8_t vector, uint32_t dest_apic_id) {
    if (!g_ioapic_base) return;
    uint32_t low = vector;
    uint32_t high = dest_apic_id << 24;
    ioapic_write_reg(IOAPIC_REDTBL + irq * 2, low);
    ioapic_write_reg(IOAPIC_REDTBL + irq * 2 + 1, high);
}

uint32_t ioapic_read(uint32_t reg) {
    if (!g_ioapic_base) return 0;
    return ioapic_read_reg(reg);
}

void ioapic_write(uint32_t reg, uint32_t val) {
    if (!g_ioapic_base) return;
    ioapic_write_reg(reg, val);
}
