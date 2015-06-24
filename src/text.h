#pragma once
  
#define NUM_GLYPHS 123
#define SCREEN_WIDTH 144
  
typedef enum {
  FontLight = 0,
  FontBold,
  FontSmall,
} FontType;

typedef struct {
  // pen_x, bitmap_left, bitmap_top
  // width, height, advance
  int x, l, t, w, h, a;
  GBitmap *b;
} TextData;

typedef struct {
  TextData *font;
  char const *text;
  GAlign align;
} SmoothTextLayer;

Layer *smooth_text_layer_create(GRect frame, FontType font, GAlign align);
void smooth_text_layer_set_text(Layer *layer, const char *text);
void smooth_text_layer_set_font(Layer *layer, FontType font);
void smooth_text_init();
void smooth_text_deinit();