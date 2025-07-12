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

static int fill(int mode, uint64_t v, struct tm *tm, uint64_t *nsec)
{
    switch (mode) {
        case 'Y':
            if (v < 1900) {
                return -ERANGE;
            }

            tm->tm_year = v - 1900;
            break;

        case 'm':
            if (v < 1 || v > 12) {
                return -ERANGE;
            }

            tm->tm_mon = v - 1;
            break;

        case 'd':
            if (v < 1 || v > mday_max(tm)) {
                return -ERANGE;
            }

            tm->tm_mday = v;
            break;

        case 'H':
            if (v > 23) {
                return -ERANGE;
            }

            tm->tm_hour = v;
            break;

        case 'M':
            if (v > 59) {
                return -ERANGE;
            }

            tm->tm_min = v;
            break;

        case 'S':
            // Note: Leap second rules are not enforced.
            if (v > 60) {
                return -ERANGE;
            }

            tm->tm_sec = v;
            break;

        case 'N':
        case 'n':
            *nsec = v;
            break;

        case 'O':
            if (v > 23) {
                return -ERANGE;
            }

            tm->tm_gmtoff = v * 3600;
            break;

        case 'o':
            if (v > 59) {
                return -ERANGE;
            }

            tm->tm_gmtoff += v * 60;
            break;
    }

    return 0;
}

int rfc3339_parse(const char *str, struct tm *tm, uint64_t *nsec)
{
    bool negative_offset = false;
    int mode = 0;
    uint64_t v = 0;

    if (!str || !tm || !nsec) {
        return -EFAULT;
    }

    memset(tm, 0, sizeof(struct tm));
    *nsec = 0;

    // Accept pseudo-regex:
    // YYYY-mm-ddTHH:MM:SS(.N{1,9})?(Z|[+-]OO:oo)

    for (const char *p = "YYYY-mm-ddTHH:MM:SS.NnnnnnnnnZ+OO:oo"; ; ++p) {
        if (*p == '.' && *str != *p) {
            // Skip optional fractional part.
            while (*p == '.' || toupper(*p) == 'N') {
                ++p;
            }
        }

        while (*p == 'n' && !isdigit(*str)) {
            // Skip optional fractional digits.
            v *= 10;
            ++p;
        }

        if (*p == 'Z' && (*str == '+' || *str == '-')) {
            // Detect time zone offset.
            p++;
        }

        if (strchr("-T:.Z+", *p)) {
            int r;

            if (!*p) {
                // Final pass to call fill().

            } else if (*p == '+' && *str == '-') {
                negative_offset = true;
                str++;

            } else if (*str++ != *p) {
                // Separator expected in input string.
                return -EILSEQ;
            }

            r = fill(mode, v, tm, nsec);
            if (r < 0) {
                return r;
            }

            v = 0;

        } else {
            if (!isdigit(*str)) {
                // Digit expected in input string.
                return -EILSEQ;
            }

            assert(isalpha(*p));
            mode = *p;
            v *= 10;
            v += *str++ - '0';
        }

        if (!*p || *p == 'Z') {
            // End of pattern reached.
            break;
        }
    }

    if (*str) {
        // Unexpected input remains.
        return -EINVAL;
    }

    if (negative_offset) {
        tm->tm_gmtoff *= -1;
    }

    return 0;
}
