#include "ota.h"

static const char *TAG = "OTA";

/*
esp_err_t ota_http_event_handler(esp_http_client_event_t *evt)
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
*/

void ota_init(){
    esp_ota.state = ESP_OTA_IDLE;
    esp_ota.progress = 0;
    esp_ota.write_bytes = 0;
    esp_ota.ota_size = 0;
}

esp_err_t ota_task_upgrade_from_url(char *err_text){
    int buf_size = CONFIG_OTA_BUF_SIZE;  //default value

    ota_nvs_data_t ota_nvs;
    get_ota_nvs_data(&ota_nvs); 
    ESP_LOGI(TAG, "OTA OTA_BUF_SIZE: %d", buf_size ); //CONFIG_OTA_BUF_SIZE);

    set_ota_state(ESP_OTA_PREPARE);
    ESP_LOGI(TAG, "Starting OTA...");
    esp_http_client_config_t config = {
        .url = strdup(ota_nvs.uri), //FIRMWARE_UPGRADE_URL,     // указать url прошивки
    //    .cert_pem = NULL,
        //.event_handler = ota_http_event_handler,
        .timeout_ms = 20000,
        .buffer_size = buf_size,
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
    };
    //esp_http_client_config_t config;
    

    //ESP_LOGI(TAG, "Downloading firmware from %s....", config.url);
    esp_http_client_handle_t client = esp_http_client_init(&config);
    //config.url = strdup(ota_nvs.uri);
    ESP_LOGI(TAG, "Downloading firmware from %s....", config.url);

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        esp_http_client_cleanup(client);
        sprintf(err_text, "Failed to open HTTP connection: %d", err);
        ESP_LOGE(TAG, err_text); 
        return err;
    }
    
    int total_len = esp_http_client_fetch_headers(client);
    int remain = total_len;  // осталось загрузить
    int one_part = total_len / 100;
    int new_part = one_part;

    set_ota_size(total_len);
    ESP_LOGI(TAG, "header: Content-Length: %d", total_len);

    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = NULL;
    
    update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
        strcpy(err_text, "Passive OTA partition not found");
        ESP_LOGE(TAG, err_text);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        set_ota_state(ESP_OTA_IDLE);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        sprintf(err_text, "esp_ota_begin failed, error=%d", err);
        ESP_LOGE(TAG, err_text);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        set_ota_state(ESP_OTA_IDLE);
        return err;
    }
    ESP_LOGI(TAG, "esp_ota_begin succeeded");
    ESP_LOGI(TAG, "Please Wait. This may take time");

    esp_err_t ota_write_err = ESP_OK;
    char *upgrade_data_buf = (char *)malloc(buf_size); //CONFIG_OTA_BUF_SIZE);
    if (!upgrade_data_buf) {
        strcpy(err_text, "Couldn't allocate memory to upgrade data buffer");
        ESP_LOGE(TAG, err_text);
        set_ota_state(ESP_OTA_IDLE);
        return ESP_ERR_NO_MEM;
    }

    //=========================================================================================
    int binary_file_len = 0;
    uint8_t retry_count = 0;
    set_ota_state(ESP_OTA_START);
    while (remain > 0) {
        //int data_read = esp_http_client_read(client, upgrade_data_buf, remain > CONFIG_OTA_BUF_SIZE ? CONFIG_OTA_BUF_SIZE : remain);
        int data_read = esp_http_client_read(client, upgrade_data_buf, remain > buf_size ? buf_size : remain);
        ESP_LOGD(TAG, "http client read %d --> %d of %d", data_read, binary_file_len, total_len);
        if (data_read == 0) {
            //ESP_LOGI(TAG, "Connection closed,all data received");
            if ( retry_count > 20 ) {
              ESP_LOGE(TAG, "Timeout ... Max retry count reached %d...  http client read %d --> %d of %d (%d%%)", retry_count, data_read, binary_file_len, total_len, binary_file_len*100/total_len);  
              break;  
            }
            //break;
            retry_count++;
            ESP_LOGE(TAG, "Timeout ... Retry count %d...  http client read %d --> %d of %d (%d%%)", retry_count, data_read, binary_file_len, total_len, binary_file_len*100/total_len);
            vTaskDelay(5 / portTICK_PERIOD_MS);
            continue;
        }
        if (data_read < 0) {
            ESP_LOGE(TAG, "Error: SSL data read error");
            set_ota_state(ESP_OTA_IDLE);
            break;
        }
        if (data_read > 0) {
            retry_count = 0;
            
            ota_write_err = esp_ota_write( update_handle, (const void *)upgrade_data_buf, data_read);
            if (ota_write_err != ESP_OK) {
                set_ota_state(ESP_OTA_IDLE);
                break;
            }
            
            binary_file_len += data_read;
            remain -= data_read;
            set_ota_state(ESP_OTA_RECVED);
            set_ota_progress(binary_file_len);
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
    sprintf(err_text, "Total binary data length writen: %d of %d", binary_file_len, total_len);
    ESP_LOGD(TAG, err_text);
    
    esp_err_t ota_end_err = esp_ota_end(update_handle);
    if (ota_write_err != ESP_OK) {
        sprintf(err_text, "Error: esp_ota_write failed! err=0x%d", err);
        ESP_LOGE(TAG, err_text);
        set_ota_state(ESP_OTA_IDLE);
        return ota_write_err;
    } else if (ota_end_err != ESP_OK) {
        sprintf(err_text, "Error: esp_ota_end failed! err=0x%d. Image is invalid", ota_end_err);
        ESP_LOGE(TAG, err_text);
        set_ota_state(ESP_OTA_IDLE);
        return ota_end_err;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        sprintf(err_text, "esp_ota_set_boot_partition failed! err=0x%d", err);
        ESP_LOGE(TAG, err_text);
        set_ota_state(ESP_OTA_IDLE);
        return err;
    }
    save_ota_upgrade_dt();

    set_ota_state(ESP_OTA_FINISH);
    strcpy(err_text+strlen(err_text), "\nesp_ota_set_boot_partition succeeded");
    strcpy(err_text+strlen(err_text), "\nPlease Restart System...");
    ESP_LOGI(TAG, err_text); 
    return ESP_OK;
}


    //xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
esp_err_t ota_task_upgrade_from_web(httpd_req_t *req, char *err_text){
    ESP_LOGI(TAG, "Starting OTA...");
    
    set_ota_state(ESP_OTA_PREPARE);

    int total_len = req->content_len;
    int recv_len;           // принято за раз
    int remain = total_len;  // осталось загрузить
    int received = 0;  // загружено
   
    int is_firts = 0;

    int one_part = total_len / 100;
    int new_part = one_part;
    int buf_size = CONFIG_OTA_BUF_SIZE;  // default

    ota_nvs_data_t ota_nvs;
    get_ota_nvs_data(&ota_nvs);
    set_ota_size(total_len);

    esp_ota_handle_t ota_handle; 
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

    char *upgrade_data_buf = (char *)malloc( buf_size );
    if (!upgrade_data_buf) {
        strcpy(err_text, "Couldn't allocate memory to upgrade data buffer");
        ESP_LOGE(TAG, err_text);
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Start uploading firmware, size %d", total_len);        
    ESP_LOGI(TAG, "upload buffer size is %d", buf_size);        

    set_ota_state(ESP_OTA_PREPARE);
    esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error With OTA Begin, Cancelling OTA");
        strcpy(err_text, "Error With OTA Begin, Cancelling OTA");
        ESP_LOGE(TAG, err_text);
        set_ota_state(ESP_OTA_IDLE);
        return ESP_ERR_FLASH_OP_FAIL;               
    } 

    set_ota_state(ESP_OTA_START);
    uint8_t retry_count = 0;
    //=========================================================================================
    while (remain > 0) {
        recv_len = httpd_req_recv(req, upgrade_data_buf, remain > buf_size ? buf_size : remain);
        if ( recv_len < 0) {
            if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                if ( retry_count > 20 ) {
                ESP_LOGE(TAG, "Timeout ... Max retry count reached %d...  httpd_req_recv %d (%d%%)", retry_count, received, received*100/total_len);  
                break;  
                }                
                retry_count++;
                ESP_LOGE(TAG, "Socket timeout, uploaded %d (%d%%)", received, received*100/total_len);        
                vTaskDelay(5 / portTICK_PERIOD_MS);
                continue;
            }
            sprintf(err_text, "File upload failed, uploaded %d%%", received*100/total_len);
            set_ota_state(ESP_OTA_IDLE); 
            ESP_LOGE(TAG, err_text);      
            return ESP_ERR_FLASH_OP_FAIL;             
        }

        // Is this the first data we are receiving
        // If so, it will have the information in the header we need.
        if ( !is_firts ) {
            is_firts = 1;
            ESP_LOGD(TAG, "Writing first block to OTA partition");
            
            // Lets find out where the actual data staers after the header info	
            char *body_start_p = strstr(upgrade_data_buf, "\r\n\r\n") + 4;
            ESP_LOGD(TAG, "body_start_p = %02X", body_start_p - upgrade_data_buf + 1);

            int body_part_len = recv_len - (body_start_p - upgrade_data_buf); 
            ESP_LOGD(TAG, "body_part_len = %d", body_part_len);

            err = esp_ota_write(ota_handle, (const void *)body_start_p, body_part_len);
            if ( err != ESP_OK) {
                strcpy(err_text, "OTA write failed");
                ESP_LOGE(TAG, err_text);
                return err;
            }    
        } else {
            // Write OTA data
            err = esp_ota_write(ota_handle, (const void *)upgrade_data_buf, recv_len);
            if ( err != ESP_OK ) {
                strcpy(err_text, "OTA write failed");
                ESP_LOGE(TAG, err_text);
                return err;
            }    
        }
        received += recv_len; 
        remain -= recv_len;
        set_ota_progress(received);
        set_ota_state(ESP_OTA_RECVED);
        if ( new_part < received ) {
            ESP_LOGI(TAG, "uploaded %d %%", new_part*100/total_len);
            new_part += one_part;
        }
    } // while
    //=========================================================================================

    free(upgrade_data_buf);
    if (esp_ota_end(ota_handle) == ESP_OK) {
        // Lets update the partition
        if (esp_ota_set_boot_partition(update_partition) == ESP_OK) {
            set_ota_state(ESP_OTA_FINISH);
            const esp_partition_t *boot_partition = esp_ota_get_boot_partition();
            ESP_LOGI("OTA", "Next boot partition subtype %d at offset 0x%x", boot_partition->subtype, boot_partition->address);
            ESP_LOGI("OTA", "Please Restart System...");

            save_ota_upgrade_dt();

        } else {
            strcpy(err_text, "OTA upgrading failed. Flashed Error!");
            ESP_LOGE(TAG, err_text);
            set_ota_state(ESP_OTA_IDLE);
            return ESP_ERR_FLASH_OP_FAIL;
        }
    } else {
        strcpy(err_text, "OTA upgrading failed. Flashed Error!");
        ESP_LOGE(TAG, err_text);
        set_ota_state(ESP_OTA_IDLE);
        return ESP_ERR_FLASH_OP_FAIL;
    } 
    return ESP_OK;        
}

