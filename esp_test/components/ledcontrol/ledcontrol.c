#include "ledcontrol.h"

static const char *TAG = "LEDCTRL";

static ledcontrol_t *ledc = NULL;
static uint16_t period;

void ledcontrol_init();
esp_err_t ledcontrol_register_channel(ledcontrol_channel_t ledc_ch);
void ledcontrol_set_duty(ledcontrol_channel_t *channel, uint16_t duty);
uint16_t ledcontrol_get_duty(ledcontrol_channel_t *channel);
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

void ledcontrol_print_html_data(char *data);
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

    ledc->print_html_data = ledcontrol_print_html_data;
    strcpy(ledc->uri, LEDC_URI);
    ledc->http_get_handler = ledcontrol_http_get_handler; 

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

    //led_channel.duty = 0;
    //led_channel.bright_tbl = NONE;
    uint8_t ch = led_channel.channel;
    memcpy( &ledc->channels[ ch ], &led_channel, sizeof(ledcontrol_channel_handle_t));
    //ledc->channels[ch] = &led_channel;

/*
    ledc->channels[ ch ].pin = led_channel->pin;
    ledc->channels[ ch ].channel = led_channel->channel;
*/    
    ledc->channels[ ch ].duty = 0;
    ledc->channels[ ch ].bright_tbl = led_channel.bright_tbl;
    ledc->channels[ ch ].inverted = led_channel.inverted;

    ledc->channels[ ch ].ledc = ledc;

    ESP_LOGI(TAG, "channel %d inverted %d", ch, led_channel.inverted);
    ESP_LOGI(TAG, "channel %d bright_tbl %d", ch, led_channel.bright_tbl);
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
        ESP_LOGI(TAG, "set duty %d for channel %d", ledc->channels[i].duty, ledc->channels[i].channel);
		pwm_set_duty( ledc->channels[i].channel, ledc->channels[i].duty );
	}	
	pwm_start();
}

// установить duty канала
void ledcontrol_set_duty(ledcontrol_channel_t *channel, uint16_t duty){
    channel->duty = duty;
    //ESP_LOGI(TAG, "*** channel %p, channel->channel %d duty %d", channel, channel->channel, duty);

    uint16_t real_duty = duty*period/MAX_DUTY;
    //ESP_LOGI(TAG, "set duty %d for channel %d (real duty %d)", channel->duty, channel->channel, real_duty);
    esp_err_t err = pwm_set_duty(channel->channel, real_duty);    
    //ESP_LOGI(TAG, esp_err_to_name(err));

    channel->duty = ledcontrol_get_duty(channel);
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
    //ESP_LOGI(TAG, __func__);
	pwm_start();

}

void ledcontrol_channel_on(ledcontrol_channel_t *channel){
    //ESP_LOGI(TAG, __func__);
    // set MAX DUTY
    ledcontrol_set_duty(channel, MAX_DUTY);
    ledcontrol_update();
}

void ledcontrol_channel_off(ledcontrol_channel_t *channel){
    //ESP_LOGI(TAG, __func__);
    // set MAX DUTY
    ledcontrol_set_duty(channel, 0);
    ledcontrol_update();
}

void ledcontrol_channel_next_duty(ledcontrol_channel_t *channel, uint8_t step)
{
    //ESP_LOGI(TAG, __func__);
    uint16_t duty = channel->duty;
    if ( duty <= MAX_DUTY - step ) 
        duty += step;
    else
        duty = MAX_DUTY;

    ledcontrol_set_duty(channel, duty);
}

void ledcontrol_channel_prev_duty(ledcontrol_channel_t *channel, uint8_t step)
{
    //ESP_LOGI(TAG, __func__);
    uint16_t duty = channel->duty;
    if ( duty >= step ) 
        duty -= step;
    else
        duty = 0;

    ledcontrol_set_duty(channel, duty);
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

static void ledcontrol_channel_fade_by_table(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay){

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
        
        ledcontrol_set_duty(channel, duty);
        ledcontrol_update();
        if ( direction == UP )
            i++;   // TODO: учесть brightness table
        else
            i--;  // TODO: учесть brightness table

        vTaskDelay( duty_delay / portTICK_RATE_MS );
    }
}   


