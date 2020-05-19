#include "u8g2.h"

#if defined(ESP8266_PLOTFORM)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include <stdio.h>

uint8_t u8x8_eps8266_i2c_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	static const bool ack_check_enable = true;
	static i2c_cmd_handle_t cmdLink;

	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
		{
			uint8_t* data = (uint8_t *)arg_ptr;;
			while( arg_int > 0 )
			{
				//printf("%02X ", *data);
				i2c_master_write_byte(cmdLink, *data, ack_check_enable);
				data++;
				arg_int--;
			}
			
			break;
		}

		case U8X8_MSG_BYTE_START_TRANSFER:
		{
			uint8_t i2c_address = u8x8_GetI2CAddress(u8x8);
			i2c_address = 0x3c;
			cmdLink = i2c_cmd_link_create();
			i2c_master_start(cmdLink);
			i2c_master_write_byte(cmdLink, ( 0x3c << 1 ) | I2C_MASTER_WRITE, ack_check_enable);
			//printf("[ ");
			break;
		}

		case U8X8_MSG_BYTE_END_TRANSFER:
		{
			//printf("]\n");
			i2c_master_stop(cmdLink);
			ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmdLink, 1000/portTICK_RATE_MS));
			i2c_cmd_link_delete(cmdLink);
			break;
		}

		case U8X8_MSG_BYTE_INIT:
		{
			break;
		}
		
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
		{
			int i2c_port = I2C_NUM_0;
			i2c_config_t conf;
			conf.mode = I2C_MODE_MASTER;
			conf.sda_io_num = 12;//d6  u8x8_GetPinValue(u8x8, U8X8_PIN_I2C_DATA);
			conf.sda_pullup_en = false;
			conf.scl_io_num = 13;//d7  u8x8_GetPinValue(u8x8, U8X8_PIN_I2C_CLOCK);
			conf.scl_pullup_en = false;
			ESP_ERROR_CHECK(i2c_driver_install(i2c_port, conf.mode));
			ESP_ERROR_CHECK(i2c_param_config(i2c_port, &conf));

			break;
		}
		
		case U8X8_MSG_DELAY_MILLI:
		{
			vTaskDelay(arg_int+1 / portTICK_RATE_MS);
			break;
		}

		default:
		{
			u8x8_SetGPIOResult(u8x8, 1);// default return value
			break;
		}
	}

	return 1;
}

#endif // ESP8266_PLOTFORM
