#include "main.h"


char path_buffer[MAX_PATH_LEN];
char *path[64];

char env_home[MAX_PATH_LEN];
char env_user[MAX_USER_LEN];

CMD commands[] = {
    {"cd",   &cmd_cd  },
    {"quit", &cmd_quit},
    {"exit", &cmd_quit}
};

/**
 * @brief Handle `cd` with a `chdir()` syscall
 */
int cmd_cd(char *args) {
    if (args == NULL || strlen(args) == 0) {
        DEBUG("cding to $HOME (%s)", env_home);
        return chdir(env_home);
    }
    
    return chdir(args);
}

/**
 * @brief Handle 'quit' and 'exit'
 */
int cmd_quit(char *args) {
    printf("Goodbye!\n");
    exit(0);

    return EXIT_SUCCESS; /* will not reach :) */
}

/**
 * @brief Finds a command handler for a specified input
 * 
 * @param input A single-word input, such as `ls` or `mkdir`
 * @return int(*)(char*) Returns a function pointer to the handler
 */
int (*find_cmd_handler(char *input))(char *) {
    CMD *cmd = &commands[0];
    DEBUG("input -> '%s'", input);
    // eh, just a simple linear O(N) search
    while(cmd && cmd->text[0]) {
        if (strcmp(input, cmd->text) == 0) {
            return cmd->handler;
        }
        cmd++;
    }

    return NULL;
}

/**
 * @brief Parse the env
 * 
 * @param env 
 */
void parse_env(char *env[]) {
    int i = 0;

    while(env[i]) {
        if (STARTS_WITH(env[i], "USER=")) {
            DEBUG("$USER = %s", env[i] + 5);
            strcpy(env_user, env[i] + 5);
        }
        else if (STARTS_WITH(env[i], "HOME=")) {
            DEBUG("$HOME = %s", env[i] + 5);
            strcpy(env_home, env[i] + 5);
        }
        else if (STARTS_WITH(env[i], "PATH=")) {
            // if (strncmp(env[i], "PATH=", 5) == 0) {
            DEBUG("*** begin parsing PATH");

            strcpy(path_buffer, env[i] + 5);

            char* token = strtok(path_buffer, ":");
            char** dest = path;

            while(token) {
                DEBUG("   path = %s", token);
                *dest = token;
                token = strtok(NULL, ":");

                dest++;
            }
            *dest = NULL;
            DEBUG("*** end parsing PATH");
        }
        
        i++;
    }
}

/**
 * @brief Handle built in functions (cd, exit)
 */