void ledcontrol_channel_fade(ledcontrol_channel_t *channel, uint16_t duty_from, uint16_t duty_to, uint16_t duty_delay) {
    //ESP_LOGI(TAG, __func__);

 
    if ( channel->bright_tbl != TBL_NONE ) {
        ledcontrol_channel_fade_by_table(channel, duty_from, duty_to, duty_delay);
        return;
    }

    direction_e direction = (duty_from < duty_to) ? UP : DOWN;

    //ESP_LOGI(TAG, "duty from %d    duty to %d   direction %d", duty_from, duty_to, direction);
    
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
    //ESP_LOGI(TAG, __func__);
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

void ledcontrol_print_html_data(char *data){
    const char *ledc_data = "<div class=\"ledc\">"
                                "%s"
                                "</div>";
    
    char ledc_str[500] = "";
    for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {   
            char tmp[100];
            ESP_LOGI(TAG, "channel %d pin %d duty %d", ledc->channels[i].channel, ledc->channels[i].pin, ledc->channels[i].duty);
            sprintf(tmp, "<p><span><b>channel:</b> %d</span><span><b>duty:</b> %d</span></p>", i, ledc->channels[i].duty);        
            strcat(ledc_str, tmp);
    }

    sprintf(data+strlen(data), ledc_data, ledc_str);
                                   
}

esp_err_t ledcontrol_http_get_handler(httpd_req_t *req){
/*
    ip/ledc?ch=<channel>&duty=<duty>
    ip/ledc?ch=<channel>&on=1
    ip/ledc?ch=<channel>&off=1
    ip/ledc?ch=<channel>&step=<step>   // next duty
    ip/ledc?ch=<channel>&fade=1&from=<duty_from>&to=<duty_to>&delay=<duty_delay>
    ip/ledc?allon=1
    ip/ledc?alloff=1
*/
    ESP_LOGI(TAG, __func__);
    char page[200] = "";
    if ( http_get_has_params(req) == ESP_OK) {
        esp_err_t err = ESP_FAIL;
        char param[30];
        if ( http_get_key_str(req, "ch", param, sizeof(param)) == ESP_OK ) {
            uint8_t ch;
            ch = atoi(param);
            ESP_LOGI(TAG, "ch: %d", ch);
            if ( ch < 0 || ch > ledc->led_cnt /*LEDCONTROL_CHANNEL_MAX*/ ) {
                err = ESP_FAIL;
                goto end;
            }
            ledcontrol_channel_t *channel = ledc->channels + ch;
            ESP_LOGI(TAG, "get %d channel %d pin %d duty %d", ch, channel->channel, channel->pin, channel->duty);
            
            long val = 0;
            if ( http_get_key_long(req, "duty", &val) == ESP_OK ) {
                // channel->set_duty
                if ( val >= 0 && val <= MAX_DUTY ) {
                    ledc->set_duty( channel, val );
                    ledc->update();
                    err = ESP_OK;
                }
            } else if ( http_get_key_long(req, "on", &val) == ESP_OK ) {
                // channel > on
                if ( val == 1) {
                    ledc->on(channel);
                    err = ESP_OK;
                }
            } else if ( http_get_key_long(req, "off", &val) == ESP_OK ) {
                // channel > off
                if ( val == 1) {
                    ledc->off(channel);
                    err = ESP_OK;
                }
            } else if ( http_get_key_long(req, "step", &val) == ESP_OK ) {
                // channel > next_duty
                err = ESP_OK;
                if ( val > 0 ) {
                    ledc->next_duty(channel, val);    
                } else if ( val < 0 ) {
                    ledc->prev_duty(channel, val*(-1));   
                }               
            } else if ( http_get_key_long(req, "fade", &val) == ESP_OK ) {
                // channel > fade
                long from, to, delay;
                if ( http_get_key_long(req, "from", &from) == ESP_OK &&
                     http_get_key_long(req, "to", &to) == ESP_OK &&
                     http_get_key_long(req, "delay", &delay) == ESP_OK )
                {
                    ESP_LOGI(TAG, "fade from %li to %li delay %li", from, to, delay);
                    ledc->fade( channel, from, to, delay);
                    err = ESP_OK;
                }
            }
        } else if ( http_get_key_str(req, "allon", param, sizeof(param)) == ESP_OK ) {
            ledc->on_all();
        } else if ( http_get_key_str(req, "alloff", param, sizeof(param)) == ESP_OK ) {
            ledc->off_all();
        }
    }
end:    
    // show page data
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;              
}