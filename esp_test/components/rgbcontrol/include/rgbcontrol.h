#ifndef __RGBCONTROL_H__
#define __RGBCONTROL_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "driver/pwm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "utils.h"
#include "ledcontrol.h"
#include "colors.h"
#include "effects.h"
#include "http_utils.h"

#define RGB_DEFAULT_FADE 1000
#define RGB_DEFAULT_FADEUP 40
#define RGB_DEFAULT_FADEDOWN 40

#define RGB_URI "/colors"

typedef void *rgbcontrol_handle_t;      // rgbcontrol object
typedef struct rgbcontrol rgbcontrol_t;

typedef void (* rgbcontrol_set_color_hsv_f)(color_hsv_t hsv);  
typedef void (* rgbcontrol_set_color_rgb_f)(color_rgb_t rgb);  
typedef void (* rgbcontrol_set_color_int_f)(uint32_t color32);  
typedef void (* rgbcontrol_set_color_hex_f)(const char *hex);  

typedef void (* rgbcontrol_set_brightness_f)(int8_t value);
typedef void (* rgbcontrol_fade_brightness_f)(int8_t brightness_from, int8_t brightness_to, int16_t brightness_delay);
typedef void (* rgbcontrol_inc_brightness_f)(int8_t step);
typedef void (* rgbcontrol_dec_brightness_f)(int8_t step);

typedef void (* rgbcontrol_set_saturation_f)(int8_t value);
typedef void (* rgbcontrol_fade_saturation_f)(int8_t saturation_from, int8_t saturation_to, int16_t saturation_delay);
typedef void (* rgbcontrol_inc_saturation_f)(int8_t step);
typedef void (* rgbcontrol_dec_saturation_f)(int8_t step);
typedef void (* rgbcontrol_set_effects_f)(void *effects);
typedef void (* rgbcontrol_html_data_f)(char *data);

struct rgbcontrol {
	color_hsv_t hsv;
    ledcontrol_t *ledc;
	ledcontrol_channel_t red; 
	ledcontrol_channel_t green; 
	ledcontrol_channel_t blue; 
	int fade_delay;
    int fadeup_delay;
    int fadedown_delay;
    int effect_id; // TODO: переделать на указатель 

    void *effects;
	// указатели на функции
	rgbcontrol_set_color_hsv_f      set_color_hsv;
	rgbcontrol_set_color_rgb_f      set_color_rgb;
	rgbcontrol_set_color_int_f      set_color_int;
	rgbcontrol_set_color_hex_f      set_color_hex;

    rgbcontrol_set_brightness_f     set_brightness;
    rgbcontrol_fade_brightness_f    fade_brightness;
    rgbcontrol_inc_brightness_f     inc_brightness;
    rgbcontrol_dec_brightness_f     dec_brightness;

    rgbcontrol_set_saturation_f     set_saturation;
    rgbcontrol_fade_saturation_f    fade_saturation;
    rgbcontrol_inc_saturation_f     inc_saturation;
    rgbcontrol_dec_saturation_f     dec_saturation;
    
    rgbcontrol_set_effects_f     set_effects;

    rgbcontrol_html_data_f print_html_data;
    // callback for parse get request
    char uri[20];
    httpd_uri_func http_get_handler; // 
};

// здесь укажем только внешние функции
// создать объект rgbcontrol
rgbcontrol_t* rgbcontrol_init(ledcontrol_t *ledc, ledcontrol_channel_t *red, ledcontrol_channel_t *green, ledcontrol_channel_t *blue);

/*

At first, create led_controller object
    ledcontrol_t *ledc;
    ledcontrol_t* ledc_h = ledcontrol_create(LED_FREQ_HZ, LED_CTRL_CNT);
    ledc = (ledcontrol_t *)ledc_h;

then register rgb-channels
    ledc->register_channel(ch_red);
    ledc->register_channel(ch_green);
    ledc->register_channel(ch_blue);

    where ledcontrol_channel_t ch_red, ch_green, ch_blue (with pin and channel)

Initialize led_Controller object
    ledc->init();

then initialize rgb_controller 
    rgbcontrol_t *rgb_ledc;
    rgb_ledc = rgbcontrol_init(ledc, ch_red, ch_green, ch_blue);

now you can change color and make animations with led
    rgb_ledc->set_color_hsv( hsv )
    rgb_ledc->set_color_rgb( rgb )
    rgb_ledc->set_color_hex( hex )
    rgb_ledc->set_color_int( int )
    rgb_ledc->set_brightness( int )
    rgb_ledc->inc_brightness( int )
    rgb_ledc->dec_brightness( int )
    rgb_ledc->fade_brightness( from to with delay )
    rgb_ledc->set_saturation( int )
    rgb_ledc->inc_saturation( int )
    rgb_ledc->dec_saturation( int )
    rgb_ledc->fade_saturation( from to with delay )

to control via http get request you need add a get request handler
    add_uri_get_handler( http_server, rgb_ledc->uri, rgb_ledc->http_get_handler);

    ip/colors?type=rgb&r=<r>&g=<g>&b=<b>
    ip/colors?type=hsv&h=<h>&s=<s>&v=<v>
    ip/colors?type=int&val=<int_value>
    ip/colors?type=hex&val=<hex_value>
    ip/colors?rgb=<r>,<g>,<b>
    ip/colors?hsv=<h>,<s>,<v>
*/

#endif