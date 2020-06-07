#include <cmath>
#include <stdio.h>
#include <string>
#include <map>

#include <esp_log.h>
#include <assert.h>
#include <driver/i2c.h>

#include "error_check.h"
#include "display.h"
#include "power_button.h"


Displayer::Displayer() : pm(&u8g2)
{

}

Displayer::Displayer(Displayer& arg) : pm(&arg.u8g2)
{
    ESP_LOGE("ErrorError", "The copy constructor is being called!!!!");
	assert(false);
}

Displayer::~Displayer()
{

}

void Displayer::begin()
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_eps32_hw_i2c, u8x8_eps32_hw_i2c);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, false);

	EC(xTaskCreate(this->task, "display", 64*1024, (void*)this, 5, NULL), pdTRUE);
    EC(xTaskCreate(this->onInit, "display-init", 8*1024, (void*)this, 5, NULL), pdTRUE);
}

void Displayer::powerSave(bool isOn)
{
    powerSaveMode = isOn;
}

void Displayer::task(void* pvParameter)
{
    auto _this = (Displayer*)pvParameter;
    PM& pm = _this->pm;
    u8g2_t& u8g2 = _this->u8g2;

    pb_lock("display", pvParameter, [](void* arg){((Displayer*)arg)->exitFlag = true;});

    TickType_t timeDiff = 0;

	while(!_this->exitFlag)
    {
        if(!pm.isEmpty())
        {
            pm.onUpdate(10 * timeDiff);

            timeDiff = xTaskGetTickCount();

            u8g2_ClearBuffer(&u8g2);
            pm.drawFrame();
            u8g2_SendBuffer(&u8g2);

            timeDiff = 10 * (xTaskGetTickCount() - timeDiff); // 计算消耗的时长

            int fps = pm.getFPSLimit();
            int delayTime = (fps>0)? (1000/fps)-timeDiff:150;
            delayTime = max(10, min(10*1000, delayTime));

            vTaskDelay(delayTime / portTICK_RATE_MS); // 等待绘制下一帧
        }else{
            vTaskDelay(300 / portTICK_RATE_MS);
        }

        if(_this->powerSaveMode!=_this->powerSaveMode2)
        {
            u8g2_SetPowerSave(&u8g2, _this->powerSaveMode);
            _this->powerSaveMode2 = _this->powerSaveMode;
        }
    }

    u8g2_SetPowerSave(&u8g2, true);

    pb_unlock("display");

    vTaskDelete(NULL);
}

void Displayer::input(KeyCode value)
{
    pm.input(value);
}