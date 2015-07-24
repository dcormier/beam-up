/**
 * Beam Up Pebble Watchface
 * Author: Chris Lewis
 */

#include "globals.h"
#include "comm.h"

#include "shim/InverterLayerCompat.h"

// Global prototypes
TextLayer *g_digits[5];
TextLayer *g_date_layer;
int g_state_now[4];
int g_state_prev[4];
char g_time_buffer[5];
char g_date_buffer[8];
bool g_do_animations;

// UI elements
static Window *s_main_window;
#ifdef PBL_PLATFORM_APLITE
static InverterLayer *s_beams[4];
static InverterLayer *s_seconds_layer, *s_battery_layer;
#elif PBL_PLATFORM_BASALT
static InverterLayerCompat *s_beams[4];
static InverterLayerCompat *s_seconds_layer, *s_battery_layer;
#endif
static BitmapLayer *s_bt_layer;
static GBitmap *s_bt_bitmap;

// Data
static bool s_tapped;
static GColor bg_color, fg_color;

void main_reload_config();

#ifdef PBL_PLATFORM_APLITE
static InverterLayer* create_inv_layer(GRect bounds) {
  return inverter_layer_create(bounds);
}
static Layer* get_inv_layer(InverterLayer *this) {
  return inverter_layer_get_layer(this);
}
static void destroy_inv_layer(InverterLayer *this) {
  inverter_layer_destroy(this);
}
#elif PBL_PLATFORM_BASALT
static InverterLayerCompat* create_inv_layer(GRect bounds) {
  return inverter_layer_compat_create(bounds);
}
static Layer* get_inv_layer(InverterLayerCompat *this) {
  return inverter_layer_compat_get_layer(this);
}
static void destroy_inv_layer(InverterLayerCompat *this) {
  inverter_layer_compat_destroy(this);
}
#endif

