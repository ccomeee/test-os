#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>

int fork(void);
int exec(const char *path, char *const argv[]);
int wait(int *status);
int pthread_create(void **thread, void *attr, void *(*start_routine)(void*), void *arg);
void pthread_exit(void *retval);

#endif