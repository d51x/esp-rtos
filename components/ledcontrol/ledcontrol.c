#include "ledcontrol.h"

static const char *TAG = "LEDCTRL";

#define IOT_CHECK(tag, a, ret)  if(!(a)) {                                             \
        ESP_LOGE(tag,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__);      \
        return (ret);                                                                   \
        }
#define ERR_ASSERT(tag, param)  IOT_CHECK(tag, (param) == ESP_OK, ESP_FAIL)
#define POINT_ASSERT(tag, param, ret)    IOT_CHECK(tag, (param) != NULL, (ret))


static ledcontrol_t *ledc = NULL;
static uint16_t period;

void ledcontrol_init();
esp_err_t ledcontrol_register_channel(ledcontrol_channel_t ledc_ch);
void ledcontrol_set_duty(ledcontrol_channel_t *channel, uint16_t duty);
void ledcontrol_update();
void ledcontrol_channel_on(ledcontrol_channel_t *channel);
void ledcontrol_channel_off(ledcontrol_channel_t *channel);
void ledcontrol_channel_next_duty(ledcontrol_channel_t *channel, uint8_t step);
void ledcontrol_channel_prev_duty(ledcontrol_channel_t *channel, uint8_t step);
void ledcontrol_channel_fade(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay);
void ledcontrol_channel_fade_to_off(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_delay);
void ledcontrol_channel_fade_to_on(ledcontrol_channel_t *channel, uint16_t duty_to, uint16_t duty_delay);
void ledcontrol_channel_set_brightness_table(ledcontrol_channel_t *channel, brightness_table_e bright_tbl);

// для нескольких каналов одного цвета
void ledcontrol_all_off();
void ledcontrol_all_on();
void ledcontrol_all_next_duty(uint8_t step);
void ledcontrol_all_prev_duty(uint8_t step);
void ledcontrol_all_fade(uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay);
void ledcontrol_all_fade_to_on(uint16_t duty_from, uint16_t duty_delay);
void ledcontrol_all_fade_to_off(uint16_t duty_from, uint16_t duty_delay);

ledcontrol_t* ledcontrol_create(uint32_t freq_hz, uint8_t channel_cnt)
{

    
    if ( channel_cnt < 1 || channel_cnt > LEDCONTROL_CHANNEL_MAX) return NULL;
    if ( freq_hz < LEDCONTROL_FREQ_MIN || freq_hz > LEDCONTROL_FREQ_MAX) return NULL;

    ledc = calloc(1, sizeof(ledcontrol_t));

    ledc->freq = freq_hz;
    ledc->led_cnt = channel_cnt;
    ledc->channels = calloc( channel_cnt, sizeof(ledcontrol_channel_t));



	// указатели на функции
	ledc->init = &ledcontrol_init;
	ledc->register_channel = &ledcontrol_register_channel;
    ledc->set_duty = &ledcontrol_set_duty;
	ledc->update = &ledcontrol_update;
	ledc->on = &ledcontrol_channel_on;
	ledc->off = &ledcontrol_channel_off;
	ledc->next_duty = &ledcontrol_channel_next_duty;
	ledc->prev_duty = &ledcontrol_channel_prev_duty;
	ledc->fade = &ledcontrol_channel_fade;
	ledc->fade_to_off = &ledcontrol_channel_fade_to_off;
	ledc->fade_to_on = &ledcontrol_channel_fade_to_on;
	ledc->bright_tbl = &ledcontrol_channel_set_brightness_table;
	ledc->off_all = &ledcontrol_all_off;
	ledc->on_all = &ledcontrol_all_on;
	ledc->next_duty_all = &ledcontrol_all_next_duty;
	ledc->prev_duty_all = &ledcontrol_all_prev_duty;
	ledc->fade_all = &ledcontrol_all_fade;
	ledc->fade_to_off_all = &ledcontrol_all_fade_to_off;
	ledc->fade_to_on_all = &ledcontrol_all_fade_to_on;
    //return (ledcontrol_handle_t) ledc;
    return (ledcontrol_handle_t) ledc;
}

  // регистрация канала
esp_err_t ledcontrol_register_channel(ledcontrol_channel_t led_channel)
{
    
    if ( ledc == NULL ) {
        ESP_LOGE(TAG, "Led controller is not initialized yet.");
        return ESP_FAIL;
    }

    if ( led_channel.channel < 0 || led_channel.channel >= LEDCONTROL_CHANNEL_MAX ) {
        ESP_LOGE(TAG, "Led channel is not correct");
        return ESP_FAIL;        
    }

    led_channel.duty = 0;
    led_channel.bright_tbl = NONE;
    memcpy( &ledc->channels[ led_channel.channel ], &led_channel, sizeof(ledcontrol_channel_handle_t));
    return ESP_OK;
}

// инициализация pwm
void ledcontrol_init()
{
    ESP_LOGI(TAG, __func__);
    uint32_t *led_pins = malloc(ledc->led_cnt * sizeof(uint32_t));

	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {
		uint8_t ch = ledc->channels[i].channel;
		led_pins[ ch ] = ledc->channels[i].pin;
	} 

    period = 1000000 / ledc->freq;  // Hz to period, Just freq_hz is useful
    uint32_t *duties = malloc( sizeof(uint32_t) * ledc->led_cnt);
    for (uint8_t i=0;i<ledc->led_cnt;i++) duties[i] = period;
    pwm_init(period, duties, ledc->led_cnt, led_pins);

    int16_t *phases = malloc( sizeof(uint16_t) * ledc->led_cnt);
    memset(phases, 0, sizeof(uint16_t) * ledc->led_cnt);
    pwm_set_phases(phases);

    pwm_start();
    free(phases);
    free(duties);
	
	vTaskDelay( 500 / portTICK_RATE_MS);
	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {
        ESP_LOGI(TAG, "set duty %d for channel %d", ledc->channels[i].duty, ledc->channels[i].channel);
		pwm_set_duty( ledc->channels[i].channel, ledc->channels[i].duty );
	}	
	pwm_start();
}

