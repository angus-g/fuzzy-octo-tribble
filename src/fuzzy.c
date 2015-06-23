#include <pebble.h>
#include "fuzzy.h"
  
#define BUF_SIZE (TIME_LINES * CHARS_PER_LINE)

// strings for minute values (excluding "o'clock")
static const char *c_mins[] = {
  "five", "ten", "quarter", "twenty", "twenty five", "half"
};
// strings for hour values -- TODO: remove duplication
static const char *c_hours[] = {
  "midnight", "one", "two", "three", "four", "five",
  "six", "seven", "eight", "nine", "ten", "eleven", "noon"
};
// time relations, e.g. "quarter past", or "twenty to"
static const char *c_rels[] = {"past", "to"};

const char *fuzzy_time(struct tm *time) {
  static char buf[BUF_SIZE];

  // round minute to index in [0,12)
  int min = (int)(time->tm_min + 2.5) / 5;
  // hour index into c_hours, taking into account noon/midnight
  // and rounding up
  int hour = time->tm_hour + (min > 6);
  hour = (hour % 12) + (hour == 12 ? 12 : 0);
  // round minutes for both ends of the hour
  min %= 12;
  
  if (min == 0) {
    // special case: on the hour
    // either "noon", "midnight", or "<hour> o'clock"
    snprintf(buf, BUF_SIZE, "%s%s", c_hours[hour],
            hour % 12 ? " o'clock" : "");
  } else {
    min--; // index into c_mins starting at five past
    int after = min >= 6; // past halfway through the hour?
    snprintf(buf, BUF_SIZE, "%s %s %s",
            c_mins[after ? 5 - (min - 5) : min],
            c_rels[after], c_hours[hour]);
  }
  
  return buf;
}