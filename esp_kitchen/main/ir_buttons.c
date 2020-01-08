#include "ir_buttons.h"

static const char *TAG = "IRBTN";

void ir_btn_all_off(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);

    white_led_smooth_off();
    // fan off
    relay_write(relay_fan_h,  RELAY_STATE_CLOSE);
    // rgb off
    effects_t *ef = (effects_t *) rgb_ledc->effects;
    if ( ef != NULL ) ef->stop();       
    rgb_ledc->set_color_int(0);
}

void ir_btn_fan_ctrl(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
    uint8_t state = relay_read(relay_fan_h);
    relay_write(relay_fan_h,  !state );
}

void ir_btn_color_effect(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
    char name[20]; 
    strcpy(name,(char *)arg);
    if ( strcmp(name, "nextrndfade") == ESP_OK) {
        static uint8_t rnd_fade = 0;
        if ( rnd_fade == 0 ) {
            strcpy(name, "rndfade7");
        } else {
            strcpy(name, "rndfade12");
        }
        ESP_LOGI(TAG, "%s %d", name, rnd_fade);
        rnd_fade = ! rnd_fade;
    } else if ( strcmp(name, "nextrndjump") == ESP_OK ) {
        static uint8_t rnd_jump = 0;
        if ( rnd_jump == 0 ) {
            strcpy(name, "rndjump7");
        } else {
            strcpy(name, "rndjump12");
        }
        ESP_LOGI(TAG, "%s %d", name, rnd_jump);
        rnd_jump = ! rnd_jump;
    }

    effects_t *ef = (effects_t *) rgb_ledc->effects;
    if ( ef != NULL )                   
        ef->set_by_name( name );    
}


void ir_btn_speed_ctrl(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
    // ??????

}

void ir_btn_brightness_dec(void *arg) {
    uint8_t ch = (uint8_t)arg;
    uint32_t duty = 0;
    if ( ch == LED_CTRL_WHITE_CH) {
        duty = ledc->get_duty( ledc->channels + ch);    
        if ( duty > 0) duty--;
        ledc->set_duty( ledc->channels + ch, duty);  
        ledc->update();         
    } else {
        duty = rgb_ledc->ledc->get_duty( rgb_ledc->ledc->channels + ch);
        if ( duty > 0) duty--;
        rgb_ledc->ledc->set_duty( rgb_ledc->ledc->channels + ch, duty);
        rgb_ledc->ledc->update();        
    }      
}

void ir_btn_brightness_inc(void *arg) {
    uint8_t ch = (uint8_t)arg;
    uint32_t duty = 0;
    if ( ch == LED_CTRL_WHITE_CH) {
        duty = ledc->get_duty( ledc->channels + ch);    
        if ( duty < MAX_DUTY ) duty++;
        ledc->set_duty( ledc->channels + ch, duty);  
        ledc->update();         
    } else {
        duty = rgb_ledc->ledc->get_duty( rgb_ledc->ledc->channels + ch);
        if ( duty < MAX_DUTY ) duty++;
        rgb_ledc->ledc->set_duty( rgb_ledc->ledc->channels + ch, duty);
        rgb_ledc->ledc->update();        
    } 
}

void ir_btn_set_color(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
    char *hex = (char *)arg;
    effects_t *ef = (effects_t *) rgb_ledc->effects;
    if ( ef != NULL ) ef->stop();       
    rgb_ledc->set_color_hex( hex );    
}

void ir_btn_led_ctrl(void *arg) {
    
    uint8_t ch = (uint8_t)arg;
    ESP_LOGI(TAG, "%s\targ: %d", __func__, ch);

    uint32_t duty = 0;
    duty = ledc->get_duty( ledc->channels + ch);
    if ( duty > 0)
        if ( ch == LED_CTRL_WHITE_CH )
            white_led_smooth_off();
        else {
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL ) ef->stop();            
            ledc->off(ledc->channels + ch);
        }
    else
        if ( ch == LED_CTRL_WHITE_CH )
            white_led_smooth_on();    
        else {    
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL ) ef->stop();            
            ledc->on(ledc->channels + ch);
        }
}
