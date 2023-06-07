int myread(int fd, char buf[], int nbytes) {
    OFT *oftp = running->fd[fd];
    int count = 0;
    MINODE *mip = oftp->mptr;
    int avail = mip->INODE.i_size - oftp->offset;
    char readbuf[BLKSIZE];
    char *cq = buf;
    int ibuf[256];

    while(nbytes && avail) {
        int blk;
        int lblk = oftp->offset / BLKSIZE;
        int startByte = oftp->offset % BLKSIZE;

        if (lblk < 12) {
            // direct
            blk = mip->INODE.i_block[lblk];
        } else if (lblk >= 12 && lblk < 256 + 12) {
            // indirect
            get_block(mip->dev, mip->INODE.i_block[12], ibuf);
            blk = ibuf[lblk - 12]; /* 12 -> 0, 13 -> 1, ... */
        } else {
            int dbloffset = lblk - 13 - 255;
            //printf("dbl offset = %d\n", dbloffset);
            // dbl indirect
            get_block(mip->dev, mip->INODE.i_block[13], ibuf);
            blk = ibuf[dbloffset / 256];
            
            get_block(mip->dev, blk, ibuf);
            blk = ibuf[dbloffset % 256];
        }

        get_block(mip->dev, blk, readbuf);
        char *cp = readbuf + startByte; // src
        int remain = BLKSIZE - startByte;

        // take the minimum size to copy
        // i.e. whichever counter runs out first
        int copysize = remain;

        if (nbytes < copysize)
            copysize = nbytes;
        if (avail < copysize)
            copysize = avail;

        memcpy(cq, cp, copysize);

        oftp->offset += copysize;
        count += copysize;
        avail -= copysize;
        nbytes -= copysize;
        remain -= copysize;

        /*while (remain > 0) {
            *cq++ = *cp++;
            oftp->offset++;
            count++;
            avail--;
            nbytes--;
            remain--;

            if (nbytes <= 0 || avail <= 0)
                break;
        }*/

               // if one data block is not enough, loop back to OUTER while for more ...

    }
    //printf("myread: read %d char from fd %d\n", count, fd);
    return count;
}

int read_file(char *args) {
    int fd, nbytes;

    if (args && strlen(args)) {
        fd = atoi(args);
    } else {
        printf("enter fd = ");
        scanf(" %d", &fd);
    }

    printf("enter nbytes =");
    scanf(" %d", &nbytes);

    OFT *oft = running->fd[fd];

    if (oft && (oft->mode == MODE_R || oft->mode == MODE_RW)) {
        printf("=> read fd %d, %d bytes:\n", fd, nbytes);
        char buf[1024] = {0};
        myread(fd, buf, nbytes);
        printf("%s\n", buf);

    } else {
        printf("invalid fd or mode\n");
        return -1;
    }
}

int mycat(char *pathname) {
    int fd = open_file(pathname, "0");
    char buffer[1024];
    int n = 0;
    while(n = myread(fd, buffer, sizeof(buffer))) {
        fwrite(buffer, 1, n, stdout);
    }
    fflush(stdout);
    close_file(fd);

    return 0;
}