#include <pebble.h>

#define TIME_LINES 3
  
static Window *s_main_window;
static TextLayer *s_text_layers[TIME_LINES];
static struct tm *s_time;

// handle tick
// update the clock every minute
// request weather every half hour
static void tick_handler(struct tm *tick, TimeUnits unit) {
  
}

static void main_window_load(Window *window) {
  // initialise text layers
  for (int i = 0; i < TIME_LINES; i++)
    s_text_layers[i] = text_layer_create(GRectZero); // position updated later
}

static void main_window_unload(Window *window) {
  for (int i = 0; i < TIME_LINES; i++)
    text_layer_destroy(s_text_layers[i]);
}

static void main_window_button_handler(ClickRecognizerRef recognizer, void *context) {
  ButtonId button = click_recognizer_get_button_id(recognizer);
  
  // change the time on button press
  if (button == BUTTON_ID_UP)
    s_time->tm_min++;
  else
    s_time->tm_min--;
}

static void main_window_click_config_provider(void *context) {
  // detect buttons with 100ms repetition
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100,
                                          main_window_button_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100,
                                          main_window_button_handler);
}
  
static void init() {
  // initialise the current time
  time_t current_time;
  current_time = time(NULL);
  s_time = localtime(&current_time);
  
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
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}