#include "distance_sensor_reader.h"

#include <stdio.h>

#include <driver/gpio.h>
#include <esp_log.h>

static const char *TAG = "dist-sensor";

typedef struct distance_sensor {
    gpio_num_t trigger_pin;
    gpio_num_t echo_pin;
} distance_sensor_t;

int32_t distance_sensor_reader_init(distance_sensor_t **self, gpio_num_t trigger_pin, gpio_num_t echo_pin){
    *self = (distance_sensor_t *) malloc(sizeof(distance_sensor_t));
    if(*self == NULL){
        return -1;
    }
    (*self)->trigger_pin = trigger_pin;
    (*self)->echo_pin = echo_pin;

    esp_err_t res;

    res = gpio_set_direction((*self)->trigger_pin, GPIO_MODE_OUTPUT);
    res |= gpio_set_direction((*self)->echo_pin, GPIO_MODE_INPUT);

    if(res != ESP_OK){
        return -1;
    }

    return 0;
}

void distance_sensor_reader_deinit(distance_sensor_t **self){
    free(*self);
    *self = NULL;
}

int32_t distance_sensor_reader_read(distance_sensor_t *self, uint32_t *distance){
    ESP_LOGI(TAG, "Reading sensor\n");
    *distance = 10;
    return 0;
}