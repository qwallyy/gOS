/* gOS Kernel — Build-Time Configuration
 * Adjust these defines before compiling to enable/disable features.
 */

#ifndef GOS_KERNEL_CONFIG_H
#define GOS_KERNEL_CONFIG_H

/* Version */
#define CONFIG_GOS_VERSION_MAJOR    0
#define CONFIG_GOS_VERSION_MINOR    1
#define CONFIG_GOS_VERSION_PATCH    0

/* Build options */
#define CONFIG_ENABLE_ASSERTIONS    1
#define CONFIG_ENABLE_DEBUG_LOG     1
#define CONFIG_ENABLE_UNIT_TESTS    1
#define CONFIG_ENABLE_INTEGRATION_TESTS 1

/* Memory */
#define CONFIG_PMM_MAX_MEMORY_MB    4096
#define CONFIG_HEAP_INITIAL_SIZE    (2 * 1024 * 1024)
#define CONFIG_HEAP_MAX_SIZE        (64 * 1024 * 1024)
#define CONFIG_STACK_SIZE           (16 * 1024)

/* Scheduler */
#define CONFIG_SCHED_HZ             100
#define CONFIG_SCHED_TIME_SLICE_MS  10
#define CONFIG_MAX_PROCESSES        256

/* Drivers */
#define CONFIG_DRIVER_SERIAL        1
#define CONFIG_DRIVER_VGA_TEXT      1
#define CONFIG_DRIVER_VGA_FB        1
#define CONFIG_DRIVER_KEYBOARD      1
#define CONFIG_DRIVER_MOUSE         1
#define CONFIG_DRIVER_RTC           1
#define CONFIG_DRIVER_PIT           1
#define CONFIG_DRIVER_HPET          1
#define CONFIG_DRIVER_PCI           1
#define CONFIG_DRIVER_ATA           1
#define CONFIG_DRIVER_AHCI          1
#define CONFIG_DRIVER_E1000         1
#define CONFIG_DRIVER_RTL8139       1

/* Filesystem */
#define CONFIG_FS_VFS               1
#define CONFIG_FS_FAT32             1

/* Network */
#define CONFIG_NET_ETHERNET         1
#define CONFIG_NET_ARP              1
#define CONFIG_NET_IP               1
#define CONFIG_NET_ICMP             1
#define CONFIG_NET_UDP              1
#define CONFIG_NET_TCP              0  /* TCP stub only */

/* ACPI */
#define CONFIG_ACPI_PARSE           1
#define CONFIG_ACPI_MADT            1
#define CONFIG_ACPI_HPET            1

/* Security */
#define CONFIG_SECURITY             1

/* IPC */
#define CONFIG_IPC_MESSAGES         1

/* SMP */
#define CONFIG_SMP                  0  /* Not yet implemented */
#define CONFIG_MAX_CPUS             1

#endif /* GOS_KERNEL_CONFIG_H */
