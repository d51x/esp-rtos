#include "pwm.h"



static const char *TAG = "PWM";

static uint16_t period;

void pwm_begin(uint16_t freq_hz, uint8_t ch_cnt, const uint32_t *channels){

// Just freq_hz is useful
// Hz to period
	period = 1000000 / freq_hz;
    uint32_t *duties = malloc( sizeof(uint32_t) * ch_cnt);
    for (uint8_t i=0;i<ch_cnt;i++) duties[i] = period;

    ESP_LOGI(TAG, "%s started...", __func__);

    ESP_LOGI(TAG, "pwm_init...");
    pwm_init(period, duties, ch_cnt, channels);

    //ESP_LOGI(TAG, "pwm_set_phase...");
    int16_t *phases = malloc( sizeof(uint16_t) * ch_cnt);
    memset(phases, 0, sizeof(uint16_t) * ch_cnt);
    pwm_set_phases(phases);

    //pwm_set_channel_invert(0x01);
    ESP_LOGI(TAG, "pwm_start...");
    pwm_start();
    free(phases);
    free(duties);

}


void pwm_write(uint8_t ch, uint16_t duty) {
    xEventGroupWaitBits(ota_event_group, OTA_IDLE_BIT, false, true, portMAX_DELAY);
    uint16_t real_duty = duty*period/MAX_DUTY;
    pwm_set_duty(ch, real_duty);
    pwm_start();
}

uint16_t pwm_state(uint8_t ch) {
    uint32_t real_duty;
    pwm_get_duty(ch, &real_duty);
    return (real_duty * MAX_DUTY) / period;
}

void pwm_task(){

    pwm_begin(PWM_FREQ_HZ, PIN_PWM_CNT, pwm_pins);

    uint32_t duty = 0;
    uint8_t direction = 0;
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        pwm_write(0, duty);
        //pwm_write(1, duty);

        if ( direction == 0 ) {
            duty += STEP;
            if ( duty == MAX_DUTY ) {
                    direction = 1;
                    //vTaskDelay(2000 / portTICK_RATE_MS);
                    vTaskDelayUntil( &xLastWakeTime, ( 500 / portTICK_RATE_MS ) );
             }
             
        } else {
            duty -= STEP;
            if ( duty == 1 ) {
                    direction = 0;
                    //vTaskDelay(2000 / portTICK_RATE_MS);
                    vTaskDelayUntil( &xLastWakeTime, ( 500 / portTICK_RATE_MS ) );
            }
        }

        
        
        vTaskDelay(FADE_TIME / portTICK_RATE_MS);
    }
}