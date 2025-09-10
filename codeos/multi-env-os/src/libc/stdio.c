#include "stdio.h"

// 简单的存根实现
int printf(const char *format, ...) {
    // 这里可以调用 vga_print 或其他输出函数
    return 0;
}

int sprintf(char *str, const char *format, ...) {
    // 格式化字符串到缓冲区
    return 0;
}

int snprintf(char *str, size_t size, const char *format, ...) {
    // 安全的格式化字符串到缓冲区
    return 0;
}

int puts(const char *s) {
    // 这里可以调用 vga_print 或其他输出函数
    return 0;
}

int putchar(int c) {
    // 这里可以调用 vga_putchar 或其他输出函数
    return c;
}

FILE *fopen(const char *filename, const char *mode) { return (FILE *)0; }
int fclose(FILE *stream) { return 0; }
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) { return 0; }
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) { return 0; }
int fputs(const char *s, FILE *stream) { return 0; }
char *fgets(char *s, int size, FILE *stream) { return NULL; }
int remove(const char *filename) { return 0; }
int rename(const char *oldname, const char *newname) { return 0; }
int fprintf(FILE *stream, const char *format, ...) { return 0; }
int sscanf(const char *str, const char *format, ...) { return 0; }