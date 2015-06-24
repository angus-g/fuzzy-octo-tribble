#pragma once
  
#define NUM_ICONS 10

static TextData m_weather_icons[NUM_ICONS] = {
  {.x = 0,   .l = -1, .t = 22, .w = 28, .h = 27, .a = 27}, // clear-day
  {.x = 27,  .l = -1, .t = 18, .w = 19, .h = 18, .a = 18}, // clear-night
  {.x = 45,  .l = -1, .t = 18, .w = 28, .h = 26, .a = 26}, // rain
  {.x = 71,  .l = -1, .t = 18, .w = 28, .h = 25, .a = 26}, // snow
  {.x = 97,  .l = -1, .t = 18, .w = 28, .h = 26, .a = 26}, // sleet
  {.x = 123, .l = -1, .t = 16, .w = 31, .h = 16, .a = 30}, // wind
  {.x = 153, .l = -1, .t = 19, .w = 33, .h = 23, .a = 31}, // fog
  {.x = 184, .l = -1, .t = 19, .w = 29, .h = 19, .a = 28}, // cloudy
  {.x = 212, .l = -1, .t = 26, .w = 31, .h = 28, .a = 30}, // partly-cloudy-day
  {.x = 242, .l = -1, .t = 19, .w = 22, .h = 21, .a = 21}, // partly-cloudy-night
};