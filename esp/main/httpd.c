
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
    char page[1024*4];  
    
    // const char* resp_str = (const char*) req->user_ctx;
    print_html_header_data(page, "Main page"); // TODO: взять из context
    print_html_devinfo(page+strlen(page));

    sprintf(page+strlen(page), "<div id=\"sens\">");        
    #ifdef DS18B20
        print_html_dsw(page+strlen(page));
    #endif
    #ifdef DHT
        print_html_dht(page+strlen(page));
    #endif
    #ifdef GPIO
        print_html_gpio(page+strlen(page));
    #endif
   
    print_html_menu(page+strlen(page));
    print_html_footer_data(page+strlen(page)); // TODO: взять из context
    
    //httpd_resp_sendstr_chunk(req, NULL);        
    
      
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    //get_main_page_data(page);
    return ESP_OK;
}


esp_err_t setup_get_handler(httpd_req_t *req){
    char page[1024];
    get_setup_page_data(page);    
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

esp_err_t debug_get_handler(httpd_req_t *req){
    char page[1024];
    get_debug_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

esp_err_t config_get_handler(httpd_req_t *req){
    char page[1024];
    get_setup_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

esp_err_t tools_get_handler(httpd_req_t *req){
    char page[1024];
    get_setup_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;    
}


esp_err_t gpio_get_handler(httpd_req_t *req){
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
            //ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "pin", param, sizeof(param)) == ESP_OK) {
                //ESP_LOGI(TAG, "Found URL query parameter => pin=%s", param);
                error = str_to_uint8(&pin, param, 10);
                //ESP_LOGI(TAG, "pin = %d", pin);
            }    
            if (httpd_query_key_value(buf, "st", param, sizeof(param)) == ESP_OK) {
                //ESP_LOGI(TAG, "Found URL query parameter => st=%s", param);
                error = error || str_to_uint8(&st, param, 10);
                if ( st > 1) error = 1;
                //ESP_LOGI(TAG, "st = %d", st);
            }        
            if (httpd_query_key_value(buf, "rdct", param, sizeof(param)) == ESP_OK) {
                //ESP_LOGI(TAG, "Found URL query parameter => rdct=%s", param);
                error = error || str_to_uint8(&redirect, param, 10);
                //ESP_LOGI(TAG, "rdct = %d", redirect);
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
   
    return ESP_OK;
}

esp_err_t gpio_post_handler(httpd_req_t *req){
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
    char page[1024];
    gpioprint_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

esp_err_t restart_get_handler(httpd_req_t *req){
    char page[1024];
    // определить наличие параметров get
    size_t buf_len;
    char*  buf;
    uint8_t found = 0;
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

    if ( http_get_has_params(req) == ESP_OK) {
        uint8_t st;
        if ( http_get_key_long(req, "st", &st) == ESP_OK ) {
            found = (st == 1);        
        }
    }              
    
    if ( found ) {  
        // restart esp and redirect to mainpage after X sec
        xTaskCreate(systemRebootTask, "systemRebootTask", 1024, 2000, 10, NULL);
        strcpy(page, html_restart_header);
        restarting_page_data(page);  
    } else {
        restart_page_data(page);          
    }
    httpd_resp_send(req, page, strlen(page));  
    //if ( found ) xTaskCreate(restart_task, "restart_task", 1024, NULL, 10, NULL);
    return ESP_OK;
}

esp_err_t ota_get_handler(httpd_req_t *req){
    ESP_LOGI(TAG, __func__);
    char page[2048];
    uint8_t found = 0;
    // check params
    ESP_LOGI(TAG, "httpd_req_get_url_query_len(req) = %d", httpd_req_get_url_query_len(req));
    if ( http_get_has_params(req) == ESP_OK) {
        ESP_LOGI(TAG, "params found");
        uint8_t st;
        if ( http_get_key_long(req, "st", &st) == ESP_OK ) {
            ESP_LOGI(TAG, "param st found");
            found = (st == 1);      
            ESP_LOGI(TAG, "param st found- value %d", st);
        } else {
            ESP_LOGI(TAG, "param st not found");
        }
    } else {
        ESP_LOGI(TAG, "params not found");
    }

    if ( found ) {  
        // download bin from url
        // show upgrading dialog or page
        uint32_t start_time = millis();
        ESP_LOGI(TAG, "start upgrade from url");
        if ( ota_task_uptade_from_url("url") == ESP_OK ) {
            // upgrading is OK, restart esp and redirect to main page in 10
            strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
            sprintf(page+strlen(page), "File uploaded, time left %d sec. Restarting....", (uint32_t)(millis()-start_time)/1000);
            ESP_LOGI(TAG, tmp);
            xTaskCreate(&systemRebootTask, "rebootTask", 1024, 3000, 5, NULL);    
        } else {
            // upgrading fail show ota page again
            // show upgrade fail and redirect to ota page in 10 sec
            strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
            strcpy(page+strlen(page), "OTA upgrade failed...");
            ESP_LOGI(TAG, tmp);
        }
    } else {
        ESP_LOGI(TAG, "just show page");
        get_ota_page_data(page);
    }    
    
    
    httpd_resp_send(req, page, strlen(page));  
    return ESP_OK;
}

esp_err_t ota_post_handler(httpd_req_t *req){
  xEventGroupClearBits(ota_event_group, OTA_IDLE_BIT);

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
    char *upgrade_data_buf = (char *)malloc( buf_size );
    if (!upgrade_data_buf) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Start uploading firmware, size %d", total_len);        
    ESP_LOGI(TAG, "upload buffer size is %d", buf_size);        

    esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error With OTA Begin, Cancelling OTA");
        // show this error on page and return to ota page
        strcpy(tmp, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/ota\" /></head>");
        strcpy(tmp+strlen(tmp), "Error With OTA Begin, Cancelling OTA");
        httpd_resp_set_status(req, HTTPD_500);
        httpd_resp_send(req, tmp, -1);
        xEventGroupSetBits(ota_event_group, OTA_IDLE_BIT); 
        return ESP_OK;               
    }

    while (remain > 0) {
        recv_len = httpd_req_recv(req, upgrade_data_buf, remain > buf_size ? buf_size : remain);
        if ( recv_len < 0) {
            if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                ESP_LOGI(TAG, "Socket timeout, uploaded %d (%d%%)", received, received*100/total_len);        
                continue;
            }
            strcpy(tmp, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/ota\" /></head>");
            sprintf(tmp+strlen(tmp), "File upload failed, uploaded %d%%", received*100/total_len);
            httpd_resp_set_status(req, HTTPD_500);
            
            httpd_resp_send(req, tmp, strlen(tmp));   
            xEventGroupSetBits(ota_event_group, OTA_IDLE_BIT);       
            return ESP_OK;             
        }
        //printf("OTA RX: %d of %d\r", received, total_len);

        // Is this the first data we are receiving
        // If so, it will have the information in the header we need.
        if ( !is_firts ) {
            is_firts = 1;
            ESP_LOGI(TAG, "Writing first block to OTA partition");
            
            // Lets find out where the actual data staers after the header info	
            char *body_start_p = strstr(upgrade_data_buf, "\r\n\r\n") + 4;
            ESP_LOGI(TAG, "body_start_p = %02X", body_start_p - upgrade_data_buf + 1);

            int body_part_len = recv_len - (body_start_p - upgrade_data_buf); 
            ESP_LOGI(TAG, "body_part_len = %d", body_part_len);

            esp_ota_write(ota_handle, (const void *)body_start_p, body_part_len);                

        } else {
            // Write OTA data
            esp_ota_write(ota_handle, (const void *)upgrade_data_buf, recv_len);
        }
        received += recv_len;  
        remain -= recv_len;
        if ( new_part < received ) {
            ESP_LOGI(TAG, "uploaded %d %%", new_part*100/total_len);
            new_part += one_part;
        }
    } // while
    free(upgrade_data_buf);
    if (esp_ota_end(ota_handle) == ESP_OK) {
        // Lets update the partition
        if (esp_ota_set_boot_partition(update_partition) == ESP_OK) {
            const esp_partition_t *boot_partition = esp_ota_get_boot_partition();
            flash_status = 1;
            ESP_LOGI("OTA", "Next boot partition subtype %d at offset 0x%x", boot_partition->subtype, boot_partition->address);
            ESP_LOGI("OTA", "Please Restart System...");
            strcpy(tmp, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
            sprintf(tmp+strlen(tmp), "File uploaded, time left %d sec. Restarting....", (uint32_t)(millis()-start_time)/1000);
            ESP_LOGI(TAG, tmp);
            xTaskCreate(&systemRebootTask, "rebootTask", 1024, 3000, 5, NULL);            
        } else {
            ESP_LOGI("OTA", "\r\n\r\n !!! Flashed Error !!!");
        }
    } else {
        ESP_LOGE("OTA", "\r\n\r\n !!! OTA End Error !!!");
    } 
    httpd_resp_send(req, tmp, -1);
    return ESP_OK;            
}


esp_err_t favicon_get_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
    const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
    return ESP_OK;
}

esp_err_t main_css_get_handler(httpd_req_t *req)
{
    extern const unsigned char main_css_start[] asm("_binary_main_css_start");
    extern const unsigned char main_css_end[]   asm("_binary_main_css_end");
    const size_t main_css_size = (main_css_end - main_css_start);
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)main_css_start, main_css_size);
    return ESP_OK;
}

void register_uri_handlers(httpd_handle_t _server) {
    for (int i = 0; i < uri_handlers_no; i++) {
        if (httpd_register_uri_handler(_server, &uri_handlers[i]) != ESP_OK) {
            //ESP_LOGW(TAG, "register uri failed for %d", i);
            return;
        }
    }
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

