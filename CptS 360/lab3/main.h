#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>

/* decorations to make the output prettier */
#define ANSI_ESC_RESET "\x1b[0m"
#define ANSI_ESC_BLUE  "\x1b[34m"
#define ANSI_ESC_GREEN "\x1b[32m"
#define ANSI_ESC_RED   "\x1b[31m"
#define ANSI_ESC_BOLD  "\x1b[1m"

// gcc -DDEBUG_ON lab2.c && ./a.out
#ifdef DEBUG_ON
    #define DEBUG(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG(fmt, ...) fmt
#endif

#define STARTS_WITH(unknown, search_val) (strncmp(unknown, search_val, sizeof(search_val) - 1) == 0)

#define MAX_PATH_LEN 256
#define MAX_USER_LEN 128


struct user_command {
    int std_fd[3];
    int open_fd[3];

    char line[MAX_PATH_LEN];

    //char *args[32];
    char **env;
};
#define STR_REDIR_APPEND ">>"
#define STR_REDIR_INPUT "<"
#define STR_REDIR_OUTPUT ">"

typedef struct cmd {
    char text[16];
    int (*handler)(char *);
} CMD;

int cmd_quit(char *);
int cmd_cd(char *);

void run_input_loop(char **env);
int parse_args(char **args, char *line);
int parse_line(struct user_command *command);
void revert_redirs(struct user_command *command);
void init_user_command(struct user_command *command);
void parse_redirs(struct user_command *command, char *line);
int handle_external(struct user_command *command/*char *line, char **env, char **args*/);
void execute_command(struct user_command *command, char **args);
void try_execute(struct user_command *command, char **args, char *fullpath);
char * find_shebang(char *fullpath, char *binary);
int handle_builtins(struct user_command *command);
void parse_env(char *env[]);
int (*find_cmd_handler(char *input))(char *);
void fork_ext(struct user_command *command, char *line);

#endif