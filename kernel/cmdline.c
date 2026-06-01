/* gOS Kernel — Command Line Parser
 * Parses key=value pairs from the kernel command line passed by GRUB.
 */

#include "gOS/kernel.h"
#include "gOS/klog.h"
#include "lib/string.h"

static const char *g_cmdline = NULL;

void cmdline_parse(const char *cmdline) {
    if (!cmdline || !cmdline[0]) return;
    g_cmdline = cmdline;
    klog_info("Kernel command line: %s", cmdline);

    const char *p = cmdline;
    while (*p) {
        /* Skip leading spaces */
        while (*p == ' ') p++;
        if (!*p) break;

        /* Find end of token */
        const char *start = p;
        while (*p && *p != ' ') p++;
        size_t len = (size_t)(p - start);

        char token[256];
        if (len >= sizeof(token)) len = sizeof(token) - 1;
        memcpy(token, start, len);
        token[len] = '\0';

        /* Check for key=value */
        char *eq = strchr(token, '=');
        if (eq) {
            *eq = '\0';
            const char *key = token;
            const char *val = eq + 1;
            klog_debug("Cmdline arg: %s = %s", key, val);
        } else {
            klog_debug("Cmdline flag: %s", token);
        }
    }
}

bool cmdline_has_flag(const char *flag) {
    if (!g_cmdline) return false;
    const char *p = g_cmdline;
    size_t flag_len = strlen(flag);
    while (*p) {
        while (*p == ' ') p++;
        if (strncmp(p, flag, flag_len) == 0 && (p[flag_len] == ' ' || p[flag_len] == '\0')) {
            return true;
        }
        while (*p && *p != ' ') p++;
    }
    return false;
}

const char *cmdline_get_value(const char *key) {
    if (!g_cmdline) return NULL;
    const char *p = g_cmdline;
    size_t key_len = strlen(key);
    while (*p) {
        while (*p == ' ') p++;
        if (strncmp(p, key, key_len) == 0 && p[key_len] == '=') {
            return p + key_len + 1;
        }
        while (*p && *p != ' ') p++;
    }
    return NULL;
}
