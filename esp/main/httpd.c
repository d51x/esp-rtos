
#include "httpd.h"
#include "http_handlers.h"
#include "http_page.h"


static const char *TAG = "HTTPD";

static esp_err_t http_get_has_params(httpd_req_t *req){
    return ( httpd_req_get_url_query_len(req) > 0 ) ? ESP_OK : ESP_FAIL;
}

static esp_err_t http_get_key_long(httpd_req_t *req, const char *param_name, long *value){
    // get params 
    char*  buf;
    size_t buf_len;
    uint8_t error = 0;
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, param_name, param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => %s=%s", param_name, param);
                error = str_to_int(value, param, 10);
                ESP_LOGI(TAG, "%s = %li", param_name, *value);
            }              
        }        
        free(buf);
    }
    return (error == 0) ? ESP_OK : ESP_FAIL;
}

/********************* Basic Handlers Start *******************/
/* An HTTP GET handler */
esp_err_t main_get_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));
    char page[1024*4];  // выше 2400 не работает, этот размер влият на размер стека задачи  ***ERROR*** A stack overflow in task ppT
    
    if ( http_get_has_params(req) == ESP_OK ) { 
        // к параметрам с данными надо передавать доп параметр, который обозначает функцию, которая будет обрабатывать параметры
        // например st=1, st=2 и т.д. либо имя функции передавать
        http_get_key_long(req, "sda", &sda);
        http_get_key_long(req, "scl", &scl);
            nvs_handle i2c_handle;
            if ( nvs_open("i2c", NVS_READWRITE, &i2c_handle) == ESP_OK) {
                ESP_LOGI(TAG, "NVS i2c section open success");
                if ( nvs_set_u8(i2c_handle, "sda", sda) == ESP_OK) {
                    ESP_LOGI(TAG, "NVS i2c.sda param save success: %d", sda);
                } else {
                    ESP_LOGE(TAG, "NVS i2c.sda param save error");
                }

                if ( nvs_set_u8(i2c_handle, "scl", scl) == ESP_OK) {
                    ESP_LOGI(TAG, "NVS i2c.scl param save success: %d", scl);
                } else {
                    ESP_LOGE(TAG, "NVS i2c.scl param save error");
                    ESP_LOGE(TAG, "NVS i2c.scl = %d", scl);
                }
                nvs_commit(i2c_handle);
            } else {
                ESP_LOGE(TAG, "NVS i2c section open error");
            }
            nvs_close(i2c_handle);
        strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"0; URL=/\" /></head>");
      
    } else {
        // const char* resp_str = (const char*) req->user_ctx;
        get_main_page_data(page);
        ESP_LOGI(TAG, "main page data len = %d", strlen(page));
    }
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));     

    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}


esp_err_t setup_get_handler(httpd_req_t *req){
     ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));
    char page[1024];
    
    get_setup_page_data(page);

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

esp_err_t debug_get_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));
    char page[1024];
    
    get_debug_page_data(page);

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

esp_err_t config_get_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));
    char page[1024];
    
    get_debug_page_data(page);

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

esp_err_t tools_get_handler(httpd_req_t *req){
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));
    char page[1024];
    
    tools_page_data(page);

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}


esp_err_t gpio_get_handler(httpd_req_t *req){
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));

    ESP_LOGI(TAG, "%s: read content length %d\n", __func__, req->content_len);
 
    uint8_t pin = 255;
    uint8_t st = 0;
    uint8_t redirect = 0;   
    uint8_t error = 0;
    char*  buf;
    size_t buf_len;
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "pin", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => pin=%s", param);
                error = str_to_uint8(&pin, param, 10);
                ESP_LOGI(TAG, "pin = %d", pin);
            }    
            if (httpd_query_key_value(buf, "st", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => st=%s", param);
                error = error || str_to_uint8(&st, param, 10);
                if ( st > 1) error = 1;
                ESP_LOGI(TAG, "st = %d", st);
            }        
            if (httpd_query_key_value(buf, "rdct", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => rdct=%s", param);
                error = error || str_to_uint8(&redirect, param, 10);
                ESP_LOGI(TAG, "rdct = %d", redirect);
            }        
        }
        free(buf);
    }

    char page[100] = "OK";
    if ( error ) {
        strcpy(page, "ERROR");
    } else {
        // control gpios
        if ( set_gpio(pin, st, 1) != ESP_OK) strcpy(page, "ERROR");
    }
    if ( redirect ) {
        strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"0; URL=/\" /></head>");
    }  
    httpd_resp_send(req, page, strlen(page));

    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }    
    return ESP_OK;
}

