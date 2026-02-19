#include "uart_buf.h"
#include "stm32f4xx_hal.h"

void uart_buf_push(uart_buf* buf, char data) {
	buf->buffer[buf->write_idx] = data;
	buf->write_idx = (buf->write_idx + 1) % UART_BUFFER_SIZE;

	if (buf->size < UART_BUFFER_SIZE) {
		buf->size++;
	} else {
		buf->read_idx = (buf->read_idx + 1) % UART_BUFFER_SIZE;
	}
}

bool uart_buf_pop(uart_buf* buf, char* data) {
	if (buf->size == 0) {
		return false;
	}

	*data = buf->buffer[buf->read_idx];
	buf->read_idx = (buf->read_idx + 1) % UART_BUFFER_SIZE;

	buf->size--;

	return true;
}

void uart_buf_reset(uart_buf* buf) {
	buf->write_idx = 0;
	buf->read_idx = 0;
	buf->size = 0;
}
