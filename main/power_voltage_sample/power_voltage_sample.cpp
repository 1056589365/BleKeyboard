#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#include "power_voltage_sample.h"
#include "utils.h"

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

PowerVoltageSample::PowerVoltageSample(adc1_channel_t channel, adc_atten_t atten)
{
    this->adc_channel = channel;
    this->atten = atten;

    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    PowerVoltageSample::print_char_val_type(val_type);

    adc1_config_channel_atten(channel, atten); // soc/include/hal/adc_types.h
}

void PowerVoltageSample::init_adc()
{
    PowerVoltageSample::check_efuse();

    adc1_config_width(ADC_WIDTH_BIT_12);

    ESP_LOGI("PowerVoltageSample", "ADC initialized");
}

void PowerVoltageSample::set_voltage_range(int min_voltage, int max_voltage)
{
    this->min_voltage = min_voltage;
    this->max_voltage = max_voltage;
}


int PowerVoltageSample::sample(int samples)
{
    uint32_t voltage = 0;
    // const float offset = 1;

    for (int i = 0; i < samples; i++) 
    {
        uint32_t adc_reading = adc1_get_raw(adc_channel);
        voltage += esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        // ESP_LOGI("PowerVoltageSample", "Raw: %d\t\tVoltage: %dmV", adc_reading, voltage/(i+1));
        vTaskDelay(2 / portTICK_RATE_MS);
    }

    voltage /= samples;

    // return (int)voltage / offset;
    return (int)voltage;
}

int PowerVoltageSample::get_power_voltage(int samples)
{
    float min_vol = (float) this->min_voltage;
    float max_vol = (float) this->max_voltage;
    
    float pv = this->sample(samples) / 0.1648036124794745;

	return map_v(min(max_vol, max(min_vol, pv)), min_vol, max_vol, 1, 100);
}

void PowerVoltageSample::PowerVoltageSample::check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        ESP_LOGI("PowerVoltageSample", "eFuse Two Point: Supported");
    } else {
        ESP_LOGI("PowerVoltageSample", "eFuse Two Point: NOT supported");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        ESP_LOGI("PowerVoltageSample", "eFuse Vref: Supported");
    } else {
        ESP_LOGI("PowerVoltageSample", "eFuse Vref: NOT supported");
    }
}

void PowerVoltageSample::print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI("PowerVoltageSample", "Characterized using Two Point Value");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI("PowerVoltageSample", "Characterized using eFuse Vref");
    } else {
        ESP_LOGI("PowerVoltageSample", "Characterized using Default Vref");
    }
}