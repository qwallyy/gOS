/* gOS Kernel — Security Subsystem
 * Reference: https://wiki.osdev.org/Security
 */

#ifndef GOS_SECURITY_H
#define GOS_SECURITY_H

#include "gOS/types.h"

#define SECURITY_MAX_CAPS   64

typedef struct security_context {
    uint32_t uid;
    uint32_t gid;
    uint32_t euid;
    uint32_t egid;
    uint64_t capabilities;
    bool     privileged;
} security_context_t;

void security_init(void);
bool security_check_permission(uint32_t required_cap);
void security_set_context(security_context_t *ctx);
security_context_t *security_get_current_context(void);
bool security_is_privileged(void);
void security_drop_privileges(void);
void security_raise_privileges(void);
void security_dump_context(void);

#endif /* GOS_SECURITY_H */