esp_err_t gpio_post_handler(httpd_req_t *req){
    //ESP_LOGI(TAG, __func__);
    //ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));

    //ESP_LOGI(TAG, "%s: read content length %d\n", __func__, req->content_len);

    char*  buf = malloc(req->content_len + 1);
    size_t off = 0;
    int    ret;

    if (!buf) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    while (off < req->content_len) {
        /* Read data received in the request */
        ret = httpd_req_recv(req, buf + off, req->content_len - off);
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req);
            }
            free (buf);
            return ESP_FAIL;
        }
        off += ret;
        //ESP_LOGI(TAG, "%s: recv length %d\n", __func__, ret);
    }
    buf[off] = '\0';

    if (req->content_len < 128) {
        //ESP_LOGI(TAG, "%s: read %s\n", __func__, buf);
    }

    httpd_resp_send(req, buf, req->content_len);
    free (buf);    
    return ESP_OK;
}

esp_err_t gpioprint_get_handler(httpd_req_t *req){
    //ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));
    char page[1024];
    
    gpioprint_page_data(page);

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

esp_err_t restart_get_handler(httpd_req_t *req){
    //ESP_LOGI(TAG, "Free Stack for server task: '%ld'", uxTaskGetStackHighWaterMark(NULL));
    char page[1024];
    // определить наличие параметров get
    size_t buf_len;
    char*  buf;
    uint8_t found = 0;
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

    buf_len = httpd_req_get_url_query_len(req) + 1;    
    if (buf_len > 1) {   
        ESP_LOGI(TAG, "restart page has get params");
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "st", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => st=%s", param);
                uint8_t st;
                found = !str_to_uint8(&st, param, 10);
                ESP_LOGI(TAG, "st = %d", st);
                found = (st == 1);
            }              
        }        
        free(buf);
    } else {
        ESP_LOGI(TAG, "restart page without params");
    }

    if ( found ) {  
        // restart esp and redirect to mainpage after X sec
        strcpy(page, html_restart_header);
        restarting_page_data(page);  
    } else {
        restart_page_data(page);  
        
    }
    httpd_resp_send(req, page, strlen(page));  
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    if ( found ) xTaskCreate(restart_task, "restart_task", 1024, NULL, 10, NULL);
    return ESP_OK;
}

