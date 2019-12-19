#ifndef __GPIO_UTILS_H__
#define __GPIO_UTILS_H__

#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"

#include "global.h"
#include "utils.h"

#define GPIO_CNT GPIO_PIN_COUNT
#define MAX_GPIO 255


typedef enum {
    OFF,
    ON
} gpio_state_t;

typedef enum {
    LOW_LEVEL,  // invert
    HIGH_LEVEL // normal
} gpio_level_t;

typedef struct {
    uint8_t pin;
    gpio_state_t state;
    gpio_mode_t mode;
    gpio_level_t level;
    gpio_int_type_t intr_type;
    //gpio_pullup_t pullup_en;
    //gpio_pulldown_t pulldown_en;
} gpio_t;

gpio_t gpio[GPIO_CNT];  // исключить 6,7,8,9,10,11

void setup_gpio(const gpio_t *gp);
void init_gpio(uint8_t pin, gpio_mode_t mode, gpio_level_t level, gpio_state_t state);
void reset_gpio(gpio_t *gp);
void reset_all_gpio(gpio_t *gpio, uint8_t cnt);

void init_gpios();
//static void setup_gpio(const gpio_t *gpio);

esp_err_t set_gpio(uint8_t gpio, uint8_t _st, uint8_t mqtt_send_en);
uint8_t get_gpio_state(uint8_t pin, uint8_t level);


#endif