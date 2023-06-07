#include "./lab2.h"

NODE *root, *cwd;
CMD commands[] = {
    /**
     * @brief All commands and their command handlers and argument hints.
     */
    {"mkdir",  &cmd_mkdir,  "<path>" },
    {"rmdir",  &cmd_rmdir,  "<path>" },
    {"cd",     &cmd_cd,     "<path>" },
    {"pwd",    &cmd_pwd,    ""       },
    {"creat",  &cmd_creat,  "<file>" },
    {"rm",     &cmd_rm,     "<file>" },
    {"save",   &cmd_save,   "<file?>"},
    {"reload", &cmd_reload, "<file?>"},
    {"menu",   &cmd_help,   ""       },
    {"ls",     &cmd_ls,     "<path?>"},
    {"help",   &cmd_help,   ""       },
    {"exit",   &cmd_quit,   ""       },
    {"quit",   &cmd_quit,   ""       },
    {0}
};

int main() {
    char buffer[128],
        command[16],
        args[64];
    char *user = getenv("USER") ? getenv("USER") : getenv("USERNAME");
    int r;

    initialize();

    while(1) {
        
        get_pwd(buffer, cwd);
        printf(
            ANSI_ESC_BOLD ANSI_ESC_GREEN "%s" ANSI_ESC_RESET ":"
            ANSI_ESC_BOLD ANSI_ESC_BLUE "%s" ANSI_ESC_RESET "%% ",
            user == NULL ? "user" : user,
            buffer
        );
        fgets(buffer, sizeof(buffer), stdin);
        r = sscanf(buffer, "%s %s", command, args);
        
        if (r == 1) {
            // if there's a single argument, just set
            // args to an empty string
            args[0] = '\0';
        }
        
        int (*handler)(char *) = find_cmd_handler(command);
        
        if (r != EOF) {
            // ensure there's any input at all before
            // trying to execute a handler

            if (handler) {
                handler(args);
            } else {
                ERROR("command '%s' not found!", command);
            }
        }
    }

    return 0;
}

/**
 * @brief Finds a command handler for a specified input
 * 
 * @param input A single-word input, such as `ls` or `mkdir`
 * @return int(*)(char*) Returns a function pointer to the handler
 */
int (*find_cmd_handler(char *input))(char *) {
    CMD *cmd = &commands[0];

    // eh, just a simple linear O(N) search
    while(cmd && cmd->text[0]) {
        if (strcmp(input, cmd->text) == 0) {
            return cmd->handler;
        }
        cmd++;
    }

    return NULL;
}

#pragma region Commands

/**
 * @brief Saves the filesystem tree to a flatfile in a text format.
 * 
 * @param name The destination file to write the filesystem to.
 * @return int 0 on success
 */
int cmd_save(char *name) {
    char buffer[128];
    char *filename = name && name[0] ? name : DEFAULT_FILE;
    FILE *fp       = fopen(filename, "w+");
    int files      = 0,
        dirs       = 0;

    if (!fp) {
        ERROR("failed to create file");
        return -1;
    }

    cmd_save_r(fp, buffer, root, &files, &dirs);
    fclose(fp);

    printf("saved %d %s and %d %s\n",
        files,
        files == 1 ? "file" : "files",
        dirs,
        dirs == 1 ? "directory" : "directories"
    );

    return 0;
}

/**
 * @brief Little recursive helper function for saving files
 */
void cmd_save_r(FILE *fp, char *buffer, NODE *node, int *files, int *dirs) {
    if (node == NULL) {
        return;
    }

    get_pwd(buffer, node);

    fprintf(fp, "%c %s\n", node->type, buffer);
    
    if (node->type == 'F') {
        *files = *files + 1;
    } else {
        *dirs = *dirs + 1;
    }
    cmd_save_r(fp, buffer, node->child, files, dirs);
    cmd_save_r(fp, buffer, node->sibling, files, dirs);
}

/**
 * @brief Loads a file to the filesystem tree.
 * 
 * @param name The file to read from.
 * @return int Returns 0 on success, -1 on failure.
 */
int cmd_reload(char *name) {
    char buffer[128];
    char type      = '\0';
    char *filename = name && name[0] ? name : DEFAULT_FILE;
    FILE *fp       = fopen(filename, "r");
    int files      = 0,
        dirs       = 0;

    if (!fp) {
        ERROR("failed to open file");
        return -1;
    } else {
        // just destroy the current tree, then reinitialize root
        destroy(root);
        initialize();
    }

    while(!feof(fp)) {
        fscanf(fp, "%c %s ", &type, buffer);
        
        DEBUG("read node: '%c' '%s'", type, buffer);

        if (type == 'F') {
            cmd_creat(buffer);
            files++;
        } else if (type == 'D') {
            if (strcmp(buffer, "/") == 0) {
                DEBUG("skipping root directory");
            } else {
                cmd_mkdir(buffer);
            }
            dirs++;
        } else {
            ERROR("unknown type: %c, file = %s", type, buffer);
        }
    }

    fclose(fp);
    printf("loaded %d %s and %d %s\n",
        files,
        files == 1 ? "file" : "files",
        dirs,
        dirs == 1 ? "directory" : "directories"
    );
    return 0;
}

