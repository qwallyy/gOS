/* gOS Kernel — Release Build Configuration
 * Disable diagnostics for smaller, faster kernel.
 */

#ifndef GOS_RELEASE_CONFIG_H
#define GOS_RELEASE_CONFIG_H

#define DEBUG_ENABLE_TRACING        0
#define DEBUG_ENABLE_ASSERTIONS     0
#define DEBUG_ENABLE_HEAP_CHECK     0
#define DEBUG_ENABLE_PANIC_DUMP     1
#define DEBUG_LOG_LEVEL             5  /* KLOG_NOTICE */

#define DEBUG_TRACE_INTERRUPTS      0
#define DEBUG_TRACE_SYSCALLS        0
#define DEBUG_TRACE_SCHEDULER       0
#define DEBUG_TRACE_MEMORY          0
#define DEBUG_TRACE_PCI             0

#define DEBUG_STOP_ON_WARN          0
#define DEBUG_STOP_ON_ERROR         0

#define DEBUG_ENABLE_VGA_LOG        0
#define DEBUG_ENABLE_SERIAL_LOG     1

#endif /* GOS_RELEASE_CONFIG_H */
