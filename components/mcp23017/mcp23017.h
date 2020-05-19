#pragma once

#include "esp_err.h"
#include "driver/i2c.h"


class MCP23017 
{
public:

	enum class REG_ADDR
	{
		IODIR		= 0x00,
		IPOL		= 0x01,
		GPINTEN		= 0x02,
		DEFVAL		= 0x03,
		INTCON		= 0x04,
		IOCON		= 0x05,
		GPPU		= 0x06,
		INTF		= 0x07,
		INTCAP		= 0x08,
		GPIO		= 0x09,
		OLAT		= 0x0A,

		/* absolute register address */
		IODIRA		= 0x00,
		IPOLA 		= 0x02,
		GPINTENA 	= 0x04,
		DEFVALA 	= 0x06,
		INTCONA 	= 0x08,
		IOCONA 		= 0x0A,
		GPPUA 		= 0x0C,
		INTFA 		= 0x0E,
		INTCAPA 	= 0x10,
		GPIOA 		= 0x12,
		OLATA 		= 0x14,

		IODIRB 		= 0x01,
		IPOLB 		= 0x03,
		GPINTENB 	= 0x05,
		DEFVALB 	= 0x07,
		INTCONB 	= 0x09,
		IOCONB 		= 0x0B,
		GPPUB 		= 0x0D,
		INTFB 		= 0x0F,
		INTCAPB 	= 0x11,
		GPIOB 		= 0x13,
		OLATB 		= 0x15,
	};

	enum IO_DIR
	{
		OUTPUT	= 0,
		INPUT	= 1
	};

	enum INTR_TYPE
	{
		CHANGE,
		FALLING,
		RISING
	};

	MCP23017(uint8_t channel=I2C_NUM_0, uint8_t address=0x20);

	void init();
	
	void pinMode(uint8_t pin, bool isGroupA, IO_DIR direction);
	uint16_t readGPIOAB();
	void writeGPIOAB(uint16_t ab);
	void digitalWrite(uint8_t pin, bool isGroupA, uint8_t value);
	void pullup(uint8_t pin, bool isGroupA, bool enable);
	bool digitalRead(uint8_t pin, bool isGroupA);

	void setupInterrupts(uint8_t mirroring, uint8_t openDrain, uint8_t polarity);
	void setupInterruptPin(uint8_t pin, bool isGroupA, INTR_TYPE mode);
	uint8_t getLastInterruptPin();
	uint8_t getLastInterruptPinValue();

private:
	const uint8_t defaultAddress = 0x20;

	uint8_t address; // i2c address
	uint8_t channel; // i2c channel on esp32

	void readRegister(uint8_t realRegAddr, uint8_t* retData, int length=1);
	void writeRegister(uint8_t realRegAddr, uint8_t* regValue, int length=1);
	void readRegister(REG_ADDR realRegAddr, uint8_t* retData, int length=1);
	void writeRegister(REG_ADDR realRegAddr, uint8_t* regValue, int length=1);

	void updateRegBit(uint8_t realRegAddr, uint8_t regBit, uint8_t newValue);
	void updateRegBit(REG_ADDR realRegAddr, uint8_t regBit, uint8_t newValue);

	uint8_t offset(REG_ADDR realRegAddr, bool isGroupA);
};


