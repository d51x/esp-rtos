

#include "main.h"
static const char *TAG = "MAIN";


httpd_handle_t http_server = NULL;

void main_task(void *arg){

    while (1) {

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main(void){

    ESP_LOGI("*******  FW_VER: ", FW_VER);
    ESP_LOGI("*******  CORE_VER: ", CORE_FW_VER);



    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    OTA_IDLE_BIT = BIT7;
    ota_event_group = xEventGroupCreate();
    xEventGroupSetBits(ota_event_group, OTA_IDLE_BIT);

    #ifdef GPIO
        init_gpios();     // configure GPIO
    #endif
    
    #ifdef DEBUG
        print_chip_info();
    #endif

    // TODO: определить, это первый запуск или нет?

    // TODO: прочитать параметры wifi из nvs
    wifi_mode_t wf = WIFI_MODE_STA; //
    //wifi_mode_t wf = WIFI_MODE_AP; //
    wifi_init(wf); 
    webserver_init(http_server);

    #ifdef MQTT
        mqtt_start();
    #endif

    #ifdef SNTP
        sntp_start();
    #endif



    xTaskCreate(main_task, "main_task", 1024, NULL, 10, NULL);

    #define LED_CTRL_CNT        3
    #define LED_FREQ_HZ         500

    #define LED_CTRL_RED_CH     0
    #define LED_CTRL_GREEN_CH   1 
    #define LED_CTRL_BLUE_CH    2 
    #define LED_CTRL_WHITE_CH  3 

    #define LED_CTRL_RED_PIN    12
    #define LED_CTRL_GREEN_PIN  13
    #define LED_CTRL_BLUE_PIN   14
    #define LED_CTRL_WHITE_PIN  2

    led_ctrl_config_t leds[LED_CTRL_CNT];

    leds[0].pin = LED_CTRL_RED_PIN;
    leds[0].ch = LED_CTRL_RED_CH;
    leds[0].type = RED;
    leds[0].duty = 0;
    leds[0].step = 1;
    leds[0].fade_up_time = 40;
    leds[0].fade_down_time = 40;
    leds[0].bright_table = 1;
                 
    leds[1].pin = LED_CTRL_GREEN_PIN;
    leds[1].ch = LED_CTRL_GREEN_CH;
    leds[1].type = GREEN;
    leds[1].duty = 0;
    leds[1].step = 1;
    leds[1].fade_up_time = 40;
    leds[1].fade_down_time = 40;
    leds[1].bright_table = 1;
            
    leds[2].pin = LED_CTRL_BLUE_PIN;
    leds[2].ch = LED_CTRL_BLUE_CH;
    leds[2].type = BLUE;
    leds[2].duty = 0;
    leds[2].step = 1;
    leds[2].fade_up_time = 40;
    leds[2].fade_down_time = 40;
    leds[2].bright_table = 1;
            /*
    leds[3].pin = LED_CTRL_WHITE_PIN;
    leds[3].ch = LED_CTRL_WHITE_CH;
    leds[3].type = WHITE;
    leds[3].duty = 0;
    leds[3].step = 1;
    leds[3].fade_up_time = 40;
    leds[3].fade_down_time = 40;
    leds[3].bright_table = 1;
    */
    ledctrl_init(LED_FREQ_HZ, LED_CTRL_CNT, leds);



    
   

    button_handle_t btn_g4_h = configure_push_button(GPIO_NUM_4, BUTTON_ACTIVE_HIGH);
    
    if (btn_g4_h) {
        
        button_cb *pressed_cb = calloc(3, sizeof(button_cb));
        pressed_cb[0] = &press_1_cb;
        pressed_cb[1] = &press_2_cb;
        pressed_cb[2] = &press_3_cb;
 button_set_on_presscount_cb(btn_g4_h, 500, 3, pressed_cb);
        
        uint32_t pressed_time = 1;
        button_add_on_press_cb(btn_g4_h, pressed_time, hold_1s_cb, NULL);  

pressed_time = 2;   // off
        button_add_on_press_cb(btn_g4_h, pressed_time, rgb_lights_off, NULL);  
        
        pressed_time = 10;   // off
        button_add_on_press_cb(btn_g4_h, pressed_time, esp_restart, NULL);  

    } 
}







void IRAM_ATTR press_1_cb() {
    set_next_color_effect();
}

void hold_1s_cb()
{
    set_prev_color_effect();
}
        static uint16_t h = 0;
void press_2_cb() {
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "plus hsv: %d", h);
    color_hsv_t *hsv = malloc( sizeof(color_hsv_t));
    hsv->h = h; hsv->s = 100; hsv->v = 100;
    ledctrl_set_color_hsv(hsv, 1);
    if ( h <=354 )
        h += 5;
    else h = 359;
    free(hsv);
}

void press_3_cb() {
    ESP_LOGI(TAG, __func__);
    if ( h >= 5) h -= 5;
    else h = 0;    
    ESP_LOGI(TAG, "minus hsv: %d", h);
    color_hsv_t *hsv = malloc( sizeof(color_hsv_t));
    hsv->h = h; hsv->s = 100; hsv->v = 100;
    ledctrl_set_color_hsv(hsv, 1);

    free(hsv);
}