#include "rfc3339.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

char *rfc3339_local_date_time(const struct tm *tm, suseconds_t usec)
{
    static char buffer[33];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.UUUUUU+ZZ:ZZ", tm);
    snprintf(strchr(buffer, 'U'), sizeof(buffer) - (size_t)(strchr(buffer, 'U') - buffer), "%06d%+03ld:%02ld", (int)usec, tm->tm_gmtoff / 3600, tm->tm_gmtoff % 3600);
    return buffer;
}

char *rfc3339_date_time(const struct tm *tm, suseconds_t usec)
{
    static char buffer[28];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.UUUUUUZ", tm);
    snprintf(strchr(buffer, 'U'), sizeof(buffer) - (size_t)(strchr(buffer, 'U') - buffer), "%06dZ", (int)usec);
    return buffer;
}

char *rfc3339_local_date_time_now(void)
{
    struct timeval tv;
    time_t t;
    struct tm *local;
    gettimeofday(&tv, 0);
    t = tv.tv_sec;
    local = localtime(&t);
    return rfc3339_local_date_time(local, tv.tv_usec);
}

char *rfc3339_date_time_now(void)
{
    struct timeval tv;
    time_t t;
    struct tm *utc;
    gettimeofday(&tv, 0);
    t = tv.tv_sec;
    utc = gmtime(&t);
    return rfc3339_date_time(utc, tv.tv_usec);
}
