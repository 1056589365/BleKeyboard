#include <string>
#include <map>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_system.h>
#include <driver/i2c.h>

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

// BleKeyboard bleKeyboard;

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

void onKey(uint8_t pin, uint8_t val)
{
	static map<int, string> keycode;

	if(keycode.size()==0)
	{
		keycode.insert(pair<int, string>(0, "a"));
		keycode.insert(pair<int, string>(1, "b"));
		keycode.insert(pair<int, string>(2, "c"));
		keycode.insert(pair<int, string>(3, "d"));
		keycode.insert(pair<int, string>(4, "e"));
		keycode.insert(pair<int, string>(5, "f"));
		keycode.insert(pair<int, string>(6, "g"));
		keycode.insert(pair<int, string>(7, "h"));
		keycode.insert(pair<int, string>(8, "i"));
		keycode.insert(pair<int, string>(9, "j"));
		keycode.insert(pair<int, string>(10, "k"));
		keycode.insert(pair<int, string>(11, "l"));
		keycode.insert(pair<int, string>(12, "m"));
		keycode.insert(pair<int, string>(13, "n"));
		keycode.insert(pair<int, string>(14, "o"));
		keycode.insert(pair<int, string>(15, "p"));
	}

	logi("pin interrupt: %d is %d", pin, val);
	
	if(val!=0/* && bleKeyboard.isConnected()*/)
	{
		//double v = rand() / (double)RAND_MAX;
		//bleKeyboard.setBatteryLevel((int)100*v);
		//bleKeyboard.print(keycode.at(pin).data());
	}
}

void initPin(int io)
{
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_POSEDGE;
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

int pins[] = {
	34, //
	35, //
	32, 
	33, 
	25, 
	26, 
	27, 
	14, 
	12 ,
	13 ,
	//15,
	4 , 
	16, 
	17,
	5,
	18,
	19, 
	21, 
	
	//22,23
	};

int led = 23;

void initLed()
{
	gpio_config_t io_conf;
	//io_conf.intr_type = GPIO_INTR_POSEDGE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = 1ULL << led;
	io_conf.pull_down_en = gpio_pulldown_t(0);
	io_conf.pull_up_en = gpio_pullup_t(0);
	gpio_config(&io_conf);

	gpio_set_level(gpio_num_t(led), false);
}

void a22333(void* arg)
{
	initLed();

	for(int i=0;i<sizeof(pins)/sizeof(int);i++)
	{
		int pin = pins[i];
		initPin(pin);
		gpio_isr_handler_add(gpio_num_t(pin), gpioisr, (void*)&pins[i]);
		logi("prepering: %d", pin);
	}

	logi("init done");

	sp snapshop;
	snapshop.pin = -1;
	snapshop.sta = -100;

    while(true)
	{
		if(xQueueReceive(isrq, &snapshop, portMAX_DELAY))
		{
			logi("GPIO %d interrrupt, val: %d", snapshop.pin, snapshop.sta);
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
	//bleKeyboard.begin();

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
	srand((unsigned int)(time(NULL)));
	
	gpio_install_isr_service(0);

	logi("Loading I2C driver..");
	init_i2c();
	
	EC(xTaskCreate(init_, "initialize", 16*1024, NULL, 10, nullptr), pdTRUE);
}

