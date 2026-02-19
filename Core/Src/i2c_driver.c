#include <stdbool.h>
#include "i2c_driver.h"
#include "i2c.h"

uint8_t inversion_config = 0x70; // 0111 0000

uint32_t last_deb_times[MATRIX_COUNT] = { 0 };
uint32_t last_move_times[MATRIX_COUNT] = { 0 };

bool was_several_buttons_pressed = false;
bool buttons_state[MATRIX_COUNT] = { 0 };
bool last_buttons_state[MATRIX_COUNT] = { 0 };
bool curr_buttons_state[MATRIX_COUNT] = { 0 };
uint32_t last_p = 0;

void keyboard_reset() {
	for (uint8_t i = 0; i < MATRIX_COUNT; ++i) {
		buttons_state[i] = 0;
		last_buttons_state[i] = 0;
		curr_buttons_state[i] = 0;
	}
}

const bool * get_state() {
	return curr_buttons_state;
}

void keyboard_init() {
	while (HAL_I2C_IsDeviceReady(&hi2c1, KEYBOARD_ADDRESS, 3, 1000) == HAL_BUSY) {continue;}
	HAL_I2C_Mem_Write(
		&hi2c1,
		KEYBOARD_ADDRESS,
		INVERSION_REGISTER,
		REGISTERS_SIZE,
		&inversion_config,
		1,
		10
	);
}

static void update_curr_state() {
	for (uint8_t i = 0; i < MATRIX_ROWS; ++i) {
		uint8_t io_config = ~((uint8_t) (1 << i));

		HAL_I2C_Mem_Write(
			&hi2c1,
			KEYBOARD_ADDRESS,
			CONFIG_REGISTER,
			REGISTERS_SIZE,
			&io_config,
			1,
			1000
		);

		HAL_I2C_Mem_Write(
			&hi2c1,
			KEYBOARD_ADDRESS,
			OUT_PORT_REGISTER,
			REGISTERS_SIZE,
			(uint8_t) 0x0,
			1,
			1000
		);

		uint8_t data = 0;
		HAL_I2C_Mem_Read(
			&hi2c1,
			KEYBOARD_ADDRESS,
			IN_PORT_REGISTER,
			REGISTERS_SIZE,
			&data,
			1,
			1000
		);

		data = (data & 0x70) >> 4;
		for (uint8_t j = 0; j < MATRIX_COLUMNS; ++j) {
			curr_buttons_state[i * MATRIX_COLUMNS + j] = (data & 0x1) == 1;
			data = data >> 1;
		}
	}
}

static bool is_all_unpressed() {
	for (uint8_t i = 0; i < MATRIX_COUNT; ++i) {
		if (buttons_state[i]) {
			return false;
		}
	}

	return true;
}

static bool is_one_pressed() {
	bool was_pressed = false;
	for (uint8_t i = 0; i < MATRIX_COUNT; ++i) {
		if (buttons_state[i]) {
			if (was_pressed) {
				return false;
			}

			was_pressed = true;
		}
	}

	return was_pressed;
}

int update_keyboard_state() {
	update_curr_state();

	for (uint8_t i = 0; i < MATRIX_COUNT; ++i) {
		if (curr_buttons_state[i] != last_buttons_state[i]) {
			last_deb_times[i] = HAL_GetTick();
			last_buttons_state[i] = curr_buttons_state[i];
		}
	}

	int pressed_n = -1;

	for (uint8_t i = 0; i < MATRIX_COUNT; ++i) {
		if ((HAL_GetTick() - last_deb_times[i]) > DEBOUNCE_DELAY) {
			if (curr_buttons_state[i] != buttons_state[i]) {
				buttons_state[i] = curr_buttons_state[i];
				last_move_times[i] = HAL_GetTick();

				if (buttons_state[i]) {
					pressed_n = i;
				}
			}
		}
	}

	if (is_one_pressed() && !was_several_buttons_pressed) {
		return pressed_n;
	}

	was_several_buttons_pressed = !is_all_unpressed();

	return -1;
}



