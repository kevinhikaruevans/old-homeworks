#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <signal.h>
#include <pthread.h>

#include "./commands.h"
#include "../shared/printutil.h"

#define LISTEN_ADDR "127.0.0.1"
#define LISTEN_PORT 8000
#define LISTEN_QUEUE 5
#define CLIENT_THREADS 8

int server_sock;
pthread_t threads[CLIENT_THREADS];
pthread_cond_t cond;
pthread_mutex_t mutex;
int accepted_socket;
bool running;

void handle_int(int sig) {
    /* capture ctrl+c and close the socket */

    //signal(sig, SIG_IGN)
    //shutdown(server_sock, 2);
    close(server_sock);
    exit(0);
}

void enable_keepalive(int sock) {
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

    int idle = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

    int interval = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

    int maxpkt = 10;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));
}

int init_server() {
    struct sockaddr_in saddr = {0};
    int enable_reuse = 1;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock < 0) {
        ERROR("socket creation failed, error: %x", server_sock);
        exit(0);
    }
    
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(int)) < 0) {
        ERROR("setsockopt(SO_REUSEADDR) failed");
        exit(0);
    }
    enable_keepalive(server_sock);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(LISTEN_ADDR);
    saddr.sin_port = htons(LISTEN_PORT);

    if (bind(server_sock, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) {
        ERROR("failed to bind to addr %s:%d", LISTEN_ADDR, LISTEN_PORT);
        exit(0);
    }

    if (listen(server_sock, LISTEN_QUEUE) != 0) {
        ERROR("failed to listen for requests");
        exit(0);
    }

    //signal(SIGINT, handle_int);

    DEBUG("listening on %s:%d", LISTEN_ADDR, LISTEN_PORT);
    return 0;
}

void *handle_socket(void *data) {
    int id = (int)data;
    int socket = -1;
    char buffer[512];
    int n, status;

    while(true) {
        //DEBUG("[thread %d] mutex locked", id);
        pthread_mutex_lock(&mutex);
        while(accepted_socket == -1) {
            pthread_cond_wait(&cond, &mutex);
        }
        socket = accepted_socket;
        accepted_socket = -1;
        pthread_mutex_unlock(&mutex);
        //DEBUG("[thread %d] mutex unlocked", id);
        DEBUG("[thread %d] took socket %x", id, socket);
        
        while(socket != -1) {
            DEBUG("reading...");
            n = read(socket, buffer, sizeof(buffer));

            if (n == 0) {
                DEBUG("[thread %d] socket %x died? closing", id, socket);
                shutdown(socket, 2);
                socket = -1;
                break;
            }

            buffer[n] = 0x00; //TODO check bound
            
            DEBUG("[thread %d][socket %x] received %d bytes", id, socket, n);
            status = handle_data_receive(socket, buffer);

            if (status != 0) {
                DEBUG("[thread %d] handler returned error, closing", id);
                shutdown(socket, 2);
                socket = -1;
                break;
            }

            DEBUG("sending OK");
            
            bzero(buffer, 0);
            strcpy(buffer, "OK");
            int written = write(socket, buffer, sizeof(buffer));
            
            DEBUG("done! written = %d", written);
            //n = write(socket, buffer, sizeof(buffer));
            //DEBUG("[thread %d][socket %x] wrote %d bytes", id, socket, n);
        }
        //shutdown(socket, 2);
        DEBUG("[thread %d] shutdown socket %x", id, socket);
    }
}

void init_threads() {
    accepted_socket = -1;

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < CLIENT_THREADS; i++) {
        pthread_create(&threads[i], 0, handle_socket, (void *)i);
    }    
}

void begin_listen_loop() {
    int client_sock;
    struct sockaddr_in caddr;
    int caddr_length = sizeof(caddr);   

    while(1) {
        DEBUG("[main] waiting for socket...");

        client_sock = accept(server_sock, (struct sockaddr *)&caddr, &caddr_length);

        if (client_sock < 0) {
            ERROR("[main] failed to accept client socket, sleeping...");
            //sleep(1);
            continue;
        }

        DEBUG("[main] accepted socket: %s:%d", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        pthread_mutex_lock(&mutex);
        //DEBUG("[main] mutex locked");
        if (accepted_socket == -1) {
            accepted_socket = client_sock;
        } else {
            ERROR("[main] all threads busy! closing socket");
            close(client_sock);
        }
        pthread_mutex_unlock(&mutex);
        //DEBUG("[main] mutex unlocked");

        pthread_cond_signal(&cond);
    }
}


int main() {
    char cwd[128];
    int tmp;

    getcwd(cwd, sizeof(cwd));
    printf("cwd = %s\n", cwd);

    if (tmp = chdir(cwd)) {
        ERROR("cd to '%s' failed: %d, %d", cwd, tmp, errno);
        return 1;
    }

    if (tmp = chroot(cwd)) {
        ERROR("chroot to '%s' failed: %d, %d. are you running as root?", cwd, tmp, errno);
        return 1;
    } else {
        DEBUG("chroot to '%s'", cwd);
    }
    
    init_server();
    init_threads();
    begin_listen_loop();

    return 0;
}