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