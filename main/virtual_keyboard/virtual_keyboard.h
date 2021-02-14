#pragma once
#include <map>
#include <set>
#include "BleKeyboard.h"

using namespace std;

class VirtualKeyboard
{
public:
	BleKeyboard* blekb = nullptr;
	map<int, uint8_t*> keycode;
	set<int> pressedPins;

	VirtualKeyboard(BleKeyboard* blekb);

	void addPin(int pin, uint8_t* key);
	bool exists(int pin);
	bool isPressing(int pin);
	bool press(int pin);
	bool release(int pin);
	bool write(int pin);
};