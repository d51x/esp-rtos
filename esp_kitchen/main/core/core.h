#ifndef __CORE_H__
#define __CORE_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/event_groups.h"
#include "relay.h"
#include "ledcontrol.h"
#include "rgbcontrol.h"
#include "effects.h"
#include "irrcv.h"
#include "pir.h"


#define FW_VER "1.12.1"
#define CORE_FW_VER "1.12"

ledcontrol_t *ledc;
rgbcontrol_t *rgb_ledc;
effects_t* effects;
relay_handle_t relay_fan_h; 
pir_handle_t pir_h;
irrcv_handle_t ir_rx_h;

#define MQTT  
#define SNTP  
#define OTA   

#define PAGE_MAIN_BUFFER_SIZE  1024*4
#define PAGE_DEFAULT_BUFFER_SIZE  1024*4
#define UPLOAD_BUFFER_SIZE 1024 //512 уже не вытягивает
#define WEB_SERVER_STACK_SIZE  1024*8


#define MQTT_BROKER_URL "mqtt://192.168.2.63:1883"
//#define MQTT_BROKER_URL "mqtt://d51x.myvnc.com:1883"
//#define MQTT_BROKER_URL "mqtt://mqtt.eclipse.org:1883"
#define MQTT_SEND_INTERVAL 60  // sec
#define MQTT_DEVICE "dacha/esp-kitchen/"   //%s/%s/        login/hostname
#define MQTT_DEVICE_UPTIME          "uptime"
#define MQTT_DEVICE_FREEMEM         "freemem"
#define MQTT_DEVICE_RSSI            "rssi"
#define MQTT_DEVICE_GPIO_INPUT      "input"
#define MQTT_DEVICE_GPIO_OUTPUT     "output"
#define MQTT_DEVICE_DHTT     "dhtt1"
#define MQTT_DEVICE_DHTH     "dhth1"
#define MQTT_DEVICE_DSW     "dsw%d"

EventGroupHandle_t ota_event_group;  /* FreeRTOS event group to signal when we are connected*/
int OTA_IDLE_BIT;  /* The event group allows multiple bits for each event, but we only care about one event - are we connected  to the AP with an IP? */


    #define LED_CTRL_CNT        4
    #define LED_FREQ_HZ         500

    #define LED_CTRL_RED_CH     0
    #define LED_CTRL_GREEN_CH   1 
    #define LED_CTRL_BLUE_CH    2 
    #define LED_CTRL_WHITE_CH   3 

    #define LED_CTRL_RED_PIN    15
    #define LED_CTRL_GREEN_PIN  12
    #define LED_CTRL_BLUE_PIN   13
    #define LED_CTRL_WHITE_PIN  2

    #define RELAY_FAN_PIN       16

    #define PIR_PIN             4
    #define PIR_TIMER_CALLBACK_DELAY 10

    #define IR_RECEIVER_PIN 5
	#define IR_RECEIVE_DELAY 100
	

#endif