static void handle_tick(struct tm *t, TimeUnits units_changed) {  
  // Get the time
  int seconds = t->tm_sec;

  // Hourly vibrate?
  if(comm_get_setting(PERSIST_KEY_HOURLY)) {
    if(t->tm_min == 0 && seconds == 0) {
      // Buzz buzz
      uint32_t segs[] = {200, 300, 200};
      VibePattern pattern = {
        .durations = segs,
        .num_segments = ARRAY_LENGTH(segs)
      };
      vibes_enqueue_custom_pattern(pattern);
    }
  }
   
  // Bottom suface
  switch(seconds) {
    // Beam Up!
    case 0:    
      util_write_time_digits(t);
       
      // Set the time off screen
      util_show_time_digits(); 
   
      // Animate stuff back into place
      if((g_state_now[3] != g_state_prev[3]) || (DEBUG_MODE)) {     
        util_animate_layer(text_layer_get_layer(g_digits[4]), GRect(MINS_UNITS_X, -50, 50, 60), GRect(MINS_UNITS_X, 53, 50, 60), 200, 100);
        util_animate_layer(get_inv_layer(s_beams[3]), GRect(MINS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), GRect(MINS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, 0), 400, 500);
        g_state_prev[3] = g_state_now[3];   // reset the thing
      }
      if((g_state_now[2] != g_state_prev[2]) || (DEBUG_MODE)) {
        util_animate_layer(text_layer_get_layer(g_digits[3]), GRect(MINS_TENS_X, -50, 50, 60), GRect(MINS_TENS_X, 53, 50, 60), 200, 100);
        util_animate_layer(get_inv_layer(s_beams[2]), GRect(MINS_TENS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), GRect(MINS_TENS_X + X_OFFSET, 0, BEAM_WIDTH, 0), 400, 500);
        g_state_prev[2] = g_state_now[2];   
      }
      if((g_state_now[1] != g_state_prev[1]) || (DEBUG_MODE)) {     
        util_animate_layer(text_layer_get_layer(g_digits[1]), GRect(HOURS_UNITS_X, -50, 50, 60), GRect(HOURS_UNITS_X, 53, 50, 60), 200, 100);
        util_animate_layer(get_inv_layer(s_beams[1]), GRect(HOURS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), GRect(HOURS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, 0), 400, 500);
        g_state_prev[1] = g_state_now[1];   
      }
      if((g_state_now[0] != g_state_prev[0]) || (DEBUG_MODE)) {
        util_animate_layer(text_layer_get_layer(g_digits[0]), GRect(HOUR_TENS_X, -50, 50, 60), GRect(HOUR_TENS_X, 53, 50, 60), 200, 100);
        util_animate_layer(get_inv_layer(s_beams[0]), GRect(HOUR_TENS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), GRect(HOUR_TENS_X + X_OFFSET, 0, BEAM_WIDTH, 0), 400, 500);
        g_state_prev[0] = g_state_now[0];   
      }
       
      // Bottom surface down
      util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 144, 5), GRect(0, 105, 0, 5), 500, 500);
      break;

    // Safetly for bad animation at t=2s
    case 2:
      // Reset TextLayers to show time
      layer_set_frame(text_layer_get_layer(g_digits[0]), GRect(HOUR_TENS_X, 53, 50, 60));
      layer_set_frame(text_layer_get_layer(g_digits[1]), GRect(HOURS_UNITS_X, 53, 50, 60));
      layer_set_frame(text_layer_get_layer(g_digits[3]), GRect(MINS_TENS_X, 53, 50, 60));
      layer_set_frame(text_layer_get_layer(g_digits[4]), GRect(MINS_UNITS_X, 53, 50, 60));

      // Reset InverterLayers
      layer_set_frame(get_inv_layer(s_beams[0]), GRect(0, 0, 0, 0));
      layer_set_frame(get_inv_layer(s_beams[1]), GRect(0, 0, 0, 0));
      layer_set_frame(get_inv_layer(s_beams[2]), GRect(0, 0, 0, 0));
      layer_set_frame(get_inv_layer(s_beams[3]), GRect(0, 0, 0, 0));

      // Get the time
      util_show_time_digits(t);
      break;

    // 15 seconds bar
    case 15:
      util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 0, 5), GRect(0, 105, 36, 5), 500, 0);
      break;

    // 30 seconds bar
    case 30:
      util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 36, 5), GRect(0, 105, 72, 5), 500, 0);
      break;

    // 45 seconds bar
    case 45:
      util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 72, 5), GRect(0, 105, 108, 5), 500, 0);
      break;

    // Complete bar
    case 58:
      util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 108, 5), GRect(0, 105, 144, 5), 500, 1000);
      break;

    // Beam down
    case 59:
      // Predict next changes
      util_predict_next_change(t); // CALLS util_write_time_digits()
       
      // Only change minutes units if its changed
      if((g_state_now[3] != g_state_prev[3]) || (DEBUG_MODE)) {
        util_animate_layer(get_inv_layer(s_beams[3]), GRect(MINS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, 0), GRect(MINS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), 400, 0);
        util_animate_layer(text_layer_get_layer(g_digits[4]), GRect(MINS_UNITS_X, 53, 50, 60), GRect(MINS_UNITS_X, -50, 50, 60), 200, 700);
      }
       
      // Only change minutes tens if its changed
      if((g_state_now[2] != g_state_prev[2]) || (DEBUG_MODE)) {
        util_animate_layer(get_inv_layer(s_beams[2]), GRect(MINS_TENS_X + X_OFFSET, 0, BEAM_WIDTH, 0), GRect(MINS_TENS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), 400, 0);
        util_animate_layer(text_layer_get_layer(g_digits[3]), GRect(MINS_TENS_X, 53, 50, 60), GRect(MINS_TENS_X, -50, 50, 60), 200, 700);
      }
       
      // Only change hours units if its changed
      if((g_state_now[1] != g_state_prev[1]) || (DEBUG_MODE)) {
        util_animate_layer(get_inv_layer(s_beams[1]), GRect(HOURS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, 0), GRect(HOURS_UNITS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), 400, 0);
        util_animate_layer(text_layer_get_layer(g_digits[1]), GRect(HOURS_UNITS_X, 53, 50, 60), GRect(HOURS_UNITS_X, -50, 50, 60), 200, 700);
      }
       
      // Only change hours tens if its changed
      if((g_state_now[0] != g_state_prev[0]) || (DEBUG_MODE)) {
        util_animate_layer(get_inv_layer(s_beams[0]), GRect(HOUR_TENS_X + X_OFFSET, 0, BEAM_WIDTH, 0), GRect(HOUR_TENS_X + X_OFFSET, 0, BEAM_WIDTH, BEAM_HEIGHT), 400, 0);
        util_animate_layer(text_layer_get_layer(g_digits[0]), GRect(HOUR_TENS_X, 53, 50, 60), GRect(HOUR_TENS_X, -50, 50, 60), 200, 700);
      }
      break;      
  }
}

