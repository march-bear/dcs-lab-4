#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "garland.h"

static uint32_t lock() {
	uint32_t pmask = __get_PRIMASK();
	__disable_irq();

	return pmask;
}

static void unlock(uint32_t pmask) {
	__set_PRIMASK(pmask);
}

typedef enum {
	RED,
	ORANGE
} led_color;

GARLAND_MODE mode = FIRST;

uint32_t speed = 25;
float user_mode_speed;
float accurate_value1;
float accurate_value2;

int value1;
int value2;

int sign1 = 1;
int sign2 = 1;

char info_buf[128];

bool reset_state = true;
uint32_t user_mode_max_light = 500;
uint32_t user_mode_green_init_light = 500;
led_color user_mode_color = RED;

void format_user_mode_info() {
	sprintf(info_buf,
			"\r\nUser mode info.\r\n"
			"\r\nLIGHT: %" PRIu32
			"\r\nCOLOR: %s"
			"\r\nBLINKING: %s\r\n\n",

			user_mode_max_light,
			((user_mode_color == RED) ? "RED" : "ORANGE"),
			((user_mode_green_init_light == 0) ? "together" : "alternately")
			);
}

void start_editing() {
	uint32_t pmask = lock();
	reset_state = true;
	mode = EDITING;
	sprintf(info_buf, "Editing mode\r\n");
	unlock(pmask);
}

void finish_editing() {
	uint32_t pmask = lock();
	reset_state = true;
	mode = FIFTH;
	sprintf(info_buf, "Finish editing. User mode\r\n");
	unlock(pmask);
}

void e_light_up() {
	uint32_t pmask = lock();
	reset_state = true;
	user_mode_max_light += 50;

	if (user_mode_max_light > MAX_LIGHT) {
		user_mode_max_light = MAX_LIGHT;
	}

	if (user_mode_green_init_light != 0) {
		user_mode_green_init_light = user_mode_max_light;
	}

	sprintf(info_buf, "New light value: %" PRIu32 "\r\n", user_mode_max_light);

	unlock(pmask);
}

void e_light_down() {
	uint32_t pmask = lock();
	reset_state = true;
	user_mode_max_light -= 50;

	if (user_mode_max_light < MIN_LIGHT + 50) {
		user_mode_max_light = MIN_LIGHT + 50;
	}

	if (user_mode_green_init_light != 0) {
		user_mode_green_init_light = user_mode_max_light;
	}

	sprintf(info_buf, "New light value: %" PRIu32 "\r\n", user_mode_max_light);

	unlock(pmask);
}

void e_switch_seq() {
	uint32_t pmask = lock();
	reset_state = true;
	user_mode_green_init_light = user_mode_max_light - user_mode_green_init_light;
	sprintf(info_buf, "New blinking mode: %s\r\n", ((user_mode_green_init_light == 0) ? "together" : "alternately"));
	unlock(pmask);
}

void e_switch_color() {
	uint32_t pmask = lock();
	reset_state = true;
	if (user_mode_color == RED) {
		user_mode_color = ORANGE;
	} else {
		user_mode_color = RED;
	}

	sprintf(info_buf, "New led color: %s\r\n", ((user_mode_color == RED) ? "RED" : "ORANGE"));

	unlock(pmask);
}

bool is_editing() {
	return mode == EDITING;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6) {
		update_state();
	}
}

static void first_mode() {
	if (reset_state) {
		sign1 = 1;
		htim4.Instance->CCR2 = 0;
		htim4.Instance->CCR3 = 0;
		htim4.Instance->CCR4 = 0;
		reset_state = false;
	}

	int32_t value = htim4.Instance->CCR2 + speed * sign1;
	if (value > MAX_LIGHT) {
		value = 2 * MAX_LIGHT - value;
		sign1 = -sign1;
	} else if (value < MIN_LIGHT) {
		value = 2 * MIN_LIGHT - value;
		sign1 = -sign1;
	}

	 htim4.Instance->CCR2 = value;
	 htim4.Instance->CCR4 = value;
}

static void second_mode() {
	if (reset_state) {
		sign1 = 1;
		sign2 = 1;
		htim4.Instance->CCR2 = 0;
		htim4.Instance->CCR3 = 0;
		htim4.Instance->CCR4 = 500;
		reset_state = false;
	}

	value1 = htim4.Instance->CCR2 + speed * sign1;
	if (value1 > MAX_LIGHT) {
		value1 = 2 * MAX_LIGHT - value1;
		sign1 = -sign1;
	} else if (value1 < MIN_LIGHT) {
		value1 = 2 * MIN_LIGHT - value1;
		sign1 = -sign1;
	}

	value2 = htim4.Instance->CCR4 + speed * sign2;
	if (value2 > MAX_LIGHT) {
		value2 = 2 * MAX_LIGHT - value2;
		sign2 = -sign2;
	} else if (value2 < MIN_LIGHT) {
		value2 = 2 * MIN_LIGHT - value2;
		sign2 = -sign2;
	}
	 htim4.Instance->CCR2 = value1;
	 htim4.Instance->CCR4 = value2;
}

