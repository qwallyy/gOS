/* gOS Kernel — ext2 Filesystem Driver (stub)
 * Reference: https://wiki.osdev.org/Ext2
 */

#ifndef GOS_EXT2_H
#define GOS_EXT2_H

#include "gOS/types.h"
#include "fs/vfs/vfs.h"

#define EXT2_SUPERBLOCK_OFFSET    1024
#define EXT2_MAGIC                0xEF53

typedef struct ext2_superblock {
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t r_blocks_count;
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t first_data_block;
    uint32_t log_block_size;
    uint32_t log_frag_size;
    uint32_t blocks_per_group;
    uint32_t frags_per_group;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;
    uint16_t mnt_count;
    uint16_t max_mnt_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;
    uint16_t def_resuid;
    uint16_t def_resgid;
} PACKED ext2_superblock_t;

typedef struct ext2_inode {
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks;
    uint32_t flags;
    uint32_t osd1;
    uint32_t block[15];
    uint32_t generation;
    uint32_t file_acl;
    uint32_t dir_acl;
    uint32_t faddr;
    uint32_t osd2[3];
} PACKED ext2_inode_t;

typedef struct ext2_dir_entry {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t  name_len;
    uint8_t  file_type;
    char     name[];
} PACKED ext2_dir_entry_t;

void ext2_init(void);
bool ext2_mount(uint32_t partition_lba, vfs_ops_t **ops_out);

#endif /* GOS_EXT2_H */
