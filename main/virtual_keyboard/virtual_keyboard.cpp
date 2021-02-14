#include <map>
#include <set>
#include "esp_log.h"

#include "BleKeyboard.h"
#include "virtual_keyboard.h"

#define logi(...) ESP_LOGI("VirtualKeyboard", ##__VA_ARGS__)

VirtualKeyboard::VirtualKeyboard(BleKeyboard* blekb)
{
	this->blekb = blekb;
}

void VirtualKeyboard::addPin(int pin, uint8_t* key)
{
	keycode.insert(make_pair(pin, key));
}

bool VirtualKeyboard::exists(int pin)
{
	return keycode.find(pin) != keycode.end();
}

bool VirtualKeyboard::isPressing(int pin)
{
	return pressedPins.find(pin) != pressedPins.end();
}

bool VirtualKeyboard::press(int pin)
{
	if(exists(pin))
	{
		// 释放之前按下的按键
		for(int p : pressedPins)
			blekb->release(keycode.at(p));
		pressedPins.clear();

		blekb->press(keycode.at(pin));
		pressedPins.insert(pin);
		logi("Pin pressed: %d", pin);
		return true;
	}

	return false;
}

bool VirtualKeyboard::release(int pin)
{
	if(exists(pin) && isPressing(pin))
	{
		blekb->release(keycode.at(pin));
		pressedPins.erase(pin);
		logi("Pin released: %d", pin);
		return true;
	}

	return false;
}

bool VirtualKeyboard::write(int pin)
{
	if(exists(pin))
	{
		blekb->write(keycode.at(pin));
		return true;
	}

	return false;
}