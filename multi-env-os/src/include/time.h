#ifndef TIME_H
#define TIME_H

#include <stddef.h>

typedef long time_t;

time_t time(time_t *t);
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
struct tm *localtime(const time_t *timer);
size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);

struct timeval {
    long tv_sec;
    long tv_usec;
};

int gettimeofday(struct timeval *tv, void *tz);

#endif // TIME_H