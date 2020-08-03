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
#include "sntp.h"
#include "core/httpd.h"
#include "core/http_page.h"

#include "i2c_http.h"

#include "esp_log.h"

#include "utils.h"

httpd_handle_t http_server = NULL;

extern void sntp_start();



#endif 