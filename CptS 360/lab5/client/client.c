// ------------------------------ Client ------------------------------–

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/dir.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#include <fcntl.h>
#include <dirent.h>
#include <libgen.h> // for dirname()/basename()
#include <time.h>

#define MAX 1024
#define BLK 1024

// struct sockaddr_in saddr;

int serverPORT, serverIP;
int sock;

// run as client server_name server_port
struct sockaddr_in server_addr;
struct hostent *hp;

char *myenv;

int init(char *argv[])
{
    printf("======= Client Init ==========\n");

    // 1. get server IP by name
    printf("1. get server IP by name\n");
    hp = gethostbyname(argv[1]);
    if (!hp)
    {
        printf("Error: Invalid hostname: %s\n", argv[1]);
        exit(0);
    }
    serverIP = *(long *)hp->h_addr;
    serverPORT = atoi(argv[2]);

    // 2. create TCP socket
    printf("2. create TCP socket\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("Error: Socket creation failed\n");
        exit(0);
    }

    // 3. fill server_addr with server IP and PORT#
    printf("3. fill in server IP and port number\n");
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = serverIP;
    server_addr.sin_port = htons(serverPORT);

    // 4. connect to server
    printf("4. connect to server\n");
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }

    // 5. confirm connection
    printf("5. connected to server OK\n");
    printf("-------------------------------------------------------\n");
    printf("Server hostname\t= %s\nServer PORT\t= %d\nServer IP\t= %s\n", hp->h_name, serverPORT, inet_ntoa(server_addr.sin_addr));
    printf("-------------------------------------------------------\n");
    printf("========= init done ===============\n");
    return 1;
}

void print_menu()
{
    printf("\n********************** menu *********************\n"
           "* get  put  ls   cd   pwd   mkdir   rmdir   rm  *\n"
           "* lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *\n"
           "*************************************************\n\n");
}

int main(int argc, char *argv[])
{
    int n;
    char line[MAX], ans[MAX];
    // tokens[MAX];
    myenv = getenv("HOME");

    // (1). Check argument values
    if (argc < 3)
    {
        printf("Error: run as client server_name server_port.\n");
        printf("Example -> './client localhost 8000'\n");
        return 0;
    }

    // (2). Connect to server at server hostname and port number
    init(argv);
    print_menu();

    // (3). Prompt user for a command line = cmd pathname
    printf("\n========================================================\n");
    printf("processing loop\n");
    while (1)
    {
        char cmd[128];
        char name[128];
        char input[MAX];
        char temp[MAX];

        printf("\nCLIENT [input a command] : ");
        bzero(line, MAX);
        fgets(line, MAX, stdin);

        line[strlen(line) - 1] = 0; // kill <CR> at end
        strcpy(temp, line);

        char *tokens = strtok(temp, " ");

        // Exit if NULL line
        if (line[0] == 0) 
        {
            printf("Command line is NULL: Exiting client.\n");
            exit(0);
        }

        // Exit if the 'quit' command is entered
        else if (strcmp(line, "quit") == 0)
        {
            printf("QUIT: Exiting client.\n");
            exit(0);
        }

        // Local commands with no pathname
        else if (!strcmp(line, "lls") || !strcmp(line, "lcat") || !strcmp(line, "lcd") 
             || !strcmp(line, "lpwd") || !strcmp(line, "lmkdir") || !strcmp(line, "lrmdir") 
             || !strcmp(line, "lrm"))
        {
            char *p = line;
            p++;
            strcpy(cmd, p);
            handle_command(cmd);
        }

        // Local commands with pathnames
        else if (!strcmp(tokens, "lls") || !strcmp(tokens, "lcat") || !strcmp(tokens, "lcd") 
             || !strcmp(tokens, "lpwd") || !strcmp(tokens, "lmkdir") || !strcmp(tokens, "lrmdir") 
             || !strcmp(tokens, "lrm"))
        {
            char *p = tokens;
            p++;
            strcpy(cmd, p);
            // printf("CMD: %s\n", cmd);
            tokens = strtok(NULL, " ");
            strcpy(input, cmd);
            if (tokens)
            {
                strcpy(name, tokens);
                // printf("name: %s\n", name);
                tokens = strtok(NULL, " ");
                strcat(input, " ");
                strcat(input, name);
            }
            handle_command(input);
        }

        // Server commands
        else
        {
            // Send ENTIRE line to server
            n = write(sock, line, strlen(line));
            printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

            // Read a line from sock and show it       
            bzero(ans, MAX);     
            
            while((n = read(sock, ans, MAX)) > 0) {
                printf(ans);
                int length = strlen(ans);

                if (length >= 2) {
                    char *end = ans + strlen(ans) - 2;

                    if (strcmp(end, "OK") == 0)
                        break;
                }
                bzero(ans, MAX);
            }
            
            //printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
        }
        memset(line, 0, sizeof(line));
        printf("\n========================================================\n");
    }
    return 0;
}

void handle_command(char command[MAX])
{
    //command[strlen(command) - 1] = 0; // fgets() has \n at end
    char *cmd = strtok(command, " ");
    char *arg1 = strtok(0, " ");
    if (!cmd)
    {
        return;
    }
    else if (!strcmp(cmd, "cd"))
    {
        if (arg1)
        {
            chdir(arg1);
        }
        else if (!arg1)
        {
            chdir(myenv);
        }
    }
    else if (!strcmp(cmd, "exit"))
    {
        exit(1);
    }
    else
    {
        char *myargv[MAX];
        int pid, status;
        int temp = 0;
        char *redirection = "";
        int j = 1;
        myargv[0] = cmd;
        while (arg1 != NULL)
        {
            myargv[j] = arg1;
            arg1 = strtok(0, " ");
            j++;
        }
        if (temp != 0)
        {
            myargv[temp] = NULL;
        }
        else
        {
            myargv[j] = NULL;
        }
        pid = fork();
        if (pid)
        {
            printf("\n");
            pid = wait(&status);
        }
        else
        {
            execvp(cmd, myargv);
            printf("cmd %s not found, child sh exit\n", cmd);
            exit(123);
        }
    }
}
