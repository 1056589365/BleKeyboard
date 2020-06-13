#include <string>
#include <map>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_system.h>
#include <driver/i2c.h>
#include <esp_system.h>

#include "BleKeyboard.h"
#include "power_button.h"
#include "qadc.h"
#include "state_led.h"
#include "uart_input.h"
#include "keyboard.h"
#include "error_check.h"

#include "project.h"

#define logi(...) ESP_LOGI("main", ##__VA_ARGS__)

using namespace std;


BleKeyboard bleKeyboard;

LED led(23);

void onUartInput(char* data, size_t len)
{
	logi("UART: %s", data);
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

bool bleConnected = false;

float map_v(float val, float I_Min, float I_Max, float O_Min, float O_Max)
{
	return(((val-I_Min)*((O_Max-O_Min)/(I_Max-I_Min)))+O_Min);
}

void onBleConnect()
{
	logi("ble connect");
	bleConnected = true;
	led.pwm(0.4, 130);
}

void onBleDisconnect()
{
	logi("ble disconnect");
	bleConnected = false;
	led.pwm(1.0, 130);
}

void a22333(void* arg)
{
	led.initialize();
	led.pwm(1.0, 10);



	// double v = esp_random() / (double)UINT32_MAX;
	float pv = qadc_get_voltage(ADC1_GPIO36_CHANNEL, 16) / 0.1648036124794745;
	int bl = map_v(min(4200.0f, max(3300.0f, pv)), 3300, 4200, 1, 100);

	logi("power info (%d/%d)", (int)pv, bl);

	bleKeyboard.setBatteryLevel(bl);

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

		gpio_isr_handler_add(gpio_num_t(pin), gpioisr, (void*)&pins[i]);
	}

	pb_lock("state_led", nullptr, [](void* arg){
		led.pwm(0, 400, true);
		pb_unlock("state_led");
	});

	logi("init done");

	sp snapshop;
	snapshop.pin = -1;
	snapshop.sta = -100;

	KeycodeMapping km;

	km.add(14, (uint8_t*)KEY_MEDIA_VOLUME_UP);
	km.add(13, (uint8_t*)KEY_MEDIA_VOLUME_DOWN);
	km.add(5,  (uint8_t*)KEY_MEDIA_PREVIOUS_TRACK);
	km.add(33, (uint8_t*)KEY_MEDIA_NEXT_TRACK);
	km.add(12, (uint8_t*)KEY_MEDIA_PLAY_PAUSE);
	km.add(17, (uint8_t*)KEY_MEDIA_MUTE);

    while(true)
	{
		if(xQueueReceive(isrq, &snapshop, portMAX_DELAY))
		{
			int  pin = snapshop.pin;
			bool sta = snapshop.sta;

			logi("GPIO %d interrrupt, val: %d", pin, sta);

			led.pwm(sta? (bleConnected? 0.4:1.0):0.0, 70);

			if(bleKeyboard.isConnected())
			{
				if(sta)
				{
					km.release(pin, bleKeyboard);
				}else{
					km.press(pin, bleKeyboard);
				}
				
			}
		}
    }
}


extern "C" void app_main()
{
	gpio_install_isr_service(0);

	
	// logi("Loading BLE service..");
	bleKeyboard.begin();
	bleKeyboard._onConnect = onBleConnect;
	bleKeyboard._onDisconnect = onBleDisconnect;

	// logi("Loading ADC..");
	qadc_initialize();
	qadc_config_io(ADC1_GPIO36_CHANNEL);

	// logi("Loading PowerButton..");
	power_button_init(15);

	// logi("Loading UART..");
	uart_input_init(onUartInput);


	xTaskCreate(a22333, "a22333", 16*1024, NULL, 5, NULL);

    logi("initialize done");
}

