#include "main.h"


static const char *TAG = "MAIN";

void test1(char *buf, void *args);
void test2(char *buf, void *args);

void test_recv1(char *buf, void *args);
void test_recv2(char *buf, void *args);

#ifdef CONFIG_MCP23017_ISR
void test_mcp23017_isr_cb1(char *buf);
void test_mcp23017_isr_cb2(char *buf);
void test_mcp23017_isr_cb3(char *buf);
void test_mcp23017_isr_cb4(char *buf);
void test_mcp23017_isr_cb5(char *buf);
void test_mcp23017_isr_cb6(char *buf);
void test_mcp23017_isr_cb7(char *buf);
void test_mcp23017_isr_cb8(char *buf);
#endif

httpd_handle_t http_server = NULL;

void app_main(void)
{
	//ESP_ERROR_CHECK(nvs_flash_init());

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    //if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //  ESP_ERROR_CHECK(nvs_flash_erase());
    //  ret = nvs_flash_init();
    //}
    ESP_ERROR_CHECK(ret);

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
	
	ESP_LOGI(TAG, "Freemem: %d", esp_get_free_heap_size());
	ESP_LOGI(TAG, "SDK: %s", esp_get_idf_version());
	
    // ========================================= MODULES initialization START
    initialize_modules();
    user_setup(NULL);

    wifi_init();
    sntp_start();

    // ========================================= HTTP modules initialization START
    webserver_init(&http_server);
    

    // ========================================= MQTT modules initialization START
    //TODO: mqtt enable config option
    mqtt_init();
    initialize_modules_mqtt();
    initialize_modules_http( http_server );
    
    while (true) {

        #ifdef CONFIG_DEBUG_PRINT_TASK_INFO
            print_tasks_info();
        #endif

        #if CONFIG_SENSOR_SHT21 && CONFIG_COMPONENT_LCD2004

            char s[20];
            //char *s = (char *) calloc( 20 + 1, sizeof(char*));
            //memset(s, 0, 20 + 1);

            if ( xSemaphoreLCD2004 != NULL && xSemaphoreTake( xSemaphoreLCD2004, I2C_SEMAPHORE_WAIT ) == pdTRUE ) 
            {
                sprintf(s, "Tmp: %2.1f Hum: %2.1f", sht21_get_temp(), sht21_get_hum());
                lcd2004_print(1, s );

                //lcd2004_set_cursor_position( 1, 1);
                //lcd2004_print_string( s );

                //memset(s, 0, 20 + 1);
                sprintf(s, "Freemem: %5d", esp_get_free_heap_size());
                lcd2004_print(2, s );

                //memset(s, 0, 20 + 1);
                get_uptime(s);
                lcd2004_print(3, s );
                //lcd2004_set_cursor_position( 1, 2);
                //lcd2004_print_string( s );
                //free(s);
                
                xSemaphoreGive( xSemaphoreLCD2004 );
            }           
        #endif
        static uint32_t sec = 0;
        user_loop(sec);
        sec++;
        vTaskDelay(1000/ portTICK_RATE_MS);
    }

	
}

//mqtt_add_periodic_publish_callback( const char *topic, func_mqtt_send_cb fn_cb);
// void mqtt_add_receive_callback( const char *topic, func_mqtt_recv_cb fn_cb); -
/*
void test1(char *buf, void *args) {
    static uint32_t cnt = 0;
    itoa(cnt++, buf, 10);
}

void test2(char *buf, void *args){
    static uint32_t cnt = 1000000;
    sprintf(buf, "%d", cnt);
    cnt -= 10;
    if ( cnt == 0) cnt = 1000000;
}

void test_recv1(char *buf, void *args)
{
    ESP_LOGI(TAG, "received topic 'recv1' with data: %s", buf);
}

void test_recv2(char *buf, void *args)
{
    ESP_LOGI(TAG, "received topic 'recv2' with data: %s", buf);
}
*/

#ifdef CONFIG_MCP23017_ISR
void test_mcp23017_isr_cb1(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 0, &val);
    val = !val;
        ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 0, val);    
}

void test_mcp23017_isr_cb2(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 1, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);    
    mcp23017_write_pin(mcp23017_h, 1, val);    
}

void test_mcp23017_isr_cb3(char *buf)
{

    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 2, &val);
    val = !val;
        ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 2, val);    
}

void test_mcp23017_isr_cb4(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 3, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);    
    mcp23017_write_pin(mcp23017_h, 3, val);    
}

void test_mcp23017_isr_cb5(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 4, &val);
    val = !val;
        ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 4, val);    
}

void test_mcp23017_isr_cb6(char *buf)
{
    static uint8_t val = 0;

    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;
    mcp23017_read_pin(mcp23017_h, 5, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 5, val);
}

