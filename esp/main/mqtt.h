#ifndef __MQTT_H__
#define __MQTT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#include "esp_log.h"
//#include "esp_wifi.h"
//#include "esp_wifi_types.h"
#include "mqtt_client.h" 

#include "main.h"
#include "utils.h"
#include "gpio_utils.h"
#include "dht.h"
#include "dsw.h"

uint32_t mqtt_error_count;
uint32_t mqtt_reconnects;
uint8_t mqtt_state;

typedef struct {
    uint8_t pin;
    uint8_t state;
} mqtt_gpio_t;

void mqtt_subscribe_topics();
void mqtt_publish_gpio_input_state(const gpio_t *gpio);
void mqtt_publish_gpio_output_state(uint8_t pin, uint8_t state);
void mqtt_publish_device_uptime();
void mqtt_publish_device_freemem();
void mqtt_publish_device_rssi();
#ifdef DHT
void mqtt_publish_dhtt();
void mqtt_publish_dhth();
#endif

void mqtt_publish_dsw();

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
void mqtt_start();

void mqtt_publish_all_task(void *arg);
void mqtt_publish_gpio_state_task(void *arg);


#endif /* __MQTT_H__ */