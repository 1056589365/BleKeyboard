#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

struct PinLevel
{
	int pin;
	int level;

	PinLevel(int pin, int level)
	{
		this->pin = pin;
		this->level = level;
	}
};

void initKeys();

extern xQueueHandle isrq;