/* gOS Kernel — Security Subsystem Implementation
 * Reference: https://wiki.osdev.org/Security
 */

#include "security/security.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"

static security_context_t g_kernel_context = {
    .uid = 0,
    .gid = 0,
    .euid = 0,
    .egid = 0,
    .capabilities = 0xFFFFFFFFFFFFFFFFULL,
    .privileged = true,
};

static security_context_t *g_current_security = &g_kernel_context;

void security_init(void) {
    klog_info("Security subsystem initialized (kernel mode, all caps)");
}

bool security_check_permission(uint32_t required_cap) {
    if (!g_current_security) return false;
    if (g_current_security->privileged) return true;
    return (g_current_security->capabilities & (1ULL << required_cap)) != 0;
}

void security_set_context(security_context_t *ctx) {
    g_current_security = ctx;
}

security_context_t *security_get_current_context(void) {
    return g_current_security;
}

bool security_is_privileged(void) {
    return g_current_security && g_current_security->privileged;
}

void security_drop_privileges(void) {
    if (g_current_security) {
        g_current_security->privileged = false;
        klog_info("Privileges dropped for current context");
    }
}

void security_raise_privileges(void) {
    if (g_current_security) {
        g_current_security->privileged = true;
        klog_info("Privileges raised for current context");
    }
}

void security_dump_context(void) {
    if (!g_current_security) {
        klog_info("No active security context");
        return;
    }
    klog_info("Security context: uid=%u gid=%u euid=%u egid=%u caps=0x%016llx privileged=%d",
              g_current_security->uid,
              g_current_security->gid,
              g_current_security->euid,
              g_current_security->egid,
              (unsigned long long)g_current_security->capabilities,
              g_current_security->privileged);
}
