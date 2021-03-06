#ifndef __PIR_H__
#define __PIR_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"


typedef struct pir pir_t;
typedef struct pir_conf pir_conf_t;
typedef void* pir_handle_t;

typedef void (*func_cb)(void *arg);

typedef enum {
    PIR_LEVEL_DISABLE,
	PIR_LEVEL_LOW,    
    PIR_LEVEL_HIGH,   
	PIR_LEVEL_ANY
} pir_active_level_t;

typedef enum {
	PIR_ISR,
	PIR_POLL
} pir_read_t;

typedef enum {
	PIR_DISABLED,
	PIR_ENABLED
} pir_status_t;

struct pir_conf {
    uint8_t pin;
	pir_active_level_t active_level;
	pir_read_t type;
	
	void *cb_high_ctx;
	func_cb high_cb;			// callback for start interrupt
	
	void *cb_low_ctx;
	func_cb low_cb;
	int interval;
	void *cb_tmr_ctx;
	func_cb tmr_cb;		// callback for end timer after start interrupt


};

struct pir {
    uint8_t pin;
	pir_status_t status;
	pir_active_level_t active_level;
	pir_read_t type;
	
	void *cb_high_ctx;
	func_cb high_cb;			// callback for start interrupt
	
	void *cb_low_ctx;
	func_cb low_cb;			// callback for start interrupt
	
	QueueHandle_t taskq;
	QueueHandle_t argq;
	TaskHandle_t task;
	TaskHandle_t task_poll;

	TimerHandle_t tmr;
	func_cb task_cb;
	TickType_t interval;
	void *cb_tmr_ctx;
	func_cb tmr_cb;		// callback for end timer after start interrupt

	func_cb enable;
	func_cb disable;	
};

pir_handle_t pir_init(pir_conf_t pir_conf);

#endif 