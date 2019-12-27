
#include "led_ctrl.h"
//20 bit
#define BRIGHTNESS_STEPS_20 20
#define BRIGHTNESS_STEPS_32 32

static uint8_t brightness_table_01[BRIGHTNESS_STEPS_20] = {0,1,2,3,4,5,7,9,12,16,21,28,37,48,64,84,111,147,194,255};  
									// 0-100%, логарифмическая, S1 = k * ln(R), k=18.04, R - значение ШИМ (0-255)
	
static uint8_t brightness_table_02[BRIGHTNESS_STEPS_20] = {0,1,2,3,4,7,11,16,23,31,42,54,69,87,107,130,156,186,219,255};  
									// 0-100%, степенная, S2 = k * R^n, k=16.06, n=0.33,  R - значение ШИМ (0-255)
									
static uint8_t brightness_table_03[BRIGHTNESS_STEPS_20] = {0,1,2,3,4,6,8,12,16,22,29,39,51,67,86,109,136,170,209,255};  
									// 0-100%, средняя, S=(S1+S2)/2
	
//32 bit
static uint8_t brightness_table_04[BRIGHTNESS_STEPS_32] = {0,1,2,3,5,8,12,16,21,26,32,38,45,52,60,68,76,85,95,105,115,125,136,148,160,172,185,198,211,225,239,255};

static const char *TAG = "LEDCTRL";

static uint32_t __fadeup = 0;
static uint32_t __fadedown = 0;

static color_hsv_t __hsv = {0, 255, 255};

static led_ctrl_config_t *_led_ctrl;
static uint8_t _led_ctr_cnt = 0;

static void* pcolors = NULL;

static TaskHandle_t xHanldeLedCtrl = NULL;

void ledctrl_init(uint16_t freq, uint8_t led_cnt, const led_ctrl_config_t *_led_ctrl_cfg) {
	ESP_LOGI(TAG, __func__);
	uint32_t *led_pins = malloc(led_cnt * sizeof(uint32_t));
	_led_ctrl = malloc(led_cnt * sizeof(led_ctrl_config_t));
	_led_ctr_cnt = led_cnt;

	memcpy(_led_ctrl, _led_ctrl_cfg, led_cnt * sizeof(led_ctrl_config_t));

	for (uint8_t i = 0; i < led_cnt; i++ ) {
		uint8_t ch = _led_ctrl[i].ch;
		led_pins[ ch ] = _led_ctrl[i].pin;
	} 

	pwm_begin(freq, led_cnt, led_pins);
	
	vTaskDelay( 500 / portTICK_RATE_MS);
	for (uint8_t i = 0; i < led_cnt; i++ ) {
		pwm_set_duty( i, 0 );
	}	
	pwm_start();
}

void ledctrl_set_duty(uint32_t ledc_channel, uint32_t ledc_duty){
	pwm_write(ledc_channel, ledc_duty);
}

void ledctrl_update(){
	pwm_start();
}

static int8_t get_ledctrl_idx_by_type(color_e type) {
	
	for (uint8_t i = 0; i < _led_ctr_cnt; i++ ) {
		if ( _led_ctrl[i].type == type ) {
			return i;
		}
	}
	return -1;
}

void ledctrl_set_color_duty(color_e type, uint32_t duty){
	uint8_t idx = get_ledctrl_idx_by_type( type );
	if ( idx < 0 ) {
		ESP_LOGD(TAG, "Can't find RED channel");
		return;
	}	
	ledctrl_set_duty( _led_ctrl[idx].ch, duty);
}



static void calc_color_duty_and_dir(uint8_t *val, color_effect_direction_e *dir) {
	if ( *val == MIN_HSV_V) *dir = UP;
	if ( *dir == UP) (*val)++;
	if ( *val == MAX_HSV_V) *dir = DOWN;
	if ( *dir == DOWN) (*val)--;
}


void ledctrl_set_color_rgb(const color_rgb_t *rgb, uint8_t del){
	if ( del ) ledctrl_delete_active_task();
	ledctrl_set_color_duty(RED,    rgb->r);
    ledctrl_set_color_duty(GREEN,  rgb->g);
    ledctrl_set_color_duty(BLUE,   rgb->b);
    ledctrl_update();	
	rgb_to_hsv(&__hsv, rgb);
}

