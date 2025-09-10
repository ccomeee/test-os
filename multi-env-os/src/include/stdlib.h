#ifndef STDLIB_H
#define STDLIB_H

void exit(int status);
int abs(int x);
int rand(void);
void srand(unsigned int seed);
int system(const char *cmd);  // <--- 新增 system 宣告

#endif // STDLIB_H