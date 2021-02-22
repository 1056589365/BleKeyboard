#include "cmath"
#include "state_led.h"
#include "power_voltage_sample.h"
#include "utils.h"
#include "esp_log.h"
#define logi(...) ESP_LOGI("state_led", ##__VA_ARGS__)

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

extern PowerVoltageSample* pvs;

StateLed::StateLed(int io)
{
	this->io = io;

	timer_config.duty_resolution = LEDC_TIMER_13_BIT;
	timer_config.freq_hz = 5000;
	timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
	timer_config.timer_num = LEDC_TIMER_0;
	timer_config.clk_cfg = LEDC_AUTO_CLK;

	channel_config.gpio_num = io;
	channel_config.speed_mode = LEDC_LOW_SPEED_MODE;
	channel_config.channel = LEDC_CHANNEL_0;
	channel_config.duty = 0;
	channel_config.timer_sel = LEDC_TIMER_0;
	// channel_config.intr_type = LEDC_INTR_FADE_END;

	ledc_timer_config(&timer_config);
	ledc_channel_config(&channel_config);
	ledc_fade_func_install(0); // 启动硬件控制的亮度变换
}

void StateLed::start()
{
	xTaskCreate(task_led_state, "task_led_state", 4*1024, this, 10, &task);
}

void StateLed::setState(State sta, bool enabled)
{
	states[sta] = enabled;
}

bool StateLed::getState(State sta)
{
	return states[sta];
}

void StateLed::task_led_state(void* arg)
{
	// 获取实例指针
	StateLed* _this = (StateLed*) arg;

	// 注册所有"状态"
	for (int i=0;i<STATE_MAX;i++)
		_this->states[State(i)] = false;
	
	// 切换到默认状态
	_this->states[State(0)] = true;

	StateLed::State lastSta = StateLed::DEFAULT;
	int interval = 0;
		
	while (true)
	{
		bool stateEnabled = false;

		for (int i=STATE_MAX-1;i>=0 && !stateEnabled;i--)
		{
			stateEnabled = _this->states[State(i)];
			bool isNewState = lastSta != State(i);

			if(stateEnabled)
			{
				// logi("state: %d", i);

				switch (i)
				{
					case DEFAULT:
					{
						_this->pwm(0.7, 50);
						vTaskDelay(500/portTICK_RATE_MS);
						_this->pwm(0.05, 50);
						interval = 500;
						break;
					}

					case CONNECTED:
						if(lastSta != PRESSING)
						{
							if(isNewState)
								_this->pwm(0.0, 10);
							_this->pwm(0.4, 1300);
						} else {
							_this->pwm(0.4, 80);
						}
						
						break;
					
					case PRESSING:
						_this->pwm(0.0, 80);
						break;
					
					case POWER_LEVEL:
					{
						_this->pwm(1.0, 10);
						vTaskDelay(300/portTICK_RATE_MS);

						int bl = pvs->get_power_voltage();
						_this->pwm(map_v(bl, 0, 100, 0.0f, 1.0f), 300);

						interval = 300;
						break;
					}

					case LOW_POWER:
					{
						_this->pwm(0.2, 10);
						_this->pwm(0.0, 350);
						_this->pwm(0.2, 350);
						_this->pwm(0.0, 350);
						_this->pwm(0.2, 350);
						interval = 1000;
						break;
					}
						

					default:
						break;
				}

				if(isNewState)
				{
					lastSta = State(i);
					interval = 10;
				} else {
					int d = min(max(interval, 0), 10000);
					// logi("Interval: %d", d);
					vTaskDelay(d/portTICK_RATE_MS);
				}
			}
		}
	}

	vTaskDelete(nullptr);
}



void StateLed::pwm(float percent, int duration, bool wait)
{
	if(lastPwm == percent)
	{
		return;
	}

	lastPwm = percent;

	ledc_mode_t mode = channel_config.speed_mode;
	ledc_channel_t channel = channel_config.channel;

	int duty =  max(0.0f, min(1.0f, percent)) * (pow(2, (uint8_t)timer_config.duty_resolution));

	// logi("new duty: %f", percent);
	ledc_set_fade_with_time(mode, channel, duty, duration);
	ledc_fade_start(mode, channel, wait? LEDC_FADE_WAIT_DONE:LEDC_FADE_NO_WAIT);
}