#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "uart_input.h"

#define logi(...) ESP_LOGI("UartInput", ##__VA_ARGS__)

#define buf_size 1024

QueueHandle_t uart_queue;
void (*uart_input_callback)(char* data, size_t len) = NULL;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(1024);

    while(true) 
    {
        if(xQueueReceive(uart_queue, (void * )&event, (portTickType)portMAX_DELAY)) 
        {
            bzero(dtmp, 1024);
            
            switch(event.type) 
            {
                case UART_DATA:
                    // logi("[UART DATA]: %d: ", event.size);
                    uart_read_bytes(UART_NUM_0, dtmp, event.size, portMAX_DELAY);
                    //uart_write_bytes(UART_NUM_0, (const char*) dtmp, event.size);
                    
                    if(uart_input_callback!=NULL)
                    {
                        uart_input_callback((char*) dtmp, event.size);
                    }

                    break;
                default:
                    logi("uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void uart_input_init(void (*callback)(char* data, size_t len))
{
    uart_driver_install(UART_NUM_0, buf_size * 2, buf_size * 2, 20, &uart_queue, 0);

    uart_input_callback = callback;

    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 10, NULL);
}