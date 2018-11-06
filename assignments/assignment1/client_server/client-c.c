/*****************************************************************************
 * client-c.c                                                                 
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

#define SEND_BUFFER_SIZE 2048


/* TODO: client()
 * Open socket and send message from stdin.
 * Return 0 on success, non-zero on failure
*/

int send_buffer(char * send_data, int count, int sockfd) {
        int call_bytes_sent, send_off;

        while (send_off != count) {
                if ((call_bytes_sent = send(sockfd,
                                                send_data+send_off,
                                                count,
                                                0)) == -1) {
                        perror("Client: Send");
                        return -1;
                }
                send_off += call_bytes_sent;
        }
        return 0;
}

int client(char *server_ip, char *server_port) {
        int s, sockfd;
        struct addrinfo hints;
        struct addrinfo *serv_info, *p;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ((s = getaddrinfo(server_ip, server_port, &hints, &serv_info)) != 0) {
                return s;
        }

        for(p=serv_info; p!=NULL; p = p->ai_next) {

                if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                        perror("client: socket");
                        return sockfd;
                }

                if ((connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
                        perror("client: listen");
                        return -1;
                }
                break;
        }

        // Take input in chunks until EOF and send
        char send_data[SEND_BUFFER_SIZE];
        int ch;
        int input_off = 0;

        while ((ch=fgetc(stdin)) != EOF) {
                send_data[input_off++] = ch;

                if (input_off == SEND_BUFFER_SIZE) {
                        // Time to ship this off
                        if (send_buffer(send_data, SEND_BUFFER_SIZE, sockfd)) {
                                return -1;
                        }

                        // Reset counters
                        input_off = 0;
                }
        }

        if (input_off != 0) {
                        if (send_buffer(send_data, input_off, sockfd)) {
                                return -1;
                        }
        }

        freeaddrinfo(serv_info);

        return 0;
}

/*
 * main()
 * Parse command-line arguments and call client function
*/
int main(int argc, char **argv) {
  char *server_ip;
  char *server_port;

  if (argc != 3) {
    fprintf(stderr, "Usage: ./client-c [server IP] [server port] < [message]\n");
    exit(EXIT_FAILURE);
  }

  server_ip = argv[1];
  server_port = argv[2];
  return client(server_ip, server_port);
}
