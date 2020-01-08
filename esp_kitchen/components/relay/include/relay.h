#ifndef _RELAY_H_
#define _RELAY_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"


typedef struct relay relay_t;
typedef void* relay_handle_t;

typedef void (*func_mqtt_send_cb)(const char *topic, const char *payload);

typedef enum {
    RELAY_LEVEL_LOW = 0,    
    RELAY_LEVEL_HIGH = 1,   
} relay_close_level_t;

typedef enum {
    RELAY_STATE_CLOSE = 0,
    RELAY_STATE_OPEN,
} relay_state_t;


struct relay {
	  gpio_num_t pin;
    relay_state_t state;
    relay_close_level_t close_level;
    func_mqtt_send_cb mqtt_send;
};   




/**
  * @brief create relay object.
  *
  * @param io_num - pin 
  * @param level - open level - normal = HIGH, invert = LOW 
  *
  * @return relay_handle_t the handle of the relay created 
  */
relay_handle_t relay_create(gpio_num_t io_num, relay_close_level_t level);

/**
  * @brief set state of relay
  *
  * @param  relay_handle
  * @param  state RELAY_STATUS_CLOSE or RELAY_STATUS_OPEN
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t relay_write(relay_handle_t relay_handle, relay_state_t state);

/**
  * @brief get state of relay
  *
  * @param relay_handle
  *
  * @return state of the relay
  */
relay_state_t relay_read(relay_handle_t relay_handle);

/**
  * @brief free the memory of relay
  *
  * @param  relay_handle
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t relay_delete(relay_handle_t relay_handle);

void relay_add_mqtt_send_cb(relay_handle_t relay_handle, func_mqtt_send_cb cb);

#endif
