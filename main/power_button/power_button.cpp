#include <map>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include <esp_sleep.h>
#include <esp_log.h>
#include <driver/rtc_io.h>

#include "power_button.h"
#include "project.h"
#include "utils.h"

gpio_num_t PowerButton::pin = GPIO_NUM_NC;
TaskHandle_t PowerButton::handle_Poweroff = nullptr;
map<string, void*> PowerButton::hooks;
map<string, void*> PowerButton::args;

void PowerButton::installISRService()
{
    gpio_install_isr_service(0);
}

void PowerButton::initializePin(int pin)
{
    PowerButton::pin = gpio_num_t(pin);

    // 配置IO中断
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE; // GPIO_INTR_ANYEDGE
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << pin;
    io_conf.pull_down_en = gpio_pulldown_t(0);
    io_conf.pull_up_en = gpio_pullup_t(1);

    gpio_config(&io_conf);

    // 创建触发任务，然后将其挂起
    PowerButton::handle_Poweroff = nullptr;
    EC(xTaskCreate(PowerButton::task_Poweroff, "PowerButton", 4*1024, NULL, 10, &PowerButton::handle_Poweroff), pdTRUE);

    // 外部中断时，继续运行，开始关闭板子电源
    gpio_isr_handler_add(PowerButton::pin, [](void* arg){
        xTaskResumeFromISR(PowerButton::handle_Poweroff);
    }, NULL);
}

void PowerButton::task_Poweroff(void* arg)
{
    // 先挂起
    vTaskSuspend(nullptr);

    ESP_LOGI("PowerButton", "Pressed!");

    // 触发事件
    for(auto hook : hooks)
    {
        ESP_LOGI("PowerButton", "Noticed %s", hook.first.data());
        ((void(*&)(void* arg))hook.second)(args.at(hook.first));
    }

    // 需要等待所有事件源自己解除监听器，做完清理工作后，再退出
    while(!hooks.empty())
    {
        ESP_LOGI("PowerButton", "---------------");

        for(auto hook : hooks)
        {
            ESP_LOGI("PowerButton", "Waiting for %s", hook.first.data());
        }

        vTaskDelay(300/portTICK_RATE_MS);
    }

    // 进入DeepSleep模式
    ESP_LOGI("PowerButton", "Enter deep sleep mode");

    // 设置好唤醒源
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(PowerButton::pin, 0));
    ESP_ERROR_CHECK(rtc_gpio_isolate(PowerButton::pin)); // 将引脚孤立，因为已经有外部上拉电阻了

    // 休眠
    vTaskDelay(1000 / portTICK_RATE_MS);
    esp_deep_sleep_start();
}

void PowerButton::hook(string name, void* arg, void(*callback)(void* arg))
{
    PowerButton::hooks.insert(pair<string, void*>(name, (void*)callback));
    PowerButton::args.insert(pair<string, void*>(name, arg));
}

void PowerButton::unhook(string name)
{
    PowerButton::hooks.erase(name);
    PowerButton::args.erase(name);

    ESP_LOGI("PowerButton", "unhooked %s", name.data());
}