#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

void *malloc(size_t size);
void free(void *ptr);
void *sbrk(intptr_t increment);
void *mmap(void *addr, size_t length, int prot, int flags, int fd, size_t offset);

#endif