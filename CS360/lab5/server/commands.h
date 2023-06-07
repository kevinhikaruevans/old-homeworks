#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include <string.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>

#include "../shared/printutil.h"

struct cmd {
    char text[16];
    int (*handler)(int, char *);
    char text_args[16];
};

int (*find_cmd_handler(char *input))(int, char *);
int handle_data_receive(int socket, char *data);

int cmd_pwd(int socket, char *args);
int cmd_ls(int socket, char *args);
int cmd_cd(int socket, char *args);
int cmd_mkdir(int socket, char *args);
int cmd_rmdir(int socket, char *args);
int cmd_rm(int socket, char *args);
int cmd_help(int socket, char *args);
int cmd_get(int socket, char *args);
int cmd_put(int socket, char *args);

int ls_file(int socket, char *fname);
int ls_dir(int socket, char *dname);

#endif