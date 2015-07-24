#pragma once

#include "globals.h"
#include "util.h"
#include "main.h"

void comm_setup();
void comm_first_time_setup();
bool comm_get_setting(int key);
GColor comm_get_foreground_color();
GColor comm_get_background_color();
