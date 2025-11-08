#ifndef ERRNO_H
#define ERRNO_H

extern int errno;
char *strerror(int errnum);

#define EAGAIN 11
#define EINVAL 22
#define ENOENT 2
#define ENOMEM 12
#define ENOSYS 38
#define ECHILD 10

#endif // ERRNO_H