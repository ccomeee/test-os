#include "mem.h"

void *malloc(size_t size) { return 0; }
void free(void *ptr) { }
void *sbrk(intptr_t increment) { return 0; }
void *mmap(void *addr, size_t length, int prot, int flags, int fd, size_t offset) { return 0; }