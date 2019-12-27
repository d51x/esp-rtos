#ifndef __CORE_H__
#define __CORE_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/event_groups.h"

#define FW_VER "1.10.1"
#define CORE_FW_VER "1.10"

//------------- GLOBAL DEFINES FOR USE MODULES ------------------------------------------

#define GPIO
#define PWM


#define DS18B20z
#define DHTz

#ifdef WIFI
    #define MQTT    // depends on WIFI
    #define SNTP    // depends on WIFI
    #define OTA     // depends on WIFI
#endif
//------------- GLOBAL DEFINES FOR USE MODULES ------------------------------------------


#ifdef DHT
    #include "sensors/dht.h"
#endif

#ifdef DS18B20
    #include "sensors/dsw.h"
#endif

#include "ir_receiver.h"

#define WIFI1


    #define ESP_WIFI_MODE_AP   1
    #ifdef WIFI1    
        #define ESP_WIFI_SSID      "Dminty"
        #define ESP_WIFI_PASS      "110funther26"
    #endif    
    #ifdef WIFI2   
        #define ESP_WIFI_SSID      "Dminty3"
        #define ESP_WIFI_PASS      "110funther26"
    #endif
    #ifdef WIFI3
        #define ESP_WIFI_SSID      "LAPTOP7954"
        #define ESP_WIFI_PASS      "110funther26"
    #endif
    #ifdef WIFI4    
        #define ESP_WIFI_SSID      "MiMix2s"
        #define ESP_WIFI_PASS      "110funther26"
    #endif
    
    #define ESP_WIFI_AP_SSID      "ZppDasdwerfds"
    #define ESP_WIFI_AP_PASS      "110funther26"
    


#define PAGE_MAIN_BUFFER_SIZE  1024*4
#define PAGE_DEFAULT_BUFFER_SIZE  1024*4
#define UPLOAD_BUFFER_SIZE 1024 //512 уже не вытягивает
#define WEB_SERVER_STACK_SIZE  1024*8


//#define MQTT_BROKER_URL "mqtt://192.168.2.63:1883"
//#define MQTT_BROKER_URL "mqtt://d51x.myvnc.com:1883"
#define MQTT_BROKER_URL "mqtt://mqtt.eclipse.org:1883"
#define MQTT_SEND_INTERVAL 60  // sec
#define MQTT_DEVICE "test/esp/"   //%s/%s/        login/hostname
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



// =========================== SENSORS ==========================
#ifdef DHT
  #define DHT2_PIN 0
  #define DHT_PIN 2
  dht_t dht;
  dht_t dht2;
#endif

#ifdef DS18B20
    #define DS18B20_PIN 5
  #define DSW_COUNT 5
  ds18b20_t ds18b20[DSW_COUNT];
#endif

#define GPIO_COUNT 3 

#define LEDCTRL
// ================================ PINS =========================


#define IR_RX_PIN 5


ir_rx_t ir_rx;


#endif