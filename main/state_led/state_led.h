#pragma once
#include "driver/ledc.h"

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