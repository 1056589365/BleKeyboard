#pragma once

void mcp23017_init(int io, void (*asd)(uint8_t pin, uint8_t val));