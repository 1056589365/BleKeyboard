#pragma once

#define ESP32_PLOTFORM
//#define ESP8266_PLOTFORM

#ifdef __cplusplus
extern "C" {
#endif

//#define U8X8_USE_PINS // defined in u8x8.h
extern int spi_speed_khz; // defined in u8g2_com_raspberrypi.c

//raspberrypi
uint8_t u8x8_raspberrypi_i2c_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_raspberrypi_i2c_sw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_raspberrypi_spi_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

// esp 8266
uint8_t u8x8_eps8266_i2c_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

//esp32
uint8_t u8x8_eps32_hw_i2c (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_eps32_hw_spi (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#define MY_ESP32_ADDR 0x3c

#ifdef __cplusplus
}
#endif
