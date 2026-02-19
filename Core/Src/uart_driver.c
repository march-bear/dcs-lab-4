#include "uart_driver.h"
#include "stm32f4xx_hal_uart.h"

uint8_t uart6_buf;

HAL_StatusTypeDef receive_char_into_buffer(uart_buf* buffer) {
	return uart6_try_push_received_char(buffer);
}

HAL_StatusTypeDef uart6_try_push_received_char(uart_buf* buffer) {
	uint8_t data;
	HAL_StatusTypeDef res = HAL_UART_Receive(&huart6, &data, 1, 10);

	if (res == HAL_OK) {
		uart_buf_push(buffer, (char) data);
	}

	return res;
}

HAL_StatusTypeDef transmit_char_from_buffer(uart_buf* buffer) {
	return uart6_try_transmit_pushed_char(buffer);
}

HAL_StatusTypeDef uart6_try_transmit_pushed_char(uart_buf* buffer) {
	char data;
	if (uart_buf_pop(buffer, &data)) {
		return HAL_UART_Transmit(&huart6, (uint8_t *) &data, 1, 10);
	}

	return HAL_ERROR;
}
