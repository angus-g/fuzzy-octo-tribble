#include <pebble.h>
#include "text.h"
#include "sans_bold.h"
#include "sans_light.h"
#include "sans_small.h"
  
static TextData *s_font_bold, *s_font_light, *s_font_small;
static GBitmap *s_bmp_bold, *s_bmp_light, *s_bmp_small;
  
static void smooth_text_layer_update(Layer *layer, GContext *ctx) {
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(layer);
  
  // not initialised with a font
  if (!stl->font || !stl->text) return;
  
  int l = strlen(stl->text);
  int pen = 0;
  
  if (stl->align == GAlignCenter) {
    int w = 0;
    // calculate width of line
    for (int i = 0; i < l; i++)
      w += stl->font[(int)stl->text[i]].a - 2;
    
    pen = (SCREEN_WIDTH - w) / 2;
  }
  
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

void smooth_text_layer_set_font(Layer *layer, FontType font) {
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(layer);
  switch (font) {
  case FontLight:
    stl->font = s_font_light;
    break;
  case FontBold:
    stl->font = s_font_bold;
    break;
  case FontSmall:
    stl->font = s_font_small;
    break;
  default:
    stl->font = NULL;
  }
}

// initialise a SmoothTextLayer with a given frame, font and alignment
Layer *smooth_text_layer_create(GRect frame, FontType font, GAlign align) {
  // create an original pebble layer
  Layer *l = layer_create_with_data(frame, sizeof(SmoothTextLayer));
  
  // wrap it in a SmoothTextLayer
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(l);
  smooth_text_layer_set_font(l, font);
  stl->text = NULL;
  stl->align = align;
  
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

static void textdata_deinit(TextData *font_data) {
  for (int i = 0; i < NUM_GLYPHS; i++)
    if (font_data[i].b) gbitmap_destroy(font_data[i].b);
}

void smooth_text_init() {
  // bold font (for hours)
  s_bmp_bold = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SOURCE_SANS_BOLD);
  s_font_bold = m_source_sans_bold;
  textdata_init(s_bmp_bold, s_font_bold);
  
  // light font (for minutes)
  s_bmp_light = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SOURCE_SANS_LIGHT);
  s_font_light = m_source_sans_light;
  textdata_init(s_bmp_light, s_font_light);
  
  // small font (for weather/date)
  s_bmp_small = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SOURCE_SANS_SMALL);
  s_font_small = m_source_sans_small;
  textdata_init(s_bmp_small, s_font_small);
}

void smooth_text_deinit() {
  textdata_deinit(s_font_bold);
  textdata_deinit(s_font_light);
  textdata_deinit(s_font_small);
  
  gbitmap_destroy(s_bmp_bold);
  gbitmap_destroy(s_bmp_light);
  gbitmap_destroy(s_bmp_small);
}