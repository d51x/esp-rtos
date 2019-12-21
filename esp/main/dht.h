#ifndef __DHT_H__
#define __DHT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define DHT_MAXTIMINGS	10000
#define DHT_BREAKTIME	20
#define DHT_MAXCOUNT	32000

typedef enum {
	DHT11,
	DHT22
} dht_type_t;

typedef struct {
	uint8_t pin;
	uint8_t type;
	float temp;
	float hum;
} dht_t;



void dht_init(dht_t *dht);
esp_err_t dht_read(dht_t *dht);

//void dht_task(void *arg);

#endif /* __DSW_H__ */