void test_mcp23017_isr_cb7(char *buf)
{
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 6, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 6, val);
}

void test_mcp23017_isr_cb8(char *buf)
{
    
    mcp23017_handle_t mcp23017_h = (mcp23017_handle_t ) buf;

    static uint8_t val = 0;
    mcp23017_read_pin(mcp23017_h, 7, &val);
    val = !val;
    ESP_LOGI(TAG, "executed callback %s %d", __func__, val);
    mcp23017_write_pin(mcp23017_h, 7, val);
}

#endif

void initialize_modules()
{
    #ifdef CONFIG_COMPONENT_RELAY
    relay_h = relay_create( "Fan", 2, RELAY_LEVEL_HIGH /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ );
    relay_write(relay_h,  RELAY_STATE_CLOSE);

    // relay_red_h = relay_create( "Red", 15, RELAY_LEVEL_LOW /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ );
    // relay_write(relay_red_h,  RELAY_STATE_CLOSE);

    // relay_green_h = relay_create( "Green", 12, RELAY_LEVEL_LOW /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ );
    // relay_write(relay_green_h,  RELAY_STATE_CLOSE);

    // relay_blue_h = relay_create( "Blue", 13, RELAY_LEVEL_LOW /*RELAY_LEVEL_LOW*/ /* RELAY_LEVEL_HIGH*/ );
    // relay_write(relay_blue_h,  RELAY_STATE_CLOSE);   
        #ifdef CONFIG_RELAY_HTTP
        http_handlers_count += RELAY_HANDLERS_COUNT;
        #endif 
    #endif

    #ifdef CONFIG_COMPONENT_IR_RECV
    ir_rx = irrcv_init();
    if ( ir_rx != NULL ) {
		//		                btn_id	  btn_code   user_ctx      callback fun
        irrcv_add_button(ir_rx, 	 0, 0x00FF14EB, "button1", NULL /*ir_button1_press*/);
        irrcv_add_button(ir_rx, 	 1, 0x00FF04FB, "button2", NULL /*ir_button2_press*/);   
		
		//Start IR Receiver to receive a code
        irrcv_start( ir_rx );
    } else {
        ESP_LOGE(TAG, "failed to init ir receiver");
    }    
        #ifdef CONFIG_IR_RECV_HTTP
        http_handlers_count += IR_RECV_HANDLERS_COUNT;
        #endif
    #endif

    #ifdef CONFIG_COMPONENT_LCD2004
        lcd2004_init();
        lcd2004_test_task()
        #ifdef CONFIG_COMPONENT_LCD2004_HTTP
        http_handlers_count += LCD2004_HANDLERS_COUNT;
        #endif
    #endif

    #ifdef CONFIG_COMPONENT_MCP23017
        mcp23017_h = mcp23017_create(0x20 /*MCP23017_ADDR_DEFAULT*/ );
        //mcp23017_test_task(mcp23017_h);
        #ifdef CONFIG_MCP23017_ISR
        // 1 - сразу при нажатии
        // 2 - только после отпускания
        mcp23017_isr_handler_add(mcp23017_h, 15, 2, test_mcp23017_isr_cb8, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 14, 1, test_mcp23017_isr_cb7, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 13, 1, test_mcp23017_isr_cb6, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 12, 1, test_mcp23017_isr_cb5, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 11, 1, test_mcp23017_isr_cb4, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 10, 1, test_mcp23017_isr_cb3, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 9, 2, test_mcp23017_isr_cb2, mcp23017_h);
        mcp23017_isr_handler_add(mcp23017_h, 8, 2, test_mcp23017_isr_cb1, mcp23017_h);
        #endif

        #ifdef CONFIG_MCP23017_HTTP
        http_handlers_count += MCP23017_HANDLERS_COUNT;
        #endif 
        
    #endif

    #ifdef CONFIG_COMPONENT_PCF8574
        pcf8574_h = pcf8574_create(0x3F /*PCF8574_ADDR_DEFAULT*/ );
        pcf8574_test_task(pcf8574_h);
    #endif      

    #ifdef CONFIG_SENSOR_SHT21
        sht21_init();
        sht21_start(5);
    #endif

    #ifdef CONFIG_LED_CONTROLLER
    // ===== create led channels ============================
    ch_red = calloc(1, sizeof(ledcontrol_channel_t)); 
    ch_red->pin = 15;
    ch_red->channel = 0;
    ch_red->bright_tbl = TBL_32B;
    ch_red->name = "Красный";
    ch_red->group = 1;
    
    ch_green = calloc(1, sizeof(ledcontrol_channel_t));
    ch_green->pin = 12;
    ch_green->channel = 1;
    ch_green->bright_tbl = TBL_32B;
    ch_green->name = "Зеленый";
    ch_green->group = 1;

    ch_blue = calloc(1, sizeof(ledcontrol_channel_t)+1);
    ch_blue->pin = 13;
    ch_blue->channel = 2;
    ch_blue->bright_tbl = TBL_32B;
    ch_blue->name = "Синий";
    ch_blue->group = 1;


    // ===== create led controller ============================
    ledc_h = ledcontrol_create(500, 3);
    ledc = (ledcontrol_t *)ledc_h;

    // ==== register led channels to led controller =============
    ledc->register_channel(*ch_red);
    ledc->register_channel(*ch_green);
    ledc->register_channel(*ch_blue);

    // ====== initialize led controller =======================
    ledc->init();  

    #ifdef CONFIG_LED_CONTROL_HTTP
    http_handlers_count += LED_CONTROL_HANDLERS_COUNT;
    #endif

    #ifdef CONFIG_RGB_CONTROLLER
        // === create and init RGB controller ================
        rgb_ledc = rgbcontrol_init(ledc, ch_red, ch_green, ch_blue);

        #ifdef CONFIG_RGB_CONTROLLER_HTTP
        http_handlers_count += RGB_CONTROL_HANDLERS_COUNT;
        #endif 

        #ifdef CONFIG_RGB_EFFECTS
        // ==== create and init rgb effects =======================
        // === setup effects to RGB controller  
        effects = effects_init(rgb_ledc, rgb_ledc->set_color_hsv);
        rgb_ledc->set_effects( effects );  
        #endif
    #endif

    #endif

    #ifdef I2C_HANDLERS_COUNT
    http_handlers_count += I2C_HANDLERS_COUNT;
    #endif
}

