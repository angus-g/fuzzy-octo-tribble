#pragma once

typedef struct {
  GBitmap *font;
  char const *text;
} SmoothTextLayer;

Layer *smooth_text_layer_create(GRect frame);
void smooth_text_layer_set_text(Layer *layer, const char *text);