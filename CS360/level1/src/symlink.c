char * my_readlink(char *pathname) {
    static char buffer[60];
    int ino = getino(pathname);
    MINODE *mip = iget(dev, ino);
    strncpy(buffer, mip->INODE.i_block, mip->INODE.i_size);
    buffer[mip->INODE.i_size] = 0;

    iput(mip);
    return buffer;
}


int symlink(char *oldname, char *newname) {
    printf("symlink: '%s' <- '%s'\n", oldname, newname);

    int oldino = getino(oldname);
    if (!oldino) {
        printf("thing '%s' does not exist\n", oldname);
        return -1;
    }
    MINODE *oldmip = iget(dev, oldino);

    if (!S_ISREG(oldmip->INODE.i_mode) && !S_ISDIR(oldmip->INODE.i_mode)) {
        iput(oldmip);
        printf("cannot create link, not a file or dir\n");
        return -1;
    }

    iput(oldmip);

    creat_file(newname);
    int ino = getino(newname);
    MINODE *mip = iget(dev, ino);
    mip->dirty = 1;
    mip->INODE.i_mode = 0xA1FF;
    //printf("inode block 0 = %d\n", mip->INODE.i_block[0]);
    memcpy(mip->INODE.i_block, oldname, strlen(oldname));
    mip->INODE.i_size = strlen(oldname);
    iput(mip);
    //mip->INODE.i_block[0]
    
    return 0;
}
