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
#include "mqtt_cl.h"
#include "mqtt_cl_http.h"

#include "sntp.h"
//#include "core/httpd.h"
#include "httpd.h"
//#include "core/http_page.h"
#include "http_page.h"

#include "i2c_http.h"

#include "esp_log.h"

#include "utils.h"

#ifdef CONFIG_COMPONENT_I2C
#include "i2c_http.h"
#endif

#ifdef CONFIG_SENSOR_SHT21
#include "sht21.h"
#include "sht21_http.h"
#endif

#ifdef CONFIG_COMPONENT_PCF8574
#include "pcf8574.h"
#endif

    #ifdef CONFIG_COMPONENT_LCD2004
    #include "lcd2004.h"
    #endif
    
    #ifdef CONFIG_COMPONENT_LCD2004_HTTP
    #include "lcd2004_http.h"
    #endif

#ifdef CONFIG_COMPONENT_MCP23017
    #include "mcp23017.h"
    #include "mcp23017_mqtt.h"
#endif

httpd_handle_t http_server = NULL;

extern void sntp_start();



#endif 