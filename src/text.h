#pragma once
  
#define NUM_GLYPHS 177
#define SCREEN_WIDTH 144

typedef struct {
  // pen_x, bitmap_left, bitmap_top
  // width, height, advance
  int x, l, t, w, h, a;
  GBitmap *b;
} TextData;

typedef struct {
  TextData *font;
  char const *text;
} SmoothTextLayer;

Layer *smooth_text_layer_create(GRect frame);
void smooth_text_layer_set_text(Layer *layer, const char *text);
void smooth_text_layer_set_bold(Layer *layer, bool bold);
void smooth_text_init();
void smooth_text_deinit();