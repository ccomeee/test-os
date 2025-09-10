#ifndef SIGNAL_H
#define SIGNAL_H

//#include <sys/types.h>

typedef void (*sighandler_t)(int);

int signal(int signum, sighandler_t handler);
int kill(int pid, int sig);
int sigaction(int signum, const void *act, void *oldact);

#endif
