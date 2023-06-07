int truncate(INODE inode) {
    for(int i = 0; i < 12; i++) {
        if (inode.i_block[i]) {
            bdalloc(dev, inode.i_block[i]);
            inode.i_block[i] = 0;
        }
    }
    return 0;
}
int unlink(char *pathname) {
    char pathname_copy[128];
    strcpy(pathname_copy, pathname);
    //MINODE *start = (pathname[0] == '/') ? root : running->cwd;

    int ino = getino(pathname);
    char *parent = dirname(pathname_copy);
    char *child = basename(pathname);
    int pino = getino(parent);
    MINODE *pip = iget(root->dev, pino);

    printf("unlink '%s' (ino %d) in parent '%s'\n", child, ino, parent);
    MINODE *mip = iget(root->dev, ino);
    if (--mip->INODE.i_links_count == 0) {
        truncate(mip->INODE);
    }
    mip->dirty = 1;
    iput(mip);

    rm_child(pip, child);
    iput(pip);
    return 0;    
}

int create_link(char *pathname, int ino) {
    char pathname_copy[128];
    strcpy(pathname_copy, pathname);
    //MINODE *start = (pathname[0] == '/') ? root : running->cwd;

    char *parent = dirname(pathname_copy);
    char *child = basename(pathname);
    int pino = getino(parent);
    MINODE *pip = iget(root->dev, pino);

    printf("create_link '%s' in parent '%s', to ino %d\n", child, parent, ino);
    iput(pip);
    enter_name(pip, ino, child);

    return 0;
}

int link(char *oldFileName, char *newFileName) {
    printf("link '%s' -> '%s'\n", oldFileName, newFileName);
    if (!oldFileName || !newFileName) {
        printf("invalid link args\n");
        return -1;
    }

    int ino = getino(oldFileName);
    MINODE *mip = iget(dev, ino);
    
    if (!S_ISREG(mip->INODE.i_mode) && !S_ISLNK(mip->INODE.i_mode)) {
        iput(mip);
        printf("cannot create link; not a reg or lnk!\n");
        return -1;
    }

    mip->dirty = 1;
    mip->INODE.i_links_count++;
    iput(mip);

    return create_link(newFileName, ino);
}
