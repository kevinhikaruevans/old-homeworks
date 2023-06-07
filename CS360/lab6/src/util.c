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
  block = (ino - 1) / 8 + gp->bg_inode_table;
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
  block = (mip->ino - 1) / 8 + gp->bg_inode_table;
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

  for (int i = 0; i < 12; i++)
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

  if (strcmp(pathname, "/") == 0)
  {
    // return root (inode = 2)
    return 2;
  }

  if (pathname[0] == '/')
  {
    // absolute
    mip = root;
  }
  else
  {
    // else, use the running proc's cwd
    // for the relative base dir
    mip = running->cwd;
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

    ino = search(mip, name[i]);
    if (!ino)
    {
      printf("invalid name: %s!\n", name[i]);
      iput(mip);
      return 0;
    }
    iput(mip);
    mip = iget(dev, ino);
  }

  iput(mip);
  return ino;
}

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
