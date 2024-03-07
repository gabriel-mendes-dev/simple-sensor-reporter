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

TaskHandle_t sensor_sampler_task_handle;
TaskHandle_t host_pc_comm_task_handle;
QueueHandle_t data_queue_handle;

void sensor_sampler_task(void *arg){
    distance_sensor_t *sensor_ptr;
    ESP_ERROR_CHECK(distance_sensor_reader_init(&sensor_ptr, DISTANCE_SENSOR_TRIGGER_GPIO, DISTANCE_SENSOR_ECHO_GPIO));

    uint32_t distance;
    while(1){
        if(distance_sensor_reader_read(sensor_ptr, &distance) == 0){
            ESP_LOGI(TAG, "Distance = %u cm", distance);
            double value = (double) distance;
            xQueueSend(data_queue_handle, &value, pdMS_TO_TICKS(500));
        } else {
            ESP_LOGE(TAG, "Failed to read sensor");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void host_pc_comm_task(void *arg){
    host_pc_comm_t *host_pc_comm_ptr;
    extern const uint8_t aes_key_start[] asm("_binary_aes_key_start");
    ESP_ERROR_CHECK(host_pc_comm_init(&host_pc_comm_ptr, HOST_PC_COMM_UART, HOST_PC_COMM_UART_TXD, HOST_PC_COMM_UART_RXD, aes_key_start));

    double value;
    while(1){
        if(xQueueReceive(data_queue_handle, &value, pdMS_TO_TICKS(500)) == pdPASS){
            host_pc_comm_send_sensor_data(host_pc_comm_ptr, value);
        }
    }
}

void app_main(void)
{
    data_queue_handle = xQueueCreate(4, sizeof(double));
    assert(data_queue_handle != NULL);
    BaseType_t res = xTaskCreatePinnedToCore(sensor_sampler_task, "sensor-sampler", 2048, NULL, configMAX_PRIORITIES - 20, &sensor_sampler_task_handle, APP_CPU_NUM);
    assert(res == pdPASS);
    res = xTaskCreatePinnedToCore(host_pc_comm_task, "host-pc-comm", 2048, NULL, configMAX_PRIORITIES - 19, &host_pc_comm_task_handle, APP_CPU_NUM);
    assert(res == pdPASS);
    vTaskSuspend(NULL); // suspend this task
}