esp_err_t ota_get_handler(httpd_req_t *req){
    char page[2048];
    get_ota_page_data(page);
    httpd_resp_send(req, page, strlen(page));  
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

esp_err_t ota_post_handler(httpd_req_t *req){
    
    
    int total_len = req->content_len;
    int recv_len;           // принято за раз
    int remain = total_len;  // осталось загрузить
    int received = 0;  // загружено
   
    int flash_status = -1;
    int is_firts = 0;

    int one_part = total_len / 100;
    int new_part = one_part;
    char tmp[200];
    uint32_t start_time = millis();
    esp_ota_handle_t ota_handle; 
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

    int buf_size = UPLOAD_BUFFER_SIZE;
    // get from nvs
    char buffer[buf_size];

    ESP_LOGI(TAG, "Start uploading firmware, size %d", total_len);        
    ESP_LOGI(TAG, "upload buffer size is %d", buf_size);        

    while (remain > 0) {
        recv_len = httpd_req_recv(req, buffer, remain > buf_size ? buf_size : remain);
        if ( recv_len < 0) {
            if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                ESP_LOGI(TAG, "Socket timeout, uploaded %d (%d%%)", received, received*100/total_len);        
                continue;
            }
            sprintf(tmp, "File upload failed, uploaded %d%%", received*100/total_len);
            httpd_resp_send(req, tmp, -1);        
            return ESP_FAIL;             
        }
        //printf("OTA RX: %d of %d\r", received, total_len);

	    // Is this the first data we are receiving
		// If so, it will have the information in the header we need.
        if ( !is_firts ) {
            is_firts = 1;

            // Lets find out where the actual data staers after the header info	
            char *body_start_p = strstr(buffer, "\r\n\r\n") + 4;
            int body_part_len = recv_len - (body_start_p - buffer); 

            esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
            if (err != ESP_OK) {
 				ESP_LOGE(TAG, "Error With OTA Begin, Cancelling OTA");
				return ESP_FAIL;               
            } else {
                
                //printf("Writing to partition subtype %d at offset 0x%x\r\n", update_partition->subtype, update_partition->address);
			                
                // Lets write this first part of data out
			    esp_ota_write(ota_handle, body_start_p, body_part_len);                
            }
        } else {
			// Write OTA data
			esp_ota_write(ota_handle, buffer, recv_len);
         
        }
			
		received += recv_len;  
        remain -= recv_len;

        if ( new_part < received ) {
            ESP_LOGI(TAG, "uploaded %d %%", new_part*100/total_len);
            new_part += one_part;
        }
        //vTaskDelay(5 / portTICK_RATE_MS); 
    }

    if (esp_ota_end(ota_handle) == ESP_OK) {
        // Lets update the partition
        if (esp_ota_set_boot_partition(update_partition) == ESP_OK) {
            const esp_partition_t *boot_partition = esp_ota_get_boot_partition();
            flash_status = 1;
			ESP_LOGI("OTA", "Next boot partition subtype %d at offset 0x%x", boot_partition->subtype, boot_partition->address);
			ESP_LOGI("OTA", "Please Restart System...");
            xTaskCreate(&systemRebootTask, "rebootTask", 2048, NULL, 5, NULL);            
        } else {
            ESP_LOGI("OTA", "\r\n\r\n !!! Flashed Error !!!");
        }
    } else {
        ESP_LOGE("OTA", "\r\n\r\n !!! OTA End Error !!!");
    }
    /*
    while (remain > 0 ) {
        recv_len = httpd_req_recv(req, buffer, remain > SIZE_OF_BUFF ? SIZE_OF_BUFF : remain);
        if ( recv_len <= 0) {
            if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
                ESP_LOGI(TAG, "RECEIVE TIMEOUT, uploaded %d (%d%%)", total_len - remain, (total_len - remain)*100/total_len);        
                continue;
            }
            
            sprintf(tmp, "File upload failed, uploaded %d%%", (total_len - remain)*100/total_len);
            httpd_resp_send(req, tmp, -1);        
            return ESP_OK;            
        }
        remain = remain - recv_len;
        //percent = (total_len - remain)*100/total_len;
        //if ( percent % 10 == 0) ESP_LOGI(TAG, "uploaded %d %%", percent);
        vTaskDelay(5 / portTICK_RATE_MS);
    }
*/ 
    strcpy(tmp, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
    sprintf(tmp+strlen(tmp), "File uploaded, time left %d sec. Restarting....", (uint32_t)(millis()-start_time)/1000);
    ESP_LOGI(TAG, tmp);
    

    httpd_resp_send(req, tmp, -1);
   // redirect to main
    return ESP_OK;
    
}

esp_err_t ota_post2_handler(httpd_req_t *req){
    
    char buf[256];
    int ret, remaining = req->content_len;
int32_t t = remaining / 100 ;
    //ESP_LOGI(TAG, "req->content_len: %d", remaining);
    
        while (remaining > 0) {
            //t = millis();
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                ESP_LOGI(TAG, "RECEIVE TIMEOUT");        
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        //httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;
        //ESP_LOGI(TAG, "remaining: %d, read: %d", remaining, ret);
        //os_delay_us(100);
        vTaskDelay(5 / portTICK_RATE_MS);
        /* Log data received */
        //ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        //ESP_LOGI(TAG, "%.*s", ret, buf);
        //ESP_LOGI(TAG, "====================================");
    }    
    // End response


        char page[20];
    
    strcpy(page, "ok");

    httpd_resp_send(req, page, strlen(page));
    
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    
    return ESP_OK;  
    
}

void register_uri_handlers(httpd_handle_t _server) {

    int i;
    //ESP_LOGI(TAG, "Registering URI handlers");
    //ESP_LOGI(TAG, "No of uri handlers = %d", uri_handlers_no);

    for (i = 0; i < uri_handlers_no; i++) {
        if (httpd_register_uri_handler(_server, &uri_handlers[i]) != ESP_OK) {
            //ESP_LOGW(TAG, "register uri failed for %d", i);
            return;
        }
    }
    //ESP_LOGI(TAG, "Success");
}


void webserver_init(httpd_handle_t _server) {
    /* Start the web server */
    if (_server == NULL) {
        _server = start_webserver();
    }
}



httpd_handle_t start_webserver(void){
    httpd_handle_t _server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = WEB_SERVER_STACK_SIZE;
    config.max_uri_handlers = uri_handlers_no; //WEB_SERVER_MAX_URI_GET_HANDLERS;
    config.recv_wait_timeout = 10;
    //        .max_uri_handlers   = 8,                        
    //    .max_resp_headers   = 8, 
    // Start the httpd server
    //ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&_server, &config) == ESP_OK) {
        // Set URI handlers

        register_uri_handlers(_server);

        return _server;
    }

    //ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t _server){
    // Stop the httpd server
    httpd_stop(_server);
}


esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            //ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            //ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            //ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            //ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

