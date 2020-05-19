#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/rmt.h>
#include <ir_tools.h>

#include "core.h"

void ir_rx_task(void *arg)
{
	struct IRremoteContext* irc = (struct IRremoteContext*)arg;
	
    int gpio = irc->gpio;
    rmt_channel_t channel = irc->channel;

    rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(gpio, channel);
    rmt_config(&rmt_rx_config);

    rmt_driver_install(channel, 1000, 0);

    ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)channel);
    ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
    ir_parser_t *ir_parser = ir_parser_rmt_new_nec(&ir_parser_config);

    //get RMT RX ringbuffer
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(channel, &rb);

    // Start receive
    rmt_rx_start(channel, true);


	rmt_item32_t *items = NULL;

	uint32_t addr = 0;
    uint32_t cmd = 0;
    uint32_t length = 0;
    bool repeat = false;

    ESP_LOGI("infrared", "task has started");

    while (!irc->exitFlag)
    {
        items = (rmt_item32_t *) xRingbufferReceive(rb, &length, 1000);

        if (items)
        {
            length /= 4; // one RMT = 4 Bytes
			
            if (ir_parser->input(ir_parser, items, length) == ESP_OK)
            {
                if (ir_parser->get_scan_code(ir_parser, &addr, &cmd, &repeat) == ESP_OK)
                {
                    ESP_LOGI("infrared", "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);

                    if(irc->callback!=NULL)
                    {
                        irc->callback(repeat, addr, cmd);
                    }
					
                }
            }

            //after parsing the data, return spaces to ringbuffer.
            vRingbufferReturnItem(rb, (void *) items);
        }
    }

    ESP_LOGI("infrared", "task is destroying..");

    ir_parser->del(ir_parser);
    rmt_driver_uninstall(channel);
    vTaskDelete(NULL);
}