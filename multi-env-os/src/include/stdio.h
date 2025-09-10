#ifndef STDIO_H
#define STDIO_H

#include "stddef.h"  // 添加这一行，定义 size_t
#include "stdarg.h"  // 添加这一行，定义 va_list

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char *format, ...);
int vprintf(const char *format, va_list ap);
int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int putchar(int c);
int puts(const char *s);

typedef struct {
    int fd;          // 文件描述符
    int flags;       // 文件标志
    int eof;         // 文件结束标志
    int err;         // 错误标志
    char *buffer;    // 缓冲区指针
    size_t bufsize;  // 缓冲区大小
    size_t pos;      // 当前位置
} FILE;

int fprintf(FILE *stream, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fputs(const char *s, FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);

#ifdef __cplusplus
}
#endif

#endif // STDIO_H