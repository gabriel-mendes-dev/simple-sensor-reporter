#ifndef __DISTANCE_SENSOR_READER_H__
#define __DISTANCE_SENSOR_READER_H__

#include <stdint.h>
#include <driver/gpio.h>

/* Opaque struct that represents distance sensor obj*/
typedef struct distance_sensor distance_sensor_t;

/*
 * @brief Initialize an object that represents a distance sensor. It will initialize a hardware timer to be used in the measurement
 * @param[in] self reference to pointer that will point to created sensor reader obj, or NULL if fail
 * @param[in] trigger_pin gpio pin number to be used as trigger
 * @param[in] echo_pin gpio pin number to receive echo
 * @return -1 if initialization fails, 0 if succeeds
 */
int32_t distance_sensor_reader_init(distance_sensor_t **self, gpio_num_t trigger_pin, gpio_num_t echo_pin);

/*
 * @brief Deinit (delete) an object that represents a distance sensor
 * @param[in] self reference to pointer that points to previously created object
 * @return void
 */
void distance_sensor_reader_deinit(distance_sensor_t **self);

/*
 * @brief Read distance from sensor, in cm. In this function, RTOS preemptions are disabled because precise time measurement is important
 * @param[in] self address of previously created sensor obj
 * @return 0 if read was successfull, -1 otherwise
 */
int32_t distance_sensor_reader_read(distance_sensor_t *self, uint32_t *distance);

#endif