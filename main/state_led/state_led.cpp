#include <cmath>
#include "state_led.h"
// #include "esp_log.h"

// #define logii(...) ESP_LOGI("state_led", ##__VA_ARGS__)

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

LED::LED(int io)
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
}

void LED::initialize()
{
	ledc_timer_config(&timer_config);

	ledc_channel_config(&channel_config);

	// 启动硬件控制的亮度变换
	ledc_fade_func_install(0);
}

void LED::pwm(float percent, int duration, bool wait)
{
	ledc_mode_t mode = channel_config.speed_mode;
	ledc_channel_t channel = channel_config.channel;

	int duty =  max(0.0f, min(1.0f, percent)) * (pow(2, (uint8_t)timer_config.duty_resolution));

	// logii("new duty: %f", percent);
	ledc_set_fade_with_time(mode, channel, duty, duration);
	ledc_fade_start(mode, channel, wait? LEDC_FADE_WAIT_DONE:LEDC_FADE_NO_WAIT);
}