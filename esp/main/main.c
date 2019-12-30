

#include "main.h"
static const char *TAG = "MAIN";


httpd_handle_t http_server = NULL;

void main_task(void *arg){

    while (1) {

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void read_gpio_task(void *arg){
        

        while (1) {
        
            uint8_t val;
            val = gpio_get_level(0);
            if ( val == 0 ) {
                ESP_LOGI(TAG, "Button on gpio0 was pressed");            
            }
            
        
            val = gpio_get_level(4);
            if ( val == 1 ) {
                ESP_LOGI(TAG, "Button on gpio4 was pressed");
            }
            vTaskDelay(300/ portTICK_RATE_MS);
        
        }
}

void read_sensors_task(void *arg){
    

 #ifdef DS18B20   
    ds18b20_init(DS18B20_PIN);
    ds18b20[0].addr[0] = 0x28; ds18b20[0].addr[1] = 0xFF; ds18b20[0].addr[2] = 0x81; ds18b20[0].addr[3] = 0xE9; ds18b20[0].addr[4] = 0x74; ds18b20[0].addr[5] = 0x16; ds18b20[0].addr[6] = 0x03; ds18b20[0].addr[7] = 0x41;
    ds18b20[1].addr[0] = 0x28; ds18b20[1].addr[1] = 0xFF; ds18b20[1].addr[2] = 0x05; ds18b20[1].addr[3] = 0xC6; ds18b20[1].addr[4] = 0x74; ds18b20[1].addr[5] = 0x16; ds18b20[1].addr[6] = 0x04; ds18b20[1].addr[7] = 0xFC;
    ds18b20[2].addr[0] = 0x28; ds18b20[2].addr[1] = 0xFF; ds18b20[2].addr[2] = 0x6D; ds18b20[2].addr[3] = 0x19; ds18b20[2].addr[4] = 0x75; ds18b20[2].addr[5] = 0x16; ds18b20[2].addr[6] = 0x04; ds18b20[2].addr[7] = 0xE3;
#endif

#ifdef DHT
    dht.pin = DHT_PIN;
    dht.type = DHT22;
    dht_init(&dht);

    dht2.pin = DHT2_PIN;
    dht2.type = DHT22;
    dht_init(&dht2);
#endif

    while (1) {
        xEventGroupWaitBits(ota_event_group, OTA_IDLE_BIT, false, true, portMAX_DELAY);

#ifdef DHT
        if ( dht_read(&dht) == ESP_OK) {
            ESP_LOGD(TAG, "DHT Temp: %.2f\t\tHumy: %.2f\t\t(gpio %d)", dht.temp, dht.hum, dht.pin);
        } else {
            ESP_LOGE(TAG, "DHT (gpio%d) data read error", dht.pin);
        }   

        vTaskDelay(5000 / portTICK_RATE_MS);

        if ( dht_read(&dht2) == ESP_OK) {
            ESP_LOGD(TAG, "DHT Temp: %.2f\t\tHumy: %.2f\t\t(gpio %d)", dht2.temp, dht2.hum, dht2.pin);
        } else {
            ESP_LOGE(TAG, "DHT (gpio%d) data read error", dht2.pin);
        }   

        vTaskDelay(5000 / portTICK_RATE_MS);
#endif

#ifdef DS18B20
        for (uint8_t i=0;i<DSW_COUNT;i++) {
			float temp;
			if ( ds18b20[i].addr[0] ) 
            {
				if (ds18b20_getTemp(ds18b20[i].addr, &temp) == ESP_OK) 
                {
				    if ( temp != 125.f) ds18b20[i].temp = temp;	
				    ESP_LOGD(TAG, "addr "DSW_ADDR_PATTERN"  temp: %.2f C", 
                                                    ds18b20[i].addr[0], ds18b20[i].addr[1], ds18b20[i].addr[2], ds18b20[i].addr[3], 
                                                    ds18b20[i].addr[4], ds18b20[i].addr[5], ds18b20[i].addr[6], ds18b20[i].addr[7], 
                                                    ds18b20[i].temp);
                } else {
                    ESP_LOGE(TAG, "DS18B20 ("DSW_ADDR_PATTERN") getting temperature FAILED!!!!", 
                                                    ds18b20[i].addr[0], ds18b20[i].addr[1], ds18b20[i].addr[2], ds18b20[i].addr[3], 
                                                    ds18b20[i].addr[4], ds18b20[i].addr[5], ds18b20[i].addr[6], ds18b20[i].addr[7]);
                }                                                            
			}
		}
#endif
        vTaskDelay(5000 / portTICK_RATE_MS);
     
    }
}

void load_params_from_nvs() {
   /*
   sda = scl = 254;
    // load from nvs 
    nvs_handle i2c_handle;
    if ( nvs_open("i2c", NVS_READONLY, &i2c_handle) == ESP_OK) {
        ESP_LOGD(TAG, "NVS i2c section open success");
        if ( nvs_get_u8(i2c_handle, "sda", &sda) == ESP_OK) {
            ESP_LOGD(TAG, "NVS i2c.sda param read success: %d", sda);
        } else {
            ESP_LOGE(TAG, "NVS i2c.sda param read error");
            ESP_LOGE(TAG, "NVS i2c.sda = %d", sda);
        }

        if ( nvs_get_u8(i2c_handle, "scl", &scl) == ESP_OK) {
            ESP_LOGD(TAG, "NVS i2c.scl param read success: %d", scl);
        } else {
            ESP_LOGE(TAG, "NVS i2c.scl param read error");
            ESP_LOGE(TAG, "NVS i2c.scl = %d", scl);
        }

        if ( nvs_get_u16(i2c_handle, "ota_buf_size", &ota_upload_buf_size) == ESP_OK) {
            ESP_LOGI(TAG, "NVS ota_upload_buf_size param read success: %d", ota_upload_buf_size);
        } else {
            ESP_LOGE(TAG, "NVS ota_upload_buf_size param read error");
            ESP_LOGE(TAG, "NVS ota_upload_buf_size = %d", ota_upload_buf_size);
            ota_upload_buf_size = UPLOAD_BUFFER_SIZE;
        }


    } else {
        ESP_LOGE(TAG, "NVS i2c section open error");
    }
    nvs_close(i2c_handle);
*/    
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

    load_params_from_nvs();
 

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

#ifdef DS18B20    
    // TODO: при запуске задачи на определение ИД датчиков задачу чтения температуры надо приостановить через event group bit (аналог wifi)
    //xTaskCreate(ds18b20_search_task, "ds18b20_search_task", 2048, NULL, 10, NULL);
    // TODO: в таске читать данные только по указанным ид датчикам, ид датчиков определять отдельно через страничку
    //xTaskCreate(ds18b20_get_temp_task, "ds18b20_get_temp_task", 2048, NULL, 10, NULL);
#endif

#ifdef DHT
   // xTaskCreate(dht_task, "dht_task", 2048, NULL, 10, NULL);
#endif

#ifdef PWM        
    //xTaskCreate(pwm_task, "pwm_task", 2048, NULL, 10, NULL);

#endif

    xTaskCreate(ir_receiver_task, "ir_receiver_task", 2048, NULL, 10, NULL); 
    xTaskCreate(read_sensors_task, "read_sensors_task", 2048, NULL, 10, NULL); 
    //xTaskCreate(read_gpio_task, "read_gpio_task", 2048, NULL, 10, NULL); 

    #define LED_CTRL_CNT        3
    #define LED_FREQ_HZ         500

    #define LED_CTRL_RED_CH     0
    #define LED_CTRL_GREEN_CH   1 
    #define LED_CTRL_BLUE_CH    2 
    #define LED_CTRL_WHITE_CH  3 

    #define LED_CTRL_RED_PIN    15
    #define LED_CTRL_GREEN_PIN  12
    #define LED_CTRL_BLUE_PIN   13
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
   
    xTaskCreate(color_effect_message_task, "color_effect_task", 2048, NULL, 10, NULL);

    button_handle_t btn_g4_h = configure_push_button(GPIO_NUM_4, BUTTON_ACTIVE_HIGH);
    
    if (btn_g4_h) {
        button_cb *pressed_cb = calloc(3, sizeof(button_cb));
        //button_cb pressed_cb[3];   // not working
        pressed_cb[0] = &press_1_cb;
        pressed_cb[1] = &press_2_cb;
        pressed_cb[2] = &press_3_cb;

        button_set_on_presscount_cb(btn_g4_h, 500, 3, pressed_cb);
        // free(pressed_cb);   // do not free here, only when deinstall callback
        uint32_t pressed_time = 2;
        button_add_on_press_cb(btn_g4_h, pressed_time, rgb_lights_off, NULL);  

        pressed_time = 10;   // off
        button_add_on_press_cb(btn_g4_h, pressed_time, esp_restart, NULL);  
    } 

}

void color_effect_message_task(void *arg){
    if (xColorEffectQueue == NULL ) {
        xColorEffectQueue = xQueueCreate(2, sizeof(uint8_t));
    }
    uint8_t effect_id;
    for( ;; ) {
        if ( xQueueReceive( xColorEffectQueue, &effect_id, (portTickType)portMAX_DELAY ) == pdPASS ) {
            handle_color_effect_default_by_id(effect_id);
        } 
        taskYIELD();  
    }
}


static void switch_effect() {
    static uint8_t effect_id = 0;
    ESP_LOGI(TAG, "effect: %d", effect_id);
    //xQueueSendToFront( xColorEffectQueue, ( void * ) &effect_id, 0); 
    handle_color_effect_default_by_id(effect_id);
    effect_id++;
    if (effect_id >= COLOR_EFFECTS_MAX) effect_id = 0;
}



void ir_receiver_task(void *arg) {
	
    ir_rx.pin = IR_RX_PIN;
	ir_receiver_init(&ir_rx);
	
	while (1) {
		xEventGroupWaitBits(ota_event_group, OTA_IDLE_BIT, false, true, portMAX_DELAY);
		ir_rx_enable();
		if ( ir_receiver_get(&ir_rx.code) == ESP_OK ) {
			ir_rx_disable();
			ESP_LOGI(TAG, "ir rx nec data total int:  %d\t\t0x%08X", (int)ir_rx.code, (int)ir_rx.code); 
		}
		vTaskDelay(100 / portTICK_RATE_MS);

	}
	vTaskDelete(NULL);
}

// ===================== buttons callbacks ======================================
void IRAM_ATTR press_1_cb() {
    ESP_LOGI(TAG, __func__);
    switch_effect();
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
