//#include "util.c"
#include <libgen.h>
#include <string.h>

int enter_name(MINODE *pip, int myino, char *myname)
{
    int block;
    int block_idx;

    for (int i = 0; i < 12; i++)
    {
        if (pip->INODE.i_block[i] == 0)
        {
            break;
        }
        block = pip->INODE.i_block[i];
        block_idx = i;
    }

    char buf[BLKSIZE];
    int name_len = strlen(myname);
    int need_length = 4 * ((8 + name_len + 3) / 4);

    get_block(pip->dev, block, buf);

    DIR *dp = (DIR *)buf;
    char *cp = buf;
    char tmp_name[128];

    printf("step to last entry in block %d:\n", block);
    while ((cp + dp->rec_len) < (buf + BLKSIZE - 1))
    {
        strncpy(tmp_name, dp->name, dp->name_len);
        tmp_name[dp->name_len] = 0;

        printf("   * %s (size %d)\n", tmp_name, dp->rec_len);
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }

    strncpy(tmp_name, dp->name, dp->name_len);
    tmp_name[dp->name_len] = 0;

    int ideal = 4 * ((8 + dp->name_len + 3) / 4);
    int remain = dp->rec_len - ideal;

    if (remain >= need_length)
    {
        printf("** trimming '%s' from %d to %d \n", tmp_name, dp->rec_len, ideal);
        // trim to a shorter length
        dp->rec_len = ideal;

        // add a new entry!
        cp += ideal;
        DIR *last = dp;
        dp = (DIR *)cp;
        strncpy(dp->name, myname, name_len);
        dp->name_len = name_len;
        dp->inode = myino;
        dp->rec_len = remain;
        dp->file_type = 1;
        printf("** adding entry '%s' with length %d\n", myname, remain);
    }
    else
    {
        // alloc a new block
        block = balloc(pip->dev);
        pip->INODE.i_block[block_idx + 1] = block;
        dp = buf;

        // have the new entry take up the whole thing
        strncpy(dp->name, myname, name_len);
        dp->name_len = name_len;
        dp->inode = myino;
        dp->rec_len = BLKSIZE;
        dp->file_type = 1;
    }

    put_block(pip->dev, block, buf);
    return 0;
}

int mymakedir(MINODE *pip, char *name)
{
    int ino = ialloc(pip->dev);
    int bno = balloc(pip->dev);

    printf("[mymakedir] ino = %d, bno = %d\n", ino, bno);
    MINODE *mip = iget(dev, ino);
    INODE *ip = &mip->INODE;

    ip->i_mode = 040755; /* 040 = dir, 755 = perms */
    ip->i_uid = running->uid;
    ip->i_gid = running->gid;
    ip->i_size = BLKSIZE;
    ip->i_links_count = 2; /* 2 for . and .. */

    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2;
    ip->i_block[0] = bno; /* one data block */
    for (int i = 1; i <= 14; i++)
        ip->i_block[i] = 0;
    mip->dirty = 1;
    iput(mip);

    char buffer[BLKSIZE];
    DIR *dp = &buffer[0];
    strcpy(dp->name, ".");
    dp->name_len = 1;
    dp->inode = ino;
    dp->rec_len = 12;

    dp = &buffer[12];
    strcpy(dp->name, "..");
    dp->name_len = 2;
    dp->inode = pip->ino;
    dp->rec_len = 1012;

    put_block(pip->dev, bno, buffer);

    return enter_name(pip, ino, name);
}

int make_dir(char *pathname)
{
    char pathname_copy[128];
    strcpy(pathname_copy, pathname);
    MINODE *start = (pathname[0] == '/') ? root : running->cwd;
    printf("start ino = %d \n", start->ino);
    char *parent = dirname(pathname_copy);
    char *child = basename(pathname);

    printf("mkdir: parent: '%s', base: '%s'\n", parent, child);

    int pino = getino(parent);
    printf("pino = %d\n", pino);
    MINODE *pip = iget(dev, pino);

    if (!S_ISDIR(pip->INODE.i_mode))
    {
        printf("error: parent '%s' is not a directory!\n", parent);
        return -1;
    }

    if (search(pip, child))
    {
        printf("error: thing '%s' already exists in parent '%s'!\n", child, parent);
        return -1;
    }

    printf("calling mymkdir\n");
    mymakedir(pip, child);

    /* 5. inc parent inodes's links count by 1; 
          touch its atime, i.e. atime = time(0L), mark it DIRTY */
    pip->INODE.i_links_count++;
    pip->INODE.i_atime = time(0L);
    pip->dirty = 1;

    iput(pip);

    return 0;
}

int my_creat(MINODE *pip, char *name)
{
    int ino = ialloc(pip->dev);

    printf("[my_creat] ino = %d\n", ino);
    MINODE *mip = iget(dev, ino);
    INODE *ip = &mip->INODE;

    ip->i_mode = 0100644; /* 040 = dir, 644 = perms */
    ip->i_uid = running->uid;
    ip->i_gid = running->gid;
    ip->i_size = 0;
    ip->i_links_count = 1;
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 0;

    for (int i = 0; i < 15; i++)
        ip->i_block[i] = 0;

    mip->dirty = 1;
    iput(mip);

    return enter_name(pip, ino, name);
}

int creat_file(char *pathname)
{
    char pathname_copy[128];
    strcpy(pathname_copy, pathname);
    MINODE *start = (pathname[0] == '/') ? root : running->cwd;

    char *parent = dirname(pathname_copy);
    char *child = basename(pathname);

    printf("creat: parent: '%s', base: '%s'\n", parent, child);

    int pino = getino(parent);
    MINODE *pip = iget(dev, pino);

    if (!S_ISDIR(pip->INODE.i_mode))
    {
        printf("error: parent '%s' is not a directory!\n", parent);
        return -1;
    }

    if (search(pip, child))
    {
        printf("error: thing '%s' already exists in parent '%s'!\n", child, parent);
        return -1;
    }

    my_creat(pip, child);

    pip->INODE.i_atime = time(0L);
    pip->dirty = 1;

    iput(pip);

    return 0;
}
