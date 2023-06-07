        ////////////////
        enum {
            REDIR_NONE,
            REDIR_APPEND,
            REDIR_INPUT,
            REDIR_OUTPUT
        } io_redir = REDIR_NONE;

        char *operator = strstr(line, ">>");
        if (operator) {
            io_redir = REDIR_APPEND;
            
            *operator++ = '\0';
            *operator = '\0';
        }

        operator = strstr(line, "<");
        if (operator) {
            io_redir = REDIR_INPUT;
            *operator++ = '\0';
        }

        operator = strstr(line, ">");
        if (operator) {
            io_redir = REDIR_OUTPUT;
            *operator++ = '\0';
        }
        
        char *redir_output = strtok(operator, " ");
        
        //int fd = -1;
        // https://stackoverflow.com/questions/9084099/re-opening-stdout-and-stdin-file-descriptors-after-closing-them
        DEBUG("io = %d, op = '%s', %s", io_redir, operator, redir_output);
        switch(io_redir) {
            case REDIR_OUTPUT:
            std_fd[1] = dup(1);
            close(1);
            fd = open(redir_output, O_WRONLY | O_CREAT, 0644);
            //close(1);
            //dup(fd);
            break;
        }

        ////////////////