#include <map>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>

#include "power_button.h"
#include "error_check.h"

gpio_num_t ext_wakeup_pin;
TaskHandle_t pid_powerButton = nullptr;
map<string, void*> hooks;
map<string, void*> args;

void pb_lock(string name, void* arg, void(*callback)(void* arg))
{
    hooks.insert(pair<string, void*>(name, (void*)callback));
    args.insert(pair<string, void*>(name, arg));
}

void pb_unlock(string name)
{
    hooks.erase(name);
    args.erase(name);
}

void task_poweroff(void* arg)
{
    vTaskSuspend(nullptr);

    ESP_LOGI("PowerButton", "pressed!");

    for(auto hook : hooks)
    {
        ((void(*&)(void* arg))hook.second)(args.at(hook.first));
        ESP_LOGI("PowerButton", "noticed %s", hook.first.data());
    }

    while(!hooks.empty())
    {
        ESP_LOGI("PowerButton", "---------------");

        for(auto hook : hooks)
        {
            ESP_LOGI("PowerButton", "waiting for %s", hook.first.data());
        }

        vTaskDelay(100/portTICK_RATE_MS);
    }

    ESP_LOGI("PowerButton", "enter deep sleep mode");

    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(ext_wakeup_pin, 0));
    ESP_ERROR_CHECK(rtc_gpio_isolate(ext_wakeup_pin));

    vTaskDelay(1000 / portTICK_RATE_MS);

    esp_deep_sleep_start();
}

void power_button_init(uint8_t wakeup_pin)
{
    ext_wakeup_pin = gpio_num_t(wakeup_pin);

    gpio_isr_handler_add(ext_wakeup_pin, [](void* arg){
        xTaskResumeFromISR(pid_powerButton);
    }, NULL);

    gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_POSEDGE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = 1ULL << ext_wakeup_pin;
	io_conf.pull_down_en = gpio_pulldown_t(0);
	io_conf.pull_up_en = gpio_pullup_t(0);
	gpio_config(&io_conf);

    EC(xTaskCreate(task_poweroff, "PowerButton", 2*1024, NULL, 10, &pid_powerButton), pdTRUE);
}