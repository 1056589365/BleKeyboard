#include "mcp23017.h"
#include "io_extension.h"

xQueueHandle exio_queue = xQueueCreate(10, sizeof(int));
void (*mcp23017_callback)(uint8_t pin, uint8_t val) = nullptr;

MCP23017 m23;

void mcp23017_main(void* arg)
{
    int io_num = -1;
    while(true)
	{
        if(xQueueReceive(exio_queue, &io_num, portMAX_DELAY))
        {
            //logi("GPIO %d interrrupt, val: %d", io_num, gpio_get_level(gpio_num_t(io_num)));
            uint8_t pin = m23.getLastInterruptPin();
            uint8_t val = m23.getLastInterruptPinValue();

            if(pin!=0xFF && val!=0xFF)
            {
				if(mcp23017_callback!=nullptr)
				{
					mcp23017_callback(pin, val);
				}
            }

        }
    }
}

void IRAM_ATTR ioex_isr(void* arg)
{
    xQueueSendFromISR(exio_queue, arg, NULL);
}

void mcp23017_init(int io, void (*callback)(uint8_t pin, uint8_t val))
{
    gpio_config_t io_conf;
	io_conf.pin_bit_mask = 1ULL << io;
	io_conf.intr_type = GPIO_INTR_ANYEDGE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_down_en = gpio_pulldown_t(0);
	io_conf.pull_up_en = gpio_pullup_t(1);
	gpio_config(&io_conf);

	xTaskCreate(mcp23017_main, "mcp23017", 8*1024, NULL, 5, NULL);

	m23.init();
    m23.setupInterrupts(true, false, 0);

	for(int i=0;i<8;i++)
	{
		m23.pinMode(i, true, MCP23017::INPUT);
		m23.pullup(i, true, true);
		m23.setupInterruptPin(i, true, MCP23017::CHANGE);
	}

	for(int i=0;i<8;i++)
	{
		m23.pinMode(i, false, MCP23017::INPUT);
		m23.pullup(i, false, true);
		m23.setupInterruptPin(i, false, MCP23017::CHANGE);
	}

	mcp23017_callback = callback;

	gpio_isr_handler_add(gpio_num_t(io), ioex_isr, (void*)&io);

	while (m23.getLastInterruptPinValue()!=0xFF); // 清空正在等待被处理的中断事件(如果有的话)
}