static void third_mode() {
	if (reset_state) {
		sign1 = 1;
		sign2 = 1;
		value1 = 0;
		value2 = 500;
		htim4.Instance->CCR2 = 0;
		htim4.Instance->CCR3 = 500;
		htim4.Instance->CCR4 = 0;
		reset_state = false;
	}

	value1 = value1 + speed * sign1;
	if (value1 > MAX_LIGHT) {
		value1 = 2 * MAX_LIGHT - value1;
		sign1 = -sign1;
	} else if (value1 < MIN_LIGHT) {
		value1 = 2 * MIN_LIGHT - value1;
		sign1 = -sign1;
	}

	value2 = value2 + speed * sign2;
	if (value2 > MAX_LIGHT) {
		value2 = 2 * MAX_LIGHT - value2;
		sign2 = -sign2;
	} else if (value2 < MIN_LIGHT) {
		value2 = 2 * MIN_LIGHT - value2;
		sign2 = -sign2;
	}
	 htim4.Instance->CCR2 = (value1 > 250) ? 500 : 0;
	 htim4.Instance->CCR3 = (value2 > 250) ? 500 : 0;
}

static void forth_mode() {
	if (reset_state) {
		sign1 = 1;
		value1 = 0;
		htim4.Instance->CCR2 = 0;
		htim4.Instance->CCR3 = 0;
		htim4.Instance->CCR4 = 0;
		reset_state = false;
	}

	value1 = value1 + speed;
	if (value1 > 3 * MAX_LIGHT) {
		value1 = 0;
	}

	 htim4.Instance->CCR2 = ((value1 >= 0 && value1 < 500) ? 500 : 0);
	 htim4.Instance->CCR3 = ((value1 >= 500 && value1 < 1000) ? 500 : 0);
	 htim4.Instance->CCR4 = ((value1 >= 1000 && value1 < 1500) ? 500 : 0);
}

static void user_mode() {
	if (reset_state) {
		accurate_value1 = user_mode_green_init_light;
		accurate_value2 = 0;
		sign1 = 1;
		sign2 = 1;
		htim4.Instance->CCR2 = user_mode_green_init_light;
		htim4.Instance->CCR3 = 0;
		htim4.Instance->CCR4 = 0;
		reset_state = false;
		user_mode_speed = (float) speed / 500.0 * user_mode_max_light;
	}

	accurate_value1 = accurate_value1 + user_mode_speed * sign1;
	if (accurate_value1 > user_mode_max_light) {
		accurate_value1 = 2 * user_mode_max_light - accurate_value1;
		sign1 = -sign1;
	} else if (accurate_value1 < MIN_LIGHT) {
		accurate_value1 = 2 * MIN_LIGHT - accurate_value1;
		sign1 = -sign1;
	}

	accurate_value2 = accurate_value2 + user_mode_speed * sign2;
	if (accurate_value2 > user_mode_max_light) {
		accurate_value2 = 2 * user_mode_max_light - accurate_value2;
		sign2 = -sign2;
	} else if (accurate_value2 < MIN_LIGHT) {
		accurate_value2 = 2 * MIN_LIGHT - accurate_value2;
		sign2 = -sign2;
	}
	 htim4.Instance->CCR2 = (uint32_t) accurate_value1;
	 if (user_mode_color == ORANGE) {
		 htim4.Instance->CCR3 = (uint32_t) accurate_value2;
	 } else {
		 htim4.Instance->CCR4 = (uint32_t) accurate_value2;
	 }
}

void speed_up() {
	uint32_t pmask = lock();
	if (speed * 11 / 10 == speed) {
		speed += 1;
	} else {
		speed = speed * 11 / 10;
	}

	if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}
	user_mode_speed = (float) speed / 500.0 * user_mode_max_light;

	sprintf(info_buf, "speed: %" PRIu32 "\r\n", speed);

	unlock(pmask);
}

void speed_down() {
	uint32_t pmask = lock();
	if (speed * 9 / 10 == speed) {
		speed -= 1;
	} else {
		speed = speed * 9 / 10;
	}
	if (speed < MIN_SPEED) {
		speed = MIN_SPEED;
	}
	user_mode_speed = (float) speed / 500.0 * user_mode_max_light;

	sprintf(info_buf, "speed: %" PRIu32 "\r\n", speed);

	unlock(pmask);
}

void next_mode() {
	uint32_t pmask = lock();
	reset_state = true;
	switch (mode) {
	case FIRST:
		mode = SECOND;
		break;
	case SECOND:
		mode = THIRD;
		break;
	case THIRD:
		mode = FORTH;
		break;
	case FORTH:
		mode = FIFTH;
		break;
	case FIFTH:
		mode = FIRST;
		break;
	default:
		break;
	}

	sprintf(info_buf, "mode: %s\r\n",
			(mode == FIRST) ? "FIRST"
			: (mode == SECOND) ? "SECOND"
					: (mode == THIRD) ? "THIRD"
							: (mode == FORTH) ? "FORTH"
							: "USER");

	unlock(pmask);
}

void prev_mode() {
	uint32_t pmask = lock();
	reset_state = true;
	switch (mode) {
	case FIRST:
		mode = FIFTH;
		break;
	case SECOND:
		mode = FIRST;
		break;
	case THIRD:
		mode = SECOND;
		break;
	case FORTH:
		mode = THIRD;
		break;
	case FIFTH:
		mode = FORTH;
		break;
	default:
		break;
	}

	sprintf(info_buf, "mode: %s\r\n",
			(mode == FIRST) ? "FIRST"
			: (mode == SECOND) ? "SECOND"
					: (mode == THIRD) ? "THIRD"
							: (mode == FORTH) ? "FORTH"
							: "USER");

	unlock(pmask);
}

void update_state() {
	switch (mode) {
	case FIRST:
		first_mode();
		break;
	case SECOND:
		second_mode();
		break;
	case THIRD:
		third_mode();
		break;
	case FORTH:
		forth_mode();
		break;
	case FIFTH:
	case EDITING:
		user_mode();
		break;
	}
}