static void bt_handler(bool connected) {
  if(connected) {
    layer_set_hidden(bitmap_layer_get_layer(s_bt_layer), true);
  } else {
    vibes_double_pulse();
    layer_set_hidden(bitmap_layer_get_layer(s_bt_layer), false);
  }
}

static void batt_anim_handler(void *context) {
  GRect start = layer_get_frame(get_inv_layer(s_battery_layer));
  GRect finish = GRect(0, 165, 0, 3);

  util_animate_layer(get_inv_layer(s_battery_layer), start, finish, 300, 0);
  s_tapped = false;
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  BatteryChargeState state = battery_state_service_peek();
  int level = state.charge_percent;
  int width = (int)(float)(((float)level / 100.0F) * (float)144);

  if(!s_tapped) {
    s_tapped = true;

    GRect start = GRect(0, 165, 0, 3);
    GRect finish = GRect(0, 165, width, 3);
    util_animate_layer(get_inv_layer(s_battery_layer), start, finish, 300, 0);

    app_timer_register(3000, batt_anim_handler, NULL);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  // Allocate text layers
  g_digits[0] = util_gen_text_layer(GRect(HOUR_TENS_X, 53, 50, 60), fg_color, GColorClear, true, RESOURCE_ID_FONT_IMAGINE_48, NULL, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(g_digits[0]));

  g_digits[1] = util_gen_text_layer(GRect(HOURS_UNITS_X, 53, 50, 60), fg_color, GColorClear, true, RESOURCE_ID_FONT_IMAGINE_48, NULL, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(g_digits[1]));

  g_digits[2] = util_gen_text_layer(GRect(68, 53, 50, 60), fg_color, GColorClear, true, RESOURCE_ID_FONT_IMAGINE_48, NULL, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(g_digits[2]));

  g_digits[3] = util_gen_text_layer(GRect(MINS_TENS_X, 53, 50, 60), fg_color, GColorClear, true, RESOURCE_ID_FONT_IMAGINE_48, NULL, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(g_digits[3]));

  g_digits[4] = util_gen_text_layer(GRect(MINS_UNITS_X, 53, 50, 60), fg_color, GColorClear, true, RESOURCE_ID_FONT_IMAGINE_48, NULL, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(g_digits[4]));

  // Allocate inverter layers
  for(int i = 0; i < 4; i++) {
    s_beams[i] = create_inv_layer(GRect(0, 0, BEAM_WIDTH, 0));
    layer_add_child(window_layer, get_inv_layer(s_beams[i]));  
  }
  s_seconds_layer = create_inv_layer(GRect(0, 0, 144, 0));
  layer_add_child(window_layer, get_inv_layer(s_seconds_layer));

  // Make sure the face is not blank
  time_t temp = time(NULL);  
  struct tm *t = localtime(&temp);  
  util_write_time_digits(t);
  
  // Stop 'all change' on first minute
  for(int i = 0; i < 4; i++) {
    g_state_prev[i] = g_state_now[i];
  }

  // User settings
  g_do_animations = comm_get_setting(PERSIST_KEY_ANIM);
  g_date_layer = util_gen_text_layer(GRect(45, 105, 100, 30), fg_color, GColorClear, true, RESOURCE_ID_FONT_IMAGINE_24, NULL, GTextAlignmentRight);
  if(comm_get_setting(PERSIST_KEY_DATE)) {
    layer_add_child(window_layer, text_layer_get_layer(g_date_layer));
  }
  s_bt_layer = bitmap_layer_create(GRect(59, 140, 27, 26));
#ifdef PBL_PLATFORM_BASALT
  bitmap_layer_set_compositing_mode(s_bt_layer, GCompOpSet);
#endif
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bt_layer));
  layer_set_hidden(bitmap_layer_get_layer(s_bt_layer), true);
  if(comm_get_setting(PERSIST_KEY_BT)) {
    if(!bluetooth_connection_service_peek()) {
      layer_set_hidden(bitmap_layer_get_layer(s_bt_layer), false);
    }
  }
  s_battery_layer = create_inv_layer(GRect(0, 165, 0, 3));
  if(comm_get_setting(PERSIST_KEY_BATTERY)) {
    layer_add_child(window_layer, get_inv_layer(s_battery_layer));
  }

  main_reload_config();

  // Set time digits now  
  util_show_time_digits();

  // Init seconds bar
  int seconds = t->tm_sec;
  if(seconds >= 15 && seconds < 30) {
    util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 0, 5), GRect(0, 105, 36, 5), 500, 0);
  } else if(seconds >= 30 && seconds < 45) {
    util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 0, 5), GRect(0, 105, 72, 5), 500, 0);
  } else if(seconds >= 45 && seconds < 58) {
    util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 0, 5), GRect(0, 105, 108, 5), 500, 0);
  } else if(seconds >= 58) {
    util_animate_layer(get_inv_layer(s_seconds_layer), GRect(0, 105, 0, 5), GRect(0, 105, 144, 5), 500, 1000);
  }
}

