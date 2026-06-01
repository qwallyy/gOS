/* gOS Kernel — ACPI (Advanced Configuration and Power Interface)
 * Reference: https://wiki.osdev.org/ACPI
 */

#ifndef GOS_ACPI_H
#define GOS_ACPI_H

#include "gOS/types.h"

#define ACPI_RSDP_SIGNATURE   "RSD PTR "
#define ACPI_RSDT_SIGNATURE   "RSDT"
#define ACPI_XSDT_SIGNATURE   "XSDT"
#define ACPI_MADT_SIGNATURE   "APIC"
#define ACPI_FADT_SIGNATURE   "FACP"
#define ACPI_HPET_SIGNATURE   "HPET"

typedef struct acpi_rsdp {
    char     signature[8];
    uint8_t  checksum;
    char     oem_id[6];
    uint8_t  revision;
    uint32_t rsdt_addr;
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t  ext_checksum;
    uint8_t  reserved[3];
} PACKED acpi_rsdp_t;

typedef struct acpi_sdt_header {
    char     signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oem_id[6];
    char     oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} PACKED acpi_sdt_header_t;

typedef struct acpi_madt {
    acpi_sdt_header_t header;
    uint32_t local_apic_addr;
    uint32_t flags;
} PACKED acpi_madt_t;

typedef struct acpi_fadt {
    acpi_sdt_header_t header;
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t  reserved0;
    uint8_t  preferred_pm_profile;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t  acpi_enable;
    uint8_t  acpi_disable;
    uint8_t  s4bios_req;
    uint8_t  pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_tmr_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t  pm1_evt_len;
    uint8_t  pm1_cnt_len;
    uint8_t  pm2_cnt_len;
    uint8_t  pm_tmr_len;
    uint8_t  gpe0_blk_len;
    uint8_t  gpe1_blk_len;
    uint8_t  gpe1_base;
    uint8_t  cst_cnt;
    uint16_t p_lvl2_lat;
    uint16_t p_lvl3_lat;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t  duty_offset;
    uint8_t  duty_width;
    uint8_t  day_alrm;
    uint8_t  mon_alrm;
    uint8_t  century;
    uint16_t iapc_boot_arch;
    uint8_t  reserved1;
    uint32_t flags;
    uint32_t reset_reg_addr;
    uint8_t  reset_reg_width;
    uint64_t reset_value;
    uint16_t arm_boot_arch;
    uint8_t  fadt_minor_version;
    uint64_t x_firmware_ctrl;
    uint64_t x_dsdt;
    uint64_t x_pm1a_evt_blk;
    uint64_t x_pm1b_evt_blk;
    uint64_t x_pm1a_cnt_blk;
    uint64_t x_pm1b_cnt_blk;
    uint64_t x_pm2_cnt_blk;
    uint64_t x_pm_tmr_blk;
    uint64_t x_gpe0_blk;
    uint64_t x_gpe1_blk;
} PACKED acpi_fadt_t;

void acpi_init(void);
bool acpi_find_rsdp(acpi_rsdp_t **rsdp);
acpi_sdt_header_t *acpi_find_table(const char *signature);
void acpi_dump_tables(void);

#endif /* GOS_ACPI_H */
