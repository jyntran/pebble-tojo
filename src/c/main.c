#include <pebble.h>
#define SETTINGS_KEY 1

static Window *s_main_window;
static TextLayer *s_time_layer;
static BitmapLayer *s_background_layer;
static GFont s_time_font_rg;
static GFont s_time_font_it;
static GBitmap *s_bitmap_white;
static GBitmap *s_bitmap_black;


typedef struct ClaySettings {
  GColor BackgroundColour;
  GColor TimeColour;
  bool BlackCrestColour;
  bool ItalicizedTime;
} ClaySettings;

static ClaySettings settings;

static void prv_default_settings() {
  settings.BackgroundColour = GColorBlack;
  settings.TimeColour = GColorWhite;
  settings.BlackCrestColour = false;
  settings.ItalicizedTime = true;
}


static void prv_load_settings() {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void prv_update_display() {
  window_set_background_color(s_main_window, settings.BackgroundColour);
  text_layer_set_text_color(s_time_layer, settings.TimeColour);

  if (settings.BlackCrestColour == true) {
    bitmap_layer_set_bitmap(s_background_layer, s_bitmap_black);
  } else {
    bitmap_layer_set_bitmap(s_background_layer, s_bitmap_white);
  }
  
  if (settings.ItalicizedTime) {
    text_layer_set_font(s_time_layer, s_time_font_it);
  } else {
    text_layer_set_font(s_time_layer, s_time_font_rg);
  }  
}

static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  prv_update_display();
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *bg_colour_t = dict_find(iter, MESSAGE_KEY_BackgroundColour);
  if (bg_colour_t) {
    settings.BackgroundColour = GColorFromHEX(bg_colour_t->value->int32);
  }

  Tuple *tm_colour_t = dict_find(iter, MESSAGE_KEY_TimeColour);
  if (tm_colour_t) {
    settings.TimeColour = GColorFromHEX(tm_colour_t->value->int32);
  }

  Tuple *ct_colour_t = dict_find(iter, MESSAGE_KEY_BlackCrestColour);
  if (ct_colour_t) {
    settings.BlackCrestColour = ct_colour_t->value->int32 == 1;
  }

  Tuple *it_time_t = dict_find(iter, MESSAGE_KEY_ItalicizedTime);
  if (it_time_t) {
    settings.ItalicizedTime = it_time_t->value->int32 == 1;
  }

  prv_save_settings();
}


static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_time_font_rg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_NEUTON_REGULAR_40));
  s_time_font_it = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_NEUTON_CURSIVE_40));
  s_bitmap_white = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TOJO_WHITE_100);
  s_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TOJO_BLACK_100);

  s_background_layer = bitmap_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 100));
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(8, 0), bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  prv_update_display();
}

static void main_window_unload(Window *window) {
  gbitmap_destroy(s_bitmap_white);
  gbitmap_destroy(s_bitmap_black);
  bitmap_layer_destroy(s_background_layer);  
  fonts_unload_custom_font(s_time_font_it);
  fonts_unload_custom_font(s_time_font_rg);
  text_layer_destroy(s_time_layer);
}


static void init() {
  prv_load_settings();
  
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  update_time();
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_stack_remove(s_main_window, false);
  if (s_main_window) {
    window_destroy(s_main_window);
  }
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}