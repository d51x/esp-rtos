

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

   
    #ifdef DEBUG
        print_chip_info();
    #endif

    // TODO: определить, это первый запуск или нет?

    // TODO: прочитать параметры wifi из nvs
    wifi_mode_t wf = WIFI_MODE_STA; //
    //wifi_mode_t wf = WIFI_MODE_AP; //
    wifi_init(wf); 
    webserver_init(&http_server);
    ESP_LOGI(TAG, "http_server is %s", (http_server != NULL) ? "not NULL" : "NULL");

    #ifdef MQTT
        mqtt_start();
    #endif

    #ifdef SNTP
        sntp_start();
    #endif



    xTaskCreate(main_task, "main_task", 1024, NULL, 10, NULL);

//    #define LED_CTRL_CNT        3
    #define LED_CTRL_CNT        1
    #define LED_FREQ_HZ         500

    // #define LED_CTRL_RED_CH     0
    // #define LED_CTRL_GREEN_CH   1 
    #define LED_CTRL_GREEN_CH   0 
    // #define LED_CTRL_BLUE_CH    2 
    // #define LED_CTRL_WHITE_CH  3 

    // #define LED_CTRL_RED_PIN    15
     #define LED_CTRL_GREEN_PIN  12
    // #define LED_CTRL_BLUE_PIN   13
    // #define LED_CTRL_WHITE_PIN  2

    // ledcontrol_channel_t ch_red = {
    //     .pin = LED_CTRL_RED_PIN,
    //     .channel = LED_CTRL_RED_CH,
    // };
    
    ledcontrol_channel_t ch_green = {
        .pin = LED_CTRL_GREEN_PIN,
        .channel = LED_CTRL_GREEN_CH,
    };

    // ledcontrol_channel_t ch_blue = {
    //     .pin = LED_CTRL_BLUE_PIN,
    //     .channel = LED_CTRL_BLUE_CH,
    // };

    ledcontrol_t* ledc_h = ledcontrol_create(LED_FREQ_HZ, LED_CTRL_CNT);
    ledc = (ledcontrol_t *)ledc_h;

    //ledc->register_channel(ch_red);
    ledc->register_channel(ch_green);
    //ledc->register_channel(ch_blue);


//ESP_LOGI(TAG, "### red addr %p", &ch_red);
//ESP_LOGI(TAG, "### green addr %p", &ch_green);
//ESP_LOGI(TAG, "### blue addr %p", &ch_blue);

    //ledc->init();
    //add_uri_get_handler( http_server, ledc->uri, ledc->http_get_handler);

    //rgb_ledc = rgbcontrol_init(ledc, ch_red, ch_green, ch_blue);
    //rgb_ledc = rgbcontrol_init(ledc, &ch_red, &ch_green, &ch_blue);
    
//ESP_LOGI(TAG, "### red addr %p", &ch_red);
//ESP_LOGI(TAG, "### green addr %p", &ch_green);
//ESP_LOGI(TAG, "### blue addr %p", &ch_blue);

    //effects = effects_init( rgb_ledc, rgb_ledc->set_color_hsv );
    ///rgb_ledc->set_effects( effects );

    //ESP_LOGI(TAG, "rgb_ledc->http_get_handler addr: %p", rgb_ledc->http_get_handler);
    //add_uri_get_handler( http_server, rgb_ledc->uri, rgb_ledc->http_get_handler);

    // button_handle_t btn_g4_h = configure_push_button(GPIO_NUM_4, BUTTON_ACTIVE_HIGH);
    // if (btn_g4_h) {
    //     int pressed_cnt = 3;
    //     button_cb *pressed_cb = calloc(pressed_cnt, sizeof(button_cb));
    //     pressed_cb[0] = &btn4_press_1_cb;  
    //     pressed_cb[1] = &btn4_press_2_cb;   
    //     pressed_cb[2] = &btn4_press_3_cb;   
    //     button_set_on_presscount_cb(btn_g4_h, 500, pressed_cnt, pressed_cb);
    //     button_add_on_press_cb(btn_g4_h, 1, btn4_hold_1s_cb, NULL); 
    // }

    // button_handle_t btn_g0_h = configure_push_button(GPIO_NUM_0, BUTTON_ACTIVE_HIGH);
    // if (btn_g0_h) {
    //     int pressed_cnt = 3;
    //     button_cb *pressed_cb = calloc(pressed_cnt, sizeof(button_cb));
    //     pressed_cb[0] = &btn0_press_1_cb;  
    //     pressed_cb[1] = &btn0_press_2_cb;   
    //     pressed_cb[2] = &btn0_press_3_cb;   
    //     button_set_on_presscount_cb(btn_g0_h, 500, pressed_cnt, pressed_cb);
    //     button_add_on_press_cb(btn_g0_h, 1, btn0_hold_1s_cb, NULL); 
    // }

    //relay02 = relay_create(2, RELAY_LEVEL_HIGH);
    //relay12 = relay_create(12, RELAY_LEVEL_LOW);
    //relay13 = relay_create(13, RELAY_LEVEL_LOW);
    //relay15 = relay_create(15, RELAY_LEVEL_LOW);

    //relays[0] = relay02;
    //relays[1] = relay12;
    //relays[2] = relay13;
    //relays[3] = relay15;
    // for (int i=0;i<4;i++){   // check for null
    //     relay_write(relays[i], RELAY_STATE_CLOSE);
    // }
    relay_write(relay12, RELAY_STATE_CLOSE);


    encoder_config_t enc_cfg = {
        .pin_btn = 13,
        .pin_clk = 4,	
        .pin_dt = 0,
        .left = btn4_press_1_cb,
	    .cb_left_ctx = "left",
	    .right = btn0_press_1_cb,
	    .cb_right_ctx = "right",
	    .press = btn4_press_2_cb,
	    .cb_press_ctx = "press",
    };

    enc_h = encoder_init(enc_cfg);

}





// будет поочереди, чтобы одновременно, надо запускать tasks
void btn4_press_1_cb() {
    ESP_LOGI(TAG, __func__);
    //relay_write(relays[1], RELAY_STATE_OPEN );
    //relay_write(relay12, RELAY_STATE_OPEN );
}

void btn4_hold_1s_cb() {

}

void btn4_press_2_cb() {
    //relay_write(relays[1], RELAY_STATE_CLOSE );
}

void btn4_press_3_cb() {

}

void btn0_press_1_cb() {
    ESP_LOGI(TAG, __func__);
    //relay_write(relays[2], RELAY_STATE_OPEN );
    //relay_write(relay12, RELAY_STATE_CLOSE );
}

void btn0_hold_1s_cb() {

}

void btn0_press_2_cb() {
    //relay_write(relays[2], RELAY_STATE_CLOSE );
}

void btn0_press_3_cb() {

}


