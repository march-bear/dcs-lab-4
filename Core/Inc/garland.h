#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "tim.h"

typedef enum {
	FIRST,
	SECOND,
	THIRD,
	FORTH,
	EDITING
} GARLAND_MODE;

#define MAX_SPEED 70
#define MIN_SPEED 5

#define MIN_LIGHT 0
#define MAX_LIGHT 500

extern char info_buf[128];

void start_editing();
void finish_editing();
void e_light_up();
void e_light_down();
void e_switch_seq();
void e_switch_color();

void format_user_mode_info();
bool is_editing();

int32_t count_value(int32_t value, int32_t max, int32_t min);

void speed_up();
void speed_down();
void next_mode();
void prev_mode();
void update_state();
