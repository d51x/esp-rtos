#include "ledcontrol.h"

static const char *TAG = "LEDCTRL";

static ledcontrol_t *ledc = NULL;
static uint16_t period;

void ledcontrol_init();
esp_err_t ledcontrol_register_channel(ledcontrol_channel_t ledc_ch);
esp_err_t ledcontrol_set_duty(ledcontrol_channel_t *channel, uint16_t duty);
uint16_t ledcontrol_get_duty(ledcontrol_channel_t *channel);
void ledcontrol_update();
esp_err_t ledcontrol_channel_on(ledcontrol_channel_t *channel);
esp_err_t ledcontrol_channel_off(ledcontrol_channel_t *channel);
esp_err_t ledcontrol_channel_next_duty(ledcontrol_channel_t *channel, uint8_t step);
esp_err_t ledcontrol_channel_prev_duty(ledcontrol_channel_t *channel, uint8_t step);
esp_err_t ledcontrol_channel_fade(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay);
esp_err_t ledcontrol_channel_fade_to_off(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_delay);
esp_err_t ledcontrol_channel_fade_to_on(ledcontrol_channel_t *channel, uint16_t duty_to, uint16_t duty_delay);
void ledcontrol_channel_set_brightness_table(ledcontrol_channel_t *channel, brightness_table_e bright_tbl);

// для нескольких каналов одного цвета
void ledcontrol_all_off();
void ledcontrol_all_on();
void ledcontrol_all_next_duty(uint8_t step);
void ledcontrol_all_prev_duty(uint8_t step);
void ledcontrol_all_fade(uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay);
void ledcontrol_all_fade_to_on(uint16_t duty_from, uint16_t duty_delay);
void ledcontrol_all_fade_to_off(uint16_t duty_from, uint16_t duty_delay);

esp_err_t ledcontrol_http_get_handler(httpd_req_t *req);

ledcontrol_handle_t* ledcontrol_create(uint32_t freq_hz, uint8_t channel_cnt)
{

    
    if ( channel_cnt < 1 || channel_cnt > LEDCONTROL_CHANNEL_MAX) return NULL;
    if ( freq_hz < LEDCONTROL_FREQ_MIN || freq_hz > LEDCONTROL_FREQ_MAX) return NULL;

    ledc = calloc(1, sizeof(ledcontrol_t));

    ledc->freq = freq_hz;
    ledc->led_cnt = channel_cnt;
    ledc->channels = calloc( channel_cnt, sizeof(ledcontrol_channel_t));



	// указатели на функции
	ledc->init = ledcontrol_init;
	ledc->register_channel = ledcontrol_register_channel;
    ledc->set_duty = ledcontrol_set_duty;
    ledc->get_duty = ledcontrol_get_duty;
	ledc->update = ledcontrol_update;
	ledc->on = ledcontrol_channel_on;
	ledc->off = ledcontrol_channel_off;
	ledc->next_duty = ledcontrol_channel_next_duty;
	ledc->prev_duty = ledcontrol_channel_prev_duty;
	ledc->fade = ledcontrol_channel_fade;
	ledc->fade_to_off = ledcontrol_channel_fade_to_off;
	ledc->fade_to_on = ledcontrol_channel_fade_to_on;
	ledc->bright_tbl = ledcontrol_channel_set_brightness_table;
	ledc->off_all = ledcontrol_all_off;
	ledc->on_all = ledcontrol_all_on;
	ledc->next_duty_all = ledcontrol_all_next_duty;
	ledc->prev_duty_all = ledcontrol_all_prev_duty;
	ledc->fade_all = ledcontrol_all_fade;
	ledc->fade_to_off_all = ledcontrol_all_fade_to_off;
	ledc->fade_to_on_all = ledcontrol_all_fade_to_on;


    return (ledcontrol_handle_t) ledc;
}

  // регистрация канала
