#ifndef PARTITION_H
#define PARTITION_H

#define MBR_PARTITION_COUNT  4
#define PARTITION_TYPE_EXT   5
#define MBR_TABLE_OFFSET     0x1BE

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct partition {
	u8 drive;             /* drive number FD=0, HD=0x80, etc. */

	u8  head;             /* starting head */
	u8  sector;           /* starting sector */
	u8  cylinder;         /* starting cylinder */

	u8  sys_type;         /* partition type: NTFS, LINUX, etc. */

	u8  end_head;         /* end head */
	u8  end_sector;       /* end sector */
	u8  end_cylinder;     /* end cylinder */

	u32 start_sector;     /* starting sector counting from 0 */
	u32 nr_sectors;       /* number of of sectors in partition */
};

const char *get_strtype(int sys_type)
{
    switch (sys_type)
    {
    case 0x07:
        return "HPFS/NTFS/exFAT";
    case 0x83:
        return "Linux";
    case 0x0C:
        return "W95 FAT32 (LBA)";
    case 0x05:
        return "Extended";
    case 0x06:
        return "FAT16";
    case 0x82:
        return "Linux swap";
    default:
        return "Unknown";
    }
}

#endif