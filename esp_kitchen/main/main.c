#include "main.h"

static const char *TAG = "MAIN";

#define CHECK_PIR() {    if ( millis() < 10*1000 ) {\
                        ESP_LOGE(TAG, "PIR is not yet initialized (hardware)"); \
                        return; \
                        }    \
                    }
httpd_handle_t http_server = NULL;


static void  init_relay() {
    //relay_fan_h = relay_create(RELAY_FAN_PIN, RELAY_LEVEL_HIGH);
    //relay_fan_h = relay_create( relay_fan_pin, RELAY_LEVEL_LOW);
    relay_fan_h = relay_create( relay_fan_pin, relay_invert ? RELAY_LEVEL_LOW : RELAY_LEVEL_HIGH);
    relay_write(relay_fan_h,  RELAY_STATE_CLOSE);
    relay_add_mqtt_send_cb(relay_fan_h, mqtt_send_gpio);
}

static void init_led_rgb_controller() {
    ledcontrol_channel_t *ch_red = calloc(1, sizeof(ledcontrol_channel_t));
    //ch_red->pin = LED_CTRL_RED_PIN;
    ch_red->pin = main_led_pins[LED_CTRL_RED_CH];
    ch_red->channel = LED_CTRL_RED_CH;
    
    ledcontrol_channel_t *ch_green = calloc(1, sizeof(ledcontrol_channel_t));
    //ch_green->pin = LED_CTRL_GREEN_PIN;
    ch_green->pin = main_led_pins[LED_CTRL_GREEN_CH];
    ch_green->channel = LED_CTRL_GREEN_CH;

    ledcontrol_channel_t *ch_blue = calloc(1, sizeof(ledcontrol_channel_t));
    //ch_blue->pin = LED_CTRL_BLUE_PIN;
    ch_blue->pin = main_led_pins[LED_CTRL_BLUE_CH];
    ch_blue->channel = LED_CTRL_BLUE_CH;

    ledcontrol_channel_t *ch_white = calloc(1, sizeof(ledcontrol_channel_t));
    //ch_white->pin = LED_CTRL_WHITE_PIN;
    ch_white->pin = main_led_pins[LED_CTRL_WHITE_CH];
    ch_white->channel = LED_CTRL_WHITE_CH;
    ch_white->bright_tbl = TBL_32B;
    

    // ===== create led controller =============================
    ledcontrol_t* ledc_h = ledcontrol_create(LED_FREQ_HZ, LED_CTRL_CNT);
    ledc = (ledcontrol_t *)ledc_h;

    // ==== register led channels to led controller =============
    ledc->register_channel(*ch_red);
    ledc->register_channel(*ch_green);
    ledc->register_channel(*ch_blue);
    ledc->register_channel(*ch_white);

    // ====== initialize led controller =======================
    ledc->init();

    // ===== add uri handler of led controller ================
    add_uri_get_handler( http_server, ledc->uri, ledc->http_get_handler);

    // === create and init RGB controller ================


    rgb_ledc = rgbcontrol_init(ledc, ch_red, ch_green, ch_blue);

    // ==== create and init rgb effects =======================
    effects = effects_init( rgb_ledc, rgb_ledc->set_color_hsv );

    // === setup effects to RGB controller 
    rgb_ledc->set_effects( effects );

    // === add uri handler of RGB controller ================
    add_uri_get_handler( http_server, rgb_ledc->uri, rgb_ledc->http_get_handler);
    rgbcontrol_set_mqtt_send_cb( mqtt_extern_publish );
}

