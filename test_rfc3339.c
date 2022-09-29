#include "rfc3339.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/// @see rfc3339_date_time_now
/// @see rfc3339_utc_date_time_now
int gettimeofday(struct timeval *restrict tp, void *restrict tzp)
{
    assert(tp != NULL);
    assert(tzp == NULL);
    tp->tv_sec = 1661362020;  // 2022-08-24T17:27:00.000048Z
    tp->tv_usec = 48;
    return 0;
}

int main()
{
    struct timeval tv;
    tv.tv_sec = 851042397;  // 1996-12-20T00:39:57Z
    tv.tv_usec = 0;

    setenv("TZ", "America/Los_Angeles", 1);
    struct tm *local = localtime(&tv.tv_sec);
    // RFC3339 §5.8 1996-12-19T16:39:57-08:00
    assert(!strcmp("1996-12-19T16:39:57.000001-08:00", rfc3339_date_time(local, 1)));

    struct tm *utc = gmtime(&tv.tv_sec);
    assert(!strcmp("1996-12-20T00:39:57.000002Z", rfc3339_utc_date_time(utc, 2)));

    setenv("TZ", "UTC", 1);
    assert(!strcmp("2022-08-24T17:27:00.000048+00:00", rfc3339_date_time_now()));
    assert(!strcmp("2022-08-24T17:27:00.000048Z", rfc3339_utc_date_time_now()));

    setenv("TZ", "Europe/Helsinki", 1);
    assert(!strcmp("2022-08-24T20:27:00.000048+03:00", rfc3339_date_time_now()));
    assert(!strcmp("2022-08-24T17:27:00.000048Z", rfc3339_utc_date_time_now()));
}
