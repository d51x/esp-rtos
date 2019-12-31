#ifndef __LED_CTRL_H__
#define __LED_CTRL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pwm.h"
#include "../rgb/colors.h"

#define LEDCTRL_MIN_FADE_DEALY 20
#define LEDCTRL_MAX_FADE_DEALY 10000

#define LEDCTRL_DEFAULT_SPEED_JUMP 1000
#define LEDCTRL_DEFAULT_SPEED_FADE 40
#define LEDCTRL_DEFAULT_SPEED_WHEEL 500

#define MIN_HSV_V 1
//#define MAX_HSV_V 255
#define MAX_HSV_V 100

#define MIN_HSV_S 1
//#define MAX_HSV_S 255
#define MAX_HSV_S 100

#define COLOR_EFFECTS_MAX 13
static const char *color_effects[COLOR_EFFECTS_MAX]  = {"jump3", "jump7", "jump12", "rndjump7", "rndjump12", 
										"fade3", "fade7", "fade12", "rndfade7", "rndfade12", 
										"wheel", "rnd", "stop"};

typedef struct {
	uint8_t pin;
	uint8_t ch;
	color_e type;
	uint16_t duty;
	uint16_t step;
	uint16_t fade_up_time;
	uint16_t fade_down_time;
	uint8_t	  bright_table;  // указатель на таблицу яркости
} led_ctrl_config_t;

typedef enum {
    JUMP,               // only on 
    FADE,               // fade up & fade down
    RANDOM_JUMP,        
    RANDOM_FADE,
    RANDOM_CB              // all random - color, brightness,
} color_effect_e;


typedef enum {
    UP,
    DOWN
} color_effect_direction_e;

typedef struct {
    uint32_t *colors;
    uint16_t colors_count;
    color_effect_e effect;
    uint32_t fadeup_delay;
    uint32_t fadedown_delay;
    uint8_t saturation;
    uint8_t saturation_max;
    uint8_t brightness;
    uint8_t brightness_max;
} color_effect_config_t;

// ======================== low level control ======================================
void ledctrl_init(uint16_t freq, uint8_t led_cnt, const led_ctrl_config_t *_led_ctrl_cfg);
void ledctrl_set_duty(uint32_t ledc_channel, uint32_t ledc_duty);
void ledctrl_update();
void ledctrl_set_color_duty(color_e type, uint32_t duty);
void ledctrl_delete_active_task();

// ========================= color control ========================================
void ledctrl_set_color_rgb(const color_rgb_t *rgb, uint8_t del);
void ledctrl_set_color_hsv(const color_hsv_t *hsv, uint8_t del);
void ledctrl_set_color_int(uint32_t color, uint8_t del);
void ledctrl_set_color_hex(const char *color, uint8_t del);
void update_brightness(uint8_t brightness);
void update_saturation(uint8_t saturation);
void update_fadeup(uint32_t speed);
void update_fadedown(uint32_t speed);

// ========================= color effect control ================================
void set_color_effect(color_effect_config_t *effect);
esp_err_t handle_color_effect_default_by_id(uint8_t id);
esp_err_t handle_color_effect_by_id(uint8_t id, uint32_t speed);
esp_err_t handle_color_effect_default_by_name(char *effect_name);
esp_err_t handle_color_effect_by_name(char *effect_name, uint32_t speed);

void set_color_effect__jump3(uint32_t speed);
void set_color_effect__jump7(uint32_t speed);
void set_color_effect__jump12(uint32_t speed);
void set_color_effect__rnd_jump7(uint32_t speed);
void set_color_effect__rnd_jump12(uint32_t speed);

void set_color_effect__fade3(uint32_t speed);
void set_color_effect__fade7(uint32_t speed);
void set_color_effect__fade12(uint32_t speed);
void set_color_effect__rnd_fade7(uint32_t speed);
void set_color_effect__rnd_fade12(uint32_t speed);

void set_color_effect__wheel(uint32_t speed);
void set_color_effect__rnd_rnd(void *arg); // random - color and brightness

void set_next_color_effect();
void set_prev_color_effect();
void rgb_lights_off();
// ========================== fade speed control ==============================
void inc_fadeup(uint32_t step);
void dec_fadeup(uint32_t step);
void inc_fadedown(uint32_t step);
void dec_fadedown(uint32_t step);

// ========================= brightness control =============================
void inc_brightness(uint8_t step);
void dec_brightness(uint8_t step);

// ======================== saturation control ==============================
void inc_saturation(uint8_t step);
void dec_saturation(uint8_t step);


#endif