esp_ota_t get_ota_state() {
    return esp_ota;
}

void set_ota_state(esp_ota_state_t st){
    esp_ota.state = st;
}

void set_ota_size(size_t ota_size){
    esp_ota.ota_size = ota_size;
}

void set_ota_progress(size_t bytes){
    esp_ota.write_bytes = bytes;
    if (esp_ota.ota_size > 0 )
        esp_ota.progress = bytes * 100 / esp_ota.ota_size;
}

void get_ota_nvs_data(ota_nvs_data_t *ota_nvs) {
    if ( nvs_param_str_load("ota", "uri",  ota_nvs->uri) != ESP_OK ) {
        strcpy(ota_nvs->uri, "");
    }
    if ( nvs_param_u16_load("ota", "bufsz",  &ota_nvs->buf_size) != ESP_OK ) {
        ota_nvs->buf_size = CONFIG_OTA_BUF_SIZE;
    }  
}

void set_ota_nvs_data(const ota_nvs_data_t *ota_nvs) {
    nvs_param_str_save("ota", "uri",  ota_nvs->uri);  
    nvs_param_u16_save("ota", "bufsz",  ota_nvs->buf_size);  
}

void save_ota_upgrade_dt(){
    char buf[25];
    get_localtime(buf);
    nvs_param_str_save("ota", "upgrade_dt",  buf); 
}

void get_ota_upgraded_dt(char *buf){
    nvs_param_str_load("ota", "upgrade_dt", buf); 
}