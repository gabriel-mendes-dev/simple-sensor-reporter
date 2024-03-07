#include "distance_sensor_reader.h"

#include <stdio.h>
#include <inttypes.h>

#include <driver/gpio.h>
#include <driver/timer.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// The conversion constant was calculated considering the sound velocity in the air as 343m/s
// The distance is x [cm] = t[us] / K [us/cm]
#define TIME_ON_AIR_TO_DISTANCE_IN_CM_CONSTANT (58)
#define START_ECHO_TIMEOUT_US (6000)
#define ECHO_DURATION_TIMEOUT (500*TIME_ON_AIR_TO_DISTANCE_IN_CM_CONSTANT) // time for 5 meters distance

typedef struct distance_sensor {
    gpio_num_t trigger_pin;
    gpio_num_t echo_pin;
} distance_sensor_t;

static const char *TAG = "dist-sensor";
static portMUX_TYPE port_critical_section_mutex = portMUX_INITIALIZER_UNLOCKED;

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

    timer_config_t timer_config = {
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = TIMER_ALARM_DIS,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .divider = TIMER_BASE_CLK/(1000000) // each timer tick will be 1us
    };

    res |= timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);

    if(res != ESP_OK){
        distance_sensor_reader_deinit(self);
        return -1;
    }

    return 0;
}

void distance_sensor_reader_deinit(distance_sensor_t **self){
    free(*self);
    *self = NULL;
}

int32_t distance_sensor_reader_read(distance_sensor_t *self, uint32_t *distance){
    uint64_t start_time, end_time;
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

    portENTER_CRITICAL(&port_critical_section_mutex);
    gpio_set_level(self->trigger_pin, 1);
    ets_delay_us(10);
    gpio_set_level(self->trigger_pin, 0);

    if(timer_start(TIMER_GROUP_0, TIMER_0) != ESP_OK){
        portEXIT_CRITICAL(&port_critical_section_mutex);
        return -1;
    }


    while (!gpio_get_level(self->echo_pin))
    {
        if(timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &start_time)){
            portEXIT_CRITICAL(&port_critical_section_mutex);
            return -1;
        }
        if (start_time > START_ECHO_TIMEOUT_US){
            portEXIT_CRITICAL(&port_critical_section_mutex);
            return -1;
        }
    }

    while(gpio_get_level(self->echo_pin)){
        if(timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &end_time)){
            portEXIT_CRITICAL(&port_critical_section_mutex);
            return -1;
        }
        if((end_time - start_time) > ECHO_DURATION_TIMEOUT){
            portEXIT_CRITICAL(&port_critical_section_mutex);
            return -1;
        }
    }
    ESP_LOGD(TAG, "%"PRIu64" us", end_time-start_time);
    portEXIT_CRITICAL(&port_critical_section_mutex);

    *distance = (end_time - start_time) / TIME_ON_AIR_TO_DISTANCE_IN_CM_CONSTANT;

    return 0;
}