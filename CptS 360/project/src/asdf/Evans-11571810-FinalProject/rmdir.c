#include <stdbool.h>

int is_empty(MINODE *mip)
{
    char *cp, buf[BLKSIZE], temp[256];
    DIR *dp;

    for (int i = 0; i < 12; i++)
    {
        if (mip->INODE.i_block[i] == 0)
        {
            break;
        }

        get_block(mip->dev, mip->INODE.i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;

        while (cp < buf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;

            if (strcmp(".", temp) != 0 && strcmp("..", temp) != 0)
            {
                return 0;
            }

            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }

    return 1;
}
int rm_child(MINODE *parent, char *name)
{
    int i;
    char *cp, temp[256], buf[BLKSIZE];
    DIR *dp;
    DIR *prev = NULL;

    for (i = 0; i < 12; i++)
    {
        if (parent->INODE.i_block[i] == 0)
        {
            continue;
        }

        get_block(parent->dev, parent->INODE.i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;
        int fix_last = 0;

        while (cp < buf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            int delta = (int)dp + (int)dp->rec_len - (int)buf;
            printf("'%s' delta = %d\n", temp, delta);

            if (fix_last && (delta + fix_last) == BLKSIZE)
            {
                dp->rec_len += fix_last;
                break;
            }
            if (strcmp(temp, name) == 0)
            {
                printf(">> found child to delete!\n");
                //TODO: check if last entry/middle/first

                if (delta == BLKSIZE)
                {
                    // last entry
                    if (prev)
                    {
                        printf("expanding prev entry\n");
                        prev->rec_len += dp->rec_len;
                    }
                    else
                    {
                        // first entry?
                        /* TODO: 
                        deallocate the data block; modify parent's file size;

                            -----------------------------------------------
                            |INO Rlen Nlen NAME                           | 
                            -----------------------------------------------
                            
                            Assume this is parent's i_block[i]:
                            move parent's NONZERO blocks upward, i.e. 
                                i_block[i+1] becomes i_block[i]
                                etc.
                            so that there is no HOLEs in parent's data block numbers */
                        printf("prev is null, need to delete entire block >:|");

                        while (1)
                            ;
                    }
                }
                else
                {
                    //memcpy(cp, cp + dp->rec_len, BLKSIZE - ())
                    //TODO: check if first entry in data block
                    printf("shifting, %x <- %x (bytes = %d)\n", cp, cp + dp->rec_len, BLKSIZE - delta);
                    fix_last = dp->rec_len;
                    memmove(cp, cp + dp->rec_len, BLKSIZE - delta);
                    printf("shifted!\n");
                    printf("need to inc last entry by %d\n", fix_last);
                    continue;
                }
            }

            prev = dp;
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }

        put_block(parent->dev, parent->INODE.i_block[i], buf);
    }

    parent->dirty = 1;
}
int rmdir(char *pathname)
{
    printf("rmdir '%s'\n", pathname);
    MINODE *start = pathname[0] == '/' ? root : running->cwd;
    int ino = getino(pathname); //search(start, pathname); // getino(pathname);
    MINODE *mip = iget(dev, ino);

    if (running->uid != 0 && running->uid != mip->INODE.i_uid)
    {
        printf("bad permissions to rmdir\n");
        iput(mip);
        return -1;
    }

    /* empty check */
    if (!S_ISDIR(mip->INODE.i_mode))
    {
        printf("cannot rmdir, not a dir!\n");
        iput(mip);
        return -1;
    }
    if (mip->refCount > 1)
    {
        printf("cannot rmdir, busy?\n");
        iput(mip);
        return -1;
    }
    if (!is_empty(mip))
    {
        printf("cannot rm, not an empty dir\n");
        iput(mip);
        return -1;
    }

    // deallocate blocks/inodes
    for (int i = 0; i < 12; i++)
    {
        if (mip->INODE.i_block[i] == 0)
            continue;
        bdalloc(mip->dev, mip->INODE.i_block[i]);
    }
    idalloc(mip->dev, mip->ino);
    iput(mip);

    // go into parent dir
    MINODE *pip = iget(mip->dev, search(mip, ".."));
    rm_child(pip, basename(pathname));

    pip->INODE.i_links_count--;
    pip->dirty = 1;
    iput(pip);
    return 0;
}