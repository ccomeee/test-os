#include "io.h"

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

int poll(struct pollfd *fds, nfds_t nfds, int timeout) { return -1; }
int select(int nfds, void *readfds, void *writefds, void *exceptfds, struct timeval *timeout) { return -1; }

int scanf(const char *fmt, ...) { return 0; }