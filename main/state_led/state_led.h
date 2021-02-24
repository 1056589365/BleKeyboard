#pragma once
#include "map"
#include "driver/ledc.h"
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"

class StateLed
{
	public:
		enum State {
			DEFAULT,
			CONNECTED,
			PRESSING,
			LOW_POWER,
			POWER_LEVEL,
			STATE_MAX
		};

		int io;
		TaskHandle_t task;
		std::map<State, bool> states;
		float lastPwm = -1;

		StateLed(int io);

		void start();
		void setState(State sta, bool enabled);
		bool getState(State sta);
		static void task_led_state(void* arg);

	private:
		ledc_timer_config_t timer_config;
		ledc_channel_config_t channel_config;

		void pwm(float percent, int duration, bool wait=true);

};