// установить duty канала
void ledcontrol_set_duty(ledcontrol_channel_t *channel, uint16_t duty){
    channel->duty = duty;
    uint16_t real_duty = duty*period/MAX_DUTY;
    ESP_LOGI(TAG, "set duty %d for channel %d (real duty %d)", channel->duty, channel->channel, real_duty);
    pwm_set_duty(channel->channel, real_duty);    
}

void ledcontrol_update(){
    ESP_LOGI(TAG, __func__);
	pwm_start();

}

void ledcontrol_channel_on(ledcontrol_channel_t *channel){
    ESP_LOGI(TAG, __func__);
    // set MAX DUTY
    ledcontrol_set_duty(channel, MAX_DUTY);
    ledcontrol_update();
}

void ledcontrol_channel_off(ledcontrol_channel_t *channel){
    ESP_LOGI(TAG, __func__);
    // set MAX DUTY
    ledcontrol_set_duty(channel, 0);
    ledcontrol_update();
}

void ledcontrol_channel_next_duty(ledcontrol_channel_t *channel, uint8_t step)
{
    ESP_LOGI(TAG, __func__);
    uint16_t duty = channel->duty;
    if ( duty <= MAX_DUTY - step ) 
        duty += step;
    else
        duty = MAX_DUTY;

    ledcontrol_set_duty(channel, duty);
}

void ledcontrol_channel_prev_duty(ledcontrol_channel_t *channel, uint8_t step)
{
    ESP_LOGI(TAG, __func__);
    uint16_t duty = channel->duty;
    if ( duty >= step ) 
        duty -= step;
    else
        duty = 0;

    ledcontrol_set_duty(channel, duty);
}

void ledcontrol_channel_fade(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay) {
    ESP_LOGI(TAG, __func__);
    
    direction_e direction = (duty_from < duty_to) ? UP : DOWN;

    ESP_LOGI(TAG, "duty from %d    duty to %d   direction %d", duty_from, duty_to, direction);

    int16_t duty = duty_from;

    while ( 
            ((direction == UP) && (duty <= duty_to)) ||
            ((direction == DOWN) && (duty >= duty_to))
          )  
    {
        ledcontrol_set_duty(channel, duty);
        ledcontrol_update();
        if ( direction == UP )
            duty++;   // TODO: учесть brightness table
        else
            duty--;  // TODO: учесть brightness table

        vTaskDelay( duty_delay / portTICK_RATE_MS );
    }

}

// с указанного уровня до 0
void ledcontrol_channel_fade_to_off(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_delay) {
    ledcontrol_channel_fade(channel, duty_from, 0, duty_delay);
}

// с нуля до указанного уровня
void ledcontrol_channel_fade_to_on(ledcontrol_channel_t *channel, uint16_t duty_to, uint16_t duty_delay) {
    ledcontrol_channel_fade(channel, 0, duty_to, duty_delay);
}

void ledcontrol_channel_set_brightness_table(ledcontrol_channel_t *channel, brightness_table_e bright_tbl){
    channel->bright_tbl = bright_tbl;
}  

void ledcontrol_all_off(){
    ledcontrol_channel_t *p = ledc->channels;
	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {
        ledcontrol_set_duty( p+i, 0);
	}
    ledcontrol_update();
}

void ledcontrol_all_on(){
    ledcontrol_channel_t *p = ledc->channels;
	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) { 
        ledcontrol_set_duty(p+i, MAX_DUTY);
	}
    ledcontrol_update();
}

void ledcontrol_all_next_duty(uint8_t step){
    ledcontrol_channel_t *p = ledc->channels;
	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {
        uint16_t duty = ledc->channels[i].duty;
        if ( duty <= MAX_DUTY - step ) 
            duty += step;
        else
            duty = MAX_DUTY;
        
        ledcontrol_set_duty( p+i, duty);
	}
    ledcontrol_update();
}

void ledcontrol_all_prev_duty(uint8_t step){
    ledcontrol_channel_t *p = ledc->channels; 
	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {
        uint16_t duty = ledc->channels[i].duty;
        if ( duty >= step ) 
            duty -= step;
        else
            duty = 0;
           
        ledcontrol_set_duty(p+i, duty);
	}
    ledcontrol_update();
}

void ledcontrol_all_fade(uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay) {
    ESP_LOGI(TAG, __func__);
    direction_e direction = (duty_from < duty_to) ? UP : DOWN;
    int16_t duty = duty_from ;

    while ( 
            ((direction == UP) && (duty <= duty_to)) ||
            ((direction == DOWN) && (duty >= duty_to))
        )  
    {
        ledcontrol_channel_t *p = ledc->channels;
        for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {   
                     
            ledcontrol_set_duty(p+i, duty);  
            if ( direction == UP )
                duty++;   // TODO: учесть brightness table
            else
                duty--;  // TODO: учесть brightness table
        }
        ledcontrol_update();
        vTaskDelay( duty_delay / portTICK_RATE_MS );
    }    
        
}

void ledcontrol_all_fade_to_off(uint16_t duty_from, uint16_t duty_delay) {
	ledcontrol_all_fade(duty_from, 0, duty_delay);
}

void ledcontrol_all_fade_to_on(uint16_t duty_to, uint16_t duty_delay) {
    ledcontrol_all_fade(0, duty_to, duty_delay);
}