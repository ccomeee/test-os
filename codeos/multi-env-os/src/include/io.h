#ifndef IO_H
#define IO_H

#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "time.h"

typedef struct pollfd {
    int fd;
    short events;
    short revents;
} pollfd;

typedef unsigned long nfds_t;

int scanf(const char *fmt, ...);
char *fgets(char *s, int size, FILE *stream);
int strncmp(const char *s1, const char *s2, size_t n);

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int select(int nfds, void *readfds, void *writefds, void *exceptfds, struct timeval *timeout);

#endif // IO_H