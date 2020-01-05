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
#include "ipwm.h"
#include "utils.h"
#include "sntp.h"
#include "button.h"
#include "ledcontrol.h"
#include "rgbcontrol.h"
#include "relay.h"


void btn4_press_1_cb();
void btn4_hold_1s_cb();
void btn4_press_2_cb();
void btn4_press_3_cb();

void btn0_press_1_cb();
void btn0_hold_1s_cb();
void btn0_press_2_cb();
void btn0_press_3_cb();

void ir_button1_press(void *arg);
void ir_button2_press(void *arg);
void ir_button3_press(void *arg);

#endif 