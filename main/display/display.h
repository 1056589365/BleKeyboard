#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#include <iwgl.h>


class Displayer
{
public:
	u8g2_t u8g2;
	PanelManager pm;

	void (*onInit)(void* arg) = nullptr;

	bool powerSaveMode = false;
	bool powerSaveMode2 = false;

	bool exitFlag = false;

	Displayer();
	Displayer(Displayer& arg);
	~Displayer();

	void begin();
	void powerSave(bool isOn);
	void input(uint8_t value);

private:
	static void task(void* pvParameter);
};