#ifndef __MQTT_H__
#define __MQTT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#include "esp_log.h"
#include "esp_system.h"
//#include "esp_wifi.h"
//#include "esp_wifi_types.h"
#include "mqtt_client.h" 
#include "nvsparam.h" 

#include "wifi.h"
#include "core.h"
#include "utils.h"
#include "rgbcontrol.h"



typedef struct {
    char broker_url[50];
    char login[20];
    char passw[20];
    uint32_t send_interval;
    uint8_t enabled;
} mqtt_config_t;


uint32_t mqtt_error_count;
uint32_t mqtt_reconnects;
uint8_t mqtt_state;



void mqtt_start(); /*const char *broker_url, uint16_t send_interval*/
void mqtt_stop();
void mqtt_set_device_name(const char *dev_name, const char *login);


void mqtt_get_current_config(mqtt_config_t *cfg);

void mqtt_subscribe_topics();
void mqtt_publish_device_uptime();
void mqtt_publish_device_freemem();
void mqtt_publish_device_rssi();

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);


void mqtt_publish_all_task(void *arg);
void mqtt_load_data_from_nvs(mqtt_config_t *cfg);
void mqtt_save_data_to_nvs(const mqtt_config_t *cfg);


void mqtt_extern_publish(const char *topic, const char *payload);

 void mqtt_publish_fan_state();
void mqtt_publish_effect_name();
void mqtt_publish_effect_id();
void mqtt_publish_ledc_duty(uint8_t channel);
void mqtt_publish_adc_thld();
void mqtt_publish_adc_thld_min();
void mqtt_publish_adc_thld_max();
void mqtt_publish_dark_time_start();
void mqtt_publish_dark_time_end();

#endif /* __MQTT_H__ */