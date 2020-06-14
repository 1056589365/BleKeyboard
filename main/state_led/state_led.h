#pragma once
#include "driver/ledc.h"
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"

class LED
{
public:
	int io;
	ledc_timer_config_t timer_config;
	ledc_channel_config_t channel_config;

	LED(int io);
	void initialize();
	void pwm(float percent, int duration, bool wait=false);
};

class LEDSTA
{
public:
	enum STA{
		WAITING,
		CONNECTED,
		PRESSING,
	};

	LED led;
	STA state = WAITING;
	STA last = state;
	TaskHandle_t work_handler;

	LEDSTA(int io);

	void newState(STA sta);

	void start();

};