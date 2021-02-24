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
#include "power_voltage_sample.h"
#include "state_led.h"
#include "uart_input.h"
#include "virtual_keyboard.h"

#include "utils.h"
#include "project.h"

#define logi(...) ESP_LOGI("main", ##__VA_ARGS__)

BleKeyboard bleKeyboard;
PowerVoltageSample* pvs;
StateLed stateLed(23);

bool bleConnected = false;

void uploadBatteryLevel()
{
	if(bleKeyboard.isConnected())
	{
		int bl = pvs->get_power_voltage();

		logi("power info (%d)", bl);

		bleKeyboard.setBatteryLevel(bl);

		if(bl<=10)
			stateLed.setState(StateLed::LOW_POWER, true);
	} else {
		logi("Faild to upload batteryLevel");
	}
}

void onBleConnect()
{
	logi("ble connect");
	bleConnected = true;
	stateLed.setState(StateLed::CONNECTED, true);

	uploadBatteryLevel();
}

void onBleDisconnect()
{
	logi("ble disconnect");
	bleConnected = false;
	stateLed.setState(StateLed::CONNECTED, false);

	logi("reconnecting..");
	bleKeyboard.advertising->stop();
	bleKeyboard.advertising->start();
}

void main_task(void* arg)
{
	VirtualKeyboard kb(&bleKeyboard);

	kb.addPin(14, (uint8_t*)KEY_MEDIA_VOLUME_UP);
	kb.addPin(4,  (uint8_t*)KEY_MEDIA_VOLUME_DOWN);
	kb.addPin(33, (uint8_t*)KEY_MEDIA_PREVIOUS_TRACK);
	kb.addPin(5,  (uint8_t*)KEY_MEDIA_NEXT_TRACK);
	kb.addPin(12, (uint8_t*)KEY_MEDIA_PLAY_PAUSE);
	kb.addPin(17, (uint8_t*)KEY_MEDIA_MUTE);

	PinLevel pinLevel(-1, -100);

    while(true)
	{
		if(xQueueReceive(isrq, &pinLevel, portMAX_DELAY))
		{
			int  pin = pinLevel.pin;
			bool sta = pinLevel.level;

			logi("IO %d: %d", pin, sta);


			if(bleKeyboard.isConnected() || kb.exists(pin))
			{
				if(sta)
				{
					kb.release(pin);
					stateLed.setState(StateLed::PRESSING, false);
				}else{
					kb.press(pin);
					stateLed.setState(StateLed::PRESSING, true);
				}
			}

			if(pin==32) // top-left corner
			{
				// int bl = (int)(((float)esp_random()) / UINT32_MAX * 100);
				// logi("Random power info: %d", bl);
				// bleKeyboard.setBatteryLevel(bl);
				
				stateLed.setState(StateLed::POWER_LEVEL, !sta);
			}
		}
    }
}

void task_power_voltage_detector(void* arg)
{
	while (true)
	{
		vTaskDelay(30*1000 / portTICK_RATE_MS);

		uploadBatteryLevel();

		// float raw = pvs->sample(16) / 0.1648036124794745;
		// int level = map_v(min(4200.0f, max(3300.0f, raw)), 3300.0f, 4200.0f, 1, 100);

		// logi("PowerVol: %d (raw: %d)", level, (int)raw);

		// if(bleKeyboard.isConnected())
		// {
		// 	bleKeyboard.setBatteryLevel(level);
		// 	// logi("Reported: %d", level);
		// }
	}
}


extern "C" void app_main()
{
	PowerButton::installISRService();

	initKeys();

	stateLed.start();

	bleKeyboard.connectionStatus->_onConnect = onBleConnect;
	bleKeyboard.connectionStatus->_onDisconnect = onBleDisconnect;
	bleKeyboard.begin();

	PowerVoltageSample::init_adc();
	pvs = new PowerVoltageSample(ADC1_CHANNEL_0);
	pvs->set_voltage_range(3500, 4200);

	PowerButton::initializePin(15);

	xTaskCreate(main_task, "main_task", 16*1024, NULL, 5, NULL);
	xTaskCreate(task_power_voltage_detector, "PowerVoltageDetector", 4*1024, NULL, 5, NULL);
}

