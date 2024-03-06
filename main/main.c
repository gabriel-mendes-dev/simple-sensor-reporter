#include <stdio.h>

#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "distance_sensor_reader.h"

#define DISTANCE_SENSOR_TRIGGER_GPIO GPIO_NUM_12
#define DISTANCE_SENSOR_ECHO_GPIO GPIO_NUM_13

static const char *TAG = "main";

void app_main(void)
{
    distance_sensor_t *sensor_ptr;
    ESP_ERROR_CHECK(distance_sensor_reader_init(&sensor_ptr, DISTANCE_SENSOR_TRIGGER_GPIO, DISTANCE_SENSOR_ECHO_GPIO));

    uint32_t distance = 0;

    while(1){
        distance_sensor_reader_read(sensor_ptr, &distance);
        ESP_LOGI(TAG, "Distance = %u\n", distance);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