static void window_unload(Window *window) {  
  // Free text layers
  for(int i = 0; i < 5; i++) {
    text_layer_destroy(g_digits[i]);
  }
  text_layer_destroy(g_date_layer);

  bitmap_layer_destroy(s_bt_layer);
  gbitmap_destroy(s_bt_bitmap);
  
  // Free inverter layers
  for(int i = 0; i < 4; i++) {
    destroy_inv_layer(s_beams[i]);
  }
  destroy_inv_layer(s_seconds_layer);
  destroy_inv_layer(s_battery_layer);
}

static void init() {
  // Prepare to receive app config
  comm_setup();

  // Nuke persist for protocol changes
  if(!persist_exists(PERSIST_VERSION_2_5)) {
    persist_write_bool(PERSIST_VERSION_2_5, true);

    for(int i = 0; i < 12; i++) {
      persist_delete(i);
    }

    // Set default colors
    persist_write_int(PERSIST_KEY_FG_R, 255);
    persist_write_int(PERSIST_KEY_FG_G, 255);
    persist_write_int(PERSIST_KEY_FG_B, 255);
    persist_write_int(PERSIST_KEY_BG_R, 0);
    persist_write_int(PERSIST_KEY_BG_G, 0);
    persist_write_int(PERSIST_KEY_BG_B, 0);
  }

  // Take care of remaining missing persist values
  comm_first_time_setup();

  // Setup colors
#ifdef PBL_PLATFORM_APLITE
  fg_color = GColorWhite;
  bg_color = GColorBlack;
#elif PBL_PLATFORM_BASALT
  fg_color = comm_get_foreground_color();
  bg_color = comm_get_background_color();
#endif

  // Localize
  setlocale(LC_ALL, "");

  // Subscribe to events
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

  // Create main window
  s_main_window = window_create();
  window_set_background_color(s_main_window, bg_color);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);

  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  accel_tap_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();

  return 0;
}

/**
 * Format the BT icon to match the foreground
 */
#ifdef PBL_PLATFORM_BASALT
static GColor get_pixel(uint8_t *fb_data, GSize fb_size, GPoint pixel) {
  if(pixel.x >= 0 && pixel.x < 144 && pixel.y >= 0 && pixel.y < 168) {
    return (GColor) { .argb = fb_data[(pixel.y * fb_size.w) + pixel.x] };
  } else {
    return GColorRed;
  }
}

static void match_foreground() {
  uint8_t *data = gbitmap_get_data(s_bt_bitmap);
  int rsb = gbitmap_get_bytes_per_row(s_bt_bitmap);
  GSize size = gbitmap_get_bounds(s_bt_bitmap).size;

  GColor foreground_color = comm_get_foreground_color();

  for(int y = 0; y < size.h; y++) {
    for(int x = 0; x < size.w; x++) {
      if(gcolor_equal(get_pixel(data, size, GPoint(x, y)), GColorWhite)) {
        // Replace only white pixels
        memset(&data[(y * size.w) + x], (uint8_t)foreground_color.argb, 1);
      }
    }
  }
}
#endif

void main_reload_config() {
#ifdef PBL_PLATFORM_BASALT
  inverter_layer_compat_set_colors(fg_color, bg_color);  
#endif
#ifdef PBL_PLATFORM_APLITE
  if(comm_get_background_color() == GColorWhite) {
    s_bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT_INV);
  } else {
    s_bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT);
  }
#elif PBL_PLATFORM_BASALT
  // Color version has only transparent and white pixels
  s_bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT);

  match_foreground();
#endif
  bitmap_layer_set_bitmap(s_bt_layer, s_bt_bitmap);

  if(comm_get_setting(PERSIST_KEY_BT)) {
    bluetooth_connection_service_subscribe(bt_handler);
  }
  if(comm_get_setting(PERSIST_KEY_BATTERY)) {
    accel_tap_service_subscribe(tap_handler);
  }  
}
