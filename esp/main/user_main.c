

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "freertos/queue.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "global.h"

#include "utils.h"

#ifdef WIFI
    #include "wifi.h"
    #include "httpd.h"
    #ifdef MQTT
        #include "mqtt.h"
    #endif
    #ifdef SNTP
        #include "sntp.h"
    #endif
#endif

#include "gpio_utils.h"

#ifdef DS18B20
    #include "dsw.h"
#endif

#ifdef DHT
    #include "dht.h"
#endif

#ifdef PWM
    #include "pwm.h"
#endif

#include "ir_receiver.h"

static const char *TAG = "MAIN";

#ifdef WIFI
    httpd_handle_t http_server = NULL;
#endif

void main_task(void *arg){
    ESP_LOGI(TAG, "%s: started\n", __func__);

    while (1) {


        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
#ifdef DEBUG
    printf("\n\n*******  FW_VER: %s\n\n", FW_VER);
#endif


    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    sda = scl = 254;
    // load from nvs 
    nvs_handle i2c_handle;
    if ( nvs_open("i2c", NVS_READONLY, &i2c_handle) == ESP_OK) {
        ESP_LOGI(TAG, "NVS i2c section open success");
        if ( nvs_get_u8(i2c_handle, "sda", &sda) == ESP_OK) {
            ESP_LOGI(TAG, "NVS i2c.sda param read success: %d", sda);
        } else {
            ESP_LOGE(TAG, "NVS i2c.sda param read error");
            ESP_LOGE(TAG, "NVS i2c.sda = %d", sda);
        }

        if ( nvs_get_u8(i2c_handle, "scl", &scl) == ESP_OK) {
            ESP_LOGI(TAG, "NVS i2c.scl param read success: %d", scl);
        } else {
            ESP_LOGE(TAG, "NVS i2c.scl param read error");
            ESP_LOGE(TAG, "NVS i2c.scl = %d", scl);
        }

    } else {
        ESP_LOGE(TAG, "NVS i2c section open error");
    }
    nvs_close(i2c_handle);

#ifdef GPIO
    init_gpios();     // configure GPIO
#endif
    
#ifdef DEBUG
    print_chip_info();
#endif

#ifdef WIFI
    wifi_mode_t wf = WIFI_MODE_STA; //
    //wifi_mode_t wf = WIFI_MODE_AP; //
    wifi_init(wf); 
    webserver_init(http_server);

    #ifdef MQTT
        mqtt_start();
        xTaskCreate(mqtt_publish_all_task, "mqtt_publish_all_task", 2048, NULL, 10, NULL);
    #endif

    #ifdef SNTP
        xTaskCreate(sntp_task, "sntp_task", 2048, NULL, 10, NULL);
    #endif
#endif


    xTaskCreate(main_task, "main_task", 1024, NULL, 10, NULL);

#ifdef DS18B20    
    // TODO: при запуске задачи на определение ИД датчиков задачу чтения температуры надо приостановить через event group bit (аналог wifi)
    xTaskCreate(ds18b20_search_task, "ds18b20_search_task", 2048, NULL, 10, NULL);
    // TODO: в таске читать данные только по указанным ид датчикам, ид датчиков определять отдельно через страничку
    xTaskCreate(ds18b20_get_temp_task, "ds18b20_get_temp_task", 2048, NULL, 10, NULL);
#endif

#ifdef DHT
    xTaskCreate(dht_task, "dht_task", 2048, NULL, 10, NULL);
#endif

#ifdef PWM        
    xTaskCreate(pwm_task, "pwm_task", 2048, NULL, 10, NULL);
#endif

    xTaskCreate(ir_receiver_task, "ir_receiver_task", 2048, NULL, 5, NULL); 
}


