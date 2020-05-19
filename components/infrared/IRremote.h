#pragma once

#include "core.h"

class IRremote
{
public:
    IRremote();
    ~IRremote();

    void begin(int gpio, rmt_channel_t channel, void (*callback)(uint8_t repeat, uint32_t addr, uint32_t cmd));
    void end();

private:
    IRremoteContext context;
};

