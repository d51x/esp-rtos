#include "main.h"


static const char *TAG = "MAIN";

void test1(char *buf, void *args);
void test2(char *buf, void *args);

void test_recv1(char *buf, void *args);
void test_recv2(char *buf, void *args);

void test_mcp23017_isr_cb1(char *buf);
void test_mcp23017_isr_cb2(char *buf);
void test_mcp23017_isr_cb3(char *buf);
void test_mcp23017_isr_cb4(char *buf);
void test_mcp23017_isr_cb5(char *buf);
void test_mcp23017_isr_cb6(char *buf);
void test_mcp23017_isr_cb7(char *buf);
void test_mcp23017_isr_cb8(char *buf);


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
	
    #ifdef CONFIG_COMPONENT_LCD2004
    lcd2004_init();
    lcd2004_test_task();
    #endif

    #ifdef CONFIG_COMPONENT_MCP23017
    mcp23017_handle_t mcp23017_h = mcp23017_create(0x20 /*MCP23017_ADDR_DEFAULT*/ );

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
    #endif
    
    wifi_init();
    sntp_start();
    
    webserver_init(&http_server);

    wifi_http_init(http_server);
    ota_http_init(http_server);

    #ifdef CONFIG_COMPONENT_I2C
    i2c_http_init(http_server);
    #endif

    #ifdef CONFIG_COMPONENT_LCD2004_HTTP
    lcd2004_http_init(http_server);
    #endif

    mqtt_init();

    mqtt_add_periodic_publish_callback( "test1", test1, NULL);
    mqtt_add_periodic_publish_callback( "test2", test2, NULL);

    mqtt_add_receive_callback("recv1", test_recv1, NULL);
    mqtt_add_receive_callback("recv2", test_recv2, NULL);

    // register mcp23017
    #ifdef CONFIG_MCP23017_HTTP
    mcp23017_mqtt_init(mcp23017_h);
    //((mcp23017_t *) mcp23017_h)->http_buttons = 0b1010100000000000;
    mcp23017_http_init(http_server, mcp23017_h);
    mcp23017_http_set_btn_name(mcp23017_h, 15, "Кнопка 1");
    mcp23017_http_set_btn_name(mcp23017_h, 13, "Кнопка 2");
    mcp23017_http_set_btn_name(mcp23017_h, 11, "Кнопка 3");
    #endif

    mqtt_http_init(http_server);

    
    #ifdef CONFIG_SENSOR_SHT21
    sht21_init();
    sht21_start( 5 );
    sht21_http_init(http_server);
    #endif


    #ifdef CONFIG_COMPONENT_PCF8574
    ESP_LOGI(TAG, "CONFIG_COMPONENT_PCF8574 AVAILABLE");
    pcf8574_handle_t pcf8574_h = pcf8574_create(0x3F /*PCF8574_ADDR_DEFAULT*/ );

    pcf8574_test_task(pcf8574_h);
    #endif    
    
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

        vTaskDelay(1000/ portTICK_RATE_MS);
    }

	
}

//mqtt_add_periodic_publish_callback( const char *topic, func_mqtt_send_cb fn_cb);
// void mqtt_add_receive_callback( const char *topic, func_mqtt_recv_cb fn_cb); -
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
