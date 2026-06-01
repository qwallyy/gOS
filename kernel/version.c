/* gOS Kernel — Version Information Implementation */

#include "lib/version.h"
#include "gOS/klog.h"

void kernel_print_version(void) {
    klog_info("%s version %s", GOS_FULL_NAME, GOS_VERSION);
    klog_info("Build: %s %s", GOS_BUILD_DATE, GOS_BUILD_TIME);
    klog_info("Architecture: x86-64");
    klog_info("Compiler: GCC %s", __VERSION__);
}
