#include "./commands.h"

const struct cmd commands[] = {
    {"pwd",   &cmd_pwd,   ""},
    {"ls",    &cmd_ls,    "<dir>"},
    {"cd",    &cmd_cd,    "[dir]"},
    {"pwd",   &cmd_pwd,   ""},
    {"mkdir", &cmd_mkdir, "[dir name]"},
    {"rmdir", &cmd_rmdir, "[dir name]"},
    {"rm",    &cmd_rm,    "[file name]"},
    {"help",  &cmd_help,  ""},
    {"get",   &cmd_get,   "[filename]"},
    {"put",   &cmd_put,   "[filename]"},
    {NULL}
};

// for kc's ls implementation:
struct stat mystat, *sp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
char TEMP_BUFFER[1024];

#define STRCATF(dest, fmt, ...)                   \
    do                                            \
    {                                             \
        sprintf(TEMP_BUFFER, fmt, ##__VA_ARGS__); \
        strcat(dest, TEMP_BUFFER);                \
    } while (0)

int handle_data_receive(int socket, char *data)
{
    char buffer[1024] = {0};
    char *line = strtok(data, "\r\n\0");
    char *token = strtok(line, " ");

    DEBUG("\t[socket %d] command received = '%s'", socket, token);

    if (token)
    {
        int (*func)(int, char *) = find_cmd_handler(token);
        int code = -1;

        if (func)
        {
            code = func(socket, strtok(NULL, "\0"));
            if (code)
            {
                sprintf(buffer, "command '%s' failed with code %x", token, code);
            }
        }
        else
        {
            sprintf(buffer, "command not found: '%s'", token);
        }
    }
    else
    {
        sprintf(buffer, "invalid input");
    }

    if (buffer[0])
    {
        write(socket, buffer, strlen(buffer));
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Finds a command handler for a specified input
 * 
 * @param input A single-word input, such as `ls` or `mkdir`
 * @return int(*)(char*) Returns a function pointer to the handler
 */
int (*find_cmd_handler(char *input))(int, char *)
{
    struct cmd *c = &commands[0];

    // eh, just a simple linear O(N) search
    while (c && c->text[0])
    {
        if (strcmp(input, c->text) == 0)
        {
            return c->handler;
        }
        c++;
    }

    return NULL;
}



int ls_file(int socket, char *fname)
{
    struct stat fstat, *sp;
    int r, i;
    char ftime[64];
    char line[1024] = {0};

    sp = &fstat;

    if ((r = lstat(fname, &fstat)) < 0)
    {
        ERROR("can’t stat %s\n", fname);
        return 1;
    }

    if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
        STRCATF(line, "%c", '-');
    if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
        STRCATF(line, "%c", 'd');
    if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
        STRCATF(line, "%c", 'l');

    for (i = 8; i >= 0; i--)
    {
        if (sp->st_mode & (1 << i)) // print r|w|x
            STRCATF(line, "%c", t1[i]);
        else
            STRCATF(line, "%c", t2[i]);
        // or print -
    }

    STRCATF(line, "%4d ", sp->st_nlink); // link count
    STRCATF(line, "%4d ", sp->st_gid);
    // gid
    STRCATF(line, "%4d ", sp->st_uid);
    // uid
    STRCATF(line, "%8d ", sp->st_size);
    // file size
    // print time

    strcpy(ftime, ctime(&sp->st_ctime)); // print time in calendar form
    ftime[strlen(ftime) - 1] = 0;

    // kill \n at end
    STRCATF(line, "%s ", ftime);

    // print name
    STRCATF(line, "%s", basename(fname)); // print file basename

    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000) == 0xA000)
    {
        // use readlink() to read linkname
        char linkname[128];
        readlink(fname, linkname, sizeof(linkname));
        STRCATF(line, " -> %s", linkname); // print linked name
    }
    STRCATF(line, "\n");
    write(socket, line, strlen(line));
    return 0;
}
int cmd_pwd(int socket, char *args)
{
    char buffer[256];
    getcwd(buffer, sizeof(buffer));

    write(socket, buffer, strlen(buffer) + 1);
    return EXIT_SUCCESS;
}

int ls_dir(int socket, char *dname)
{
    struct dirent *p;
    struct stat s;
    DIR *dir;
    char buffer[128];

    stat(dname, &s);

    if (S_ISDIR(s.st_mode))
    {
        dir = opendir(dname);

        DEBUG("ls dir -> %s", dname);
        while (dir && (p = readdir(dir)))
        {
            // not super safe, but it'll do...
            strcpy(buffer, dname);
            strcat(buffer, "/");
            strcat(buffer, p->d_name);

            ls_file(socket, buffer);
        }
    }
    else
    {
        DEBUG("ls file -> %s", dname);
        ls_file(socket, dname);
    }
    return EXIT_SUCCESS;
}

int cmd_ls(int socket, char *args)
{
    char *path = args;
    char buffer[128];

    if (!path)
    {
        path = getcwd(buffer, sizeof(buffer));
    }

    return ls_dir(socket, path);
}

int cmd_cd(int socket, char *args)
{
    char *target = strtok(args, "\r\n ");

    if (target)
    {
        return chdir(target);
    }

    return EXIT_FAILURE;
}

int cmd_mkdir(int socket, char *args)
{
    char *target = strtok(args, "\r\n ");

    if (target)
    {
        return mkdir(target, 0755);
    }

    return EXIT_FAILURE;
}

int cmd_rmdir(int socket, char *args)
{
    char *target = strtok(args, "\r\n ");

    if (target)
    {
        return rmdir(target);
    }

    return EXIT_FAILURE;
}

int cmd_rm(int socket, char *args)
{
    char *target = strtok(args, "\r\n ");

    if (target)
    {
        return unlink(target);
    }

    return EXIT_FAILURE;
}

int cmd_help(int socket, char *args)
{
    char buffer[256];
    struct cmd *c = commands;

    strcat(buffer, "available commands:\n");
    write(socket, buffer, strlen(buffer));

    while (c && c->text[0])
    {
        sprintf(buffer, "\t%s %s\n", c->text, c->text_args);
        write(socket, buffer, strlen(buffer));
        c++;
    }

    return EXIT_SUCCESS;
}

int cmd_get(int socket, char *args)
{
    char buffer[1024];
    int bytes;
    int fd = open(args, O_RDONLY, 0644);

    DEBUG("running cmd_get on '%s', fd = %d", args, fd);

    if (fd < 0)
    {
        ERROR("cannot open file '%s'", args);
        return EXIT_FAILURE;
    }

    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0)
    {
        DEBUG("write %d bytes to socket", bytes);
        write(socket, buffer, bytes);
    }

    close(fd);
    return EXIT_SUCCESS;
}

int cmd_put(int socket, char *args) {
    char buffer[1024];
    int fd = open(args, O_CREAT, 0644);
    int bytes;

    while((bytes = read(socket, buffer, sizeof(buffer))) > 0) {
        write(fd, buffer, bytes);
    }

    close(fd);
    return EXIT_SUCCESS;
}