esp_err_t ledcontrol_register_channel(ledcontrol_channel_t led_channel)
{
    
    if ( ledc == NULL ) {
        ESP_LOGE(TAG, "Led controller is not initialized yet.");
        return ESP_FAIL;
    }

    if ( led_channel.channel < 0 || led_channel.channel >= ledc->led_cnt ) {
        ESP_LOGE(TAG, "Led channel is not correct");
        return ESP_FAIL;        
    }

    uint8_t ch = led_channel.channel;
    memcpy( &ledc->channels[ ch ], &led_channel, sizeof(ledcontrol_channel_handle_t));
    ledc->channels[ ch ].duty = 0;
    ledc->channels[ ch ].bright_tbl = led_channel.bright_tbl;
    ledc->channels[ ch ].inverted = led_channel.inverted;
    ledc->channels[ ch ].name = led_channel.name;
    ledc->channels[ ch ].ledc = ledc;

    return ESP_OK;
}

// инициализация pwm
void ledcontrol_init()
{
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

	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {
		uint8_t ch = ledc->channels[i].channel;
		if (ledc->channels[i].inverted )
            pwm_set_channel_invert( 0x1  << ledc->channels[i].channel );
	}    
    

    pwm_start();
    free(phases);
    free(duties);
	
	vTaskDelay( 500 / portTICK_RATE_MS);
	for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {
		pwm_set_duty( ledc->channels[i].channel, ledc->channels[i].duty );
	}	
	pwm_start();
}

// установить duty канала
esp_err_t ledcontrol_set_duty(ledcontrol_channel_t *channel, uint16_t duty){
    channel->duty = duty;
    uint16_t real_duty = duty*period/MAX_DUTY;
    esp_err_t err = pwm_set_duty(channel->channel, real_duty);
    return err;    
}

// получить duty канала
uint16_t ledcontrol_get_duty(ledcontrol_channel_t *channel){
    uint32_t real_duty;
    pwm_get_duty(channel->channel, &real_duty); 
    float tmp = real_duty;
    tmp = tmp * MAX_DUTY * channel->ledc->freq / 1000000;
    channel->duty = uround(tmp);
    return channel->duty;
}

void ledcontrol_update(){
	pwm_start();

}

esp_err_t ledcontrol_channel_on(ledcontrol_channel_t *channel){
    // set MAX DUTY
    esp_err_t err = ledcontrol_set_duty(channel, MAX_DUTY);
    ledcontrol_update();
    return err;
}

esp_err_t ledcontrol_channel_off(ledcontrol_channel_t *channel){
    // set MAX DUTY
    esp_err_t err = ledcontrol_set_duty(channel, 0);
    ledcontrol_update();
    return err;
}

esp_err_t ledcontrol_channel_next_duty(ledcontrol_channel_t *channel, uint8_t step)
{
    uint16_t duty = channel->duty;
    if ( duty <= MAX_DUTY - step ) 
        duty += step;
    else
        duty = MAX_DUTY;

    esp_err_t err = ledcontrol_set_duty(channel, duty);
    return err;
}

esp_err_t ledcontrol_channel_prev_duty(ledcontrol_channel_t *channel, uint8_t step)
{
    uint16_t duty = channel->duty;
    if ( duty >= step ) 
        duty -= step;
    else
        duty = 0;

    esp_err_t err = ledcontrol_set_duty(channel, duty);
    return err;
}

static uint8_t get_min_index_from_brightness_table(ledcontrol_channel_t *channel, uint16_t duty) {
    uint8_t idx = 0;
    if ( channel->bright_tbl != TBL_NONE ) {
        uint32_t *tbl = tbl_brightness[ channel->bright_tbl ]; // pointer to brightness table
        uint8_t step = bright_steps[ channel->bright_tbl  ]; // brightness step count

        for (uint8_t i = 0; i < step+1 ; i++) {
            uint8_t val = tbl[i];  // brightness value from table
            uint8_t val_next = ( i + 1 >= step + 1 ) ? val : tbl[i + 1];
            if ( duty >= val && duty < val_next ) {
                idx = i;
                break;
            } else if ( duty == val_next) {
                idx = i + 1;
                break;
            }
        }
    }
    return idx;
}

