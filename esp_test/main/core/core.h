#ifndef __CORE_H__
#define __CORE_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/event_groups.h"
#include "button.h"
#include "relay.h"
#include "ledcontrol.h"
#include "rgbcontrol.h"

#define FW_VER "1.12.1"
#define CORE_FW_VER "1.12"

ledcontrol_t *ledc;
// rgbcontrol_t *rgb_ledc;
// effects_t* effects;
relay_handle_t relay02, relay12, relay13, relay15; 

relay_handle_t relays[4];

//------------- GLOBAL DEFINES FOR USE MODULES ------------------------------------------

#define PWM



    #define MQTT    // depends on WIFI
    #define SNTP    // depends on WIFI
    #define OTA     // depends on WIFI

//------------- GLOBAL DEFINES FOR USE MODULES ------------------------------------------

    #define ESP_WIFI_MODE_AP   1
    
        #define ESP_WIFI_SSID      "Dminty"
        #define ESP_WIFI_PASS      "110funther26"
    

    
    #define ESP_WIFI_AP_SSID      "ZppDasdwerfds"
    #define ESP_WIFI_AP_PASS      "110funther26"
    


#define PAGE_MAIN_BUFFER_SIZE  1024*4
#define PAGE_DEFAULT_BUFFER_SIZE  1024*4
#define UPLOAD_BUFFER_SIZE 1024 //512 уже не вытягивает
#define WEB_SERVER_STACK_SIZE  1024*8


#define MQTT_BROKER_URL "mqtt://192.168.2.63:1883"
//#define MQTT_BROKER_URL "mqtt://d51x.myvnc.com:1883"
//#define MQTT_BROKER_URL "mqtt://mqtt.eclipse.org:1883"
#define MQTT_SEND_INTERVAL 60  // sec
#define MQTT_DEVICE "esp/test/"   //%s/%s/        login/hostname
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

EventGroupHandle_t wifi_event_group;  /* FreeRTOS event group to signal when we are connected*/
int WIFI_CONNECTED_BIT;  /* The event group allows multiple bits for each event, but we only care about one event - are we connected  to the AP with an IP? */


#endif