#include "signal.h"

int signal(int signum, sighandler_t handler) { return -1; }
int kill(int pid, int sig) { return -1; }
int sigaction(int signum, const void *act, void *oldact) { return -1; }