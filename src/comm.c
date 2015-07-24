#include "comm.h"

#ifdef PBL_BW
static bool gcolor_is_dark(int r, int g, int b) {
  return r < 128 && g < 128 && b < 128;
}
#endif

static void in_recv_handler(DictionaryIterator *iter, void *context) {
  Tuple *t = dict_read_first(iter);

  while(t) {
    if(t->key != PERSIST_KEY_THEME) {
      // Super settings one liner!
      persist_write_bool(t->key, strcmp(t->value->cstring, "true") == 0 ? true : false);
    } else {
#ifdef PBL_PLATFORM_BASALT
      switch(t->key) {
        case PERSIST_KEY_THEME: {
          APP_LOG(APP_LOG_LEVEL_INFO, "START");
          // Get foreground color
          Tuple *color_tuple = dict_find(iter, PERSIST_KEY_FG_R);
          if(color_tuple) {
            persist_write_int(PERSIST_KEY_FG_R, (int)color_tuple->value->int32);
            APP_LOG(APP_LOG_LEVEL_INFO, "value: %d", (int)color_tuple->value->int32);
          }
          color_tuple = dict_find(iter, PERSIST_KEY_FG_G);
          if(color_tuple) {
            persist_write_int(PERSIST_KEY_FG_G, (int)color_tuple->value->int32);
            APP_LOG(APP_LOG_LEVEL_INFO, "value: %d", (int)color_tuple->value->int32);
          }
          color_tuple = dict_find(iter, PERSIST_KEY_FG_B);
          if(color_tuple) {
            persist_write_int(PERSIST_KEY_FG_B, (int)color_tuple->value->int32);
            APP_LOG(APP_LOG_LEVEL_INFO, "value: %d", (int)color_tuple->value->int32);
          }

          // Get background color
          color_tuple = dict_find(iter, PERSIST_KEY_BG_R);
          if(color_tuple) {
            persist_write_int(PERSIST_KEY_BG_R, (int)color_tuple->value->int32);
            APP_LOG(APP_LOG_LEVEL_INFO, "value: %d", (int)color_tuple->value->int32);
          }
          color_tuple = dict_find(iter, PERSIST_KEY_BG_G);
          if(color_tuple) {
            persist_write_int(PERSIST_KEY_BG_G, (int)color_tuple->value->int32);
            APP_LOG(APP_LOG_LEVEL_INFO, "value: %d", (int)color_tuple->value->int32);
          }
          color_tuple = dict_find(iter, PERSIST_KEY_BG_B);
          if(color_tuple) {
            persist_write_int(PERSIST_KEY_BG_B, (int)color_tuple->value->int32);
            APP_LOG(APP_LOG_LEVEL_INFO, "value: %d", (int)color_tuple->value->int32);
          }
          break;
        }
        default: 
          APP_LOG(APP_LOG_LEVEL_ERROR, "Unknown key: %d", (int)t->key);
          break;
      }
#endif
    }

    t = dict_read_next(iter);
  }

  vibes_short_pulse();
  main_reload_config();
}

static void in_failed_handler(AppMessageResult reason, void *context) {
  util_interpret_message_result(reason);
}

void comm_first_time_setup() {
  if(!persist_exists(PERSIST_KEY_DATE)) {
    persist_write_bool(PERSIST_KEY_DATE, false);
  }
  if(!persist_exists(PERSIST_KEY_ANIM)) {
    persist_write_bool(PERSIST_KEY_ANIM, true);
  }
  if(!persist_exists(PERSIST_KEY_BT)) {
    persist_write_bool(PERSIST_KEY_BT, true);
  }
  if(!persist_exists(PERSIST_KEY_BATTERY)) {
    persist_write_bool(PERSIST_KEY_BATTERY, false);
  }
  if(!persist_exists(PERSIST_KEY_HOURLY)) {
    persist_write_bool(PERSIST_KEY_HOURLY, false);
  }
}

void comm_setup() {
  app_message_register_inbox_dropped(in_failed_handler);
  app_message_register_inbox_received(in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

bool comm_get_setting(int key) {
  return persist_read_bool(key);
}

GColor comm_get_foreground_color() {
  int red = persist_read_int(PERSIST_KEY_FG_R);
  int green = persist_read_int(PERSIST_KEY_FG_G);
  int blue = persist_read_int(PERSIST_KEY_FG_B);

#ifdef PBL_BW
  return (gcolor_is_dark(red, green, blue)) ? GColorBlack : GColorWhite;
#elif PBL_COLOR
  return GColorFromRGB(red, green, blue);
#endif
}

GColor comm_get_background_color() {
  int red = persist_read_int(PERSIST_KEY_BG_R);
  int green = persist_read_int(PERSIST_KEY_BG_G);
  int blue = persist_read_int(PERSIST_KEY_BG_B);

#ifdef PBL_BW
  return (gcolor_is_dark(red, green, blue)) ? GColorBlack : GColorWhite;
#elif PBL_COLOR
  return GColorFromRGB(red, green, blue);
#endif
}
