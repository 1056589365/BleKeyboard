#pragma once
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate

class PowerVoltageSample
{
    public:
    adc1_channel_t adc_channel;
    esp_adc_cal_characteristics_t* adc_chars;
    adc_atten_t atten;

    int max_voltage;
    int min_voltage;

    PowerVoltageSample(adc1_channel_t channel, adc_atten_t atten=ADC_ATTEN_DB_11);

    static void init_adc();
    void set_voltage_range(int min_voltage, int max_voltage);
    int sample(int samples=8);
    int get_power_voltage(int samples=8);

    private:
    static void check_efuse();
    static void print_char_val_type(esp_adc_cal_value_t val_type);

};