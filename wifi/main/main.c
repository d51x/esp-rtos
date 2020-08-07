#include "main.h"


static const char *TAG = "MAIN";

void test1(char *buf);
void test2(char *buf);

void test_recv1(char *buf);
void test_recv2(char *buf);

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
    lcd2004_init(LCD2004_ADDR_DEFAULT, 20, 4);
    lcd2004_test_task();
    
    #endif

    wifi_init();



    sntp_start();
    
    webserver_init(&http_server);

    #ifdef CONFIG_COMPONENT_I2C
    i2c_register_http_handler(http_server);
    i2c_register_http_print_data();
    #endif

    #ifdef CONFIG_COMPONENT_LCD2004_HTTP
    lcd2004_register_http_print_data();
    lcd2004_register_http_handler(http_server);
    #endif
        mqtt_init();
        

        mqtt_add_periodic_publish_callback( "test1", test1);
        mqtt_add_periodic_publish_callback( "test2", test2);

        mqtt_add_receive_callback("recv1", test_recv1);
        mqtt_add_receive_callback("recv2", test_recv2);

        mqtt_register_http_print_data();

    #ifdef CONFIG_SENSOR_SHT21
    sht21_init();
    sht21_start( 5 );
    sht21_register_http_print_data();
    #endif


    #ifdef CONFIG_COMPONENT_PCF8574
    ESP_LOGI(TAG, "CONFIG_COMPONENT_PCF8574 AVAILABLE");
    pcf8574_handle_t pcf8574_h = pcf8574_create(0x3F /*PCF8574_ADDR_DEFAULT*/ );

    pcf8574_test_task(pcf8574_h);
    #endif



    while (true) {
        
        
            //ESP_LOGI(TAG, "SHT21 Temperature: %0.2fC", (float) sht21_get_temp());
            //ESP_LOGI(TAG, "SHT21 Humidity: %0.2f%%", (float) sht21_get_hum());

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
void test1(char *buf) {
    static uint32_t cnt = 0;
    itoa(cnt++, buf, 10);
}

void test2(char *buf){
    static uint32_t cnt = 1000000;
    sprintf(buf, "%d", cnt);
    cnt -= 10;
    if ( cnt == 0) cnt = 1000000;
}

void test_recv1(char *buf)
{
    ESP_LOGI(TAG, "received topic 'recv1' with data: %s", buf);
}

void test_recv2(char *buf)
{
    ESP_LOGI(TAG, "received topic 'recv2' with data: %s", buf);
}

