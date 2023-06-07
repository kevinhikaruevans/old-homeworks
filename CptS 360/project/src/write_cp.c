
int mywrite(int fd, char buf[], int nbytes)
{
    OFT *oftp = running->fd[fd];
    MINODE *mip = oftp->mptr;
    char wbuf[BLKSIZE];
    char *cq = buf;
    int ibuf[256];
    while (nbytes > 0)
    {
        int blk;
        int lblk = oftp->offset / BLKSIZE;
        int startByte = oftp->offset % BLKSIZE;

        if (lblk < 12)
        {
            if (mip->INODE.i_block[lblk] == 0)
            {
                // no data block yet
                mip->INODE.i_block[lblk] = balloc(mip->dev);
            }
            blk = mip->INODE.i_block[lblk];
        }
        else if (lblk >= 12 && lblk < 256 + 12)
        {
            // indirect block
            if (mip->INODE.i_block[12] == 0)
            {
                mip->INODE.i_block[12] = balloc(mip->dev);
            }
            get_block(mip->dev, mip->INODE.i_block[12], ibuf);
            if (ibuf[lblk - 12] == 0)
            {
                ibuf[lblk - 12] = balloc(mip->dev);
                put_block(mip->dev, mip->INODE.i_block[12], ibuf);
            }
            blk = ibuf[lblk - 12];
        }
        else
        {
            int dbloffset = lblk - 13 - 255;
            if (mip->INODE.i_block[13] == 0) {
                mip->INODE.i_block[13] = balloc(mip->dev);
                put_block(mip->dev, mip->INODE.i_block, ibuf);
            }
            get_block(mip->dev, mip->INODE.i_block[13], ibuf);
            if (ibuf[dbloffset / 256] == 0) {
                ibuf[dbloffset / 256] = balloc(mip->dev);
                put_block(mip->dev, mip->INODE.i_block[13], ibuf);
            }
            blk = ibuf[dbloffset / 256];
            
            get_block(dev, blk, ibuf);
            if (ibuf[dbloffset % 256] == 0) {
                ibuf[dbloffset % 256] = balloc(mip->dev);
                put_block(mip->dev, blk, ibuf);
            }
            blk = ibuf[dbloffset % 256];
        }

        get_block(mip->dev, blk, wbuf);
        char *cp = wbuf + startByte;

        int remain = BLKSIZE - startByte;
        int towrite = remain;

        if (nbytes < remain)
            towrite = nbytes;

        memcpy(cp, cq, towrite);
        nbytes -= towrite;
        remain -= towrite;
        oftp->offset += towrite;
        if (oftp->offset > mip->INODE.i_size)
            mip->INODE.i_size += towrite;
        // while(remain > 0) {
        //     *cp++ = *cq++;
        //     nbytes--;
        //     remain--;

        //     oftp->offset++;
        //     if (oftp->offset > mip->INODE.i_size) {
        //         mip->INODE.i_size++;
        //     }

        //     if (nbytes <= 0)
        //         break;
        // }
        put_block(mip->dev, blk, wbuf);
    }
    mip->dirty = 1;
    return nbytes;
}

int write_file()
{
    int fd;
    char buffer[1024];

    printf("enter fd = ");
    scanf(" %d", &fd);
    getchar();

    printf("enter string = ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer) - 1] = 0;
    printf("write '%s' of size %d\n", buffer, strlen(buffer));
    return mywrite(fd, buffer, strlen(buffer));
}

int mycp(char *src, char *dest)
{
    printf("mycp src: %s, dest %s\n", src, dest);

    int fd = myopen(src, MODE_R);
    creat_file(dest);

    int gd = myopen(dest, MODE_W);
    int n;
    char buf[BLKSIZE];

    while (n = myread(fd, buf, BLKSIZE))
    {
        mywrite(gd, buf, n);
    }

    close_file(fd);
    close_file(gd);

    return 0;
}

int mymv(char *src, char *dest)
{
    char temp[128];
    int ino = getino(src);
    MINODE *mip = iget(dev, ino);

    strcpy(temp, dest);
    char *destParent = dirname(temp);

    int pino = getino(destParent);
    MINODE *pip = iget(dev, pino);

    if (mip->dev == pip->dev)
    {
        link(src, dest);
    }
    else
    {
        mycp(src, dest);
    }

    unlink(src);
    printf("mymv src: %s, dest: %s\n", src, dest);

    iput(mip);
    iput(pip);
    return 0;
}