static uint8_t get_max_index_from_brightness_table(ledcontrol_channel_t *channel, uint16_t duty) {
    uint8_t idx = 0;
    if ( channel->bright_tbl != TBL_NONE ) {
        uint32_t *tbl = tbl_brightness[ channel->bright_tbl ]; // pointer to brightness table
        uint8_t step = bright_steps[ channel->bright_tbl  ]; 
        for (uint8_t i = 0; i < step+1 ; i++) {
            uint8_t val = tbl[i];  // brightness value from table
            uint8_t val_next = ( i + 1 >= step + 1 ) ? val : tbl[i + 1];
            if ( duty > val && duty <= val_next ) {
                idx = i+1;
                break;
            } else if ( duty == val) {
                idx = i;
                break;
            }
        }
    }
    return idx;
}

static esp_err_t ledcontrol_channel_fade_by_table(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay){
    esp_err_t err = ESP_FAIL;
    direction_e direction;
    uint8_t idx_start = 0;
    uint8_t idx_stop = 0;

    if (duty_from < duty_to) {
        direction = UP;
        idx_start = get_max_index_from_brightness_table(channel, duty_from);
        idx_stop = get_max_index_from_brightness_table(channel, duty_to);
    } else {
        direction = DOWN;
        idx_start = get_min_index_from_brightness_table(channel, duty_from);
        idx_stop = get_min_index_from_brightness_table(channel, duty_to);
    }

    int16_t i = idx_start;
    uint32_t *tbl = tbl_brightness[ channel->bright_tbl ];
    
    while ( (direction == UP && i <= idx_stop) ||
            (direction == DOWN && i >= idx_stop) 
          ) 
    {
        
        uint8_t duty = tbl[i];
        
        err = ledcontrol_set_duty(channel, duty);
        if ( err == ESP_FAIL )
            break;
        ledcontrol_update();
        if ( direction == UP )
            i++;   // TODO: учесть brightness table
        else
            i--;  // TODO: учесть brightness table

        vTaskDelay( duty_delay / portTICK_RATE_MS );
    }
    return err;
}   


esp_err_t ledcontrol_channel_fade(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay) {
    esp_err_t err = ESP_FAIL;
    if ( channel->bright_tbl != TBL_NONE ) {
        err = ledcontrol_channel_fade_by_table(channel, duty_from, duty_to, duty_delay);
        return err;
    }

    direction_e direction = (duty_from < duty_to) ? UP : DOWN;

    int16_t duty = duty_from;

    while ( 
            ((direction == UP) && (duty <= duty_to)) ||
            ((direction == DOWN) && (duty >= duty_to))
          )  
    {

        err = ledcontrol_set_duty(channel, duty);
        if ( err == ESP_FAIL )
            break;        
        ledcontrol_update();
        if ( direction == UP )
            duty++;   // TODO: учесть brightness table
        else
            duty--;  // TODO: учесть brightness table

        vTaskDelay( duty_delay / portTICK_RATE_MS );
    }

    return err;
}

// с указанного уровня до 0
esp_err_t ledcontrol_channel_fade_to_off(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_delay) {
    return ledcontrol_channel_fade(channel, duty_from, 0, duty_delay);
}

// с нуля до указанного уровня
esp_err_t ledcontrol_channel_fade_to_on(ledcontrol_channel_t *channel, uint16_t duty_to, uint16_t duty_delay) {
    return ledcontrol_channel_fade(channel, 0, duty_to, duty_delay);
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

    // TODO: учесть таблицы яркости
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
    // TODO: учесть таблицы яркости
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
    // TODO: учесть таблицы яркости только если все каналы имеют одну и туже таблицу яркости

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

