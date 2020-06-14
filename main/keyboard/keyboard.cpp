#include "keyboard.h"
#include "esp_log.h"

#define logi(...) ESP_LOGI("keyboard", ##__VA_ARGS__)

void KeycodeMapping::add(uint8_t key, uint8_t code)
{
	if(check(key)) return;

	keycodea.insert(make_pair(key, code));
}

void KeycodeMapping::add(uint8_t key, uint8_t* code)
{
	if(check(key)) return;

	keycodeb.insert(make_pair(key, code));
}

bool KeycodeMapping::check(uint8_t key)
{
	return checka(key) || checkb(key);
}

bool KeycodeMapping::checka(uint8_t key)
{
	return keycodea.find(key)!=keycodea.end();
}

bool KeycodeMapping::checkb(uint8_t key)
{
	return keycodeb.find(key)!=keycodeb.end();
}

bool KeycodeMapping::isPressing(uint8_t key)
{
	return pressings.find(key)!=pressings.end();
}

bool KeycodeMapping::press(uint8_t key, BleKeyboard& blekb, void (*callback)(uint8_t key))
{
	if(checka(key) && !isPressing(key))
	{
		blekb.press(keycodea.at(key));
		pressings.insert(key);
		if(callback) callback(key);
		logi("key %d pressed", key);
		return true;
	}

	if(checkb(key) && !isPressing(key))
	{
		blekb.press(keycodeb.at(key));
		pressings.insert(key);
		if(callback) callback(key);
		logi("key %d pressed", key);
		return true;
	}

	return false;
}

bool KeycodeMapping::release(uint8_t key, BleKeyboard& blekb, void (*callback)(uint8_t key))
{
	if(checka(key) && isPressing(key))
	{
		blekb.release(keycodea.at(key));
		pressings.erase(key);
		if(callback) callback(key);
		logi("key %d released", key);
		return true;
	}

	if(checkb(key) && isPressing(key))
	{
		blekb.release(keycodeb.at(key));
		pressings.erase(key);
		if(callback) callback(key);
		logi("key %d released", key);
		return true;
	}

	return false;
}

bool KeycodeMapping::write(uint8_t key, BleKeyboard& blekb, void (*callback)(uint8_t key))
{
	if(checka(key))
	{
		blekb.write(keycodea.at(key));
		if(callback) callback(key);
		return true;
	}

	if(checkb(key))
	{
		blekb.write(keycodeb.at(key));
		if(callback) callback(key);
		return true;
	}

	return false;
}