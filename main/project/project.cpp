#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include "project.h"

#define logi(...) ESP_LOGI("projectCpp", ##__VA_ARGS__)

int pins[] = {
    32, 33, 25, 26,
    27, 14, 12, 13 /*15 // power button*/ ,
    4 , 16, 17, 5,
    18, 19, 21, /*//22,23 // reserve*/ 
};

xQueueHandle isrq = xQueueCreate(10, sizeof(PinLevel));


void IRAM_ATTR __gpioisr(void* arg)
{
	int pin = *((int*)arg);
    int level = gpio_get_level(gpio_num_t(pin));

	PinLevel pl(pin, level);

    xQueueSendFromISR(isrq, &pl, NULL);
}

void initKeys()
{
    for(int i=0;i<sizeof(pins)/sizeof(int);i++)
	{
		int pin = pins[i];

		gpio_config_t io_conf;
		io_conf.intr_type = GPIO_INTR_ANYEDGE;
		io_conf.mode = GPIO_MODE_INPUT;
		io_conf.pin_bit_mask = 1ULL << pin;
		io_conf.pull_down_en = gpio_pulldown_t(0);
		io_conf.pull_up_en = gpio_pullup_t(1);
		gpio_config(&io_conf);

		gpio_isr_handler_add(gpio_num_t(pin), __gpioisr, (void*)&pins[i]);
	}
}

