#pragma once

#define x2CheckGetBits(eventGroup, bits) (xEventGroupGetBits(eventGroup) & (bits))
#define x2CheckAllBits(eventGroup, bits) (xEventGroupGetBits(eventGroup) & (bits))==bits

#define EC(value, exception) do { \
        esp_err_t err = value; \
        if(err!=exception){ \
            ESP_LOGE("ErrorCheck", "%s at %d, reason: %d (%s)", __FILE__, __LINE__, err, esp_err_to_name(err)); \
        } \
    }while(0)

inline float map_v(float val, float I_Min, float I_Max, float O_Min, float O_Max)
{
	return (((val-I_Min)*((O_Max-O_Min)/(I_Max-I_Min)))+O_Min);
}