void initialize_modules_mqtt()
{
    //mqtt_add_periodic_publish_callback( "test1", test1, NULL);
    //mqtt_add_periodic_publish_callback( "test2", test2, NULL);

    //mqtt_add_receive_callback("recv1", test_recv1, NULL);
    //mqtt_add_receive_callback("recv2", test_recv2, NULL);

    #ifdef CONFIG_COMPONENT_RELAY
    relay_mqtt_init();
    #endif

    #ifdef CONFIG_COMPONENT_MCP23017
    mcp23017_mqtt_init(mcp23017_h);
    #endif

    #ifdef CONFIG_SENSOR_SHT21
    sht21_mqtt_init();
    #endif     

    #ifdef CONFIG_LED_CONTROLLER
    ledcontrol_mqtt_init(ledc_h);
    #endif
}

void initialize_modules_http(httpd_handle_t _server)
{

    wifi_http_init( _server );
    ota_http_init( _server );

    #ifdef CONFIG_RELAY_HTTP
    relay_http_init( _server );
    #endif

    #ifdef CONFIG_IR_RECV_HTTP
    irrcv_http_init( _server, ir_rx );
    #endif

    #ifdef CONFIG_COMPONENT_I2C
    i2c_http_init( _server );
    #endif

    #ifdef CONFIG_COMPONENT_LCD2004_HTTP
    lcd2004_http_init( _server );
    #endif

    #ifdef CONFIG_MCP23017_HTTP
    //((mcp23017_t *) mcp23017_h)->http_buttons = 0b1010100000000000;
    mcp23017_http_init(_server, mcp23017_h);
    mcp23017_http_set_btn_name(mcp23017_h, 15, "Кнопка 1");
    mcp23017_http_set_btn_name(mcp23017_h, 13, "Кнопка 2");
    mcp23017_http_set_btn_name(mcp23017_h, 11, "Кнопка 3");
    #endif

    mqtt_http_init(_server);

    #ifdef CONFIG_SENSOR_SHT21
    sht21_http_init(_server);
    #endif

    #ifdef CONFIG_LED_CONTROL_HTTP
    //ledcontrol_http_add_group(ledc_h, "RGB Controller", 1, 5);
    //ledcontrol_http_add_group(ledc_h, "Сине-зеленая подсветка", 2, 6);
    ledcontrol_http_init(http_server, ledc_h);

        #ifdef CONFIG_RGB_CONTROLLER_HTTP
            rgbcontrol_http_init(_server, rgb_ledc );
        #endif
    #endif   

    //register_print_page_block( "user1", PAGES_URI[ PAGE_URI_ROOT], 0, user_web_main, NULL, NULL, NULL  );     
    //register_print_page_block( "user2", PAGES_URI[ PAGE_URI_CONFIG], 0, user_web_options, NULL, user_process_param, NULL  ); 
}