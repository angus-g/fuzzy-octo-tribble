#include <pebble.h>
#include "fuzzy.h"
#include "text.h"
  
#define LAYER_HEIGHT 40
#define SCREEN_HEIGHT 168
  
static Window *s_main_window;
static Layer *s_text_layers[TIME_LINES];
static struct tm *s_time;

static void update_time() {
  // get the fuzzy time for the current time
  const fuzzy_time_t *f = fuzzy_time(s_time);
  
  for (int i = 0; i < f->num_lines; i++) {
    // position the layer and unhide
    layer_set_frame(s_text_layers[i], GRect(0, SCREEN_HEIGHT / 2 - ((float)f->num_lines / 2 * LAYER_HEIGHT) + i*LAYER_HEIGHT,
                                           SCREEN_WIDTH, LAYER_HEIGHT));
    
    // set font
    //text_layer_set_font(s_text_layers[i], f->bold_line == i ? s_font_bold : s_font_light);
    smooth_text_layer_set_bold(s_text_layers[i], f->bold_line == i);
    smooth_text_layer_set_text(s_text_layers[i], f->lines[i]);
    
    layer_set_hidden(s_text_layers[i], false);
  }
  
  for (int i = f->num_lines; i < TIME_LINES; i++)
    layer_set_hidden(s_text_layers[i], true);
}

// handle tick
// update the clock every minute
// request weather every half hour
static void tick_handler(struct tm *tick, TimeUnits unit) {
  
}

static void main_window_load(Window *window) {
  Layer *root_layer = window_get_root_layer(window);
  
  // initialise text layers
  for (int i = 0; i < TIME_LINES; i++) {
    s_text_layers[i] = smooth_text_layer_create(GRectZero); // position updated later
    layer_set_hidden(s_text_layers[i], true);
    
    // set initial text properties
    //text_layer_set_font(s_text_layers[i], s_font_light);
    //text_layer_set_text_alignment(s_text_layers[i], GTextAlignmentCenter);
    //text_layer_set_background_color(s_text_layers[i], GColorClear);
    //text_layer_set_text_color(s_text_layers[i], GColorWhite);
    
    layer_add_child(root_layer, s_text_layers[i]);
  }
}

static void main_window_unload(Window *window) {
  for (int i = 0; i < TIME_LINES; i++)
    layer_destroy(s_text_layers[i]);
}

static void main_window_button_handler(ClickRecognizerRef recognizer, void *context) {
  ButtonId button = click_recognizer_get_button_id(recognizer);
  
  time_t ts = mktime(s_time);
  
  // change the time on button press
  if (button == BUTTON_ID_UP)
    ts += 60;
  else if (button == BUTTON_ID_DOWN)
    ts -= 60;
  else
    ts += 3600;
  
  s_time = localtime(&ts);
  
  update_time();
}

static void main_window_click_config_provider(void *context) {
  // detect buttons with 100ms repetition  
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100,
                                          main_window_button_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100,
                                          main_window_button_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, main_window_button_handler);
}
  
static void init() {
  // initialise the current time
  time_t current_time;
  current_time = time(NULL);
  s_time = localtime(&current_time);
  
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
  
  // register click config provider
  window_set_click_config_provider(s_main_window, main_window_click_config_provider);
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