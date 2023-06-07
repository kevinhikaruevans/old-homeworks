/*********** util.c file ****************/

int get_block(int dev, int blk, char *buf)
{
  lseek(dev, (long)blk * BLKSIZE, 0);
  read(dev, buf, BLKSIZE);
}

int put_block(int dev, int blk, char *buf)
{
  lseek(dev, (long)blk * BLKSIZE, 0);
  write(dev, buf, BLKSIZE);
}

int tokenize(char *pathname)
{
  // copy pathname into gpath[]; tokenize it into name[0] to name[n-1]
  // Code in Chapter 11.7.2
  char *s;
  strcpy(gpath, pathname);

  n = 0;
  s = strtok(gpath, "/");
  while (s)
  {
    name[n++] = s;
    s = strtok(NULL, "/");
  }

  return n;
}

MINODE *mialloc()
{
  for (int i = 0; i < NMINODE; i++)
  {
    MINODE *p = minode + i;
    if (!p->refCount)
    {
      p->refCount = 1;
      return p;
    }
  }
  printf("FS panic: out of minode\n");
  return NULL;
}
void midalloc(MINODE *mip)
{
  mip->refCount = 0;
}
MINODE *iget(int dev, int ino)
{
  // return minode pointer of loaded INODE=(dev, ino)
  // Code in Chapter 11.7.2

  MINODE *mip;
  MTABLE *mp;
  INODE *ip;

  int i, block, offset;
  char buf[BLKSIZE];

  // look in memory first and return it if it exists
  for (i = 0; i < NMINODE; i++)
  {
    mip = minode + i;

    if (mip->refCount && (mip->dev == dev) && (mip->ino == ino))
    {
      mip->refCount++;
      return mip;
    }
  }

  mip = mialloc();
  mip->dev = dev;
  mip->ino = ino;

  // block address pair:
  block = (ino - 1) / 8 + inode_start /*gp->bg_inode_table*/;
  offset = (ino - 1) % 8;

  get_block(dev, block, buf);
  ip = (INODE *)buf + offset;
  mip->INODE = *ip;

  mip->refCount = 1;
  mip->mounted = mip->dirty = mip->mptr = 0;
  return mip;
}

void iput(MINODE *mip)
{
  // dispose of minode pointed by mip
  // Code in Chapter 11.7.2
  INODE *ip;
  int i, block, offset;
  char buf[BLKSIZE];

  if (mip == 0)
    return;

  mip->refCount--;
  if (mip->refCount > 0)
  {
    // other procs are still using this, so do nothing
    return;
  }
  if (mip->dirty == 0)
  {
    // nothing needs to be done as the inode is still
    // clean
    return;
  }

  // do a write back to the disk file
  block = (mip->ino - 1) / 8 + inode_start; //gp->bg_inode_table;
  offset = (mip->ino - 1) % 8;

  get_block(mip->dev, block, buf);
  ip = (INODE *)buf + offset;
  *ip = mip->INODE;

  put_block(mip->dev, block, buf);
  midalloc(mip);
}

