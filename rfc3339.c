#include "rfc3339.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int rfc3339_format_local(const struct tm *tm, uint64_t nsec, char *buf, size_t cap)
{
    size_t len;

    if (!tm || !buf) {
        return -EFAULT;
    }

    if (nsec > 999999999ULL) {
        return -ERANGE;
    }

    len = strftime(buf, cap, "%Y-%m-%dT%H:%M:%S", tm);
    if (len == 0) {
        return -ENOMEM;
    }

    buf += len;
    cap -= len;

    len = snprintf(buf, cap, ".%09" PRIu64 "%+03ld:%02ld", nsec, tm->tm_gmtoff / 3600, tm->tm_gmtoff % 3600);
    if (len >= cap) {
        return -ENOMEM;
    }

    return 0;
}

int rfc3339_format(const struct tm *tm, uint64_t nsec, char *buf, size_t cap)
{
    size_t len;

    if (!tm || !buf) {
        return -EFAULT;
    }

    if (nsec > 999999999ULL) {
        return -ERANGE;
    }

    len = strftime(buf, cap, "%Y-%m-%dT%H:%M:%S", tm);
    if (len == 0) {
        return -ENOMEM;
    }

    buf += len;
    cap -= len;

    len = snprintf(buf, cap, ".%09" PRIu64 "Z", nsec);
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
    return rfc3339_format_local(&local, (uint64_t)tv.tv_usec * 1000, buf, cap);
}

int rfc3339_format_now(char *buf, size_t cap)
{
    struct timeval tv;
    time_t t;
    struct tm utc;

    gettimeofday(&tv, 0);
    t = tv.tv_sec;
    gmtime_r(&t, &utc);
    return rfc3339_format(&utc, (uint64_t)tv.tv_usec * 1000, buf, cap);
}

static bool is_leap_year(int year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static unsigned mday_max(const struct tm *tm)
{
    switch (tm->tm_mon + 1) {
        default:
        case 1:
            return 31;
        case 2:
            return is_leap_year(tm->tm_year + 1900) ? 29 : 28;
        case 3:
            return 31;
        case 4:
            return 30;
        case 5:
            return 31;
        case 6:
            return 30;
        case 7:
            return 31;
        case 8:
            return 31;
        case 9:
            return 30;
        case 10:
            return 31;
        case 11:
            return 30;
        case 12:
            return 31;
    }
}

int rfc3339_parse(const char *str, struct tm *tm, uint64_t *nsec)
{
    uint64_t value = 0;
    uint64_t tmp;

    if (!str || !tm || !nsec) {
        return -EFAULT;
    }

    memset(tm, 0, sizeof(struct tm));
    *nsec = 0;

#define digit() \
    if (isdigit(*str)) value = value * 10 + *str++ - '0'; else return -EILSEQ

#define optional_digit() \
    if (isdigit(*str)) value = value * 10 + *str++ - '0'; else value *= 10

#define match(c) \
    if (*str++ != (c)) return -EILSEQ

#define peek(c) \
    (*str == (c))

#define clamp(low, high) \
    if (value < (low) || value > (high)) return -ERANGE

#define take() \
    (tmp = value, value = 0, tmp)

    digit();
    digit();
    digit();
    digit();
    match('-');
    clamp(1900, 9999);
    tm->tm_year = take() - 1900;

    digit();
    digit();
    match('-');
    clamp(1, 12);
    tm->tm_mon = take() - 1;

    digit();
    digit();
    match('T');
    clamp(1, mday_max(tm));
    tm->tm_mday = take();

    digit();
    digit();
    match(':');
    clamp(0, 23);
    tm->tm_hour = take();

    digit();
    digit();
    match(':');
    clamp(0, 59);
    tm->tm_min = take();

    digit();
    digit();
    // RFC allows 60 for leap second.
    clamp(0, 60);
    tm->tm_sec = take();

    if (peek('.')) {
        match('.');
        // One or more fractional digits, nanosecond resolution.
        digit();
        for (int i = 2; i <= 9; ++i) {
            optional_digit();
        }
        *nsec = take();
    }

    if (peek('Z')) {
        match('Z');

    } else {
        int negative = peek('-');
        if (negative) {
            match('-');
        } else {
            match('+');
        }

        digit();
        digit();
        match(':');
        clamp(0, 23);
        tm->tm_gmtoff = take() * 3600;

        digit();
        digit();
        clamp(0, 59);
        tm->tm_gmtoff += take() * 60;

        if (negative) {
            tm->tm_gmtoff *= -1;
        }
    }

    if (*str) {
        return -EINVAL;
    }

    return 0;
}
