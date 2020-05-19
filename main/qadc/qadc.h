#pragma once
#include <driver/adc.h>

extern bool qadcInitialized;

void qadc_initialize();

void qadc_config_io(adc1_channel_t channel, adc_atten_t atten=ADC_ATTEN_DB_11);

int qadc_get_voltage(adc1_channel_t channel, int samples=8);