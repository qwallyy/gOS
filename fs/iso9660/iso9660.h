/* gOS Kernel — ISO 9660 Filesystem
 * Reference: https://wiki.osdev.org/ISO_9660
 */

#ifndef GOS_ISO9660_H
#define GOS_ISO9660_H

#include "gOS/types.h"

#define ISO_SECTOR_SIZE     2048
#define ISO_VOLUME_DESC_OFF 0x8000  /* 32 * 2048 */

#define ISO_VD_PRIMARY      1
#define ISO_VD_SUPPLEMENT   2
#define ISO_VD_PARTITION    3
#define ISO_VD_TERMINATOR   255

#define ISO_FLAG_HIDDEN     0x01
#define ISO_FLAG_DIRECTORY  0x02
#define ISO_FLAG_ASSOCIATED 0x04

typedef struct iso9660_volume_desc {
    uint8_t  type;
    char     id[5];
    uint8_t  version;
    uint8_t  data[2041];
} PACKED iso9660_volume_desc_t;

typedef struct iso9660_primary {
    uint8_t  type;
    char     id[5];
    uint8_t  version;
    uint8_t  unused1;
    char     system_id[32];
    char     volume_id[32];
    uint8_t  unused2[8];
    uint32_t volume_space_size_le;
    uint32_t volume_space_size_be;
    uint8_t  unused3[32];
    uint16_t volume_set_size_le;
    uint16_t volume_set_size_be;
    uint16_t volume_seq_le;
    uint16_t volume_seq_be;
    uint16_t logical_block_size_le;
    uint16_t logical_block_size_be;
    uint32_t path_table_size_le;
    uint32_t path_table_size_be;
    uint32_t path_table_lba_le;
    uint32_t path_table_opt_lba_le;
    uint32_t path_table_lba_be;
    uint32_t path_table_opt_lba_be;
    uint8_t  root_dir[34];
    char     volume_set_id[128];
    char     publisher_id[128];
    char     preparer_id[128];
    char     application_id[128];
} PACKED iso9660_primary_t;

typedef struct iso9660_dir_entry {
    uint8_t  length;
    uint8_t  ext_attr_length;
    uint32_t extent_lba_le;
    uint32_t extent_lba_be;
    uint32_t data_length_le;
    uint32_t data_length_be;
    uint8_t  recording_date[7];
    uint8_t  file_flags;
    uint8_t  file_unit_size;
    uint8_t  gap_size;
    uint16_t vol_seq_le;
    uint16_t vol_seq_be;
    uint8_t  name_len;
    char     name[];
} PACKED iso9660_dir_entry_t;

void iso9660_init(void);
bool iso9660_probe(uint64_t start_lba);
void iso9660_list_root(void);

#endif /* GOS_ISO9660_H */
