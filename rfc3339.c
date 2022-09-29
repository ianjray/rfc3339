#include "rfc3339.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int rfc3339_format_local(const struct tm *tm, uint32_t usec, char *buf, size_t cap)
{
    size_t len;

    if (!tm || !buf) {
        return -EFAULT;
    }

    len = strftime(buf, cap, "%Y-%m-%dT%H:%M:%S", tm);
    if (len == 0) {
        return -ENOMEM;
    }

    buf += len;
    cap -= len;

    len = snprintf(buf, cap, ".%06" PRIu32 "%+03ld:%02ld", usec, tm->tm_gmtoff / 3600, tm->tm_gmtoff % 3600);
    if (len >= cap) {
        return -ENOMEM;
    }

    return 0;
}

int rfc3339_format(const struct tm *tm, uint32_t usec, char *buf, size_t cap)
{
    size_t len;

    if (!tm || !buf) {
        return -EFAULT;
    }

    len = strftime(buf, cap, "%Y-%m-%dT%H:%M:%S", tm);
    if (len == 0) {
        return -ENOMEM;
    }

    buf += len;
    cap -= len;

    len = snprintf(buf, cap, ".%06" PRIu32 "Z", usec);
    if (len >= cap) {
        return -ENOMEM;
    }

    return 0;
}

int rfc3339_format_local_now(char *buf, size_t cap)
{
    struct timeval tv;
    time_t t;
    struct tm local;

    gettimeofday(&tv, 0);
    t = tv.tv_sec;
    localtime_r(&t, &local);
    return rfc3339_format_local(&local, tv.tv_usec, buf, cap);
}

int rfc3339_format_now(char *buf, size_t cap)
{
    struct timeval tv;
    time_t t;
    struct tm utc;

    gettimeofday(&tv, 0);
    t = tv.tv_sec;
    gmtime_r(&t, &utc);
    return rfc3339_format(&utc, tv.tv_usec, buf, cap);
}
