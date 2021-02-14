#pragma once
#include <string>

using namespace std;

class PowerButton
{
    public:
    static gpio_num_t pin;

    static TaskHandle_t handle_Poweroff;
    static map<string, void*> hooks;
    static map<string, void*> args;

    static void installISRService();

    static void initializePin(int pin);

    static void task_Poweroff(void* arg);

    static void hook(string name, void* arg, void(*callback)(void* arg));

    static void unhook(string name);
};