#pragma once

#include <pebble.h>

#define DEBUG_MODE false

// Layout
#define BEAM_WIDTH    30
#define BEAM_HEIGHT   101
#define HOUR_TENS_X   -13 
#define HOURS_UNITS_X 21
#define MINS_TENS_X   63
#define MINS_UNITS_X  97
#define X_OFFSET      14

// Settings keys
#define PERSIST_KEY_DATE         1
#define PERSIST_KEY_ANIM         2
#define PERSIST_KEY_BT           3
#define PERSIST_KEY_BATTERY      4
#define PERSIST_KEY_HOURLY       5
#define PERSIST_KEY_FG_R         6
#define PERSIST_KEY_FG_G         7
#define PERSIST_KEY_FG_B         8
#define PERSIST_KEY_BG_R         9
#define PERSIST_KEY_BG_G         10
#define PERSIST_KEY_BG_B         11
#define PERSIST_KEY_THEME        12 // Used to detect colors in dict
#define PERSIST_KEY_LEADING_ZERO 13

#define PERSIST_VERSION_2_5  23348  // Nuke for new color protocol

extern TextLayer *g_digits[5];
extern TextLayer *g_date_layer;
extern int g_state_now[4];
extern int g_state_prev[4];
extern char g_time_buffer[5];
extern char g_date_buffer[8];
extern bool g_do_animations;
