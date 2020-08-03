#ifndef __MQTT_CL_H__
#define __MQTT_CL_H__

//#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "mqtt_client.h" 
#include "nvsparam.h"
#include "wifi.h"
#include "utils.h"


#define MQTT_BROKER_URL "mqtt://192.168.2.63:1883"
#define MQTT_SEND_INTERVAL 60  // sec

#define MQTT_DEVICE_UPTIME          "uptime"
#define MQTT_DEVICE_FREEMEM         "freemem"
#define MQTT_DEVICE_RSSI            "rssi"

#define MQTT_CFG_LOGIN_LENGTH 16

typedef struct {
    char broker_url[32];
    char login[MQTT_CFG_LOGIN_LENGTH];
    char password[16];
    uint16_t send_interval;
    uint8_t enabled;
} mqtt_config_t;



uint32_t mqtt_error_count;
uint32_t mqtt_reconnects;
uint8_t mqtt_state;

typedef void (* func_mqtt_send_cb)(char *payload);  
typedef void (* func_mqtt_recv_cb)(const char *payload);  
// typedef void (*func_mqtt_send_cb)(const char *topic, const char *payload);

#define TOPIC_END_NAME_LENGTH 10

typedef struct {
    char topic[TOPIC_END_NAME_LENGTH];
    func_mqtt_send_cb fn_cb;
} mqtt_send_t;
#define MQTT_SEND_CB 5

typedef struct {
    char topic[TOPIC_END_NAME_LENGTH];
    func_mqtt_recv_cb fn_cb;
} mqtt_recv_t;
#define MQTT_RECV_CB 5

void mqtt_init(); /*const char *broker_url, uint16_t send_interval*/
void mqtt_deinit();
void mqtt_load_cfg(mqtt_config_t *cfg);
void mqtt_get_cfg(mqtt_config_t *cfg);
void mqtt_save_cfg(const mqtt_config_t *cfg);
void mqtt_set_device_name(const char *dev_name);
void mqtt_publish(const char *_topic, const char *payload);
// просто отправить данные
// mqtt publish - topic w/o device_name and login + data



// ФУНКЦИИ БЕЗ ПРОВЕРОК И ДИНАМИЧЕСКОГО ВЫДЕЛЕНИЯ ПАМЯТИ
// НУЖНО ИЗМЕНИТЬ ЗНАЧЕНИЯ MQTT_SEND_CB и MQTT_RECV_CB, если мало
// название топика не длинее TOPIC_END_NAME_LENGTH - название без учета  "login/hostname/""
// зарегистрировать функцию колбека, которая будет вызвана при периодической отправки данных с настроенным интервалом
void mqtt_add_periodic_publish_callback( const char *topic, func_mqtt_send_cb fn_cb);

// зарегистрировать функцию колбека, которая будет вызвана при получении данных в указанном топике
void mqtt_add_receive_callback( const char *topic, func_mqtt_recv_cb fn_cb);  

#endif /* __MQTT_H__ */