#include "http_ota.h"

static const char *TAG = "OTA";

/*
void simple_ota_example_task(void * pvParameter){
    ESP_LOGI(TAG_OTA, "Starting OTA example...");
    
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    ESP_LOGI(TAG_OTA, "Connect to Wifi ! Start to Connect to Server....");
    
    esp_http_client_config_t config = {
        .url = FIRMWARE_UPGRADE_URL,     // указать url прошивки
        //.cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
    };
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        ESP_LOGE(TAG_OTA, "Firmware Upgrades Failed");
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
*/

    //xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);