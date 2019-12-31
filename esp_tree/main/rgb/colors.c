#include "colors.h"

/****************************************************************
hue - 0..359	цвет
//---- sat - 0..255	насыщенность
sat - 0..100	насыщенность
//---- val - 0..255	яркость
val - 0..100	яркость
*****************************************************************/

void hsv_to_rgb(volatile color_rgb_t *rgb, const color_hsv_t hsv) {
	uint8_t hi,fr, p, q, t;
	volatile uint8_t h_pr;

	if( hsv.s == 0) {
		/* color is grayscale */
		rgb->r = rgb->g = rgb->b = hsv.v;
		return;
	}

   // hsv.s from 100 to 255
   // hsv.v from 100 to 255
   uint8_t s = map(hsv.s, 0, 100, 0, 255);
   uint8_t v = map(hsv.v, 0, 100, 0, 255);

	hi = hsv.h / 60;
	h_pr = hsv.h - hi*60; 

	fr = ( h_pr * 255 ) / 60;
	//p  = hsv.v * ( 255 - hsv.s ) / 255;
	p  = v * ( 255 - s ) / 255;
	//q  = hsv.v * ( 255 - ( ( hsv.s * fr ) / 255 ) ) / 255;
	q  = v * ( 255 - ( ( s * fr ) / 255 ) ) / 255;
	//t  = hsv.v * ( 255 - ( hsv.s * ( 255 - fr ) / 255 ) ) / 255;
	t  = v * ( 255 - ( s * ( 255 - fr ) / 255 ) ) / 255;

	switch ( hi ) {
		//case 0: rgb->r = hsv.v; 	rgb->g = t; 		rgb->b = p; break;
		case 0: rgb->r = v; 	   rgb->g = t; 		rgb->b = p; break;
		//case 1: rgb->r = q; 		rgb->g = hsv.v; 	rgb->b = p; break;
		case 1: rgb->r = q; 		rgb->g = v; 	rgb->b = p; break;
		//case 2: rgb->r = p; 		rgb->g = hsv.v; 	rgb->b = t; break;
		case 2: rgb->r = p; 		rgb->g = v; 	rgb->b = t; break;
		//case 3: rgb->r = p; 		rgb->g = q; 		rgb->b = hsv.v; break;
		case 3: rgb->r = p; 		rgb->g = q; 		rgb->b = v; break;
		//case 4: rgb->r = t; 		rgb->g = p; 		rgb->b = hsv.v; break;
		case 4: rgb->r = t; 		rgb->g = p; 		rgb->b = v; break;
		//case 5: rgb->r = hsv.v; 	rgb->g = p; 		rgb->b = q; break;
		case 5: rgb->r = v; 	rgb->g = p; 		rgb->b = q; break;
	}
}

void  int_to_rgb(uint32_t color32, volatile color_rgb_t *rgb) {
    rgb->r = (color32 >> 16) & 0xff;
    rgb->g = (color32 >> 8) & 0xff;
    rgb->b = color32 & 0xff;
}

void  hex_to_rgb(const char *color, volatile color_rgb_t *rgb) {
   uint32_t color32 = hex2int(color);
    rgb->r = (color32 >> 16) & 0xff;
    rgb->g = (color32 >> 8) & 0xff;
    rgb->b = color32 & 0xff;
}

#define MIN(x,y)   ((x) < (y))?(x):(y)
#define MAX(x,y)   ((x) > (y))?(x):(y)
#define HUE_QUADRANT   ((360 + 5)/ 6)

void  rgb_to_hsv(const color_rgb_t *rgb, color_hsv_t *hsv){
   uint8_t min, max, delta;
   int16_t hsvh;

   min = MIN(rgb->r, MIN(rgb->g, rgb->b));
   max = MAX(rgb->r, MAX(rgb->g, rgb->b));

   hsv->v = max;                // v, 0..255
   delta = max - min;                      // 0..255, < v

   if(max != 0)
      hsv->s = (int)(delta)*255 / max;        // s, 0..255
   else {
      // r = g = b = 0        // s = 0, v is undefined
      hsv->s = 0;
      hsv->h = 0;
      return;
   }

   if(rgb->r == max)
      hsvh = (rgb->g - rgb->b)*HUE_QUADRANT/delta;        // between yellow & magenta
   else if(rgb->g == max)
      hsvh = (HUE_QUADRANT*2) + (rgb->b - rgb->r)*HUE_QUADRANT/delta;    // between cyan & yellow
   else
      hsvh = (HUE_QUADRANT*4) + (rgb->r - rgb->g)*HUE_QUADRANT/delta;    // between magenta & cyan

   if(hsvh < 0) hsvh += HUE_MAX;

   hsv->h = hsvh;	

   // from 0.255, to 0..100
   hsv->s = map(hsv->s, 0, 255, 0, 100);
   hsv->v = map(hsv->v, 0, 255, 0, 100);
}