/**
 * @brief Prints a list of available commands.
 */
int cmd_help(char *name) {
    CMD *cmd = &commands[0];

    printf("available commands: \n");
    while(cmd && cmd->text[0]) {
        printf("   %-8s %s \n", cmd->text, cmd->text_args);
        cmd++;
    }

    return 0;
}

/**
 * @brief Prints the present working directory.
 */
int cmd_pwd(char *name) {
    char buffer[128];
    get_pwd(buffer, cwd);

    printf("%s\n", buffer);
}

/**
 * @brief Changes the current working directory to the argument passed.
 * 
 * @param name The target directory
 * @return int Returns 0 on success
 */
int cmd_cd(char *name) {
    NODE *child = find_node(name);

    if (child && child->type == 'D') {
        cwd = child;
        return 0;
    } else {
        ERROR("Cannot cd to '%s'", name);
    }

    return -1;
}

/**
 * @brief Removes a directory
 */
int cmd_rmdir(char *name) {
    return remove_node('D', name);
}

/**
 * @brief Makes a directory
 */
int cmd_mkdir(char *name) {
    return insert_node('D', name);
}

/**
 * @brief Creates a file
 */
int cmd_creat(char *name) {
    if (name[strlen(name) - 1] == '/') {
        ERROR("file name cannot end in a slash");
        return -1;
    }
    return insert_node('F', name);
}

/**
 * @brief Removes a file
 */
int cmd_rm(char *name) {
    if (name[strlen(name) - 1] == '/') {
        ERROR("file name cannot end in a slash");
        return -1;
    }

    return remove_node('F', name);
}

/**
 * @brief Prints a listing of all nodes
 */
int cmd_ls(char *args) {
    NODE *search = (args && strlen(args)) ? find_node(args) : cwd;

    if (search == NULL || search->type == 'F') {
        ERROR("directory '%s' does not exist", args);
        return -1;
    }

    NODE *p = search->child;
    printf("contents of %s:\n", search->name);
    
    if (p == NULL) {
        printf("   (empty dir)\n");
        return 0;
    }

    while(p) {
        printf("   [%c] %s%s%s \n",
            p->type,
            p->type == 'D' ? (ANSI_ESC_BLUE ANSI_ESC_BOLD) : ANSI_ESC_RESET,
            p->name,
            ANSI_ESC_RESET
        );
        p = p->sibling;
    }

    return 0;
}

/**
 * @brief Saves the output tree to a file and exits the program.
 */
int cmd_quit(char *args) {
    cmd_save(DEFAULT_FILE);

    printf("goodbye!\n");
    exit(0);
}

#pragma endregion

#pragma region Node operations

/**
 * @brief Removes a node
 * 
 * @param type The type (F/D) of node
 * @param name The pathname to the node to delete
 * @return int Returns 0 on success.
 */
int remove_node(char type, char *name) {
    NODE *node = find_node(name);

    if (node == NULL || node->type != type) {
        ERROR("%s '%s' does not exist", TYPE_TO_TEXT(type), name);
        return -1;
    }

    if (IS_ROOT(node)) {
        ERROR("cannot delete root directory");
        return -1;
    }

    if (type == 'D' && node->child) {
        ERROR("cannot delete non-empty directory");
        return -1;
    }

    NODE *parent = node->parent;
    
    if (parent->child == node) {
        parent->child = node->sibling;
    } else {
        // set to first child
        NODE *prev = parent->child;
        
        // iterate over siblings until one has the target
        // as the sibling
        for(;
            prev && prev->sibling != node;
            prev = prev->sibling)
            ;
        
        // skip over the target node
        prev->sibling = node->sibling;
    }

    if (node == cwd) {
        // special case, if they're currently in the
        // directory being deleted, go up a dir
        cwd = node->parent;
    }

    free(node);

    return 0;
}

/**
 * @brief Inserts a node to the tree
 * 
 * @param type The type of node (F/D)
 * @param name The pathname to create
 * @return int Returns 0 on success
 */
