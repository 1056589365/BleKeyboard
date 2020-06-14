#pragma once
#include <map>
#include <set>
#include "BleKeyboard.h"

using namespace std;

class KeycodeMapping
{
public:
	map<uint8_t, uint8_t> keycodea;
	map<uint8_t, uint8_t*> keycodeb;
	set<uint8_t> pressings;

	void add(uint8_t key, uint8_t code);

	void add(uint8_t key, uint8_t* code);

	bool check(uint8_t key);

	bool checka(uint8_t key);

	bool checkb(uint8_t key);

	bool isPressing(uint8_t key);

	bool press(uint8_t key, BleKeyboard& blekb, void (*callback)(uint8_t key));

	bool release(uint8_t key, BleKeyboard& blekb, void (*callback)(uint8_t key));

	bool write(uint8_t key, BleKeyboard& blekb, void (*callback)(uint8_t key));
};