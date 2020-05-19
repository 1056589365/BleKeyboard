#include "u8g2.h"

#if defined(ESP32_PLOTFORM)

#include  <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"

#include "my_devices.h"


#define CE2(ret) \
	do { \
		esp_err_t err = ret; \
		if(err!=ESP_OK){ \
			ESP_LOGE("ErrorCheck", "%s at %d, reason: %d (%s)", __FILE__, __LINE__, err, esp_err_to_name(err)); \
		} \
	}while(0)

i2c_cmd_handle_t cmdLink;

uint8_t u8x8_eps32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	const int checkAck = 1;
	const int channel = I2C_NUM_0;

	switch(msg)
	{
		case U8X8_MSG_BYTE_START_TRANSFER:
		{
			cmdLink = i2c_cmd_link_create();
			CE2(i2c_master_start(cmdLink)); 
			CE2(i2c_master_write_byte(cmdLink, MY_ESP32_ADDR << 1 | I2C_MASTER_WRITE, checkAck));
			break;
		}

		case U8X8_MSG_BYTE_SEND:
		{
			uint8_t* data = (uint8_t*)arg_ptr;

			while( arg_int > 0 )
			{
				CE2(i2c_master_write_byte(cmdLink, *data, checkAck));
				data++;
				arg_int--;
			}
			
			break;
		}

		case U8X8_MSG_BYTE_END_TRANSFER:
		{
			CE2(i2c_master_stop(cmdLink));
			CE2(i2c_master_cmd_begin(channel, cmdLink, 100/portTICK_RATE_MS));
			i2c_cmd_link_delete(cmdLink);
			break;
		}

		case U8X8_MSG_BYTE_INIT:
		{
			break;
		}
		
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
		{
			break;
		}
		
		case U8X8_MSG_DELAY_MILLI:
		{
			vTaskDelay(arg_int / portTICK_RATE_MS);
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



uint8_t u8x8_eps32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	const int miso = 17;
	const int mosi = 26;
	const int sclk = 27;
	const int cs = 25;
	const int rst = 22;
	const int dc = 23;

	static spi_device_handle_t spi;

	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
		{
			uint8_t* data = (uint8_t *)arg_ptr;
			int len = arg_int;
			
			/*
			while( arg_int > 0 )
			{
				i2c_master_write_byte(cmdLink, *data, ack_check_enable);
				data++;
				arg_int--;
			}
			*/

			spi_transaction_t t;
			memset(&t, 0, sizeof(t));
			t.length=8*len;
			t.tx_buffer=data;
			t.user=(void*)0;
			assert(spi_device_polling_transmit(spi, &t)==ESP_OK);
			
			break;
		}

		case U8X8_MSG_BYTE_START_TRANSFER:
		{
			break;
		}

		case U8X8_MSG_BYTE_END_TRANSFER:
		{
			break;
		}

		case U8X8_MSG_BYTE_INIT:
		{
			gpio_set_level(rst, 0);
			vTaskDelay(100 / portTICK_RATE_MS);
			gpio_set_level(rst, 1);
			vTaskDelay(100 / portTICK_RATE_MS);
			
			break;
		}
		
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
		{
			spi_bus_config_t buscfg={
				.miso_io_num=miso,
				.mosi_io_num=mosi,
				.sclk_io_num=sclk,
				.quadwp_io_num=-1,
				.quadhd_io_num=-1,
				.max_transfer_sz=0
			};

			spi_device_interface_config_t devcfg={
				.clock_speed_hz = 8000000, //1*1000*1000,       //Clock out at 10 MHz
				.mode=0,
				.spics_io_num=cs,
				.queue_size=7,
				//.pre_cb=callback,
			};

			ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 1));
			ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi));

			gpio_set_direction(dc, GPIO_MODE_OUTPUT);
			gpio_set_direction(rst, GPIO_MODE_OUTPUT);

			break;
		}

		case U8X8_MSG_BYTE_SET_DC:
		case U8X8_MSG_GPIO_DC:
		{
			gpio_set_level(dc, arg_int);
			//printf("DC: %d\n", arg_int);
			break;
		}
		
		case U8X8_MSG_DELAY_MILLI:
		{
			vTaskDelay(arg_int / portTICK_RATE_MS);
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


#endif // ESP32_PLOTFORM
