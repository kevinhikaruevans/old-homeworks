#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

#include <sys/types.h>
#include <unistd.h>

#define BLKSIZE 1024

typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD *gp;
INODE *ip;
DIR *dp;

char buf[BLKSIZE];
int fd, firstdata, inodesize, blksize, iblock;
char *dev = "vdisk";

int get_block(int fd, int blk, char *buf) {
    lseek(fd, blk*BLKSIZE, SEEK_SET);
    return read(fd, buf, BLKSIZE);
}

int inode(char *dev) {
    int i;
    fd = open(dev, O_RDONLY);

    if (fd < 0) {
        printf("open failed\n");
        exit(1);
    }

    int r = get_block(fd, 2, buf);
    gp = (GD *)buf;

    printf("bmap block = %d, imap_block = %d, inodes tbl = %d\n",
        gp->bg_block_bitmap,
        gp->bg_inode_bitmap,
        gp->bg_inode_table);

    iblock = gp->bg_inode_table; 

    close(fd);
}

int main() {
    inode(dev);
}