#ifndef RFC3339__H
#define RFC3339__H

#include <stdint.h>
#include <time.h>

/// Format RFC3339 date-time string
/// @discussion Generate RFC3339 date-time string for given local @c tm and fractional seconds @c nsec.
/// The format is of the form "YYYY-MM-DDTHH:MM:SS.NNNNNNNNN+ZZ:ZZ".
/// @see https://datatracker.ietf.org/doc/html/rfc3339
/// @return Zero on success, negative errno otherwise.
int rfc3339_format_local(const struct tm *tm, uint64_t nsec, char *buf, size_t cap);

/// Format RFC3339 date-time string (UTC)
/// @discussion Generate RFC3339 date-time string for given UTC @c tm and fractional seconds @c nsec.
/// The format is of the form "YYYY-MM-DDTHH:MM:SS.NNNNNNNNNZ".
/// @return Zero on success, negative errno otherwise.
int rfc3339_format(const struct tm *tm, uint64_t nsec, char *buf, size_t cap);

/// Get current time as RFC3339 date-time string
/// @discussion Generate RFC3339 date-time string for current local time in current time zone.
/// @see rfc3339_format_local
/// @return Zero on success, negative errno otherwise.
int rfc3339_format_local_now(char *buf, size_t cap);

/// Get current time as RFC3339 date-time string (UTC)
/// @discussion Generate RFC3339 date-time string for current time in UTC.
/// @see rfc3339_format
/// @return Zero on success, negative errno otherwise.
int rfc3339_format_now(char *buf, size_t cap);

#endif
