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
    r = rfc3339_format_local(local, 123, buf, strlen("YYYY-MM-DDTHH:MM:SS.NNNNNNNNN+hh:mm"));
    assert(-ENOMEM == r);
    r = rfc3339_format_local(local, 999999999ULL+1, buf, strlen("YYYY-MM-DDTHH:MM:SS.NNNNNNNNN+hh:mm") + 1);
    assert(-ERANGE == r);

    r = rfc3339_format_local(local, 123, buf, strlen("YYYY-MM-DDTHH:MM:SS.NNNNNNNNN+hh:mm") + 1);
    assert(0 == r);
    assert(!strcmp("1996-12-19T16:39:56.000000123-08:00",buf));
}

static void test_rfc3339_format_local_now(void)
{
    char buf[80];
    int r;

    r = rfc3339_format_local_now(buf, strlen("YYYY-MM-DDTHH:MM:SS.NNNNNNNNN+hh:mm") + 1);
    assert(0 == r);
    assert(!strcmp("1996-12-19T16:39:57.000001000-08:00", buf));
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
    r = rfc3339_format(utc, 34, buf, strlen("YYYY-MM-DDTHH:MM:SS.NNNNNNNNNZ"));
    assert(-ENOMEM == r);
    r = rfc3339_format(utc, 999999999ULL+1, buf, strlen("YYYY-MM-DDTHH:MM:SS.NNNNNNNNNZ") + 1);
    assert(-ERANGE == r);

    r = rfc3339_format(utc, 34, buf, strlen("YYYY-MM-DDTHH:MM:SS.NNNNNNNNNZ") + 1);
    assert(0 == r);
    assert(!strcmp("1996-12-20T00:39:56.000000034Z", buf));
}

static void test_rfc3339_format_now(void)
{
    char buf[80];
    int r;

    r = rfc3339_format_now(buf, sizeof(buf));
    assert(0 == r);
    assert(!strcmp("1996-12-20T00:39:57.000001000Z", buf));
}

static void test_rfc3339_parse(void)
{
    struct tm tm;
    uint64_t nsec;
    long off;
    time_t t;
    int r;

    r = rfc3339_parse("1999-", &tm, NULL);
    assert(-EFAULT == r);

    r = rfc3339_parse("", &tm, &nsec);
    assert(-EILSEQ == r);

    r = rfc3339_parse("01066-...", &tm, &nsec);
    assert(-EILSEQ == r);
    r = rfc3339_parse("106a-", &tm, &nsec);
    assert(-EILSEQ == r);
    r = rfc3339_parse("1066-...", &tm, &nsec);
    assert(-ERANGE == r);

    r = rfc3339_parse("1999-012-...", &tm, &nsec);
    assert(-EILSEQ == r);
    r = rfc3339_parse("1999-13-...", &tm, &nsec);
    assert(-ERANGE == r);

    r = rfc3339_parse("1999-10-012t...", &tm, &nsec);
    assert(-EILSEQ == r);
    r = rfc3339_parse("1999-10-32T...", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-10-01-...", &tm, &nsec);
    assert(-EILSEQ == r);

    r = rfc3339_parse("1999-00-01T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-01-00T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-01-32T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-02-29T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("2000-02-30T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-03-32T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-04-31T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-05-32T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-06-31T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-07-32T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-08-32T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-09-31T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-10-32T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-11-31T", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-12-32T", &tm, &nsec);
    assert(-ERANGE == r);

    r = rfc3339_parse("1999-10-20T012:...", &tm, &nsec);
    assert(-EILSEQ == r);
    r = rfc3339_parse("1999-10-20T24:...", &tm, &nsec);
    assert(-ERANGE == r);

    r = rfc3339_parse("1999-10-20T11:012:...", &tm, &nsec);
    assert(-EILSEQ == r);
    r = rfc3339_parse("1999-10-20T11:60:...", &tm, &nsec);
    assert(-ERANGE == r);

    r = rfc3339_parse("1999-10-20T11:22:012Z", &tm, &nsec);
    assert(-EILSEQ == r);
    r = rfc3339_parse("1999-10-20T11:22:61Z", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("1999-10-20T11:22:60.Z", &tm, &nsec);
    assert(-EILSEQ == r);

    r = rfc3339_parse("1999-10-20T11:22:60", &tm, &nsec);
    assert(-EILSEQ == r);

    r = rfc3339_parse("2022-08-24T20:27:00.000048+24:00", &tm, &nsec);
    assert(-ERANGE == r);
    r = rfc3339_parse("2022-08-24T20:27:00.000048+23:60", &tm, &nsec);
    assert(-ERANGE == r);

    r = rfc3339_parse("2022-08-24T20:27:00.000048+03:00!", &tm, &nsec);
    assert(-EINVAL == r);

    r = rfc3339_parse("1996-12-19T16:39:57.001-08:00", &tm, &nsec);
    assert(0 == r);
    assert(1000000 == nsec);
    assert(-8*60*60 == tm.tm_gmtoff);
    off = tm.tm_gmtoff;
    t = timegm(&tm) - off;
    assert(851042397 == t);

    r = rfc3339_parse("1996-12-20T00:39:57.001Z", &tm, &nsec);
    assert(0 == r);
    assert(1000000 == nsec);
    assert(0 == tm.tm_gmtoff);
    t = timegm(&tm);
    assert(851042397 == t);

    r = rfc3339_parse("1996-12-20T00:39:57Z", &tm, &nsec);
    assert(0 == r);
    assert(0 == nsec);
    t = timegm(&tm);
    assert(851042397 == t);
}

int main(void)
{
    test_rfc3339_format_local();
    test_rfc3339_format_local_now();
    test_rfc3339_format();
    test_rfc3339_format_now();
    test_rfc3339_parse();
}
