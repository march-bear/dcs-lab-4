#pragma once

#include <stdbool.h>
#include "i2c_driver.h"
#include "i2c.h"

#define DEBOUNCE_DELAY 		50

#define KEYBOARD_ADDRESS 	0xE2

#define IN_PORT_REGISTER 	0x0
#define OUT_PORT_REGISTER 	0x1
#define INVERSION_REGISTER 	0x2
#define CONFIG_REGISTER 	0x3

#define MATRIX_ROWS			4
#define MATRIX_COLUMNS 		3

#define MATRIX_COUNT		(MATRIX_ROWS * MATRIX_COLUMNS)

#define REGISTERS_SIZE 		1

void keyboard_reset();
void keyboard_init();
int update_keyboard_state();