static void init_pir() {
    pir_conf_t pir_cfg;
    
    //pir_cfg.pin = PIR_PIN,                        
    pir_cfg.pin = pirpin;                        
    pir_cfg.interval_low = pir_timer_off_delay;
    pir_cfg.type = PIR_ISR;                       
    pir_cfg.active_level = PIR_LEVEL_ANY;        
    pir_cfg.high_cb = pir_high_cb;                 // определим начало движения и выставим переменные, плавно включим подсветку, если она не включена           
    pir_cfg.low_cb = pir_low_cb;                   // определим завершение движения и обновим переменные
    pir_cfg.tmr_low_cb = pir_timer_low_cb;         // запустим плавное выключение ленты      
        //.tmr_high_cb = pir_timer_high_cb,                     
    
    pir_h = pir_init(pir_cfg);
    pir_t *pir = (pir_t *)pir_h;
    if ( is_pir_enabled )
            pir->enable( pir_h );
    else
        pir->disable( pir_h );
}


static void init_ir_receiver() {
    //ir_rx_h = irrcv_init(IR_RECEIVER_PIN, IR_RECEIVE_DELAY, IR_RECEIVER_BUTTONS_COUNT);
    ir_rx_h = irrcv_init(ir_pin, ir_delay, IR_RECEIVER_BUTTONS_COUNT);

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

    // TODO: get options

    OTA_IDLE_BIT = BIT7;
    ota_event_group = xEventGroupCreate();
    xEventGroupSetBits(ota_event_group, OTA_IDLE_BIT);

    wifi_mode_t wf = WIFI_MODE_STA; //
    wifi_init(wf); 
    webserver_init(&http_server);

    // todo wait wifi connected bit
    mqtt_start();
    
    sntp_start();


    load_params();


    init_led_rgb_controller();
    init_relay();
    init_pir();
    init_ir_receiver();

}


static void count_up_cb(xTimerHandle tmr) {
    uint32_t *p = (uint32_t *) pvTimerGetTimerID(tmr);
    count_up_motion = *p;
    count_up_motion++;
}

static void count_down_cb(xTimerHandle tmr) {
    uint32_t *p = (uint32_t *) pvTimerGetTimerID(tmr);
    count_down_off = *p;
    count_down_off--;
}

static void stop_timer(){
    if ( tmr_cnt ) {
        xTimerStop( tmr_cnt, 0);
        xTimerDelete( tmr_cnt, 0);
        tmr_cnt = NULL;
    }
    count_down_off = pir_timer_off_delay;
    count_up_motion = 0;
}

static void start_count_up(){
    stop_timer();
    tmr_cnt = xTimerCreate("tmr_cnt", 1000 / portTICK_PERIOD_MS, pdTRUE, &count_up_motion, count_up_cb);	
    xTimerStart(tmr_cnt, 0);		
}

static void start_count_down(){
    stop_timer();
    tmr_cnt = xTimerCreate("tmr_cnt", 1000 / portTICK_PERIOD_MS, pdTRUE, &count_down_off, count_down_cb);
    xTimerStart(tmr_cnt, 0);	
}



void pir_low_cb(void *arg) {
    CHECK_PIR();
    if ( !is_motion ) return;
    is_motion = false;
    mqtt_send_pir();

    // start countdown
    start_count_down();
}

void pir_high_cb(void *arg) {
    CHECK_PIR();
    is_motion = true;
    mqtt_send_pir();

    start_count_up();

    if ( !is_white_led_auto ) return;
    white_led_smooth_on();
}

void pir_timer_low_cb(void *arg) {
    CHECK_PIR();
    stop_timer();
    if ( !is_white_led_auto ) return;
    white_led_smooth_off();
}

void white_led_smooth_off(){
    uint32_t duty = 0;
    duty = ledc->get_duty( ledc->channels + LED_CTRL_WHITE_CH);
    ledc->fade_to_off( ledc->channels + LED_CTRL_WHITE_CH, duty, white_led_fadeout_delay);
}

void white_led_smooth_on(){
    uint32_t duty = 0;
    duty = ledc->get_duty( ledc->channels + LED_CTRL_WHITE_CH);
    if (  duty < white_led_max_duty )  
        ledc->fade( ledc->channels + LED_CTRL_WHITE_CH, duty, white_led_max_duty, white_led_fadeup_delay);
}