void ledctrl_set_color_hsv(const color_hsv_t *hsv, uint8_t del)
{
	if ( del ) ledctrl_delete_active_task();
	color_rgb_t *rgb = malloc( sizeof(color_rgb_t));
	memcpy(&__hsv, hsv, sizeof(color_hsv_t));
	hsv_to_rgb(rgb, __hsv);
	
	ledctrl_set_color_duty(RED,    rgb->r);
	ledctrl_set_color_duty(GREEN,  rgb->g);
	ledctrl_set_color_duty(BLUE,   rgb->b);
	ledctrl_update();	
	free(rgb);
}

void ledctrl_set_color_hex(uint32_t color, uint8_t del) {
	if ( del ) ledctrl_delete_active_task();
	color_rgb_t *rgb = malloc( sizeof(color_rgb_t));
	hex_to_rgb( color, rgb);
	ledctrl_set_color_rgb(rgb, 0);
	free(rgb);
}


// =========================================== JUMP3 =======================================
void task_color_effect__jump3(uint32_t *speed){
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_JUMP;
	color_effect_config_t effect;
	effect.colors_count = 3;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	effect.colors[0] = 0; effect.colors[1] = 120; effect.colors[2] = 240;
    effect.effect = JUMP;
    effect.fadeup_delay = _sp;	effect.fadedown_delay = 0;
    effect.saturation = 255;
    effect.brightness = 255;
	set_color_effect(&effect);
}

void set_color_effect__jump3(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__jump3, "color_jump3", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== FADE3 =======================================
void task_color_effect__fade3(uint32_t *speed){
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_FADE;
	color_effect_config_t effect;
	effect.colors_count = 3;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	effect.colors[0] = 0; effect.colors[1] = 120; effect.colors[2] = 240;
    effect.effect = FADE;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;
	set_color_effect(&effect);
}

void set_color_effect__fade3(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__fade3, "color_fade3", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== JUMP7 =======================================
void task_color_effect__jump7(uint32_t *speed){
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_JUMP;
	color_effect_config_t effect;
	effect.colors_count = 7;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	memcpy(effect.colors, hsv_colors_7, effect.colors_count * sizeof(effect.colors));

    effect.effect = JUMP;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);

}

void set_color_effect__jump7(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__jump7, "color_jump7", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== RANDOM JUMP7 =======================================
void task_color_effect__rnd_jump7(uint32_t *speed){
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_JUMP;
	color_effect_config_t effect;
	effect.colors_count = 7;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	memcpy(effect.colors, hsv_colors_7, effect.colors_count * sizeof(effect.colors));

    effect.effect = RANDOM_JUMP;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);

}

void set_color_effect__rnd_jump7(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__rnd_jump7, "color_rndjump7", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== FADE7 =======================================
void task_color_effect__fade7(uint32_t *speed){
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_FADE;
	color_effect_config_t effect;
	effect.colors_count = 7;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	memcpy(effect.colors, hsv_colors_7, effect.colors_count * sizeof(effect.colors));

    effect.effect = FADE;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);

}

void set_color_effect__fade7(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__fade7, "color_fade7", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== RANDOM FADE7 =======================================
void task_color_effect__rnd_fade7(uint32_t *speed){
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_FADE;
	color_effect_config_t effect;
	effect.colors_count = 7;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	memcpy(effect.colors, hsv_colors_7, effect.colors_count * sizeof(effect.colors));

    effect.effect = RANDOM_FADE;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);

}

void set_color_effect__rnd_fade7(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__rnd_fade7, "color_rndfade7", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== FADE12 =======================================
void task_color_effect__fade12(uint32_t *speed) {
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_FADE;
	color_effect_config_t effect;
	effect.colors_count = 12;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	
	for (uint8_t i = 0; i < effect.colors_count; i++) {
		effect.colors[i] = 30*i;     	// red	

	}

    effect.effect = FADE;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);
}
void set_color_effect__fade12(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__fade12, "color_fade12", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== RANDOM FADE12 =======================================
void task_color_effect__rnd_fade12(uint32_t *speed) {
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_FADE;	
	color_effect_config_t effect;
	effect.colors_count = 12;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	for (uint8_t i = 0; i < effect.colors_count; i++) {
		effect.colors[i] = 30*i;     	// red	
	}

    effect.effect = RANDOM_FADE;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);
}

void set_color_effect__rnd_fade12(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__rnd_fade12, "color_rndfade12", 2048, speed, 10, &xHanldeLedCtrl);
}


// =========================================== JUMP12 =======================================
void task_color_effect__jump12(uint32_t *speed) {
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_JUMP;	
	color_effect_config_t effect;
	effect.colors_count = 12;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	for (uint8_t i = 0; i < effect.colors_count; i++) {
		effect.colors[i] = 30*i;     	// red	
	}

    effect.effect = JUMP;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);
}

