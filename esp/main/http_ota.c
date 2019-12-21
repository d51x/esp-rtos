#include "http_ota.h"

static const char *TAG = "OTA";


static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

esp_err_t ota_task_uptade_from_url(const char *url){
    ESP_LOGI(TAG, "Starting OTA...");
    
    //xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Downloading firmware from %s....", FIRMWARE_UPGRADE_URL);
    
    esp_http_client_config_t config = {
        .url = FIRMWARE_UPGRADE_URL,     // указать url прошивки
        .cert_pem = NULL,
        .event_handler = _http_event_handler,
    };
    int buf_size = UPLOAD_BUFFER_SIZE;
    ESP_LOGI(TAG, "OTA OTA_BUF_SIZE: %d", buf_size ); //CONFIG_OTA_BUF_SIZE);

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        esp_http_client_cleanup(client);
        ESP_LOGE(TAG, "Failed to open HTTP connection: %d", err);
        return err;
    }
    int total_len = esp_http_client_fetch_headers(client);
    int remain = total_len;  // осталось загрузить
    int one_part = total_len / 100;
    int new_part = one_part;

    ESP_LOGI(TAG, "header: Content-Length: %d", total_len);

    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = NULL;
    ESP_LOGI(TAG, "Starting OTA...");
    update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
        ESP_LOGE(TAG, "Passive OTA partition not found");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return err;
    }
    ESP_LOGI(TAG, "esp_ota_begin succeeded");
    ESP_LOGI(TAG, "Please Wait. This may take time");

    esp_err_t ota_write_err = ESP_OK;
    char *upgrade_data_buf = (char *)malloc(buf_size); //CONFIG_OTA_BUF_SIZE);
    if (!upgrade_data_buf) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        return ESP_ERR_NO_MEM;
    }

    //=========================================================================================
    int binary_file_len = 0;
    int is_firts = 0; 
     while (remain > 0) {
        //int data_read = esp_http_client_read(client, upgrade_data_buf, remain > CONFIG_OTA_BUF_SIZE ? CONFIG_OTA_BUF_SIZE : remain);
        int data_read = esp_http_client_read(client, upgrade_data_buf, remain > buf_size ? buf_size : remain);
        ESP_LOGD(TAG, "http client read %d --> %d of %d", data_read, binary_file_len, total_len);
        if (data_read == 0) {
            //ESP_LOGI(TAG, "Connection closed,all data received");
            //break;
            ESP_LOGI(TAG, "Timeout .....  http client read %d --> %d of %d", data_read, binary_file_len, total_len);
            continue;
        }
        if (data_read < 0) {
            ESP_LOGE(TAG, "Error: SSL data read error");
            break;
        }
        if (data_read > 0) {
            ota_write_err = esp_ota_write( update_handle, (const void *)upgrade_data_buf, data_read);
            if (ota_write_err != ESP_OK) {
                break;
            }
            binary_file_len += data_read;
            remain -= data_read;
            ESP_LOGD(TAG, "Written image length %d of (%d)", binary_file_len, total_len);

            if ( new_part < binary_file_len ) {
                ESP_LOGI(TAG, "uploaded %d %%", new_part*100/total_len);
                new_part += one_part;
            }            
        }
    }
    //=========================================================================================

    free(upgrade_data_buf);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
    ESP_LOGD(TAG, "Total binary data length writen: %d of %d", binary_file_len, total_len);
    
    esp_err_t ota_end_err = esp_ota_end(update_handle);
    if (ota_write_err != ESP_OK) {
        ESP_LOGE(TAG, "Error: esp_ota_write failed! err=0x%d", err);
        return ota_write_err;
    } else if (ota_end_err != ESP_OK) {
        ESP_LOGE(TAG, "Error: esp_ota_end failed! err=0x%d. Image is invalid", ota_end_err);
        return ota_end_err;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%d", err);
        return err;
    }
    ESP_LOGI(TAG, "esp_ota_set_boot_partition succeeded"); 
    ESP_LOGI("OTA", "Please Restart System...");
    return ESP_OK;
}


    //xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
void ota_task_uptade_from_web(httpd_req_t *req){

  
    
}