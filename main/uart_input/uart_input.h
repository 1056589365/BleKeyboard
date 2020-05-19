#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void uart_input_init(void (*callback)(char* data, size_t len));

#ifdef __cplusplus
}
#endif