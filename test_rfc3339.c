#include "rfc3339.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Mock.
int gettimeofday(struct timeval *restrict tp, void *restrict tzp)
{
    assert(tp != NULL);
    assert(tzp == NULL);
    tp->tv_sec = 851042397; // 1996-12-20T00:39:57Z
    tp->tv_usec = 1;
    return 0;
}

static void test_rfc3339_format_local(void)
{
    struct timeval tv;
    struct tm *local;
    char buf[80];
    int r;

    setenv("TZ", "America/Los_Angeles", 1);
    gettimeofday(&tv, NULL);
    tv.tv_sec--;
    local = localtime(&tv.tv_sec);

    r = rfc3339_format_local(local, 123, NULL, 0);
    assert(-EFAULT == r);
    r = rfc3339_format_local(local, 123, buf, strlen("YYYY-MM-DDTHH:MM:SS"));
    assert(-ENOMEM == r);
    r = rfc3339_format_local(local, 123, buf, strlen("YYYY-MM-DDTHH:MM:SS.UUUUUU+hh:mm"));
    assert(-ENOMEM == r);

    r = rfc3339_format_local(local, 123, buf, strlen("YYYY-MM-DDTHH:MM:SS.UUUUUU+hh:mm") + 1);
    assert(0 == r);
    assert(!strcmp("1996-12-19T16:39:56.000123-08:00",buf));
}

static void test_rfc3339_format_local_now(void)
{
    char buf[80];
    int r;

    r = rfc3339_format_local_now(buf, strlen("YYYY-MM-DDTHH:MM:SS.UUUUUU+hh:mm") + 1);
    assert(0 == r);
    assert(!strcmp("1996-12-19T16:39:57.000001-08:00", buf));
}

static void test_rfc3339_format(void)
{
    struct timeval tv;
    struct tm *utc;
    char buf[80];
    int r;

    gettimeofday(&tv, NULL);
    tv.tv_sec--;
    utc = gmtime(&tv.tv_sec);

    r = rfc3339_format(utc, 34, NULL, 0);
    assert(-EFAULT == r);
    r = rfc3339_format(utc, 34, buf, strlen("YYYY-MM-DDTHH:MM:SS"));
    assert(-ENOMEM == r);
    r = rfc3339_format(utc, 34, buf, strlen("YYYY-MM-DDTHH:MM:SS.UUUUUUZ"));
    assert(-ENOMEM == r);

    r = rfc3339_format(utc, 34, buf, strlen("YYYY-MM-DDTHH:MM:SS.UUUUUUZ") + 1);
    assert(0 == r);
    assert(!strcmp("1996-12-20T00:39:56.000034Z", buf));
}

static void test_rfc3339_format_now(void)
{
    char buf[80];
    int r;

    r = rfc3339_format_now(buf, sizeof(buf));
    assert(0 == r);
    assert(!strcmp("1996-12-20T00:39:57.000001Z", buf));
}

int main(void)
{
    test_rfc3339_format_local();
    test_rfc3339_format_local_now();
    test_rfc3339_format();
    test_rfc3339_format_now();
}
