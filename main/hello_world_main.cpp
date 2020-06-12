#include <string>
#include <map>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_system.h>
#include <driver/i2c.h>
#include "driver/ledc.h"

#include "BleKeyboard.h"
#include "power_button.h"
#include "qadc.h"
// #include "io_extension.h"
#include "uart_input.h"
#include "error_check.h"

#include "project.h"

#define logi(...) ESP_LOGI("main", ##__VA_ARGS__)

using namespace std;

#define SDA 26
#define SCL 25

BleKeyboard bleKeyboard;

void init_i2c()
{
	const int channel = I2C_NUM_0;

	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = SDA;
	conf.sda_pullup_en = 1;
	conf.scl_io_num = SCL;
	conf.scl_pullup_en = 1;
	conf.master.clk_speed = 1000*1000;

	ESP_ERROR_CHECK(i2c_param_config(channel, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(channel, conf.mode, 0, 0, 0));
}

void onUartInput(char* data, size_t len)
{
	logi("UART: %s", data);
	//double v = rand() / (double)RAND_MAX;
	//bleKeyboard.setBatteryLevel((int)100*v);
}


void initPin(int io)
{
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_ANYEDGE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = 1ULL << io;
	io_conf.pull_down_en = gpio_pulldown_t(0);
	io_conf.pull_up_en = gpio_pullup_t(1);
	gpio_config(&io_conf);
}

struct sp
{
	int pin;
	int sta;
};

xQueueHandle isrq = xQueueCreate(10, sizeof(sp));

void IRAM_ATTR gpioisr(void* arg)
{
	int pin = *((int*)arg);
	sp ssss;
	ssss.pin = pin;
	ssss.sta = gpio_get_level(gpio_num_t(pin));

    xQueueSendFromISR(isrq, &ssss, NULL);
}

int pins[] = {32, 33, 25, 26, 27, 14, 12 ,13 /*15 // power button*/ ,4 , 16, 17,5,18,19, 21, /*//22,23 // reserve*/ };


ledc_channel_config_t ledc_channel;
	



int led = 23;

void pwm(int duty, int time=1000, bool wait=false) // 8192
{
	duty = (int)(((float)duty)/100*1024);
	logi("duty to: %d", duty);
	ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, duty, time);
	ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, wait? LEDC_FADE_WAIT_DONE:LEDC_FADE_NO_WAIT);
}

void initLed()
{
	ledc_timer_config_t ledc_timer;

	ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;
	ledc_timer.freq_hz = 5000;
	ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_timer.timer_num = LEDC_TIMER_0;
	ledc_timer.clk_cfg = LEDC_AUTO_CLK;

	ledc_timer_config(&ledc_timer);


	//////
	ledc_channel.gpio_num = led;
	ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_channel.channel = LEDC_CHANNEL_0;
	ledc_channel.duty = 8191;
	ledc_channel.timer_sel = LEDC_TIMER_0;

	ledc_channel_config(&ledc_channel);


	ledc_fade_func_install(0);

	vTaskDelay(700/portTICK_RATE_MS);

	pwm(40, 500, true);
}


void a22333(void* arg)
{
	initLed();

	for(int i=0;i<sizeof(pins)/sizeof(int);i++)
	{
		int pin = pins[i];
		initPin(pin);
		gpio_isr_handler_add(gpio_num_t(pin), gpioisr, (void*)&pins[i]);
		//logi("prepering: %d", pin);
	}

	pb_lock("state_led", nullptr, [](void* arg){
		pwm(0, 350, true);
		pb_unlock("state_led");
	});

	logi("init done");

	sp snapshop;
	snapshop.pin = -1;
	snapshop.sta = -100;

	map<int, uint8_t> keycode;
	map<int, uint8_t*> keycode2;
	keycode2.insert(make_pair(14, (uint8_t*)KEY_MEDIA_VOLUME_UP));
	keycode2.insert(make_pair(13, (uint8_t*)KEY_MEDIA_VOLUME_DOWN));
	

    while(true)
	{
		if(xQueueReceive(isrq, &snapshop, portMAX_DELAY))
		{
			int  pin = snapshop.pin;
			bool sta = snapshop.sta;

			logi("GPIO %d interrrupt, val: %d", pin, sta);

			pwm(sta? 50:5, 150, true);

			if(!sta && bleKeyboard.isConnected())
			{
				if(keycode.find(pin)!=keycode.end())
				{
					bleKeyboard.write(keycode.at(pin));
					logi("pressed %d", pin);
				}else

				if(keycode2.find(pin)!=keycode2.end())
				{
					bleKeyboard.write(keycode2.at(pin));
					logi("pressed2 %d", pin);
				}
			}
		}
    }
}

void initIo()
{
	xTaskCreate(a22333, "a22333", 8*1024, NULL, 5, NULL);
}

void init_(void* arg)
{
	logi("Loading BLE service..");
	bleKeyboard.begin();

	initIo();

	logi("Loading ADC..");
	qadc_initialize();

	logi("Loading PowerButton..");
	power_button_init(15);

	logi("Loading UART..");
	uart_input_init(onUartInput);

    logi("initialize finished");

	vTaskDelete(nullptr);
}

extern "C" void app_main()
{
	srand((unsigned int)(time(NULL))); // double v = rand() / (double)RAND_MAX;
	
	gpio_install_isr_service(0);

	logi("Loading I2C driver..");
	init_i2c();
	
	EC(xTaskCreate(init_, "initialize", 16*1024, NULL, 10, nullptr), pdTRUE);
}

