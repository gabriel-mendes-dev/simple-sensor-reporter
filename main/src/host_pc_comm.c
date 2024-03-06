#include "host_pc_comm.h"

#include <esp_log.h>

#define HOST_PC_COMM_UART_RX_BUFFER_SIZE    256
#define HOST_PC_COMM_UART_BAUD_RATE 115200

typedef struct host_pc_comm {
    uart_port_t uart_port_num;
    gpio_num_t tx_gpio_pin;
    gpio_num_t rx_gpio_pin;
} host_pc_comm_t;

static const char *TAG = "host-pc-comm";


int32_t host_pc_comm_init(host_pc_comm_t **self, uart_port_t uart_port_num, gpio_num_t tx_gpio_pin, gpio_num_t rx_gpio_pin){
     *self = (host_pc_comm_t *) malloc(sizeof(host_pc_comm_t));
    if(*self == NULL){
        return -1;
    }
    (*self)->uart_port_num = uart_port_num;
    (*self)->tx_gpio_pin = tx_gpio_pin;
    (*self)->rx_gpio_pin = rx_gpio_pin;

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = HOST_PC_COMM_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

    esp_err_t res;

    res = uart_driver_install((*self)->uart_port_num , HOST_PC_COMM_UART_RX_BUFFER_SIZE, 0, 0, NULL, intr_alloc_flags);
    res |= uart_param_config((*self)->uart_port_num , &uart_config);
    res |= uart_set_pin((*self)->uart_port_num , (*self)->tx_gpio_pin , (*self)->rx_gpio_pin , -1, -1);

    if(res != ESP_OK){
        return -1;
    }

    return 0;
}

int32_t host_pc_comm_deinit(host_pc_comm_t **self){
    if((*self) == NULL){
        return -1;
    }
    esp_err_t res = uart_driver_delete((*self)->uart_port_num);
    if(res != ESP_OK){
        return -1;
    }
    free(*self);
    *self = NULL;
    return 0;
}

int32_t host_pc_comm_send_sensor_data(host_pc_comm_t *self, double value){
    ESP_LOGI(TAG, "sending data %f", value);
    uart_write_bytes(self->uart_port_num, &value, sizeof(double));
    return 0;
}