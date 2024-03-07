#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>

int  socket_create(int domain, int type, int protocol);
void socket_connect(int sockfd, struct sockaddr_storage *addr, in_port_t port);
void socket_close(int client_fd);

#endif /* SOCKET_H */
