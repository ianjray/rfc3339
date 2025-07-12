# rfc3339
RFC3339 Date/Time Formatter and Parser

Provides string formatting and parsing of [RFC3339](https://datatracker.ietf.org/doc/html/rfc3339) date/time.

## Example

```c
#include <assert.h>
#include <librfc3339/rfc3339.h>
#include <string.h>

int main(void)
{
    struct tm tm;
    uint64_t nsec;
    int r;
    long off;
    time_t t;
    char buf[64];

    r = rfc3339_parse("2006-01-02T15:04:05.123456789+07:00", &tm, &nsec);
    assert(0 == r);
    off = tm.tm_gmtoff;
    t = timegm(&tm);
    assert(1136189045 + off == t);
    assert(123456789 == nsec);

    r = rfc3339_parse("2006-01-02T08:04:05.123Z", &tm, &nsec);
    assert(0 == r);
    t = timegm(&tm);
    assert(1136189045 == t);
    assert(123000000 == nsec);

    r = rfc3339_format(&tm, 456, buf, sizeof(buf));
    assert(0 == r);
    assert(!strcmp("2006-01-02T08:04:05.000000456Z", buf));
}
```
