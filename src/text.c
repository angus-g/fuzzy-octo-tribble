#include <pebble.h>
#include "text.h"
  
static void smooth_text_layer_update(Layer *layer, GContext *ctx) {
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(layer);
  
  graphics_context_set_fill_color(ctx, GColorClear);
  graphics_context_set_text_color(ctx, GColorWhite);
  
  graphics_draw_text(ctx, stl->text, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT),
                    layer_get_bounds(layer), GTextOverflowModeFill, GTextAlignmentCenter,
                     NULL);
}

void smooth_text_layer_set_text(Layer *layer, const char *text) {
  SmoothTextLayer *stl = (SmoothTextLayer *)layer_get_data(layer);
  stl->text = text;
  layer_mark_dirty(layer);
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