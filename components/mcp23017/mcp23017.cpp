#include "mcp23017.h"
#include <driver/gpio.h>
#include <driver/i2c.h>
#include "esp_log.h"


#define EC(value, exception) do { esp_err_t err = value; if(err!=exception){ ESP_LOGE("ErrorCheck", "%s at %d, reason: %d (%s)", __FILE__, __LINE__, err, esp_err_to_name(err)); } }while(0)

#define bitWrite(data, bit, value) if(value) { data |= (1 << bit); } else { data &= ~(1 << bit);}

#define bitRead(value, bit) (value >> bit) & 0x01

MCP23017::MCP23017(uint8_t channel, uint8_t address)
{
	this->channel = channel;
	this->address = address;
}

void MCP23017::init()
{
	// make all I/O's input

	uint8_t input = 0xFF;
	
	writeRegister(REG_ADDR::IODIRA, &input);
	writeRegister(REG_ADDR::IODIRB, &input);
}



void MCP23017::readRegister(REG_ADDR realRegAddr, uint8_t* retData, int length)
{
	readRegister((uint8_t)realRegAddr, retData, length);
}

void MCP23017::writeRegister(REG_ADDR realRegAddr, uint8_t* regValue, int length)
{
	writeRegister((uint8_t)realRegAddr, regValue, length);
}

void MCP23017::updateRegBit(REG_ADDR realRegAddr, uint8_t regBit, uint8_t newValue)
{
	updateRegBit((uint8_t)realRegAddr, regBit, newValue);
}


void MCP23017::readRegister(uint8_t realRegAddr, uint8_t* retData, int length)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	EC(i2c_master_start(cmd), ESP_OK);
	EC(i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_WRITE, true), ESP_OK);
	EC(i2c_master_write_byte(cmd, realRegAddr, true), ESP_OK);
	EC(i2c_master_stop(cmd), ESP_OK);
	EC(i2c_master_cmd_begin(channel, cmd, 100 / portTICK_RATE_MS), ESP_OK);
	i2c_cmd_link_delete(cmd);

	cmd = i2c_cmd_link_create();
	EC(i2c_master_start(cmd), ESP_OK);
	EC(i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_READ, true), ESP_OK);

	for(int i=0;i<length;i++)
	{
		EC(i2c_master_read_byte(cmd, retData, I2C_MASTER_ACK), ESP_OK);
		retData ++;
	}

	EC(i2c_master_cmd_begin(channel, cmd, 100 / portTICK_RATE_MS), ESP_OK);
	i2c_cmd_link_delete(cmd);

}

void MCP23017::writeRegister(uint8_t realRegAddr, uint8_t* regValue, int length)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	EC(i2c_master_start(cmd), ESP_OK);
	EC(i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_WRITE, true), ESP_OK);
	EC(i2c_master_write_byte(cmd, realRegAddr, true), ESP_OK);
	

	for(int i=0;i<length;i++)
	{
		EC(i2c_master_write_byte(cmd, *regValue, true), ESP_OK);
		regValue ++;
	}

	EC(i2c_master_stop(cmd), ESP_OK);
	EC(i2c_master_cmd_begin(channel, cmd, 100 / portTICK_RATE_MS), ESP_OK);
	i2c_cmd_link_delete(cmd);
}

void MCP23017::updateRegBit(uint8_t realRegAddr, uint8_t regBit, uint8_t newValue)
{
	uint8_t current_value;

	readRegister(realRegAddr, &current_value);

	bitWrite(current_value, regBit, newValue)
	
	writeRegister(realRegAddr, &current_value);
}

uint8_t MCP23017::offset(REG_ADDR realRegAddr, bool isGroupA)
{
	uint8_t rga = (uint8_t)realRegAddr;
	return isGroupA? (rga << 1):(rga << 1) | 1;
}

void MCP23017::pinMode(uint8_t pin, bool isGroupA, IO_DIR direction)
{
	updateRegBit(offset(REG_ADDR::IODIR, isGroupA), pin, (uint8_t)direction);
}

uint16_t MCP23017::readGPIOAB()
{
	uint8_t ab[2] = {0, 0};

	readRegister(REG_ADDR::GPIOA, ab, 2);

	return (ab[0] << 8) + ab[1];
}

