#include "main.h"

static const char *TAG = "MAIN";


httpd_handle_t http_server = NULL;


static void  init_relay() {
    relay_fan_h = relay_create(RELAY_FAN_PIN, RELAY_LEVEL_HIGH);
    relay_write(relay_fan_h,  RELAY_STATE_CLOSE);
}

static void init_led_rgb_controller() {
    ledcontrol_channel_t ch_red = {
        .pin = LED_CTRL_RED_PIN,
        .channel = LED_CTRL_RED_CH,
    };
    
    ledcontrol_channel_t ch_green = {
        .pin = LED_CTRL_GREEN_PIN,
        .channel = LED_CTRL_GREEN_CH,
    };

    ledcontrol_channel_t ch_blue = {
        .pin = LED_CTRL_BLUE_PIN,
        .channel = LED_CTRL_BLUE_CH,
    };

    ledcontrol_channel_t ch_white = {
        .pin = LED_CTRL_WHITE_PIN,
        .channel = LED_CTRL_WHITE_CH,
        .bright_tbl = TBL_32B,
        .inverted = true,
    };

    // ===== create led controller =============================
    ledcontrol_t* ledc_h = ledcontrol_create(LED_FREQ_HZ, LED_CTRL_CNT);
    ledc = (ledcontrol_t *)ledc_h;

    // ==== register led channels to led controller =============
    ledc->register_channel(ch_red);
    ledc->register_channel(ch_green);
    ledc->register_channel(ch_blue);
    ledc->register_channel(ch_white);

    // ====== initialize led controller =======================
    ledc->init();

    // ===== add uri handler of led controller ================
    add_uri_get_handler( http_server, ledc->uri, ledc->http_get_handler);

    // === create and init RGB controller ================
    rgb_ledc = rgbcontrol_init(ledc, &ch_red, &ch_green, &ch_blue);

    // ==== create and init rgb effects =======================
    effects = effects_init( rgb_ledc, rgb_ledc->set_color_hsv );

    // === setup effects to RGB controller 
    rgb_ledc->set_effects( effects );

    // === add uri handler of RGB controller ================
    add_uri_get_handler( http_server, rgb_ledc->uri, rgb_ledc->http_get_handler);
}

static void init_pir() {
    pir_conf_t pir_cfg = {
        .pin = PIR_PIN,                         // pin
        .interval_low = PIR_TIMER_CALLBACK_DELAY,   // interval for callback timer after HIGH / LOW trigger occurs
        .interval_high = PIR_TIMER_CALLBACK_DELAY,   // interval for callback timer after HIGH / LOW trigger occurs
        .type = PIR_ISR,                        // use interrupt (PIR_ISR) or polling gpio (ISR_POLL and .interval of loop ( msec ))
        .active_level = PIR_LEVEL_ANY,         // type of interrupt
        //.high_cb = pir_high_cb,                    // high_cb function when interrupt triggering with HIGH level GPIO
        .low_cb = pir_low_cb,                    // high_cb function when interrupt triggering with HIGH level GPIO
        .tmr_low_cb = pir_timer_low_cb,                      // tmr_cb function occurs when timer end after interrupt
        //.tmr_high_cb = pir_timer_high_cb,                      // tmr_cb function occurs when timer end after interrupt
    };

    pir_h = pir_init(pir_cfg);
    pir_t *pir = (pir_t *)pir_h;
    pir->enable( pir_h );
}


static void init_ir_receiver() {
    ir_rx_h = irrcv_init(IR_RECEIVER_PIN, IR_RECEIVE_DELAY, IR_RECEIVER_BUTTONS_COUNT);

    if ( ir_rx_h == NULL ) {
        ESP_LOGE(TAG, "FAIL: ir receiver not created.");
        return;
    }

    for (uint8_t i = 0; i < IR_RECEIVER_BUTTONS_COUNT; i++) {
        ir_btn_t btn = ir_buttons[i];
        irrcv_add_button(ir_rx_h, 	 i, btn.code, btn.user_ctx, btn.cb);
    }

    irrcv_start( ir_rx_h );
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

    wifi_mode_t wf = WIFI_MODE_STA; //
    wifi_init(wf); 
    webserver_init(&http_server);

    // todo wait wifi connected bit
    mqtt_start();
    sntp_start();

    init_led_rgb_controller();
    init_relay();
    init_pir();
    init_ir_receiver();
}



void pir_low_cb(void *arg) {
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "PIR active LOW");
}

void pir_high_cb(void *arg) {
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "PIR active HIGH");
}

void pir_timer_low_cb(void *arg) {
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "PIR timer low end");
}

void pir_timer_high_cb(void *arg) {
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "PIR timer high end");
}