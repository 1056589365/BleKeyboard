#pragma once
#include <string>

using namespace std;

void pb_lock(string name, void* arg, void(*callback)(void* arg));

void pb_unlock(string name);

void power_button_init(uint8_t wakeup_pin);