void MCP23017::writeGPIOAB(uint16_t ab)
{
	uint8_t a = (ab >> 8) & 0xFF;
	uint8_t b =  ab & 0xFF;
	uint8_t AB[2] = {a, b};

	writeRegister(REG_ADDR::GPIOA, AB, 2);

	AB[0] = 0;
}

void MCP23017::digitalWrite(uint8_t pin, bool isGroupA, uint8_t value)
{
	updateRegBit(offset(REG_ADDR::OLATA, isGroupA), pin, value);
}

void MCP23017::pullup(uint8_t pin, bool isGroupA, bool enable)
{
	updateRegBit(offset(REG_ADDR::GPPU,  isGroupA), pin, enable);
}

bool MCP23017::digitalRead(uint8_t pin, bool isGroupA)
{
	uint8_t state;

	readRegister(offset(REG_ADDR::GPIO, isGroupA), &state);

	return (state >> pin) & 0x1;
}


/**
 * Configures the interrupt system. both port A and B are assigned the same configuration.
 * Mirroring will OR both INTA and INTB pins.
 * Opendrain will set the INT pin to value or open drain.
 * polarity will set LOW or HIGH on interrupt.
 * Default values after Power On Reset are: (false, false, LOW)
 * If you are connecting the INTA/B pin to arduino 2/3, you should configure the interupt handling as FALLING with
 * the default configuration.
 */
void MCP23017::setupInterrupts(uint8_t mirroring, uint8_t openDrain, uint8_t polarity)
{
	uint8_t ioconf;
	readRegister(REG_ADDR::IOCONA, &ioconf);

	bitWrite(ioconf, 6, mirroring);
	bitWrite(ioconf, 2, openDrain);
	bitWrite(ioconf, 1, polarity);

	writeRegister(REG_ADDR::IOCONA, &ioconf);

	////

	readRegister(REG_ADDR::IOCONB, &ioconf);

	bitWrite(ioconf, 6, mirroring);
	bitWrite(ioconf, 2, openDrain);
	bitWrite(ioconf, 1, polarity);

	writeRegister(REG_ADDR::IOCONB, &ioconf);
}

/**
 * Set's up a pin for interrupt. uses arduino MODEs: CHANGE, FALLING, RISING.
 *
 * Note that the interrupt condition finishes when you read the information about the port / value
 * that caused the interrupt or you read the port itself. Check the datasheet can be confusing.
 *
 */
void MCP23017::setupInterruptPin(uint8_t pin, bool isGroupA, INTR_TYPE mode)
{
	// set the pin interrupt control (0 means change, 1 means compare against given value);
	updateRegBit(offset(REG_ADDR::INTCON, isGroupA), pin, (mode!=INTR_TYPE::CHANGE));
	// if the mode is not CHANGE, we need to set up a default value, different value triggers interrupt

	// In a RISING interrupt the default value is 0, interrupt is triggered when the pin goes to 1.
	// In a FALLING interrupt the default value is 1, interrupt is triggered when pin goes to 0.
	updateRegBit(offset(REG_ADDR::DEFVAL, isGroupA), pin, (mode==INTR_TYPE::FALLING));

	// enable the pin for interrupt
	updateRegBit(offset(REG_ADDR::GPINTEN, isGroupA), pin, (uint8_t)1);
}


uint8_t MCP23017::getLastInterruptPin()
{
	uint8_t intFlag = 0xFF;

	// try port A
	readRegister(REG_ADDR::INTFA, &intFlag);
	
	for(int i=0;i<8;i++)
		if (bitRead(intFlag, i))
			return i;

	// try port 
	readRegister(REG_ADDR::INTFB, &intFlag);
	
	for(int i=0;i<8;i++)
		if (bitRead(intFlag, i))
			return i + 8;

	return 0xFF;
}

uint8_t MCP23017::getLastInterruptPinValue()
{
	uint8_t intPin = getLastInterruptPin();

	if(intPin!=0xFF)
	{
		uint8_t realIntCaptureReg = offset(REG_ADDR::INTCAP, intPin<8);
		uint8_t bit = intPin % 8;

		uint8_t value;
		readRegister(realIntCaptureReg, &value);
		return (value>>bit) & (0x01);
	}

	return 0xFF;
}

	
