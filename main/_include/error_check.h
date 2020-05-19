#pragma once

#include <esp_log.h>

#define EC(value, exception) do { esp_err_t err = value; if(err!=exception){ ESP_LOGE("ErrorCheck", "%s at %d, reason: %d (%s)", __FILE__, __LINE__, err, esp_err_to_name(err)); } }while(0)
