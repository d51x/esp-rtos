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


#define FW_VER "1.14.33"
#define CORE_FW_VER "1.14"

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
    #define LED_CTRL_MAX        5
    uint8_t main_led_cnt;

    #define LED_FREQ_HZ         500

    #define LED_CTRL_RED_CH     0
    #define LED_CTRL_GREEN_CH   1 
    #define LED_CTRL_BLUE_CH    2 
    #define LED_CTRL_WHITE_CH   3 
    #define LED_CTRL_WWHITE_CH   4 

    #define LED_CTRL_RED_PIN    15
    #define LED_CTRL_GREEN_PIN  12
    #define LED_CTRL_BLUE_PIN   13
    #define LED_CTRL_WHITE_PIN  2
    #define LED_CTRL_WWHITE_PIN  14
    uint8_t main_led_pins[LED_CTRL_MAX];

    #define RELAY_FAN_PIN       16
    uint8_t relay_fan_pin;
    uint8_t relay_invert;

    //#define PIR_PIN             4
    #define PIR_PIN             1
    uint8_t pirpin;

    #define PIR_TIMER_CALLBACK_DELAY 15

    #define WHITE_LED_FADEUP_DELAY 100
    #define WHITE_LED_FADEDOWN_DELAY 150

    #define IR_RECEIVER_PIN 5
    uint8_t ir_pin;
    uint16_t ir_delay;

	#define IR_RECEIVE_DELAY 100
	
    #define DEFAULT_DARK_TIME_START 23*60
    #define DEFAULT_DARK_TIME_END 4*60

    #define DEFAULT_ADC_LEVEL 600
    #define DEFAULT_ADC_LEVEL_MIN 600
    #define DEFAULT_ADC_LEVEL_MAX 600

    uint16_t adc_lvl;
    uint16_t adc_lvl_min;   // min - значение для включения
    uint16_t adc_lvl_max;   // max - значение для выключения

    typedef enum {
        PIR_MODE_NONE,
        PIR_MODE_SUSNSET,       // закат
        PIR_MODE_DLR,           // датчик освещенности
        PIR_MODE_MIX            // комбинированный
    } pir_mode_t;

    char pir_mode_desc[4][30];

    typedef enum {
        LED_SPEED_DOWN,
        LED_SPEED_UP
    } led_speed_cmd_t;

    typedef enum {
        LED_BRIGHT_DOWN,
        LED_BRIGHT_UP
    } led_bright_cmd_t;

    bool is_pir_enabled; // OPTIONS: взять из настроек - pir вкл / выкл
    pir_mode_t pir_mode;  // OPTIONS: взять из настроек
    bool is_motion;
    //uint32_t count_down_off;         // осталось до выключения
    //uint32_t count_up_motion;        // прошло после начала движения

    bool is_sunset;     // значение получаем по mqtt, 0 - день, 1 - ночь... там же вычисляется is_dark
    bool is_dark; // закат или данные с датчика освещенности
    bool is_white_led_auto; // is_pir_enabled & is_dark 

    //uint8_t pwm_max_light;  // максимальное значение pwm 
    

    uint16_t pir_timer_off_delay; // OPTIONS: взять из настроек
    uint16_t white_led_fadeup_delay; // OPTIONS: взять из настроек
    uint16_t white_led_fadeout_delay; // OPTIONS: взять из настроек
    uint8_t  white_led_max_duty;  // макс яркость, регулируется пультом
    uint8_t  white_led_max_duty_dark;   // макс яркость в ночное время, взять из настроек
    uint16_t dark_time_start;  // начало ночного времени в минутах дня
    uint16_t dark_time_end; // окончание ночного времени в минутах дня
    static TimerHandle_t tmr_cnt = NULL;
    static TimerHandle_t tmr_adc = NULL;
    

    bool get_dark_mode(pir_mode_t mode);
    bool is_night_mode();

#endif