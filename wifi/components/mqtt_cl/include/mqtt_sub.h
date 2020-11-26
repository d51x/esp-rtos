#pragma once

#ifndef __MQTT_SUB_H__
#define __MQTT_SUB_H__

#include <string.h>
#include "mqtt_cl.h"

#define MQTT_SUBSCRIBER_MAX_BASE_TOPICS 5
#define MQTT_SUBSCRIBER_MAX_END_POINTS 10 * MQTT_SUBSCRIBER_MAX_BASE_TOPICS  // общее кол-во endpoints на все base topics

#define MQTT_SUBSCRIBER_BASE_TOPIC_MAX_LENGTH 36
#define MQTT_SUBSCRIBER_END_POINT_MAX_LENGTH 28
#define MQTT_SUBSCRIBER_END_POINT_VALUE_MAX_LENGTH 16

typedef struct {
    char base[MQTT_SUBSCRIBER_BASE_TOPIC_MAX_LENGTH]; // [MQTT_SUBSCRIBER_BASE_TOPIC_MAX_LENGTH]
    uint8_t id;
} mqtt_sub_base_topic_t;

typedef struct {
    char endpoint[MQTT_SUBSCRIBER_END_POINT_MAX_LENGTH]; // [MQTT_SUBSCRIBER_END_POINT_MAX_LENGTH] max length
    uint8_t base_id;
    uint8_t id;
    //char *name; //[12] max, param name
} mqtt_sub_endpoint_t;

typedef struct {
    uint8_t id;
    char value[MQTT_SUBSCRIBER_END_POINT_VALUE_MAX_LENGTH]; // значение 
} mqtt_sub_endpoint_value_t;

void mqtt_subscriber_init(); 

// endpoints - строка endpoint's через ";", например "mcpgpio0;mpgpio14;mcpgpio15"
esp_err_t mqtt_subscriber_add(const char* base_topic, const char *endpoints); 

// удаляет base topic и все его endpoints
esp_err_t mqtt_subscriber_del(const char* base_topic); 

void mqtt_subscriber_clear_all();

//TODO:
// 1. подписка на топики по base_topic + endpoint
    // при удалении топика делать отписку

// +++++++2. callback получения данных обновляет значение endtopic_values[]
// 3. вывод на главной
// 4. страница управления - поля, кнопки, обработчик get/post запросов

// отобразить настройки на веб странице
// void mqtt_subscriber_http_print_page();

// get обработчик при сохранении настроек
// void mqtt_subscriber_http_get_handler();

#endif