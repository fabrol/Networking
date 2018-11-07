/*****************************************************************************
 * server-c.c                                                                 
 * Name:
 * NetId:
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048

/* TODO: server()
 * Open socket and wait for client to connect
 * Print received message to stdout
 * Return 0 on success, non-zero on failure
 */
int server(char *server_port) {

        int s, sockfd;
        struct addrinfo hints;
        struct addrinfo *serv_info, *p;
        char recv_data[RECV_BUFFER_SIZE];

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

        if ((s = getaddrinfo(NULL, server_port, &hints, &serv_info)) != 0) {
                return s;
        }

        for(p=serv_info; p!=NULL; p = p->ai_next) {

                if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                        perror("server: socket");
                        return sockfd;
                }

                int yes=1;
                if (setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
                        perror("setsockopt");
                        exit(1);
                }

                if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
                        perror("server: bind");
                        return -1;
                }

                if ((listen(sockfd, QUEUE_LENGTH)) == -1) {
                        perror("server: listen");
                        return -1;
                }
                break;
        }

        int client_sock_fd;
        struct sockaddr client_addr;
        socklen_t addr_size;

        while(1) {
                if ((client_sock_fd = accept(sockfd, &client_addr, &addr_size)) == -1) {
                        perror("server : Client error. Continue");
                        continue;
                }

                while (1) {
                        s = recv(client_sock_fd, &recv_data, RECV_BUFFER_SIZE, 0);
                        if (s == -1) {
                                perror("server : Client error. Continue");
                                close(client_sock_fd);
                                break;
                        } else if (s == 0) {
                                close(client_sock_fd);
                                break;
                        }

                        fwrite(recv_data, sizeof(char), s, stdout);
                        fflush(stdout);
                }
        }

        close(sockfd);
        freeaddrinfo(serv_info);

        return 0;
}

/*
 * main():
 * Parse command-line arguments and call server function
 */
int main(int argc, char **argv) {
        char *server_port;

        if (argc != 2) {
                fprintf(stderr, "Usage: ./server-c [server port]\n");
                exit(EXIT_FAILURE);
        }

        server_port = argv[1];
        return server(server_port);
}
