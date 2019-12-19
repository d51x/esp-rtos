#ifndef __IR_RECEIVER_H__
#define __IR_RECEIVER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/ir_rx.h"

// TSOP4838

#define IR_RX_PIN 4
uint32_t last_code;

void ir_receiver_task(void *arg);

#endif