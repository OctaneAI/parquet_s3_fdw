#include "pg_helper.hpp"

#include "limits.h"

int32 pg_atoi_polyfill(const char *s, int size, int c) {
  long l;
  char *badp;

  /*
   * Some versions of strtol treat the empty string as an error, but some
   * seem not to.  Make an explicit test to be sure we catch it.
   */
  if (s == NULL)
    elog(ERROR, "NULL pointer");
  if (*s == 0)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for type %s: \"%s\"", "integer", s)));

  errno = 0;
  l = strtol(s, &badp, 10);

  /* We made no progress parsing the string, so bail out */
  if (s == badp)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for type %s: \"%s\"", "integer", s)));

  switch (size) {
  case sizeof(int32):
    if (errno == ERANGE
#if defined(HAVE_LONG_INT_64)
        /* won't get ERANGE on these with 64-bit longs... */
        || l < INT_MIN || l > INT_MAX
#endif
    )
      ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                      errmsg("value \"%s\" is out of range for type %s", s,
                             "integer")));
    break;
  case sizeof(int16):
    if (errno == ERANGE || l < SHRT_MIN || l > SHRT_MAX)
      ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                      errmsg("value \"%s\" is out of range for type %s", s,
                             "smallint")));
    break;
  case sizeof(int8):
    if (errno == ERANGE || l < SCHAR_MIN || l > SCHAR_MAX)
      ereport(ERROR,
              (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
               errmsg("value \"%s\" is out of range for 8-bit integer", s)));
    break;
  default:
    elog(ERROR, "unsupported result size: %d", size);
  }

  /*
   * Skip any trailing whitespace; if anything but whitespace remains before
   * the terminating character, bail out
   */
  while (*badp && *badp != c && isspace((unsigned char)*badp))
    badp++;

  if (*badp && *badp != c)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for type %s: \"%s\"", "integer", s)));

  return (int32)l;
}