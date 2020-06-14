#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#include "error_check.h"

#define x2CheckGetBits(eventGroup, bits) (xEventGroupGetBits(eventGroup) & (bits))
#define x2CheckAllBits(eventGroup, bits) (xEventGroupGetBits(eventGroup) & (bits))==bits


float map_v(float val, float I_Min, float I_Max, float O_Min, float O_Max)
{
	return(((val-I_Min)*((O_Max-O_Min)/(I_Max-I_Min)))+O_Min);
}