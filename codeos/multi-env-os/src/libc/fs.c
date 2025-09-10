#include "fs.h"

int open(const char *pathname, int flags, int mode) { return -1; }
int read(int fd, void *buf, size_t count) { return -1; }
int write(int fd, const void *buf, size_t count) { return -1; }
int close(int fd) { return -1; }
int stat(const char *pathname, void *statbuf) { return -1; }