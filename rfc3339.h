#include <time.h>
#include <sys/time.h>

/// @brief Format RFC3339 date-time string
/// @discussion Generate RFC3339 date-time string for given local @c tm and fractional seconds @c usec.
/// The format is of the form "YYYY-MM-DDTHH:MM:SS.UUUUUU+ZZ:ZZ".
/// @see https://datatracker.ietf.org/doc/html/rfc3339
/// @return pointer To RFC3339 date-time string.
char *rfc3339_local_date_time(const struct tm *tm, suseconds_t usec);

/// @brief Format RFC3339 date-time string (UTC)
/// @discussion Generate RFC3339 date-time string for given UTC @c tm and fractional seconds @c usec.
/// The format is of the form "YYYY-MM-DDTHH:MM:SS.UUUUUUZ".
/// @return pointer To RFC3339 date-time string.
char *rfc3339_date_time(const struct tm *tm, suseconds_t usec);

/// @brief Get current time as RFC3339 date-time string
/// @discussion Generate RFC3339 date-time string for current local time in current time zone.
/// @see rfc3339_local_date_time
/// @return pointer To RFC3339 date-time string.
char *rfc3339_local_date_time_now(void);

/// @brief Get current time as RFC3339 date-time string (UTC)
/// @discussion Generate RFC3339 date-time string for current time in UTC.
/// @see rfc3339_date_time
/// @return pointer To RFC3339 date-time string.
char *rfc3339_date_time_now(void);
