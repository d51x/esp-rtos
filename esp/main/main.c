

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "freertos/queue.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "main.h"

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
    ESP_LOGD(TAG, "%s: started\n", __func__);
    
    ds18b20_init(DS18B20_PIN);
    ds18b20[0].addr[0] = 0x28; ds18b20[0].addr[1] = 0xFF; ds18b20[0].addr[2] = 0x81; ds18b20[0].addr[3] = 0xE9; ds18b20[0].addr[4] = 0x74; ds18b20[0].addr[5] = 0x16; ds18b20[0].addr[6] = 0x03; ds18b20[0].addr[7] = 0x41;
    ds18b20[1].addr[0] = 0x28; ds18b20[1].addr[1] = 0xFF; ds18b20[1].addr[2] = 0x05; ds18b20[1].addr[3] = 0xC6; ds18b20[1].addr[4] = 0x74; ds18b20[1].addr[5] = 0x16; ds18b20[1].addr[6] = 0x04; ds18b20[1].addr[7] = 0xFC;
    ds18b20[2].addr[0] = 0x28; ds18b20[2].addr[1] = 0xFF; ds18b20[2].addr[2] = 0x6D; ds18b20[2].addr[3] = 0x19; ds18b20[2].addr[4] = 0x75; ds18b20[2].addr[5] = 0x16; ds18b20[2].addr[6] = 0x04; ds18b20[2].addr[7] = 0xE3;


    dht.pin = DHT_PIN;
    dht.type = DHT22;
    dht_init(&dht);

    while (1) {
        xEventGroupWaitBits(ota_event_group, OTA_IDLE_BIT, false, true, portMAX_DELAY);

        if ( dht_read(&dht) == ESP_OK) {
            ESP_LOGD(TAG, "DHT Temp: %.2f\t\tHumy: %.2f\t\t(gpio %d)", dht.temp, dht.hum, dht.pin);
        } else {
            ESP_LOGE(TAG, "DHT (gpio%d) data read error", dht.pin);
        }   

        vTaskDelay(5000 / portTICK_RATE_MS);

        for (uint8_t i=0;i<DSW_COUNT;i++) {
			float temp;
			if ( ds18b20[i].addr[0] ) {
				if (ds18b20_getTemp(ds18b20[i].addr, &temp) == ESP_OK) {
				if ( temp != 125.f) ds18b20[i].temp = temp;	
				ESP_LOGD(TAG, "addr %02x %02x %02x %02x %02x %02x %02x %02x  temp: %.2f C", 
                                                    ds18b20[i].addr[0], ds18b20[i].addr[1], ds18b20[i].addr[2], ds18b20[i].addr[3], 
                                                    ds18b20[i].addr[4], ds18b20[i].addr[5], ds18b20[i].addr[6], ds18b20[i].addr[7], 
                                                    ds18b20[i].temp);
                } else {
                    ESP_LOGE(TAG, "DS18B20 getting temperature FAILED!!!!");
                }                                                            
			}

		}

        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}

void load_params_from_nvs() {
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
}

void app_main(void){

    ESP_LOGI("*******  FW_VER: %s", FW_VER);
    ESP_LOGI("*******  CORE_VER: %s", CORE_FW_VER);



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
    //xTaskCreate(ds18b20_search_task, "ds18b20_search_task", 2048, NULL, 10, NULL);
    // TODO: в таске читать данные только по указанным ид датчикам, ид датчиков определять отдельно через страничку
    //xTaskCreate(ds18b20_get_temp_task, "ds18b20_get_temp_task", 2048, NULL, 10, NULL);
#endif

#ifdef DHT
   // xTaskCreate(dht_task, "dht_task", 2048, NULL, 10, NULL);
#endif

#ifdef PWM        
    xTaskCreate(pwm_task, "pwm_task", 2048, NULL, 10, NULL);
#endif

    xTaskCreate(ir_receiver_task, "ir_receiver_task", 2048, NULL, 10, NULL); 
    xTaskCreate(read_sensors_task, "read_sensors_task", 2048, NULL, 10, NULL); 
    xTaskCreate(read_gpio_task, "read_gpio_task", 2048, NULL, 10, NULL); 
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