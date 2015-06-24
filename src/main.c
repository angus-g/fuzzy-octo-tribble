#include <pebble.h>
#include "fuzzy.h"
#include "text.h"
  
#define DELAY_MS 30
  
// weather data keys
enum {
  KEY_TEMP = 0
};
  
#define LAYER_HEIGHT 40
#define SCREEN_HEIGHT 168
  
static Window *s_main_window;
static Layer *s_text_layers[TIME_LINES];
static Layer *s_weather_layer;
static fuzzy_time_t s_fuzzy;
static PropertyAnimation *s_prop_anims_out[TIME_LINES];
static PropertyAnimation *s_prop_anims_in[TIME_LINES];

static void update_time_stopped(Animation *anim, bool finished, void *context) {
  int i = (int)context;
  Layer *l = s_text_layers[i];
  
  // layer is not part of the current time
  if (i >= s_fuzzy.num_lines) {
    layer_set_hidden(l, true);
    return;
  }
  
  // start right of screen
  GRect from_frame = GRect(SCREEN_WIDTH,
                           SCREEN_HEIGHT / 2 - ((float)s_fuzzy.num_lines / 2 * LAYER_HEIGHT) + i*LAYER_HEIGHT,
                           SCREEN_WIDTH, LAYER_HEIGHT);
  GRect to_frame = from_frame;
  to_frame.origin.x = 0; // shift frame to left of screen
  
  layer_set_hidden(l, false);
  smooth_text_layer_set_font(l, s_fuzzy.bold_line == i ? FontBold : FontLight);
  smooth_text_layer_set_text(l, s_fuzzy.lines[i]);
  
  s_prop_anims_in[i] = property_animation_create_layer_frame(l, &from_frame, &to_frame);
  animation_schedule(property_animation_get_animation(s_prop_anims_in[i]));
}

static void update_time() {
  // initialise the current time
  time_t current_time = time(NULL);
  struct tm *s_time = localtime(&current_time);
  
  // get the fuzzy time for the current time
  static fuzzy_time_t f_prev;
  s_fuzzy = *fuzzy_time(s_time);
  
  // determine changed lines
  bool changed[TIME_LINES];
  for (int i = 0; i < TIME_LINES; i++) {
    if (i < s_fuzzy.num_lines && f_prev.lines[i] && !strcmp(f_prev.lines[i], s_fuzzy.lines[i]))
      changed[i] = false;
    else
      changed[i] = true;
  }
  
  int change_idx = 0;
  
  // animate out changed times
  for (int i = 0; i < TIME_LINES; i++) {
    if (!changed[i]) continue;
    
    // move the current layer off-screen
    Layer *l = s_text_layers[i];
    GRect from_frame = layer_get_frame(l);
    GRect to_frame = from_frame;
    to_frame.origin.x -= to_frame.size.w;
    
    s_prop_anims_out[i] = property_animation_create_layer_frame(l, &from_frame, &to_frame);
    Animation *anim = property_animation_get_animation(s_prop_anims_out[i]);
    animation_set_delay(anim, DELAY_MS * change_idx);
    animation_set_handlers(anim, (AnimationHandlers) { .stopped = update_time_stopped }, (void *)i);
    animation_schedule(anim);
      
    change_idx++;
  }
  
  f_prev = s_fuzzy;
}

// handle tick
// update the clock every minute
// request weather every half hour
static void tick_handler(struct tm *tick, TimeUnits unit) {
  update_time();
  
  // request new weather every half hour
  if (tick->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
  }
}

static void main_window_load(Window *window) {
  Layer *root_layer = window_get_root_layer(window);
  
  // initialise text layers
  for (int i = 0; i < TIME_LINES; i++) {
    s_text_layers[i] = smooth_text_layer_create(GRectZero, FontLight, GAlignCenter); // position updated later
    layer_set_hidden(s_text_layers[i], true);
    layer_add_child(root_layer, s_text_layers[i]);
  }
  
  s_weather_layer = smooth_text_layer_create(GRect(5, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40), FontSmall, GAlignLeft);
  layer_add_child(root_layer, s_weather_layer);
}

static void main_window_unload(Window *window) {
  for (int i = 0; i < TIME_LINES; i++)
    layer_destroy(s_text_layers[i]);
  
  layer_destroy(s_weather_layer);
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple *t = dict_read_first(iter);
  
  static char weather_buf[10];
  
  while (t) {
    switch (t->key) {
    case KEY_TEMP:
      snprintf(weather_buf, sizeof(weather_buf), "%dDC", (int)t->value->int32);
    }
    
    t = dict_read_next(iter);
  }
  
  smooth_text_layer_set_text(s_weather_layer, weather_buf);
}
  
static void init() {
  smooth_text_init();
  
  // create main window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorBlack);
  window_stack_push(s_main_window, true); // push with animation enabled
  
  // minute-resolution timer for updating clock
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // register for inbox messages from weather
  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  smooth_text_deinit();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}