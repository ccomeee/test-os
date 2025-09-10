#ifndef STDDEF_H
#define STDDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define NULL ((void*)0)

typedef unsigned long size_t;
typedef long ptrdiff_t;
typedef long intptr_t;

// typedef unsigned short wchar_t;

#define offsetof(type, member) ((size_t)&(((type *)0)->member))

#ifdef __cplusplus
}
#endif

#endif // STDDEF_H