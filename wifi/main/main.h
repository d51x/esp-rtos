#pragma once

#ifndef __MAIN_H__
#define __MAIN_H__



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs.h"
#include "nvs_flash.h"


#include "wifi.h"
#include "wifi_http.h"
#include "mqtt_cl.h"
#include "mqtt_cl_http.h"
#include "ota_http.h"
#include "sntp.h"
#include "httpd.h"
#include "http_page.h"
#include "esp_log.h"
#include "utils.h"
#include "user.h"

#ifdef CONFIG_COMPONENT_I2C
#include "i2c_http.h"
#endif

#ifdef CONFIG_COMPONENT_RELAY
#include "relay.h"
#include "relay_mqtt.h"
#ifdef CONFIG_RELAY_HTTP
#include "relay_http.h"
#endif
#endif

#ifdef CONFIG_COMPONENT_IR_RECV
#include "irrcv.h"
#ifdef CONFIG_IR_RECV_HTTP
#include "irrcv_http.h"
#endif
#endif

#ifdef CONFIG_SENSOR_SHT21
#include "sht21.h"
#include "sht21_http.h"
#include "sht21_mqtt.h"
#endif

#ifdef CONFIG_COMPONENT_PCF8574
#include "pcf8574.h"
#endif

#ifdef CONFIG_COMPONENT_LCD2004
#include "lcd2004.h"
    #ifdef CONFIG_COMPONENT_LCD2004_HTTP
    #include "lcd2004_http.h"
    #endif
#endif



#ifdef CONFIG_COMPONENT_MCP23017
#include "mcp23017.h"
#include "mcp23017_mqtt.h"
    #ifdef CONFIG_MCP23017_HTTP
    #include "mcp23017_http.h"
    #endif
#endif

#ifdef CONFIG_LED_CONTROLLER
#include "ledcontrol.h"
#include "ledcontrol_mqtt.h"

    #ifdef CONFIG_LED_CONTROL_HTTP
    #include "ledcontrol_http.h"
    #endif
#endif


#ifdef CONFIG_RGB_CONTROLLER
    #include "rgbcontrol.h"
    #include "rgbcontrol_mqtt.h"

    #ifdef CONFIG_RGB_CONTROLLER_HTTP
        #include "rgbcontrol_http.h"
    #endif
    
    #ifdef CONFIG_RGB_EFFECTS
        #include "effects.h"
        
        #ifdef RGB_EFFECTS_HTTP

        #endif
    #endif
#endif
//======================== variable definitions ===================================
extern httpd_handle_t http_server;

extern void sntp_start();

    #ifdef CONFIG_COMPONENT_IR_RECV
        irrcv_handle_t ir_rx;
    #endif

    #ifdef CONFIG_COMPONENT_RELAY
    relay_handle_t relay_h;
    // relay_handle_t relay_red_h;
    // relay_handle_t relay_blue_h;
    // relay_handle_t relay_green_h;
    #endif

    #ifdef CONFIG_COMPONENT_MCP23017
        mcp23017_handle_t mcp23017_h;
    #endif
    
    #ifdef CONFIG_COMPONENT_PCF8574
        pcf8574_handle_t pcf8574_h;
    #endif    


    #ifdef CONFIG_LED_CONTROLLER
    ledcontrol_channel_t *ch_red;
    ledcontrol_channel_t *ch_green;
    ledcontrol_channel_t *ch_blue;
    ledcontrol_t *ledc_h;
    ledcontrol_t *ledc;

        #ifdef CONFIG_RGB_CONTROLLER
        rgbcontrol_t *rgb_ledc;

            #ifdef CONFIG_RGB_EFFECTS
                effects_t* effects;
            #endif    
        #endif
    #endif

extern void sntp_start();

void initialize_modules();
void initialize_modules_mqtt();
void initialize_modules_http(httpd_handle_t _server);

#endif 