void set_color_effect__jump12(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__jump12, "color_jump12", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== RANDOM JUMP12 =======================================
void task_color_effect__rnd_jump12(uint32_t *speed) {
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_JUMP;
	color_effect_config_t effect;
	effect.colors_count = 12;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	for (uint8_t i = 0; i < effect.colors_count; i++) {
		effect.colors[i] = 30*i;     	// red	
	}

    effect.effect = RANDOM_JUMP;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);
}

void set_color_effect__rnd_jump12(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__rnd_jump12, "color_rndjump12", 2048, speed, 10, &xHanldeLedCtrl);
}


// =========================================== WHEEL =======================================
void task_color_effect__wheel(uint32_t *speed) {
	uint32_t _sp = (uint32_t *)speed ? (uint32_t *)speed : LEDCTRL_DEFAULT_SPEED_WHEEL;
	color_effect_config_t effect;
	effect.colors_count = 360;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	for (uint16_t i = 0; i < effect.colors_count; i++) {
		effect.colors[i] = i;     	
	}

    effect.effect = JUMP;
    effect.fadeup_delay = effect.fadedown_delay = _sp;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);
}

void set_color_effect__wheel(uint32_t speed){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__wheel, "color_rndjump12", 2048, speed, 10, &xHanldeLedCtrl);
}

// =========================================== RANDOM =======================================
void task_color_effect__rnd_rnd(void *arg) {
	color_effect_config_t effect;
	effect.colors_count = 360;
	effect.colors = malloc(effect.colors_count * sizeof(effect.colors));
	for (uint16_t i = 0; i < effect.colors_count; i++) {
		effect.colors[i] = i;     	
	}

    effect.effect = RANDOM_CB;
    effect.fadeup_delay = 500; effect.fadedown_delay = 500;
    effect.saturation = 255;
    effect.brightness = 255;

	set_color_effect(&effect);
}

void set_color_effect__rnd_rnd(void *arg){
	ledctrl_delete_active_task();
	xTaskCreate(task_color_effect__rnd_rnd, "color_rndrnd", 2048, NULL, 10, &xHanldeLedCtrl);
}



void set_color_effect(color_effect_config_t *effect) {
	color_effect_direction_e dir = UP, prev_dir = UP;
   	static uint16_t mm = 0;

    __hsv.s = effect->saturation;
    __hsv.v = effect->brightness;
    	
	__fadeup = effect->fadeup_delay;
	__fadedown = effect->fadedown_delay;		

	pcolors = effect->colors;
	//ESP_LOGI(TAG, "set color effect. pcolors %p,  effect->colors %p", pcolors, effect->colors);
	while( 1 ) {
			if ( effect->effect == JUMP ) {
				__hsv.h = effect->colors[mm];
			} else if ( effect->effect == FADE ) {
				prev_dir = dir;
				calc_color_duty_and_dir(&__hsv.v, &dir);
				__hsv.h = effect->colors[mm];
			} else if ( effect->effect == RANDOM_JUMP ) {
				__hsv.h = effect->colors[ esp_random() % effect->colors_count ];
			} else if ( effect->effect == RANDOM_FADE ) {
				prev_dir = dir;
				calc_color_duty_and_dir(&__hsv.v, &dir);
			} else if ( effect->effect == RANDOM_CB ) {
				__hsv.h = effect->colors[ esp_random() % effect->colors_count ];				
				__hsv.v =  esp_random() % 255;
			}

		ledctrl_set_color_hsv( &__hsv, 0);
 
 		if ( effect->effect == JUMP || effect->effect == RANDOM_JUMP ) {
			++mm;
    		if	(mm == effect->colors_count ) mm = 0;        
		} else if ( effect->effect == FADE || effect->effect == RANDOM_FADE) {
			if ( __hsv.v == MIN_HSV_V) {
				dir = UP;
				++mm;
				if (mm == effect->colors_count ) mm = 0;        
				if ( effect->effect == RANDOM_FADE ) {
					__hsv.h = effect->colors[ esp_random() % effect->colors_count ];
				}
			}
		}
		
        if ( dir == UP ) {
			vTaskDelay( __fadeup / portTICK_RATE_MS);
		} else {
			vTaskDelay( __fadedown / portTICK_RATE_MS);
		}	
	}
	ESP_LOGE(TAG, "FATAL: EXIST FROM TASK!!!");
	free( effect->colors);
	effect->colors = NULL;
	vTaskDelete(NULL);
}

