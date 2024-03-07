#ifndef __HOST_PC_COMM_H__
#define __HOST_PC_COMM_H__

#include <driver/uart.h>
#include <driver/gpio.h>

typedef struct host_pc_comm host_pc_comm_t;

/*
 * @brief Initialize an object that communicates with a host pc.
 * @param[in] self reference to pointer that will point to created obj, or NULL if fail
 * @param[in] tx_gpio_pin gpio pin number to be used as tx
 * @param[in] rx_gpio_pin gpio pin number to be used as rx
 * @param[in] aes_key aes key to be used in the communication (32 bytes)
 * @return -1 if initialization fails, 0 if succeeds
 */
int32_t host_pc_comm_init(host_pc_comm_t **self, uart_port_t uart_port_num, gpio_num_t tx_gpio_pin, gpio_num_t rx_gpio_pin, const uint8_t *aes_key);

/*
 * @brief Deinit (delete) an object that communicates with a host pc
 * @param[in] self reference to pointer that points to previously created object
 * @return void
 */
int32_t host_pc_comm_deinit(host_pc_comm_t **self);

/*
 * @brief Send data to the Host PC
 * @param[in] self address of previously created communication obj
 * @return 0 if read was successfull, -1 otherwise
 */
int32_t host_pc_comm_send_sensor_data(host_pc_comm_t *self, double value);

#endif