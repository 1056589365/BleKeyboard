#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#include "qadc.h"

#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate

bool qadcInitialized = false;

esp_adc_cal_characteristics_t* adc_chars;

void check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        ESP_LOGI("Voltage", "eFuse Two Point: Supported");
    } else {
        ESP_LOGI("Voltage", "eFuse Two Point: NOT supported");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        ESP_LOGI("Voltage", "eFuse Vref: Supported");
    } else {
        ESP_LOGI("Voltage", "eFuse Vref: NOT supported");
    }
}

void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI("Voltage", "Characterized using Two Point Value");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI("Voltage", "Characterized using eFuse Vref");
    } else {
        ESP_LOGI("Voltage", "Characterized using Default Vref");
    }
}

void qadc_initialize()
{
    check_efuse();

    adc1_config_width(ADC_WIDTH_BIT_12);

    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    qadcInitialized = true;
    ESP_LOGI("Voltage", "ADC init done");
}

void qadc_config_io(adc1_channel_t channel, adc_atten_t atten)
{
    adc1_config_channel_atten(channel, atten); // soc/include/hal/adc_types.h
}

int qadc_get_voltage(adc1_channel_t channel, int samples)
{
    uint32_t voltage = 0;
    const float offset = 1;

    for (int i = 0; i < samples; i++) 
    {
        uint32_t adc_reading = adc1_get_raw(channel);
        voltage += esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        // ESP_LOGI("Voltage", "Raw: %d\t\tVoltage: %dmV", adc_reading, voltage/(i+1));
        vTaskDelay(2 / portTICK_RATE_MS);
    }

    voltage /= samples;

    return (int)voltage / offset;
}