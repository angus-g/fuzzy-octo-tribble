#pragma once
  
#define TIME_LINES 3
#define CHARS_PER_LINE 10
  
typedef struct {
  // the raw fuzzy text
  char const *lines[TIME_LINES];
  // number of filled-in lines
  int num_lines;
  // which line is bold (i.e. contains the hour)
  int bold_line;
} fuzzy_time_t;

const fuzzy_time_t *fuzzy_time(struct tm *time);