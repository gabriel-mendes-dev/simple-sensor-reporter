#include <stdio.h>

#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "distance_sensor_reader.h"
#include "host_pc_comm.h"

#define DISTANCE_SENSOR_TRIGGER_GPIO GPIO_NUM_25
#define DISTANCE_SENSOR_ECHO_GPIO GPIO_NUM_13

#define HOST_PC_COMM_UART UART_NUM_2
#define HOST_PC_COMM_UART_TXD GPIO_NUM_17
#define HOST_PC_COMM_UART_RXD GPIO_NUM_16

static const char *TAG = "main";

void app_main(void)
{
    distance_sensor_t *sensor_ptr;
    host_pc_comm_t *host_pc_comm_ptr;

    ESP_ERROR_CHECK(distance_sensor_reader_init(&sensor_ptr, DISTANCE_SENSOR_TRIGGER_GPIO, DISTANCE_SENSOR_ECHO_GPIO));

    extern const uint8_t aes_key_start[] asm("_binary_aes_key_start");
    ESP_ERROR_CHECK(host_pc_comm_init(&host_pc_comm_ptr, HOST_PC_COMM_UART, HOST_PC_COMM_UART_TXD, HOST_PC_COMM_UART_RXD, aes_key_start));

    uint32_t distance = 0;

    while(1){
        if(distance_sensor_reader_read(sensor_ptr, &distance) == 0){
            ESP_LOGI(TAG, "Distance = %u cm", distance);
            host_pc_comm_send_sensor_data(host_pc_comm_ptr, distance);
        } else {
            ESP_LOGE(TAG, "Failed to read sensor");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));

    }
}