int insert_node(char type, char *name) {
    DEBUG("insert node: type=%c, name=%s", type, name);

    if (strlen(name) == 0) {
        ERROR("no %s specified!", TYPE_TO_TEXT(type));
        return -1;
    }
    /*char *bn      = get_basename(name);   
    char *dirname = name;
    
    if (dirname == bn) {
        dirname = ".";
    } else if (dirname[0] == '\0') {
        dirname = "/";
    }*/

    // copy the name to the stack, so that basename and
    // dirname do not interact with each other
    char bn_buffer[128];
    strcpy(bn_buffer, name);

    char *bn = basename(bn_buffer);
    char *dn = dirname(name);

    DEBUG("dn = '%s', bn = '%s'", dn, bn);

    NODE *dir = find_node(dn);
    if (dir == NULL || dir->type != 'D') {
        // parent dir existence check
        ERROR("directory '%s' does not exist", dn);
        return -1;
    }

    NODE *search = search_child(dir, bn);
    if (search != NULL) {
        // child exists check
        ERROR("%s '%s' already exists!", TYPE_TO_TEXT(search->type), search->name);
        return -1;
    }

    // all good, just insert the node
    NODE *node = create_node(dir, type, bn);
    insert_child(dir, node);

    return 0;
}

/**
 * @brief Allocates and creates a node
 * 
 * @param parent The parent of the node
 * @param type The type of node
 * @param name The basename of the node
 * @return NODE* Returns a pointer to the newly created node
 */
NODE *create_node(NODE *parent, char type, const char *name) {
    NODE *node = (NODE *)malloc(sizeof(NODE));

    if (!node) {
        ERROR("failed to allocate NODE!");

        exit(EXIT_FAILURE);
    }

    node->child   = NULL;
    node->sibling = NULL;
    node->type    = type;
    node->parent  = parent == NULL ? node : parent;

    strcpy(node->name, name);

    DEBUG("Created node: %s", name);

    return node;
}

#pragma endregion

/**
 * @brief Initializes the root directory
 */
void initialize() {
    root = cwd = create_node(NULL, 'D', "/");
    DEBUG("Initialized root directory");
}

/**
 * @brief Finds the pointer to a node given a pathname
 * 
 * @param pathname The path of the node
 * @return NODE* The pointer to the node or NULL if it's not found
 */
NODE *find_node(const char *pathname) {
    NODE *p   = NULL;
    char *tok = NULL;
    char buffer[128];

    if (pathname[0] == '/') {
        p = root;
    } else {
        p = cwd;
    }
    
    strcpy(buffer, pathname);

    tok = strtok(buffer, "/");

    while(tok) {
        //DEBUG("tok = %s", tok);
        if (strcmp(tok, ".") == 0) {
            // do nothing
        } else if (strcmp(tok, "..") == 0) {
            // traverse up
            p = p->parent;
        } else {
            DEBUG("tok: %s, p: %s", tok, p->name);
            p = search_child(p, tok);

            if (!p) break;
        }
        
        tok = strtok(NULL, "/");
    }

    return p;
}

/**
 * @brief Searches a directory for a child node
 * 
 * @param parent The dir to search
 * @param name The basename of the node
 * @return NODE* Returns the node or NULL if not found
 */
NODE *search_child(NODE *parent, char *name)
{
  NODE *p;
  DEBUG("search for '%s' in '%s'", name, parent->name);
  p = parent->child;
  if (p==0)
    return 0;
  while(p) {
    if (strcmp(p->name, name)==0)
      return p;
    p = p->sibling;
  }
  return 0;
}

/**
 * @brief Inserts a child onto a directory's linked list
 * 
 * @param parent The directory
 * @param q The node to insert
 * @return int Returns 0
 */
int insert_child(NODE *parent, NODE *q)
{
  NODE *p;
  DEBUG("insert NODE %s into parent child list", q->name);
  p = parent->child;
  if (p==0)
    parent->child = q;
  else{
    while(p->sibling)
      p = p->sibling;
    p->sibling = q;
  }
  q->parent = parent;
  q->child = 0;
  q->sibling = 0;

  return 0;
}

// char* get_basename(char *pathname)
// {
//     int length = strlen(pathname);
//     char *p = pathname + length - 1;
    
//     if (*p == '/') {
//         DEBUG("ends with slash");
//         // if it ends with a slash, just ignore it
//         *p-- = '\0';
//     }

//     while(*p != '/' && p != pathname) {
//         p--;
//     }

//     if (*p == '/') {
//         // if we've iterated down to a slash
//         *p++ = '\0';
//     }

//     return p;
// }

/**
 * @brief Get the path of a node and saves it to the buffer
 * TODO check for bounds
 * 
 * @param buffer The buffer to save onto
 * @param cur The node to find a path to
 */
void get_pwd(char *buffer, NODE *cur) {
    if (cur == cur->parent) {
        buffer[0] = '/';
        buffer[1] = '\0';
        return;
    }
    get_pwd(buffer, cur->parent);
    int len = strlen(buffer);
    if (len != 1) {
        strcpy(buffer + len, "/");
    }
    strcpy(buffer + strlen(buffer), cur->name);
}

void destroy(NODE *parent) {
    if (parent == NULL)
        return;

    destroy(parent->sibling);
    destroy(parent->child);

    free(parent);
}