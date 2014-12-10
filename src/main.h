#pragma once

#include <pebble.h>

#define DEBUG_MODE false

// Layout
#define BEAM_WIDTH 30
#define BEAM_HEIGHT 101
#define HOUR_TENS_X -13 
#define HOURS_UNITS_X 21
#define MINS_TENS_X 63
#define MINS_UNITS_X 97
#define X_OFFSET 14

// Settings keys
#define PERSIST_KEY_INVERTED 0
#define PERSIST_KEY_DATE     1
#define PERSIST_KEY_NO_ANIM  2
#define PERSIST_KEY_BT       3
#define PERSIST_KEY_BATTERY  4
#define PERSIST_KEY_HOURLY   5

// Globals
extern TextLayer *s_digits[5];
extern char s_time_buffer[5];
extern int s_state_now[4];
extern int s_state_prev[4];

// Modules
#include "util.h"
#include "comm.h"