void update_fadeup(uint32_t speed) {
	if ( speed < LEDCTRL_MIN_FADE_DEALY ) 
		__fadeup = LEDCTRL_MIN_FADE_DEALY;
	else if ( speed > LEDCTRL_MAX_FADE_DEALY)
		__fadeup = LEDCTRL_MAX_FADE_DEALY;
	else		
		__fadeup = speed;
}

void inc_fadeup(uint32_t step) {
	__fadeup += step;
	if ( __fadeup > LEDCTRL_MAX_FADE_DEALY) __fadeup = LEDCTRL_MAX_FADE_DEALY;
}

void dec_fadeup(uint32_t step) {
	if ( __fadeup - step < LEDCTRL_MIN_FADE_DEALY ) __fadeup = LEDCTRL_MIN_FADE_DEALY;
	__fadeup -= step;
}

void update_fadedown(uint32_t speed) {
	if ( speed < LEDCTRL_MIN_FADE_DEALY ) 
		__fadedown = LEDCTRL_MIN_FADE_DEALY;
	else if ( speed > LEDCTRL_MAX_FADE_DEALY)
		__fadedown = LEDCTRL_MAX_FADE_DEALY;
	else		
		__fadedown = speed;	
}

void inc_fadedown(uint32_t step) {
	__fadedown += step;
	if ( __fadedown > LEDCTRL_MAX_FADE_DEALY) __fadedown = LEDCTRL_MAX_FADE_DEALY;	
}

void dec_fadedown(uint32_t step) {
	if ( __fadedown - step < LEDCTRL_MIN_FADE_DEALY ) __fadedown = LEDCTRL_MIN_FADE_DEALY;
	__fadedown -= step;	
}

void update_brightness(uint8_t brightness) {
	__hsv.v = brightness;
}

void inc_brightness(uint8_t step){
	if ( __hsv.v + step > MAX_HSV_V ) 
		__hsv.v = MAX_HSV_V;
	else
		__hsv.v += step;	
}

void dec_brightness(uint8_t step) {
	if ( __hsv.v - step < MIN_HSV_V ) 
		__hsv.v = MIN_HSV_V;
	else
		__hsv.v -= step;	
}

void update_saturation(uint8_t saturation) {
	__hsv.s = saturation;
}
void inc_saturation(uint8_t step){
	if ( __hsv.s + step > MAX_HSV_S ) 
		__hsv.s = MAX_HSV_S;
	else
		__hsv.s += step;	
}

void dec_saturation(uint8_t step) {
	if ( __hsv.s - step < MIN_HSV_S ) 
		__hsv.s = MIN_HSV_S;
	else
		__hsv.s -= step;	
}

void ledctrl_delete_active_task(){

	if ( pcolors != NULL ) {
		free(pcolors);
		pcolors = NULL;
	}

    if ( xHanldeLedCtrl != NULL ) { 
		vTaskDelete( xHanldeLedCtrl ); 
		xHanldeLedCtrl = NULL; 
	}	
}


esp_err_t handle_color_effect_by_id(uint8_t id, uint32_t speed) {
	ESP_LOGI(TAG, "effect %d: %s", id, color_effects[id]);
	esp_err_t err = ESP_OK;
	switch (id) {
		case 0: set_color_effect__jump3(speed); break;
		case 1: set_color_effect__jump7(speed); break;
		case 2: set_color_effect__jump12(speed); break;
		case 3: set_color_effect__rnd_jump7(speed); break;
		case 4: set_color_effect__rnd_jump12(speed); break;
		case 5: set_color_effect__fade3(speed); break;
		case 6: set_color_effect__fade7(speed); break;
		case 7: set_color_effect__fade12(speed); break;
		case 8: set_color_effect__rnd_fade7(speed); break;
		case 9: set_color_effect__rnd_fade12(speed); break;
		case 10: set_color_effect__wheel(speed); break;
		case 11: set_color_effect__rnd_rnd( NULL ); break;
		case 12: ledctrl_set_color_hex( 0, 1 ); 	break;
		default: 
			err = ESP_FAIL;
		break;
	}
	return err;
}

esp_err_t handle_color_effect_default_by_id(uint8_t id) {
	return handle_color_effect_by_id(id, 0);
}


esp_err_t handle_color_effect_by_name(char *effect_name, uint32_t speed) {
	esp_err_t err = ESP_FAIL;
	for (int i=0; i<COLOR_EFFECTS_MAX;i++) {
		if ( strcmp( effect_name, color_effects[i]) == ESP_OK ) {
			err = handle_color_effect_by_id(i, speed);
			break;
		}
	}
	return err;
}

esp_err_t handle_color_effect_default_by_name(char *effect_name) {
	return handle_color_effect_by_name(effect_name, 0);
}