#ifndef __PWM_H__
#define __PWM_H__


#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "driver/pwm.h"
#include "lwip/apps/sntp.h"

#define PIN_PWM_0 14
//#define PIN_PWM_1 12
#define PIN_PWM_CNT 1

#define PWM_FREQ_HZ    500  // 1000 Hz
#define MAX_DUTY 255  // 8 bit
#define STEP 1
#define FADE_TIME 40

static const uint32_t pwm_pins[PIN_PWM_CNT] = {
    PIN_PWM_0,
    //PIN_PWM_1
};






void pwm_begin(uint16_t freq_hz, uint8_t ch_cnt, const uint32_t *channels);
void pwm_write(uint8_t ch, uint16_t dt);
uint16_t pwm_state(uint8_t ch);

void pwm_task();

#endif