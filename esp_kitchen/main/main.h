#ifndef __MAIN_H__
#define __MAIN_H__



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
//#include "freertos/task.h"
//#include "freertos/queue.h"
#include "esp_http_server.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "core/httpd.h"
#include "core/mqtt.h"
#include "core/core.h"
#include "sntp.h"
#include "ir_buttons.h"


//===== pir callbacks ========
void pir_high_cb(void *arg);
void pir_low_cb(void *arg);
void pir_timer_low_cb(void *arg);
void white_led_smooth_off();
void white_led_smooth_on();

void adc_cb(xTimerHandle tmr);

void mqtt_send_gpio(const char *topic, const char *payload);
void mqtt_send_pir();

void load_params();

#endif 