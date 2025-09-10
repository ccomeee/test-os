#include "dl.h"

void *dlopen(const char *filename, int flag) { return 0; }
void *dlsym(void *handle, const char *symbol) { return 0; }
int dlclose(void *handle) { return -1; }