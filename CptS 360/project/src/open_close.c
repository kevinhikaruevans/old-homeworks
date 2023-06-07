
enum file_mode
{
    MODE_R,
    MODE_W,
    MODE_RW,
    MODE_APPEND
};

int truncate_mip(MINODE *mip)
{
    truncate(mip->INODE);

    mip->INODE.i_mtime = time(NULL);
    mip->INODE.i_size = 0;
    mip->dirty = 1;
    return 0;
}
OFT *oft_alloc(MINODE *mip, enum file_mode mode)
{
    OFT *target = NULL;

    for (int i = 0; i < NOFT; i++)
    {
        if (oft[i].refCount <= 0)
        {
            target = &oft[i];
            break;
        }
    }
    target->mode = mode;
    target->refCount = 1;
    target->mptr = mip;

    switch (mode)
    {
    case MODE_RW:
    case MODE_R:
        target->offset = 0;
        break;
    case MODE_W:
        truncate_mip(mip);
        target->offset = 0;
        break;
    case MODE_APPEND:
        target->offset = mip->INODE.i_size;
        break;

    default:
        target = NULL;
        printf("invalid mode!\n");
        break;
    }

    return target;
}

int fd_alloc(PROC *proc)
{
    for (int i = 0; i < NFD; i++)
    {
        if (proc->fd[i] == NULL)
            return i;
    }
    return -1;
}
int myopen(char *pathname, enum file_mode mode)
{

    printf("open '%s' with mode %d\n", pathname, mode);

    int ino = getino(pathname);
    MINODE *mip = iget(dev, ino);

    if (!S_ISREG(mip->INODE.i_mode))
    {
        printf("cannot open file %s, not reg inode\n");
        iput(mip);
        return 0;
    }
    OFT *oftp = oft_alloc(mip, mode);
    int fd = fd_alloc(running);
    running->fd[fd] = oftp;

    switch (mode)
    {
    case MODE_W:
    case MODE_RW:
    case MODE_APPEND:
        mip->INODE.i_mtime = time(NULL);
    case MODE_R:
        mip->INODE.i_atime = time(NULL);
        mip->dirty = 1;
        break;
    }

    return fd;
}
int open_file(char *pathname, char *smode)
{
    enum file_mode mode;
    int dev = pathname[0] == '/' ? root->dev : running->cwd->dev;

    if (!smode || !strlen(smode))
    {
        printf("enter open mode: ");
        mode = getchar() - '0';
        getchar(); // consume the extra \n
    }
    else
    {
        mode = atoi(smode);
    }

    return myopen(pathname, mode);
}

int close_file(int fd)
{
    if (fd < 0 || fd >= NFD)
    {
        printf("fd out of range: %d\n", fd);
        return -1;
    }

    OFT *oftp = running->fd[fd];
    if (!oftp)
    {
        printf("fd is invalid in running->fd\n");
        return -1;
    }

    running->fd[fd] = NULL;
    oftp->refCount--;

    if (oftp->refCount > 0)
        return 0;

    MINODE *mip = oftp->mptr;

    iput(mip);
    return 0;
}

int my_lseek(int fd, int pos)
{
    //TODO: bound check, and oftp != null
    OFT *oftp = running->fd[fd];
    int prev = oftp->offset;
    if (pos >= 0 && pos <= oftp->mptr->INODE.i_size)
    {
        oftp->offset = pos;
    }
    else
    {
        printf("invalid offset: %d (file size: %d)\n", pos, oftp->mptr->INODE.i_size);
    }

    return prev;
}

void pfd()
{
    // print fds
    printf("+----+--------+----------+-----------+\n");
    printf("| fd | mode   | offset   | INODE     |\n");
    printf("+----+--------+----------+-----------+\n");

    for (int i = 0; i < NFD; i++)
    {
        OFT *oftp = running->fd[i];
        if (oftp)
        {

            printf("| %2d | ", i);
            switch (oftp->mode)
            {
            case MODE_R:
                printf("READ  ");
                break;
            case MODE_W:
                printf("WRITE ");
                break;
            case MODE_RW:
                printf("READWR");
                break;
            case MODE_APPEND:
                printf("APPEND");
                break;
            }
            printf(" | %8d |", oft->offset);
            printf(" [%2d, %2d ] |\n", oftp->mptr->dev, oftp->mptr->ino);
        }
    }
    printf("+----+--------+----------+-----------+\n");
}

int my_dup(int fd)
{
}

int dup2(int fd, int gd)
{
}