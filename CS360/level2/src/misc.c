struct stat mystat(char *filename)
{
    int ino = getino(filename);
    MINODE *mip = iget(dev, ino);

    struct stat myst = {
        .st_dev = dev,
        .st_ino = ino,
        .st_mode = mip->INODE.i_mode,

        .st_size = mip->INODE.i_size,

        .st_ctim = mip->INODE.i_ctime,
        .st_atim = mip->INODE.i_atime,
        .st_mtim = mip->INODE.i_mtime,

        .st_uid = mip->INODE.i_uid,
        .st_gid = mip->INODE.i_gid};

    iput(mip);
    return myst;
}

int chmod_path(char *mode, char *filename) {
    int imode = strtol(mode, NULL, mode[0] == '0' ? 8 : 10);
    printf("chmod called with %d \n", mode);
    int ino = getino(filename);
    MINODE *mip = iget(dev, ino);
    mip->INODE.i_mode |= imode;
    mip->dirty = 1;
    iput(mip);
    return 0;
}

int utime(char *filename) {
    int ino = getino(filename);
    MINODE *mip = iget(dev, ino);
    mip->INODE.i_atime = time(NULL);
    mip->dirty = 1;
    iput(mip);
    return 0;
}