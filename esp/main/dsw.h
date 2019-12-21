#ifndef __DSW_H__
#define __DSW_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"


#include "esp_log.h"


#include "onewire.h"

//#define DS18B20_PIN 14


typedef struct {
    float temp;
    uint8_t addr[8];
} ds18b20_t;

#define DSW_COUNT 5
ds18b20_t ds18b20[DSW_COUNT];
uint16_t ds18b20_total_crc_error;

void ds18b20_init(uint8_t pin);
esp_err_t ds18b20_getTemp(const uint8_t *_addr, float *temp);
//void ds18b20_search_task(void *arg);
//void ds18b20_get_temp_task(void *arg);
#endif /* __DSW_H__ */