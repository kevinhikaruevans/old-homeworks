#include <fcntl.h>

int print_mtable() {
    printf("Mount table: \n");
    for(int i = 0; i < NMTABLE; i++) {
        if (mtable[i].dev) {
            printf("* %d\t%s\t%s\n", mtable[i].dev, mtable[i].devName, mtable[i].mntName);
        }
    }
}

int mtablealloc(char *pathname) {
    for(int i = 0;i < NMTABLE; i++) {
        if (mtable[i].dev == 0) {
            mtable[i].dev = open(pathname, O_RDWR);
            return i;
        }
    }

    return -1;
}
int mtabledalloc(int idx) {
    mtable[idx].dev = 0;

    return idx;
}

int mount(char *pathname, char *mount_point) {
    if (!strlen(pathname)) {
        return print_mtable();
    }

    // search if it's already mounted
    for(int i = 0; i < NMTABLE; i++) {
        if (mtable[i].dev) {
            //TODO: check devName
            if (strcmp(mtable[i].devName, pathname) == 0) {
                printf("already mounted!\n");
                return -1;
            }
        }
    }

    int idx = mtablealloc(pathname);
    MTABLE *mt = &mtable[idx];
    char buf[BLKSIZE];
    SUPER *super;

    get_block(mt->dev, 1, buf);
    super = (SUPER *)buf;

    if (super->s_magic != 0xEF53) {
        printf("magic number is invalid\n");
        return -1;
    }

    //////////////
    int ino = getino(mount_point);
    MINODE *mip = iget(dev, ino);

    if (!S_ISDIR(mip->INODE.i_mode) || mip->refCount > 1) {
        printf("invalid mode or is busy\n");
        mtabledalloc(idx);
        return -1;
    }

    strcpy(mt->mntName, mount_point);
    strcpy(mt->devName, pathname);
    //mt->dev = dev;
    mt->ninodes = super->s_inodes_count;
    mt->nblocks = super->s_blocks_count;

    get_block(mt->dev, 2, buf);
    GD *gp = (GD *)buf;
    mt->bmap = gp->bg_block_bitmap;
    mt->imap = gp->bg_inode_bitmap;
    mt->iblock = gp->bg_inode_table;
    mt->mntDirPtr = mip;
    //mip->dev = mt->dev;
    mip->mounted = 1;
    mip->mptr = mt;
    
    //mt->bmap
    // mip->mptr = mt;
    // mip->dev = dev;
}

int umount(char *filesys) {
    int idx = -1;
    // int ino = getino(filesys);
    // MINODE *mip = iget(dev, ino);
    for(int i = 0; i < NMTABLE; i++) {
        if (strcmp(mtable[i].mntName, filesys) == 0) {
            idx = i;
            break;
        }
    }
    //iput(mip);

    if (idx == -1) {
        printf("invalid mount point\n");
        return -1;
    }

    int dev = mtable[idx].dev;
    for(int i = 0; i < NMTABLE; i++) {
        if (minode[i].dev == dev && minode[i].refCount) {
            printf("cannot umount, device is busy\n");
            return -1;
        }
    }
    MINODE *mip = mtable[idx].mntDirPtr;
    mip->mounted = 0;
    mip->mptr = NULL;
    iput(mip);

    close(mtable[idx].dev);
    mtable[idx].dev = 0;

    return 0;
}