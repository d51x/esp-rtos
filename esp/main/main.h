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
#include "button.h"





QueueHandle_t xColorEffectQueue;

void color_effect_message_task(void *arg);
void load_params_from_nvs();

void ir_receiver_task(void *arg);

void press_1_cb();
void press_2_cb();
void press_3_cb();


#endif 