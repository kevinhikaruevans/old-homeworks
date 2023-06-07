/****************************************************************************
*                   KCW testing ext2 file system                            *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

#include "type.h"

// globals
MINODE minode[NMINODE];
MINODE *root;

PROC proc[NPROC], *running;

char gpath[128]; // global for tokenized components
char *name[32];  // assume at most 32 components in pathname
int n;           // number of component strings

int fd, dev;
int nblocks, ninodes, bmap, imap, inode_start;

MINODE *iget();

#include "util.c"
#include "symlink.c"
#include "cd_ls_pwd.c"
#include "mkdir_creat.c"
#include "rmdir.c"
#include "link_unlink.c"
#include "misc.c"

int init()
{
  int i, j;
  MINODE *mip;
  PROC *p;

  printf("init()\n");

  for (i = 0; i < NMINODE; i++)
  {
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i = 0; i < NPROC; i++)
  {
    p = &proc[i];
    p->pid = i;
    p->uid = p->gid = 0;
    p->cwd = 0;
    p->status = FREE;
    for (j = 0; j < NFD; j++)
      p->fd[j] = 0;
  }
}

// load root INODE and set root pointer to it
int mount_root()
{
  printf("mount_root()\n");
  root = iget(dev, 2);
}

char *disk = "diskimage";

int main(int argc, char *argv[])
{
  int ino;
  char buf[BLKSIZE];
  char line[128], cmd[32], pathname[128], pathname2[128];

  if (argc > 1)
    disk = argv[1];

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0)
  {
    printf("open %s failed\n", disk);
    exit(1);
  }
  dev = fd;

  /********** read super block  ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system ***********/
  if (sp->s_magic != 0xEF53)
  {
    printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
    exit(1);
  }
  printf("EXT2 FS OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf);
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  inode_start = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, inode_start);

  init();
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  ///////////////////////////////////////////////////////////////
  
  printf("creating P0 as running process\n");
  running = &proc[0];
  running->status = READY;
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  while (1)
  {
    printf("input command : [ls|cd|pwd|quit\n");
    printf("                 mkdir|creat|rmdir\n");
    printf("                 link|unlink|symlink|readlink\n");
    printf("                 chmod|atime] ");

    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0;

    if (line[0] == 0)
      continue;
    pathname[0] = pathname2[0] = 0;
    
    sscanf(line, "%s %s %s", cmd, pathname, pathname2);
    printf("cmd=%s pathname=%s\n", cmd, pathname);

    if (strcmp(cmd, "ls") == 0)
      ls(pathname);
    if (strcmp(cmd, "cd") == 0)
      chdir(pathname);
    if (strcmp(cmd, "pwd") == 0)
      pwd(running->cwd);
    if (strcmp(cmd, "mkdir") == 0)
      make_dir(pathname);
    if (strcmp(cmd, "creat") == 0)
      creat_file(pathname);
    if (strcmp(cmd, "rmdir") == 0)
      rmdir(pathname);
    if (strcmp(cmd, "link") == 0)
      link(pathname, pathname2);
    if (strcmp(cmd, "unlink") == 0)
      unlink(pathname);
    if (strcmp(cmd, "symlink") == 0)
      symlink(pathname, pathname2);
    if (strcmp(cmd, "readlink") == 0)
      printf("%s\n", my_readlink(pathname));
    if (strcmp(cmd, "chmod") == 0)
      chmod_path(pathname, pathname2);
    if (strcmp(cmd, "utime") == 0)
      utime(pathname);
    if (strcmp(cmd, "quit") == 0)
      quit();
  }
}

int quit()
{
  int i;
  MINODE *mip;
  for (i = 0; i < NMINODE; i++)
  {
    mip = &minode[i];
    if (mip->refCount > 0)
      iput(mip);
  }
  //write_super_gd();
  exit(0);
}

int write_super_gd() {
  char buf[BLKSIZE];
  /********** read super block  ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system ***********/
  
  printf("EXT2 FS OK\n");
  sp->s_inodes_count = ninodes;
  sp->s_blocks_count = nblocks;
  
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;

  gp->bg_block_bitmap = bmap;
  gp->bg_inode_bitmap = imap;
  gp->bg_inode_table = inode_start;
  put_block(dev, 2, buf);
}
