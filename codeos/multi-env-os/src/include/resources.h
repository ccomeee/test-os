#ifndef RESOURCES_H
#define RESOURCES_H

#include <libc.h>

// 查找資源，成功回傳 0 並填入指標與長度；失敗回傳負值
int find_resource(const char* name, const unsigned char** data, unsigned int* len);

#endif // RESOURCES_H