int handle_builtins(struct user_command *command) {    
    char **args = command->head_args;
    int (*handler)(char *) = find_cmd_handler(args[0]);

    if (handler) {
        handler(args[1]);

        return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

/**
 * @brief Find the shebang/crunchbang #! symbol and the associated binary
 */
char * find_shebang(char *fullpath, char *binary) {
    int fd = open(fullpath, O_RDONLY);
    char buffer[256];
    int r;

    if (!fd) {
        return NULL;
    }

    r = (int)read(fd, buffer, 256);
    
    if (r && strncmp(buffer, "#!", 2) == 0) {
        DEBUG("   '%s' is actually a script!", fullpath);
        char *tok = strtok(buffer + 2, " \t\r\n");
        DEBUG("       * tok = '%s'", tok);

        if (tok) {
            strcpy(binary, tok);
            return binary;
        }
    }
    close(fd);

    return NULL;
}

/**
 * @brief Try to execute a command
 */
void try_execute(struct user_command *command, char **args, char *fullpath) {
    char *script_bin = find_shebang(fullpath, script_bin);
    if (script_bin) {
        // this is ugly and hacky as it does not account for args passed into the script
        args[0] = basename(script_bin);
        args[1] = fullpath;
        args[2] = NULL; 
        
        execve(script_bin, args, command->env);
    }
    DEBUG("   search: %s", fullpath);
    execve(fullpath, args, command->env);
}

void execute_command(struct user_command *command, char **args) {
    int i     = 0;
    char *dir = path[i];
    char fullpath[MAX_PATH_LEN];
    char script_bin[256];

    DEBUG("** executing: '%s'", args[0]);
    
    if (args[0][0] == '/' || args[0][0] == '.') {
        DEBUG("abs or rel path for binary");
        //char *real = realpath(args[0], fullpath);
        //DEBUG("resolved = '%s'", real);
        // absolute

        try_execute(command, args, args[0]);
    }

    while(dir = path[i]) {
        strcpy(fullpath, dir);
        strcat(fullpath, "/");
        strcat(fullpath, args[0]);
        try_execute(command, args, fullpath);
        i++;
    }
    //execve(args[0], args, env);

    printf("command not found: '%s'\n", args[0]);
    exit(EXIT_FAILURE);
}

/**
 * @brief Handle external commands, like binaries and script files.
 */
int handle_external(struct user_command *command/*char *line, char **env, char **args*/) {
    int status;
    int pid = fork();
    
    if (pid) {
        DEBUG("[parent] pid = %d, waiting...", getpid());
        pid = wait(&status);
        DEBUG("[parent] child pid = %d, died with status %04x", pid, status);

        return status;
    }

    if (command->tail_args[0]) {
        int pd[2];
        pipe(pd);
        int pid2 = fork();
        
        if (!pid2) {
            DEBUG("--- head --- %d", pid2);
            close(pd[0]);
            close(1);
            dup(pd[1]);
            close(pd[1]);
            
            execute_command(command, command->head_args);
        } else {
            DEBUG("--- tail ---");
            close(pd[1]);
            close(0);
            dup(pd[0]);
            close(pd[0]);
            execute_command(command, command->tail_args);
        }

    } else {
        execute_command(command, command->head_args);
    }

    return EXIT_FAILURE; // should never reach this point
}

/**
 * @brief Parse the redirections, like <, >>, >
 */
void parse_redirs(struct user_command *command, char *line) {
    int input_redir_mode    = 0,
        output_redir_mode   = 0;
    char *output_redir_symb = strstr(line, STR_REDIR_APPEND);
    char *input_redir_symb  = strstr(line, STR_REDIR_INPUT);

    if (output_redir_symb) {     
        // first check for append   
        *output_redir_symb++ = '\0';
        *output_redir_symb++ = '\0';
        output_redir_mode = O_WRONLY | O_APPEND;
    } else {
        // then check for output
        output_redir_symb = strstr(line, STR_REDIR_OUTPUT);
        if (output_redir_symb) {
            *output_redir_symb++ = '\0';
            output_redir_mode = O_WRONLY | O_CREAT;
        }
    }

    // shared case for output/append
    if (output_redir_symb) {
        char *output_filename = strtok(output_redir_symb, " ");
        command->std_fd[1] = dup(1);
        close(1);
        command->open_fd[1] = open(output_filename, output_redir_mode, 0644);
    }

    // input redirection
    if (input_redir_symb) {
        *input_redir_symb++ = '\0';
        input_redir_mode = O_RDONLY;

        char *input_filename = strtok(input_redir_symb, " ");

        command->std_fd[0] = dup(0);
        close(0);
        command->open_fd[0] = open(input_filename, input_redir_mode, 0600);
    }
}

/**
 * @brief Zero the fd values
 */
void init_user_command(struct user_command *command) {
    // could just memset
    for(int i = 0; i < 3; i++) {
        command->open_fd[i] = 0;
        command->std_fd[i] = 0;
    }
}

/**
 * @brief Restore and close any open fds
 */
void revert_redirs(struct user_command *command) {
    for(int i = 0; i < 3; i++) {
        if (command->open_fd[i] || command->std_fd[i]) {
            close(command->open_fd[i]);
            dup2(command->std_fd[i], i);
            close(command->std_fd[i]);
        }
    }
}

/**
 * @brief Parses a line of input
 * 
 * @param command The command containing the line
 * @return int 
 */
int parse_line(struct user_command *command) {
    char * head = command->line;
    char * tail = strchr(command->line, '|');
    parse_redirs(command, head);

    if (tail) {
        *tail++ = '\0';
        strtok(tail, "|");
        command->next_tail = strtok(NULL, "|");

        parse_redirs(command, tail);
        parse_args(command->tail_args, tail);

        DEBUG("pipe detected: (%s) -> (%s) -> (%s)", head, tail, command->next_tail);
    } else {
        command->tail_args[0] = NULL;
    } 

    return parse_args(command->head_args, head);
}

/**
 * @brief Parses the arguments in a line of input
 * 
 */
int parse_args(char **args, char *line /*struct user_command *command*/) {
    char *ptr = strtok(line, " ");
    char **dest = args;

    while(ptr) {
        *dest = ptr;
        
        ptr = strtok(NULL, " ");
        dest++;
    }

    *dest = NULL; // NULL terminate the list
    return dest - args;
}

/**
 * @brief Runs the main input loop and executes commands
 */
void run_input_loop(char **env) {
    char* user = env_user[0] ? env_user : "user";
    char cwd[MAX_PATH_LEN];
    struct user_command command;
    int argc;

    command.env = env;

    while(1) {
        init_user_command(&command);
        
        getcwd(cwd, MAX_PATH_LEN);

        printf(
            ANSI_ESC_BOLD ANSI_ESC_GREEN "%s" ANSI_ESC_RESET ":"
            ANSI_ESC_BOLD ANSI_ESC_BLUE "%s" ANSI_ESC_RESET "%% ",
            user,
            cwd
        );
        fgets(command.line, MAX_PATH_LEN, stdin);
        command.line[strlen(command.line) - 1] = '\0'; // \n to \0
        
        argc = parse_line(&command);

        if (argc) {
            if (handle_builtins(&command) == EXIT_SUCCESS) {
                DEBUG("> used built-in command");
                continue;
            }
            
            handle_external(&command);
            revert_redirs(&command);
        }
    }
}


int main(int argc, char *argv[], char *env[]) {
    parse_env(env);

    run_input_loop(env);
}