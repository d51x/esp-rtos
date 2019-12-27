#ifndef __COLORS_H__
#define __COLORS_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"

/* hsv --> rgb */
#define hue_t   uint16_t
#define HUE_MAX   360
#define SAT_MAX   255
#define VAL_MAX   255

typedef struct color_rgb_t {
   uint8_t      r;
   uint8_t      g;
   uint8_t      b;
} color_rgb_t;

typedef struct color_hsv_t {
   hue_t      h;
   uint8_t      s;
   uint8_t      v;
} color_hsv_t;

static const char *color_names[5]  = {"RED", "GREEN", "BLUE", "WHITE", "WWHITE"};


static const uint32_t hsv_colors_7[7] = {  0,     	// red	
                                    30,		// orange
                                    60,		// yellow
                                    120,		// green
                                    180,		// cyan
                                    240,		// blue
                                    300		// purple
                                };

/* Colors                                R   G   B*/
static const uint8_t COLOR_BLACK[]         = {  0,  0,  0};
// main 7 colors   
static const uint8_t COLOR_RED[]           = {255,   0,   0};
static const uint8_t COLOR_ORANGE[]        = {255, 128,   0};      
static const uint8_t COLOR_YELLOW[]        = {255, 255,   0};
static const uint8_t COLOR_GREEN[]         = {  0, 255,   0};
static const uint8_t COLOR_CYAN[]          = {  0, 255, 255};
static const uint8_t COLOR_BLUE[]          = {  0,   0, 255};
static const uint8_t COLOR_PURPLE[]        = {255,   0, 255};
static const uint8_t COLOR_WHITE[]         = {255, 255, 255};
// light colors
static const uint8_t COLOR_LIMEGREEN[]     = {128, 255,   0}; 
static const uint8_t COLOR_LIGHTBLUE[]     = {  0, 128, 255}; 
static const uint8_t COLOR_VIOLET[]        = {128,   0, 255}; 
static const uint8_t COLOR_LIGHTGREEN[]    = {  0, 255, 128}; 
static const uint8_t COLOR_PINK[]          = {255,   0, 128}; 

static const uint8_t COLOR_DARKPURPLE[]    = {128,   0, 128};
static const uint8_t COLOR_TEAL[]          = {  0, 128, 128};
static const uint8_t COLOR_OLIVE[]         = {128, 128,   0};


typedef enum {
	RED,
	GREEN,
	BLUE,
    WHITE,
	WWHITE
} color_e;

color_hsv_t hsv;
color_rgb_t *rgb;


void  hex_to_rgb(uint32_t color32, volatile color_rgb_t *rgb);
void  hsv_to_rgb(volatile color_rgb_t *rgb, const color_hsv_t hsv);
void  rgb_to_hsv(const color_rgb_t *rgb, color_hsv_t *hsv);
#endif