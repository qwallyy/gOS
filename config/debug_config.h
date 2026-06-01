/* gOS Kernel — Debug Build Configuration
 * Enable extra diagnostics and tracing.
 */

#ifndef GOS_DEBUG_CONFIG_H
#define GOS_DEBUG_CONFIG_H

#define DEBUG_ENABLE_TRACING        1
#define DEBUG_ENABLE_ASSERTIONS     1
#define DEBUG_ENABLE_HEAP_CHECK     1
#define DEBUG_ENABLE_PANIC_DUMP     1
#define DEBUG_LOG_LEVEL             7  /* KLOG_DEBUG */

#define DEBUG_TRACE_INTERRUPTS      0
#define DEBUG_TRACE_SYSCALLS        1
#define DEBUG_TRACE_SCHEDULER       1
#define DEBUG_TRACE_MEMORY          1
#define DEBUG_TRACE_PCI             1

#define DEBUG_STOP_ON_WARN          0
#define DEBUG_STOP_ON_ERROR         1

#define DEBUG_ENABLE_VGA_LOG        1
#define DEBUG_ENABLE_SERIAL_LOG     1

#endif /* GOS_DEBUG_CONFIG_H */
