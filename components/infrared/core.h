#pragma once

#include "driver/rmt.h"

#ifdef __cplusplus
extern "C"{
#endif


struct IRremoteContext
{
	int gpio;
    rmt_channel_t channel;

	bool exitFlag;

	void (*callback)(uint8_t repeat, uint32_t addr, uint32_t cmd);
};


void ir_rx_task(void *arg);


#ifdef __cplusplus
}
#endif