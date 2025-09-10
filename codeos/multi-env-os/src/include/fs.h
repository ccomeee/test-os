#ifndef FS_H
#define FS_H

#include <stddef.h>

int open(const char *pathname, int flags, int mode);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int close(int fd);
int stat(const char *pathname, void *statbuf);

#endif