#include <pebble.h>
#include "text.h"
#include "sans_bold.h"
#include "sans_light.h"
  
static TextData *s_font_bold, *s_font_light;
static GBitmap *s_bmp_bold, *s_bmp_light;
  
static void smooth_text_layer_update(Layer *layer, GContext *ctx) {
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(layer);
  
  // not initialised with a font
  if (!stl->font) return;
  
  int l = strlen(stl->text);
  int w = 0;
  // calculate width of line
  for (int i = 0; i < l; i++)
    w += stl->font[(int)stl->text[i]].a - 2;
  
  GRect bounds = layer_get_bounds(layer);
  int pen = (SCREEN_WIDTH - w) / 2;
  
  for (int i = 0; i < l; i++) {
    TextData *d = stl->font + stl->text[i];
    graphics_draw_bitmap_in_rect(ctx, d->b, GRect(pen, 30 - d->t, d->w + d->l, d->h));
    pen += d->a - 2;
  }
}

void smooth_text_layer_set_text(Layer *layer, const char *text) {
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(layer);
  stl->text = text;
  layer_mark_dirty(layer);
}

void smooth_text_layer_set_bold(Layer *layer, bool bold) {
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(layer);
  stl->font = bold ? s_font_bold : s_font_light;
}

// initialise a SmoothTextLayer with a given frame
Layer *smooth_text_layer_create(GRect frame) {
  // create an original pebble layer
  Layer *l = layer_create_with_data(frame, sizeof(SmoothTextLayer));
  
  // wrap it in a SmoothTextLayer
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(l);
  stl->font = NULL;
  stl->text = NULL;
  
  layer_set_update_proc(l, smooth_text_layer_update);
  
  return l;
}

static void textdata_init(GBitmap *bitmap, TextData *font_data) {
  // for each non-null letter, create a sub-bitmap in the TextData
  for (int i = 0; i < NUM_GLYPHS; i++) {
    if (!font_data[i].a) continue;
    
    TextData *d = font_data + i;
    GRect sub = GRect(d->x, 40 - d->t, d->w + d->l, d->h);
    font_data[i].b = gbitmap_create_as_sub_bitmap(bitmap, sub);
  }
}

void smooth_text_init() {
  s_bmp_bold = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SOURCE_SANS_BOLD);
  s_font_bold = m_source_sans_bold;
  textdata_init(s_bmp_bold, s_font_bold);
  
  s_bmp_light= gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SOURCE_SANS_LIGHT);
  s_font_light = m_source_sans_light;
  textdata_init(s_bmp_light, s_font_light);
}

void smooth_text_deinit() {
  for (int i = 0; i < NUM_GLYPHS; i++) {
    if (s_font_bold[i].b) gbitmap_destroy(s_font_bold[i].b);
    if (s_font_light[i].b) gbitmap_destroy(s_font_light[i].b);
  }
  
  gbitmap_destroy(s_bmp_bold);
  gbitmap_destroy(s_bmp_light);
}