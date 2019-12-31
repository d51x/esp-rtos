#ifndef __RGB_H__
#define __RGB_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"

#define RED_LED_CHANNEL 0
#define GREEN_LED_CHANNEL 1
#define BLUE_LED_CHANNEL 2

typedef struct {
    uint8_t red_ch_pin;
    uint8_t green_ch_pin;
    uint8_t blue_ch_pin;
    uint32_t freq_hz;

} rgb_controller_config_t; 

#endif