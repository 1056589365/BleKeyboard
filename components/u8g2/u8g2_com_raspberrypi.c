#include "u8g2.h"
#include "my_devices.h"

#if defined(RASPBERRYPI_PLOTFORM)
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

uint8_t u8x8_raspberrypi_i2c_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	static int fd;
	
	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
		{
			static void* data;
			int len;
			
			data = arg_ptr;
			len = arg_int;
			
			if(len > 0)
			{
				if(write(fd, data, len) <0)
					printf("i2c write failed\n");
			}
			
			break;
		}

		case U8X8_MSG_BYTE_START_TRANSFER:
		{
			
			break;
		}

		case U8X8_MSG_BYTE_END_TRANSFER:
		{
			close(fd);
			break;
		}
		
		case U8X8_MSG_BYTE_INIT:
		{
			fd = open("/dev/i2c-1", O_RDWR);
			
			int i2cadd = u8x8_GetI2CAddress(u8x8)>>1;
			
			if(fd < 0)
				printf("i2c open failed\n");
			
			printf("Devices Address: %02x\n", i2cadd);
			
			if(ioctl(fd, I2C_SLAVE, i2cadd)<0)
				printf("ioctl failed\n");
			
			break;
		}
		
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
		{
			break;
		}
		
		case U8X8_MSG_DELAY_MILLI:
		{
			delay(arg_int);
			break;
		}

		default:
			break;
	}
	return 1;
}

uint8_t u8x8_raspberrypi_i2c_sw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	static int sda;
	static int scl;
	
	switch(msg)
	{
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
		{
			scl = u8x8_GetPinValue(u8x8, U8X8_PIN_I2C_CLOCK);
			sda = u8x8_GetPinValue(u8x8, U8X8_PIN_I2C_DATA);
			
			printf("software i2c pin: scl:%d, sda: %d\n", scl, sda);
			
			wiringPiSetup();
			pinMode(scl, OUTPUT);
			pinMode(sda, OUTPUT);
			break;
		}
		case U8X8_MSG_DELAY_MILLI:
		{
			delay(arg_int);
			break;
		}
		case U8X8_MSG_DELAY_I2C:/* arg_int is 1 or 4: 100KHz (5us) or 400KHz (1.25us) */
		{
			delayMicroseconds(arg_int==1? 5:2);
			break;
		}
		case U8X8_MSG_GPIO_I2C_CLOCK:
		{
			digitalWrite(scl, arg_int);
			break;
		}
		
		case U8X8_MSG_GPIO_I2C_DATA:
		{
			digitalWrite(sda, arg_int);
			break;
		}
		default:
			break;
	}
	u8x8_byte_sw_i2c(u8x8, msg,arg_int, arg_ptr);
	return 1;
}


int spi_speed_khz;

uint8_t u8x8_raspberrypi_spi_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	static int fd;
	static int rst;
	static int dc;
	
	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
		{
			int channel = 0;
			
			uint8_t* data = malloc(sizeof(uint8_t) * arg_int);
			if(!data)
			{
				printf("data is null\n");
			}
			memcpy(data, arg_ptr, arg_int);
			wiringPiSPIDataRW(channel, data, arg_int);
			free(data);
			
			/*
			uint8_t mdata;
			while(arg_int > 0)
			{
				mdata = *(uint8_t*)arg_ptr;
				printf("%02X ", mdata);
				//wiringPiSPIDataRW(channel, &mdata, 1);
				mdata++;
				arg_int--;
			}
			*/
			break;
		}

		case U8X8_MSG_BYTE_START_TRANSFER:
		{
			//printf("[ ");
			break;
		}

		case U8X8_MSG_BYTE_END_TRANSFER:
		{
			//printf("]\n");
			break;
		}
		
		case U8X8_MSG_BYTE_INIT:
		{
			int channel = 0;
			long speed = spi_speed_khz;//u8x8->spi_speed_khz*1000;//32*1000*1000;
			if(fd = wiringPiSPISetup(channel, speed) ==-1)
			{
				printf("wiringPiSpi setup failed\n");
			}
			printf("SPI speed: %ld\n", speed);
			break;
		}
		
		
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
		{
			wiringPiSetup();
			
			rst = u8x8_GetPinValue(u8x8, U8X8_PIN_RESET);
			dc = u8x8_GetPinValue(u8x8, U8X8_PIN_DC);
			
			printf("hardware spi pin: rst:%d, dc: %d\n", rst, dc);
			
			pinMode(rst, OUTPUT);
			pinMode(dc, OUTPUT);
			
			digitalWrite(rst, 0);
			delay(20);
			digitalWrite(rst, 1);
	
			break;
		}
		
		case U8X8_MSG_BYTE_SET_DC:
		case U8X8_MSG_GPIO_DC:
		{
			digitalWrite(dc, arg_int);
			break;
		}
		
		case U8X8_MSG_DELAY_MILLI:
		{
			delay(arg_int);
			break;
		}
		
		default:
			return 0;
			break;
	}
	return 1;
}

#endif // RASPBERRYPI_PLOTFORM
