/* gOS Kernel — TAR Archive Parser
 * Reference: https://wiki.osdev.org/Tar
 */

#ifndef GOS_TAR_H
#define GOS_TAR_H

#include "gOS/types.h"

#define TAR_BLOCK_SIZE      512
#define TAR_NAME_SIZE       100
#define TAR_MAGIC           "ustar"

typedef struct tar_header {
    char name[TAR_NAME_SIZE];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
} PACKED tar_header_t;

#define TAR_TYPE_REGULAR    '0'
#define TAR_TYPE_LINK       '1'
#define TAR_TYPE_SYMLINK    '2'
#define TAR_TYPE_CHAR       '3'
#define TAR_TYPE_BLOCK      '4'
#define TAR_TYPE_DIR        '5'

void tar_init(uintptr_t start, size_t size);
bool tar_is_valid(const tar_header_t *header);
size_t tar_get_size(const tar_header_t *header);
const tar_header_t *tar_find(const char *name);
void tar_list(void);

#endif /* GOS_TAR_H */
