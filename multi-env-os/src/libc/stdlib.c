#include "stdlib.h"

void exit(int status) { while(1); }
int abs(int x) { return x >= 0 ? x : -x; }
int rand(void) { return 4; }
void srand(unsigned int seed) { }
int system(const char *cmd) { return -1; } // <--- 新增 system stub
