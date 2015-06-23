#include <pebble.h>
#include "fuzzy.h"

// strings for minute values (excluding "o'clock")
// "twenty" is duplicated because the "five" gets pushed to the next line
static const char *c_mins[] = {
  "five", "ten", "quarter", "twenty", "twenty", "half"
};
// strings for hour values -- TODO: remove duplication
static const char *c_hours[] = {
  "midnight", "one", "two", "three", "four", "five",
  "six", "seven", "eight", "nine", "ten", "eleven", "noon"
};
// time relations, e.g. "quarter past", or "twenty to"
static const char *c_rels[] = {"past", "to", "five past", "five to"};

const fuzzy_time_t *fuzzy_time(struct tm *time) {
  static fuzzy_time_t fuzzy;

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
    if (hour % 12) {
      fuzzy.lines[0] = c_hours[hour];
      fuzzy.lines[1] = "o'clock";
      fuzzy.num_lines = 2;
    } else {
      fuzzy.lines[0] = c_hours[hour];
      fuzzy.num_lines = 1;
    }
    fuzzy.bold_line = 0;
  } else {
    min--; // index into c_mins starting at five past
    int after = min >= 6; // past halfway through the hour?
    int min_idx = after ? 5 - (min - 5) : min; // reflect through half past
    
    fuzzy.lines[0] = c_mins[min_idx];
    fuzzy.lines[1] = c_rels[after + 2 * (min_idx == 4)];
    fuzzy.lines[2] = c_hours[hour];
    fuzzy.num_lines = 3;
    fuzzy.bold_line = 2;
  }
  
  return &fuzzy;
}