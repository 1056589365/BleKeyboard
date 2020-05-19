#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#include "error_check.h"

#define x2CheckGetBits(eventGroup, bits) (xEventGroupGetBits(eventGroup) & (bits))
#define x2CheckAllBits(eventGroup, bits) (xEventGroupGetBits(eventGroup) & (bits))==bits