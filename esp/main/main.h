#ifndef __MAIN_H__
#define __MAIN_H__

#include "dht.h"
#include "ir_receiver.h"

#define FW_VER "1.7.1"
#define CORE_FW_VER "1.7"


#define UPLOAD_BUFFER_SIZE 1024 //512 уже не вытягивает

#define DEBUG
//------------- GLOBAL DEFINES FOR USE MODULES ------------------------------------------
#define WIFI

#ifdef WIFI
    #include "wifi.h"
    #define MQTT    // depends on WIFI
    #define SNTP    // depends on WIFI
    #define OTA     // depends on WIFI
#endif

#define GPIO
#define PWM
#define DS18B20
#define DS18B20_PIN 14

#define DHT
#define DHT_PIN 13
dht_t dht;

#define IR_RX_PIN 5
ir_rx_t ir_rx;
//------------- GLOBAL DEFINES END ------------------------------------------

#define WIFI1

#ifdef WIFI
    #define ESP_WIFI_MODE_AP   1
    #ifdef WIFI1    
        #define ESP_WIFI_SSID      "Dminty"
        #define ESP_WIFI_PASS      "110funther26"
    #else
        //#define ESP_WIFI_SSID      "LAPTOP7954"
        //#define ESP_WIFI_SSID      "MiMix2s"
        #define ESP_WIFI_PASS      "110funther26"
        //#define ESP_WIFI_SSID      "TheKey24"
        //#define ESP_WIFI_PASS      "Brainstorm42_24"        
    #endif    
    
    #define ESP_WIFI_AP_SSID      "ZppDasdwerfds"
    #define ESP_WIFI_AP_PASS      "110funther26"
    

    EventGroupHandle_t wifi_event_group;  /* FreeRTOS event group to signal when we are connected*/
    int WIFI_CONNECTED_BIT;  /* The event group allows multiple bits for each event, but we only care about one event - are we connected  to the AP with an IP? */

    EventGroupHandle_t ota_event_group;  /* FreeRTOS event group to signal when we are connected*/
    int OTA_IDLE_BIT;  /* The event group allows multiple bits for each event, but we only care about one event - are we connected  to the AP with an IP? */

#endif // WIFI

#ifdef MQTT
//#define MQTT_BROKER_URL "mqtt://192.168.2.63:1883"
    #define MQTT_BROKER_URL "mqtt://d51x.myvnc.com:1883"
    #define MQTT_SEND_INTERVAL 1000*60
    #define MQTT_DEVICE "test/esp/"   //%s/%s/        login/hostname
    #define MQTT_DEVICE_UPTIME          "uptime"
    #define MQTT_DEVICE_FREEMEM         "freemem"
    #define MQTT_DEVICE_RSSI            "rssi"
    #define MQTT_DEVICE_GPIO_INPUT      "input"
    #define MQTT_DEVICE_GPIO_OUTPUT     "output"
    #define MQTT_DEVICE_DHTT     "dhtt1"
    #define MQTT_DEVICE_DHTH     "dhth1"
    #define MQTT_DEVICE_DSW     "dsw%d"
#endif


//================ USER GPIOS ============================
#define GPIO_COUNT 1 

#define WEB_SERVER_STACK_SIZE  1024*8
//#define WEB_SERVER_MAX_URI_GET_HANDLERS  20

//======================= CONFIG VALUES ===========================
static uint8_t sda = 2;
static uint8_t scl = 0;
static uint16_t ota_upload_buf_size = 2048;

void load_params_from_nvs();

void ir_receiver_task(void *arg);
#endif /* __GLOBAL_H__ */