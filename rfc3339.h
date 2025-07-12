#ifndef RFC3339__H
#define RFC3339__H

#include <stdint.h>
#include <time.h>

/// Format RFC3339 date-time string
/// @discussion Generate RFC3339 date-time string for given local @c tm and fractional seconds @c nsec.
/// The format is of the form "YYYY-MM-DDTHH:MM:SS.NNNNNNNNN+ZZ:ZZ".
/// @see https://datatracker.ietf.org/doc/html/rfc3339
/// @return Zero on success, negative errno otherwise.
/// @return -EFAULT bad pointer.
/// @return -ENOMEM buffer too small.
/// @return -ERANGE nsec greater than 999,999,999.
int rfc3339_format_local(const struct tm *tm, uint64_t nsec, char *buf, size_t cap);

/// Format RFC3339 date-time string (UTC)
/// @discussion Generate RFC3339 date-time string for given UTC @c tm and fractional seconds @c nsec.
/// The format is of the form "YYYY-MM-DDTHH:MM:SS.NNNNNNNNNZ".
/// @return Zero on success, negative errno otherwise.
/// @return -EFAULT bad pointer.
/// @return -ENOMEM buffer too small.
/// @return -ERANGE nsec greater than 999,999,999.
int rfc3339_format(const struct tm *tm, uint64_t nsec, char *buf, size_t cap);

/// Get current time as RFC3339 date-time string
/// @discussion Generate RFC3339 date-time string for current local time in current time zone.
/// @see rfc3339_format_local
/// @return Zero on success, negative errno otherwise.
/// @return -EFAULT bad pointer.
/// @return -ENOMEM buffer too small.
int rfc3339_format_local_now(char *buf, size_t cap);

/// Get current time as RFC3339 date-time string (UTC)
/// @discussion Generate RFC3339 date-time string for current time in UTC.
/// @see rfc3339_format
/// @return Zero on success, negative errno otherwise.
/// @return -EFAULT bad pointer.
/// @return -ENOMEM buffer too small.
int rfc3339_format_now(char *buf, size_t cap);

/// Parse an RFC3339 date-time string.
/// @discussion Supports both local and UTC formats (with optional fractional seconds).
/// The parser accepts time zone "±23:59".
/// @note The output @c tm structure is not normalized.
/// @return Zero on success, negative errno otherwise.
/// @return -EFAULT bad pointer.
/// @return -EINVAL invalid input (trailing characters).
/// @return -EILSEQ invalid input (malformed).
/// @return -ERANGE invalid input (field value out of range).
int rfc3339_parse(const char *str, struct tm *tm, uint64_t *nsec);

#endif