void mqtt_send_gpio(const char *topic, const char *payload){
    mqtt_extern_publish(topic, payload);
}

void mqtt_send_pir(){
    char payload[2];
    itoa(is_motion, payload, 10);
    mqtt_extern_publish("pir", payload);
}


static bool check_adc_dark() {
    return get_adc() < adc_lvl;
}

bool get_dark_mode(pir_mode_t mode) {
    bool res = false;
    if ( mode == PIR_MODE_SUSNSET) {
        res = is_sunset; 
    } else if (mode == PIR_MODE_MIX ) {
        res = is_sunset || check_adc_dark();
    } else if ( mode == PIR_MODE_DLR ) {
        res = check_adc_dark(); 
    } else {
        res = true;
    }  
    return res;
}

void load_params(){

    esp_err_t err;

    err = nvs_param_load("main", "channels_pin", main_led_pins); //, LED_CTRL_MAX*sizeof(uint8_t)); 
    if ( err != ESP_OK ) memset(main_led_pins, 255, LED_CTRL_MAX*sizeof(uint8_t)) ;

    err = nvs_param_u8_load("main", "fanpin",        &relay_fan_pin); 
    if ( err != ESP_OK ) relay_fan_pin = RELAY_FAN_PIN;    
    
    err = nvs_param_u8_load("main", "faninv",        &relay_invert); 
    if ( err != ESP_OK ) relay_invert = RELAY_LEVEL_HIGH;
    
    err = nvs_param_u8_load("main", "pirpin",        &pirpin); 
    if ( err != ESP_OK ) pirpin = PIR_PIN;

    err = nvs_param_u8_load("main", "irpin",        &ir_pin); 
    if ( err != ESP_OK ) ir_pin = IR_RECEIVER_PIN;    
    
    err = nvs_param_u16_load("main", "irdelay",        &ir_delay); 
    if ( err != ESP_OK ) ir_delay = IR_RECEIVE_DELAY;
    
    err = nvs_param_u16_load("main", "adclvl",        &adc_lvl); 
    if ( err != ESP_OK ) adc_lvl = DEFAULT_ADC_LEVEL;
    
    

    err = nvs_param_u8_load("pir", "enabled",        &is_pir_enabled);           // is_pir_enabled = true;
    if ( err != ESP_OK ) is_pir_enabled = true;

    err = nvs_param_u8_load("pir", "mode",           &pir_mode);                 // pir_mode = PIR_MODE_MIX; 
    if ( err != ESP_OK ) pir_mode = PIR_MODE_MIX;

    err = nvs_param_u16_load("pir", "piroffdelay",    &pir_timer_off_delay);
    if ( err != ESP_OK )  pir_timer_off_delay = PIR_TIMER_CALLBACK_DELAY;

    err = nvs_param_u16_load("pir", "fadeupdelay",    &white_led_fadeup_delay);
    if ( err != ESP_OK ) white_led_fadeup_delay = WHITE_LED_FADEUP_DELAY; 

    err = nvs_param_u16_load("pir", "fadedowndelay",  &white_led_fadeout_delay);
    if ( err != ESP_OK ) white_led_fadeout_delay = WHITE_LED_FADEDOWN_DELAY;

    is_motion = false;
    is_sunset = false;
    is_dark = get_dark_mode( pir_mode );
    white_led_max_duty = MAX_DUTY; 

    is_white_led_auto = is_pir_enabled & is_dark;

    strcpy(pir_mode_desc[PIR_MODE_NONE],    "Всегда включать");
    strcpy(pir_mode_desc[PIR_MODE_SUSNSET], "По закату");
    strcpy(pir_mode_desc[PIR_MODE_DLR],     "По датчику");
    strcpy(pir_mode_desc[PIR_MODE_MIX],     "Комбинированный");

}