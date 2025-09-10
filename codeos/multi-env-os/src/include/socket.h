#ifndef SOCKET_H
#define SOCKET_H

#include <stddef.h>
#include <stdint.h>

int socket(int domain, int type, int protocol);
int bind(int sockfd, const void *addr, size_t addrlen);
int listen(int sockfd, int backlog);
int connect(int sockfd, const void *addr, size_t addrlen);
int accept(int sockfd, void *addr, size_t *addrlen);
int send(int sockfd, const void *buf, size_t len, int flags);
int recv(int sockfd, void *buf, size_t len, int flags);

#endif