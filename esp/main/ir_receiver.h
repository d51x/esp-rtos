#ifndef __IR_RECEIVER_H__
#define __IR_RECEIVER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/ir_rx.h"

// TSOP4838
void ir_receiver_init(uint8_t pin);
esp_err_t ir_receiver_get(uint32_t *code);

#endif