int search(MINODE *mip, char *name)
{
  // search for name in (DIRECT) data blocks of mip->INODE
  // return its ino
  // Code in Chapter 11.7.2
  int i;
  char *cp, temp[256], sbuf[BLKSIZE];
  DIR *dp;

  for (i = 0; i < 12; i++)
  {
    // search the first 12 pointers (the direct blocks)
    // note: indirect blocks are not implemented

    if (mip->INODE.i_block[i] == 0)
    {
      // first empty, just return
      return 0;
    }

    get_block(mip->dev, mip->INODE.i_block[i], sbuf);
    dp = (DIR *)sbuf;
    cp = sbuf;

    while (cp < sbuf + BLKSIZE)
    {
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;
      if (strcmp(name, temp) == 0)
      {
        //printf("name matches: %s, ino = %d\n", name, dp->inode);
        return dp->inode;
      }

      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }

  return 0;
}

int getino(char *pathname)
{
  // return ino of pathname
  // Code in Chapter 11.7.2

  MINODE *mip;
  int i, ino;

  //int dev;
  if (strcmp(pathname, "/") == 0)
  {
    // return root (inode = 2)
    return 2;
  }

  if (pathname[0] == '/')
  {
    // absolute
    mip = root;
    dev = root->dev;
  }
  else
  {
    // else, use the running proc's cwd
    // for the relative base dir
    mip = running->cwd;
    dev = mip->dev;
  }

  mip->refCount++;

  tokenize(pathname);

  for (i = 0; i < n; i++)
  {
    if (!S_ISDIR(mip->INODE.i_mode))
    {
      // not a dir, cannot traverse into
      printf("%s is not a dir!\n", name[i]);
      iput(mip); // release
      return 0;
    }

    if (strcmp(name[i], "..") == 0 && mip->ino == 2 && mip->dev != root->dev)
    {
      printf("up cross\n");
      //char *realpath = NULL;
      for (int i = 0; i < NMTABLE; i++)
      {
        if (mtable[i].dev == mip->dev)
        {
          mip = mtable[i].mntDirPtr;
          ino = mtable[i].mntDirPtr->ino;
          dev = root->dev;
          break;
        }
      }
    }

    ino = search(mip, name[i]);
    if (!ino)
    {
      printf("invalid name: %s!\n", name[i]);
      iput(mip);
      return 0;
    }
    iput(mip);
    //mip = iget(dev, ino);
    mip = iget(mip->dev /*mip->dev*/, ino);
    //printf("* '%s'\n", name[i]);

    if (mip->mounted)
    {
      //printf("mip '%s' mounted: dev=%d\n", name[i], mip->mptr->dev);
      dev = mip->mptr->dev;
      ino = 2;
      //iput(mip);
      mip = iget(dev, ino); // mip->mptr->iblock;
    }
  }
  //printf("getino -> dev = %d\n", dev);
  iput(mip);
  return ino;
}

////////////////////////////
// Level 1 util functions //
////////////////////////////
int tst_bit(char *buf, int bit)
{
  // in Chapter 11.3.1
  int index = bit / 8;
  int offset = bit % 8;

  return !!(buf[index] & (1 << offset));
}

int set_bit(char *buf, int bit)
{
  // in Chapter 11.3.1
  int index = bit / 8;
  int offset = bit % 8;

  buf[index] |= (1 << offset);
  return 1;
}
int clr_bit(char *buf, int bit) // clear bit in char buf[BLKSIZE]
{
  buf[bit / 8] &= ~(1 << (bit % 8));
  return 1;
}

enum update_type
{
  INODES,
  BLOCKS
};
enum update_dir
{
  INC = 1,
  DEC = -1
};

int update_super_gd(int dev, enum update_type type, enum update_dir dir)
{
  //TODO on quit, ensure sp/gd is saved
  char buf[BLKSIZE];
  // p 332
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  sp->s_inodes_count = ninodes;
  sp->s_blocks_count = nblocks;
  if (type == INODES)
  {
    sp->s_free_inodes_count += dir;
  }
  else
  {
    sp->s_free_blocks_count += dir;
  }
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  //gp->bg_block_bitmap = bmap;
  //gp->bg_inode_bitmap = imap;
  //gp->bg_inode_table = inode_start;
  if (type == INODES)
  {
    gp->bg_free_inodes_count += dir;
  }
  else
  {
    gp->bg_free_blocks_count += dir;
  }
  put_block(dev, 2, buf);
}

int ialloc(int dev) // allocate an inode number from inode_bitmap
{
  int i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(dev, imap, buf);

  for (i = 0; i < ninodes; i++)
  {
    if (tst_bit(buf, i) == 0)
    {
      set_bit(buf, i);
      put_block(dev, imap, buf);
      update_super_gd(dev, INODES, DEC);

      // bits count from 0; ino from 1
      printf("[ialloc] allocated ino = %d\n", i + 1);
      return i + 1;
    }
  }
  return 0;
}

int idalloc(int dev, int ino)
{
  char buf[BLKSIZE];
  //MTABLE *mp = (MTABLE *)get_mtable(dev);

  if (ino > ninodes)
  {
    printf("inumber out of range: %d\n", ino);
    return -1;
  }

  get_block(dev, imap, buf);
  clr_bit(buf, ino - 1);
  put_block(dev, imap, buf);
  update_super_gd(dev, INODES, INC);

  return 0;
}

int bdalloc(int dev, int bno)
{
  char buf[BLKSIZE];

  if (bno > nblocks)
  {
    printf("bno out of range: %d\n", bno);
    return -1;
  }
  get_block(dev, bmap, buf);
  clr_bit(buf, bno - 1);
  put_block(dev, bmap, buf);
  update_super_gd(dev, BLOCKS, INC);

  return 0;
}
int balloc(int dev)
{
  // basically the same as ialloc, but for blocks
  int i;
  char buf[BLKSIZE];

  get_block(dev, bmap, buf);

  for (int i = 0; i < nblocks; i++)
  {
    if (tst_bit(buf, i) == 0)
    {
      set_bit(buf, i);
      put_block(dev, bmap, buf);
      update_super_gd(dev, BLOCKS, DEC);
      printf("[balloc] allocated block = %d", i + 1);
      int blk = i + 1;

      // zero out block
      get_block(dev, blk, buf);
      bzero(buf, BLKSIZE);
      put_block(dev, blk, buf);

      return blk;
    }
  }
  return 0;
}

// unused (in template though)
int findmyname(MINODE *parent, u32 myino, char *myname)
{
  // WRITE YOUR code here:
  // search parent's data block for myino;
  // copy its name STRING to myname[ ];
}

int findino(MINODE *mip, u32 *myino) // myino = ino of . return ino of ..
{
  // mip->a DIR minode. Write YOUR code to get mino=ino of .
  //                                         return ino of ..
}
