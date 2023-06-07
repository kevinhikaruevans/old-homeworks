#ifndef LAB2_H
#define LAB2_H

/* decorations to make the output prettier */
#define ANSI_ESC_RESET "\x1b[0m"
#define ANSI_ESC_BLUE  "\x1b[34m"
#define ANSI_ESC_GREEN "\x1b[32m"
#define ANSI_ESC_RED   "\x1b[31m"
#define ANSI_ESC_BOLD  "\x1b[1m"

// gcc -DDEBUG_ON lab2.c && ./a.out
#ifdef DEBUG_ON
    #define DEBUG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG(fmt, ...) fmt
#endif

// print to stderr for errors and add some pretty red color
#define ERROR(fmt, ...)    fprintf(stderr, ANSI_ESC_BOLD ANSI_ESC_RED "error: " ANSI_ESC_RESET fmt "\n", ##__VA_ARGS__)

#define TYPE_TO_TEXT(type) (type == 'D' ? "directory" : "file")
#define IS_ROOT(node)      (node->parent == node)
#define DEFAULT_FILE       "myfile" 

#include <stdio.h>             // for I/O
#include <stdlib.h>            // for lib functions
#include <libgen.h>            // for dirname()/basename()
#include <string.h>

typedef struct node{
    char  name[64];       // node's name string
    char  type;
    struct node *child, *sibling, *parent;
} NODE;

typedef struct cmd {
    char text[16];
    int (*handler)(char *);
    char text_args[16];
} CMD;


void initialize();

int cmd_mkdir(char *);
int cmd_rmdir(char *);
int cmd_creat(char *);
int cmd_rm(char *);
int cmd_ls(char *);
int cmd_quit(char *);
int cmd_cd(char *);
int cmd_pwd(char *);
int cmd_help(char *);
int cmd_save(char *);
int cmd_reload(char *);

void cmd_save_r(FILE *fp, char *buffer, NODE *node, int *, int *);

//char* get_basename(char *pathname);
void get_pwd(char *buffer, NODE *cur);

NODE *find_node(const char *pathname);
int (*find_cmd_handler(char *input))(char *);

NODE *search_child(NODE *parent, char *name);
int insert_child(NODE *parent, NODE *q);

NODE *create_node(NODE *parent, char type, const char *name);
int insert_node(char type, char *name);
int remove_node(char type, char *name);

void destroy(NODE *parent);
#endif
