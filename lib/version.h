/* gOS Kernel — Version Information */

#ifndef GOS_VERSION_H
#define GOS_VERSION_H

#define GOS_NAME        "gOS"
#define GOS_VERSION     "0.1.0-dev"
#define GOS_FULL_NAME   "gOS Kernel"
#define GOS_COPYRIGHT   "Copyright (C) gOS Project"
#define GOS_LICENSE     "Educational"

#define GOS_BUILD_DATE  __DATE__
#define GOS_BUILD_TIME  __TIME__

static inline const char *gos_version_string(void) {
    return GOS_VERSION;
}

static inline const char *gos_name_string(void) {
    return GOS_NAME;
}

#endif /* GOS_VERSION_H */
