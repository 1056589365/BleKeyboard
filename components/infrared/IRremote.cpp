#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"

#include "IRremote.h"

#define EC(value, exception) do { esp_err_t err = value; if(err!=exception){ ESP_LOGE("ErrorCheck", "%s at %d, reason: %d (%s)", __FILE__, __LINE__, err, esp_err_to_name(err)); } }while(0)

IRremote::IRremote()
{
    
}

IRremote::~IRremote()
{
}

void IRremote::begin(int gpio, rmt_channel_t channel, void (*callback)(uint8_t repeat, uint32_t addr, uint32_t cmd))
{
    context.gpio = gpio;
    context.channel = channel;
    context.exitFlag = false;
    context.callback = callback;

    ESP_LOGI("infrared", "task is starting");

    EC(xTaskCreate(ir_rx_task, "infrared", 2*1024, &context, 10, NULL), pdTRUE);
}

void IRremote::end()
{
    context.exitFlag = true;
    ESP_LOGI("infrared", "stop signal has sent!");
}

