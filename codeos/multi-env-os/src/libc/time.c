#include "time.h"

time_t time(time_t *t) {
    static time_t fake_time = 1710000000; // 固定假時間
    if (t) *t = fake_time;
    return fake_time;
}

struct tm *localtime(const time_t *timer) {
    static struct tm tm;
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 12;
    tm.tm_mday = 1;
    tm.tm_mon = 0;
    tm.tm_year = 124;
    tm.tm_wday = 1;
    tm.tm_yday = 1;
    tm.tm_isdst = 0;
    return &tm;
}

size_t strftime(char *s, size_t max, const char *format, const struct tm *tm) {
    // 只回傳固定格式
    const char *fixed = "2024-01-01 12:00:00";
    size_t len = 0;
    while (fixed[len] && len < max - 1) {
        s[len] = fixed[len];
        len++;
    }
    s[len] = '\0';
    return len;
}

int gettimeofday(struct timeval *tv, void *tz) { return -1; }