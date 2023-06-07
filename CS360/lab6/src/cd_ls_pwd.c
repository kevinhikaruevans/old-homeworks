/************* cd_ls_pwd.c file **************/

int chdir(char *pathname)
{
  printf("chdir %s\n", pathname);
  // READ Chapter 11.7.3 HOW TO chdir

  int ino = getino(pathname);

  if (ino == 0)
  {
    printf("error\n");
    return 1;
  }

  MINODE *mip = iget(dev, ino);

  if (!S_ISDIR(mip->INODE.i_mode))
  {
    printf("not a directory!\n");
    return 1;
  }

  iput(running->cwd);
  running->cwd = mip;
  return 0;
}

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(MINODE *mip, char *name)
{
  //printf("ls_file: to be done: READ textbook for HOW TO!!!!\n");
  // READ Chapter 11.7.3 HOW TO ls
  int inode = search(mip, name);

  MINODE *child = iget(mip->dev, inode);
  int mode = child->INODE.i_mode;
  int i;
  char ftime[64];
  //printf("[ %d %s] ", inode, name);

  if (S_ISREG(mode))
    putchar('-');
  if (S_ISDIR(mode))
    putchar('d');
  if (S_ISLNK(mode))
    putchar('l');
    
  for (i = 8; i >= 0; i--)
  {
      if (mode & (1 << i)) // print r|w|x
          putchar(t1[i]);
      else
          putchar(t2[i]);
      // or print -
  }

  printf("%4d ", child->INODE.i_links_count);
  printf("%4d ", child->INODE.i_gid);
  printf("%4d ", child->INODE.i_uid);
  printf("%8d ", child->INODE.i_size);

  //strcpy(ftime, ctime(&child->INODE.i_ctime)); // print time in calendar form
  //localtime()
  //strftime(ftime, sizeof(ftime), "%c", localtime());
  time_t time = child->INODE.i_ctime;
  strcpy(ftime, ctime(&time));
  ftime[strlen(ftime) - 1] = 0;

  printf("%s ", ftime);
  printf("%s ", name);
  printf("\n");
  iput(child);
}

int ls_dir(MINODE *mip)
{
  //printf("ls_dir: list CWD's file names; YOU do it for ls -l\n");

  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  // Assume DIR has only one data block i_block[0]
  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;

  while (cp < buf + BLKSIZE)
  {
    strncpy(temp, dp->name, dp->name_len);
    temp[dp->name_len] = 0;
    ls_file(mip, temp);
    
    //printf("[%d %s]  ", dp->inode, temp); // print [inode# name]

    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  printf("\n");

  return 0;
}

int ls(char *pathname)
{
  if (pathname == NULL || !strlen(pathname)) {
    return ls_dir(running->cwd);
  } else {
    int ino = getino(pathname);
    MINODE *mip = iget(root->dev, ino);

    if (S_ISDIR(mip->INODE.i_mode)) {
      ls_dir(mip);
    } else if (S_ISREG(mip->INODE.i_mode)) {
      char *parent = pathname;
      char *child = strrchr(pathname, '/');

      if (child) {
        *child = '\0';
      } else {
        child = parent;
      }

      if (child == parent) {
        // root
        mip = root;
      } else {
        ino = getino(parent);
        mip = iget(root->dev, ino);
      }
      if (mip) {
        ls_file(mip, child);
      }
      //printf("'%s', '%s'\n", parent, child);
      //ls_file(mip, "x");
    }
  }
}

void rpwd(MINODE *wd)
{
  if (wd == root)
    return;

  //char sbuf2[BLKSIZE];
  //int cur = wd->INODE.i_block[0];
  //  int parent = wd->INODE.i_block[1];

  int my_ino = search(wd, ".");
  int parent_ino = search(wd, "..");

  MINODE *pip = iget(dev, parent_ino);
  DIR *dp;
  char sbuf[BLKSIZE];
  char *cp = sbuf;
  char namebuf[256] = {0};

  for (int i = 0; i < 12; i++)
  {
    if (namebuf[0] || pip->INODE.i_block[i] == 0)
    {
      break;
    }

    get_block(pip->dev, pip->INODE.i_block[i], sbuf);
    dp = (DIR *)sbuf;
    cp = sbuf;
    while (cp < sbuf + BLKSIZE)
    {
      if (dp->inode == my_ino)
      {
        strncpy(namebuf, dp->name, dp->name_len);
        break;
      }
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
  //pip->INODE.i_block

  rpwd(pip);
  printf("/%s", namebuf);
}

char *pwd(MINODE *wd)
{
  if (wd == root)
  {
    printf("/\n");
    return;
  }
  rpwd(wd);
  printf("\n");
}
