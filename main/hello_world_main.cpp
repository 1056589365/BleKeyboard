#include <string>
#include <map>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_system.h>
#include <driver/i2c.h>

#include "BleKeyboard.h"
#include "display.h"
#include "power_button.h"
#include "qadc.h"
#include "io_extension.h"
#include "uart_input.h"
#include "error_check.h"

#include "Startup.h"
#include "Home.h"
#include "project.h"

#define logi(...) ESP_LOGI("main", ##__VA_ARGS__)

using namespace std;

#define SDA 26
#define SCL 25

BleKeyboard bleKeyboard;
Displayer dp;

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
	//logi("UART: %s", data);

	switch (data[0])
	{
		case 'w':
			dp.input(KeyCode::UP);
			break;
		case 's':
			dp.input(KeyCode::DOWN);
			break;
		case 'a':
			dp.input(KeyCode::LEFT);
			break;
		case 'd':
			dp.input(KeyCode::RIGHT);
			break;
		case ' ':
			dp.input(KeyCode::ENTER);
			break;

	}
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
	
	if(val!=0 && bleKeyboard.isConnected())
	{
		double v = rand() / (double)RAND_MAX;
		bleKeyboard.setBatteryLevel((int)100*v);
		//bleKeyboard.print(keycode.at(pin).data());
	}
}

void onDisplayInit(void* arg)
{
	Displayer& dp = *(Displayer*)arg;
	PM& pm = dp.pm;

	(new Home(&pm))->switchIn();

	Startup& su = *(new Startup(&pm));
    su.switchIn();

    su.println("加载BLE键盘..");
	vTaskDelay(1700 / portTICK_RATE_MS);
    bleKeyboard.begin();

    vTaskDelay(700 / portTICK_RATE_MS);

    su.println("加载键盘驱动..");
    mcp23017_init(17, onKey);

    vTaskDelay(700 / portTICK_RATE_MS);

    su.println("加载ADC..");
	qadc_initialize();

    vTaskDelay(700 / portTICK_RATE_MS);

    su.println("加载电源按钮..");
	power_button_init(15);

	vTaskDelay(700 / portTICK_RATE_MS);

	su.println("加载UART..");
	uart_input_init(onUartInput);

	vTaskDelay(400 / portTICK_RATE_MS);

    su.exit();
    
    vTaskDelete(NULL);
}

extern "C" void app_main()
{
	srand((unsigned int)(time(NULL)));
	
	gpio_install_isr_service(0);

	init_i2c();

	dp.onInit = onDisplayInit;
	dp.begin();
	dp.powerSave(false);

    logi("initialize done");
}

