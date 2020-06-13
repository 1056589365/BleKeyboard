#include "keyboard.h"

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

bool KeycodeMapping::press(uint8_t key, BleKeyboard& blekb)
{
	if(checka(key) && !isPressing(key))
	{
		blekb.press(keycodea.at(key));
		pressings.insert(key);
		return true;
	}

	if(checkb(key) && !isPressing(key))
	{
		blekb.press(keycodeb.at(key));
		pressings.insert(key);
		return true;
	}

	return false;
}

bool KeycodeMapping::release(uint8_t key, BleKeyboard& blekb)
{
	if(checka(key) && isPressing(key))
	{
		blekb.release(keycodea.at(key));
		pressings.erase(key);
		return true;
	}

	if(checkb(key) && isPressing(key))
	{
		blekb.release(keycodeb.at(key));
		pressings.erase(key);
		return true;
	}

	return false;
}

bool KeycodeMapping::write(uint8_t key, BleKeyboard& blekb)
{
	if(checka(key))
	{
		blekb.write(keycodea.at(key));
		return true;
	}

	if(checkb(key))
	{
		blekb.write(keycodeb.at(key));
		return true;
	}

	return false;
}