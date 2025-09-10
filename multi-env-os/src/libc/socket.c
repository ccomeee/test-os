#include "socket.h"

int socket(int domain, int type, int protocol) { return -1; }
int bind(int sockfd, const void *addr, size_t addrlen) { return -1; }
int listen(int sockfd, int backlog) { return -1; }
int connect(int sockfd, const void *addr, size_t addrlen) { return -1; }
int accept(int sockfd, void *addr, size_t *addrlen) { return -1; }
int send(int sockfd, const void *buf, size_t len, int flags) { return -1; }
int recv(int sockfd, void *buf, size_t len, int flags) { return -1; }