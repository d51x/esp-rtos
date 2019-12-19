
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

#define LEDC_MAX_DUTY 255

#define LED_CTRL_PIN_0 4
#define LED_CTRL_PIN_1 5
#define LED_CTRL_PIN_2 12
#define LED_CTRL_PIN_3 14

#define LED_CTRL_CHANNELS_CNT 4
#define LED_CTRL_FREQ_HZ PWM_FREQ_HZ

static const uint32_t led_ctrl_pins[LED_CTRL_CHANNELS_CNT] = {
    LED_CTRL_PIN_0,
    LED_CTRL_PIN_1,
    LED_CTRL_PIN_2,
    LED_CTRL_PIN_3
};

typedef struct {
	uint8_t ch;
	uint16_t duty;
	uint16_t step_duty;
	uint16_t fade_up_time;
	uint16_t fade_down_time;
	uint8_t	  bright_table;  // указатель на таблицу яркости
	uint8_t	  bright_res;	 // размерность таблицы яркости	
} led_ctrl_config_t;



void led_ctrl_init(){
	// initialize led struct channel and pwm_init
	pwm_begin(LED_CTRL_FREQ_HZ, LED_CTRL_CHANNELS_CNT, pwm_pins);
}

esp_err_t ledc_set_duty(uint32_t ledc_channel, uint32_t ledc_duty){
	return ESP_OK;
}




static void led_ctrl_start(){}
static void led_ctrl_fade_up(){}
static void led_ctrl_fade_down(){}
static void led_ctrl_stop(){}

static void set_led_red_channel(){}
static void set_led_green_channel(){}
static void set_led_blue_channel(){}
static void set_led_white_channel(){}