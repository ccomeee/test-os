#include <libc.h>
#include <resources.h>
#include <string.h>

// 暫時 stub：尚未接入資源包，全部回傳未找到
int find_resource(const char* name, const unsigned char** data, unsigned int* len) {
    (void)name; (void)data; (void)len;
    return -1;
}
