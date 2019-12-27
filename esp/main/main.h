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
#include "core/wifi.h"
#include "core/httpd.h"
#include "core/mqtt.h"
#include "core/core.h"
#include "core/gpio.h"
#include "core/pwm.h"
#include "core/utils.h"
#include "rgb/colors.h"
#include "core/led_ctrl.h"
#include "core/button.h"





uint8_t pressed_count = 0;
TimerHandle_t tmr_btn_pressed;
QueueHandle_t xColorEffectQueue;

void color_effect_message_task(void *arg);
void vTmrPressCntCb( TimerHandle_t xTimer );
void load_params_from_nvs();

void ir_receiver_task(void *arg);

void btn_short_press_cb(void *arg);
void btn_rls_cb(void *arg);
void btn_press_2sec_cb(void *arg);
void btn_press_3sec_cb(void *arg);
void btn_press_6sec_cb(void *arg);

#endif 