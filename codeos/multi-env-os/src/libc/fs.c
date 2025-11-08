#include "fs.h"
#include "errno.h"

int open(const char *pathname, int flags, int mode) {
    (void)pathname;
    (void)flags;
    (void)mode;
    errno = ENOSYS;
    return -1;
}

int read(int fd, void *buf, size_t count) {
    (void)fd;
    (void)buf;
    (void)count;
    errno = ENOSYS;
    return -1;
}

int write(int fd, const void *buf, size_t count) {
    (void)fd;
    (void)buf;
    (void)count;
    errno = ENOSYS;
    return -1;
}

int close(int fd) {
    (void)fd;
    errno = ENOSYS;
    return -1;
}

int stat(const char *pathname, void *statbuf) {
    (void)pathname;
    (void)statbuf;
    errno = ENOSYS;
    return -1;
}