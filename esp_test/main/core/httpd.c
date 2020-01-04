#include "httpd.h"
#include "http_handlers.h"


static const char *TAG = "HTTPD";


/********************* Basic Handlers Start *******************/
/* An HTTP GET handler */
esp_err_t main_get_handler(httpd_req_t *req){
    char page[PAGE_MAIN_BUFFER_SIZE];  
    // const char* resp_str = (const char*) req->user_ctx;
    get_main_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

esp_err_t setup_get_handler(httpd_req_t *req){
    
    char page[PAGE_DEFAULT_BUFFER_SIZE];
    
    if ( http_get_has_params(req) == ESP_OK) {

        wifi_nvs_cfg_t *wifi_nvs_cfg = malloc( sizeof(wifi_nvs_cfg_t));
        // wifi ssid
        //char wifi_ssid[20], wifi_pass[20];
        ESP_LOGD(TAG, "Find wifi ssid");
        if ( http_get_key_str(req, "ssid", wifi_nvs_cfg->ssid, sizeof(wifi_nvs_cfg->ssid)) != ESP_OK ) {
            strcpy(wifi_nvs_cfg->ssid, "");
            ESP_LOGE(TAG, "wifi ssid not found, set to %s", wifi_nvs_cfg->ssid);
        } else {
            ESP_LOGD(TAG, "wifi ssid found - %s", wifi_nvs_cfg->ssid);
        }

        ESP_LOGD(TAG, "Find wifi pass");
        //if ( http_get_key_str(req, "pass", wifi_pass, sizeof(wifi_pass)) != ESP_OK ) {
        if ( http_get_key_str(req, "pass", wifi_nvs_cfg->password, sizeof(wifi_nvs_cfg->password)) != ESP_OK ) {
            strcpy(wifi_nvs_cfg->password, "");
            ESP_LOGE(TAG, "wifi pass not found, set to %s", wifi_nvs_cfg->password);
        } else {
            ESP_LOGD(TAG, "wifi pass found - %s", wifi_nvs_cfg->password);
        }

        //uint8_t wifi_mode;
        //char wifi_mode[10];
        uint8_t wifi_mode = WIFI_MODE_NULL;
        ESP_LOGD(TAG, "Find wifi mode");
        //if ( http_get_key_str(req, "wifi_mode", wifi_mode, sizeof(wifi_mode)) != ESP_OK ) {
        //if ( http_get_key_uint8(req, "wifi_mode", (uint8_t *)&wifi_mode) != ESP_OK ) {
        if ( http_get_key_uint8(req, "wifi_mode", (uint8_t *)&wifi_nvs_cfg->mode) != ESP_OK ) {
            //strcpy(wifi_mode, "wifi_sta");
            ESP_LOGE(TAG, "wifi mode not found, set to %d", wifi_nvs_cfg->mode);
        } else {
            ESP_LOGD(TAG, "wifi mode found - %d", wifi_nvs_cfg->mode);
        }


        wifi_save_data_to_nvs(wifi_nvs_cfg);
        free(wifi_nvs_cfg);

        ESP_LOGD(TAG, "Find MQTT data");
        // ======= mqtt data =====
        mqtt_config_t *mqtt_cfg = malloc(sizeof(mqtt_config_t));
        //uint32_t mqtt_en;        
        //if ( http_get_key_long(req, "mqtt_en", (long *)&mqtt_en) != ESP_OK ) mqtt_en = 1;
        //if ( http_get_key_uint8(req, "mqtt_en", (uint8_t *)&mqtt_cfg->enabled) != ESP_OK ) {
        char en[10];    
        if ( http_get_key_str(req, "mqtt_en", en, sizeof(en)) != ESP_OK ) {
            mqtt_cfg->enabled = 0;
            ESP_LOGE(TAG, "MQTT Enabled not found, set to %d", mqtt_cfg->enabled);
        } else {
            mqtt_cfg->enabled = 1;
            ESP_LOGD(TAG, "MQTT Enabled found - %d", mqtt_cfg->enabled);
        }

        ESP_LOGD(TAG, "Find MQTT broker url");
        char mqtt_host[100];
        //if ( http_get_key_str(req, "mqtt_host", mqtt_host, sizeof(mqtt_host)) != ESP_OK ) strcpy(mqtt_host, "");
        if ( http_get_key_str(req, "mqtt_host", mqtt_host, sizeof(mqtt_host)) != ESP_OK ) {
            strcpy(mqtt_cfg->broker_url, MQTT_BROKER_URL);
            ESP_LOGE(TAG, "MQTT broker url not found, set to %s", mqtt_cfg->broker_url);
        } else {
            url_decode(mqtt_host, mqtt_cfg->broker_url);
            ESP_LOGD(TAG, "MQTT broker url found %s", mqtt_cfg->broker_url);
        }


        ESP_LOGD(TAG, "Find MQTT login");
        //char mqtt_login[20];
        //if ( http_get_key_str(req, "mqtt_login", mqtt_login, sizeof(mqtt_login)) != ESP_OK ) strcpy(mqtt_login, "");
        if ( http_get_key_str(req, "mqtt_login",  mqtt_cfg->login, sizeof( mqtt_cfg->login)) != ESP_OK ) {
            strcpy( mqtt_cfg->login, "");
            ESP_LOGE(TAG, "MQTT login not found, set to %s", mqtt_cfg->login);
        } else {
            ESP_LOGD(TAG, "MQTT login found - %s", mqtt_cfg->login);
        }    

        ESP_LOGI(TAG, "Find MQTT send interval");
        //uint32_t mqtt_sendint; 
        //if ( http_get_key_long(req, "mqtt_sint", (long *) &mqtt_sendint) != ESP_OK ) mqtt_sendint = 60;
        if ( http_get_key_long(req, "mqtt_sint", (long *)&mqtt_cfg->send_interval) != ESP_OK ) {
            mqtt_cfg->send_interval = MQTT_SEND_INTERVAL;
            ESP_LOGE(TAG, "MQTT send interval not found, set to %d", mqtt_cfg->send_interval);
        } else {
            ESP_LOGD(TAG, "MQTT send interval found - %d", mqtt_cfg->send_interval);
        }

        ESP_LOGD(TAG, "Try to save MQTT data to nvs......");
        mqtt_save_data_to_nvs(mqtt_cfg);

        free(mqtt_cfg);


        ESP_LOGD(TAG, "Find OTA url...");
        char ota_uri[100];
        if ( http_get_key_str(req, "ota_uri", ota_uri, sizeof(ota_uri)) != ESP_OK ) {
            strcpy(ota_uri, "");
            ESP_LOGE(TAG, "OTA url not found, set to %s", ota_uri);
        } else {
            ESP_LOGD(TAG, "OTA url found - %s", ota_uri);
        }
        

        ESP_LOGD(TAG, "Find OTA buf size...");
        uint32_t ota_bufsz; 
        if ( http_get_key_long(req, "ota_bufsz", (long *)&ota_bufsz) != ESP_OK ) {
            ota_bufsz = UPLOAD_BUFFER_SIZE;
            ESP_LOGE(TAG, "OTA buf size not found, set to %d", ota_bufsz);
        } else {
            ESP_LOGD(TAG, "OTA buf size found - %d", ota_bufsz);
        }

        // nvs save
        ESP_LOGD(TAG, "Try to save OTA data to nvs......");
        ota_nvs_data_t *nvs_ota = malloc( sizeof(ota_nvs_data_t));
        url_decode(ota_uri, nvs_ota->uri);
        ESP_LOGD(TAG, "Try to save ota_uri %s", ota_uri);
        ESP_LOGD(TAG, "Try to save ota_uri %s", nvs_ota->uri);
        nvs_ota->buf_size = ota_bufsz;
        set_ota_nvs_data( nvs_ota );
        free(nvs_ota);


        // TODO: restart mqtt
        //mqtt_stop();
        //mqtt_start();
        //char header[40] = "";
        //set_redirect_header(0, "/setup", header);
        //strcpy(page, header);
        httpd_resp_set_status(req, "307 Temporary Redirect");
        httpd_resp_set_hdr(req, "Location", "/setup");
        //httpd_resp_send(req, page, strlen(page));
        httpd_resp_send(req, NULL, 0);
        return ESP_OK;
    } else {
        ESP_LOGD(TAG, "Just show setup data");
        get_setup_page_data(page);            
        
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_send(req, page, strlen(page));
        return ESP_OK;
    }
}

esp_err_t debug_get_handler(httpd_req_t *req){
    char page[PAGE_DEFAULT_BUFFER_SIZE];
    get_debug_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

esp_err_t config_get_handler(httpd_req_t *req){
    char page[PAGE_DEFAULT_BUFFER_SIZE];
    get_setup_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

esp_err_t tools_get_handler(httpd_req_t *req){
    char page[1024];
    tools_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;    
}


esp_err_t gpio_get_handler(httpd_req_t *req){
    ESP_LOGD(TAG, __func__);
    uint8_t redirect = 1;
    //uint8_t found = 0;
    char page[200];
    if ( http_get_has_params(req) == ESP_OK) {
        uint8_t fn;  // function Id
        uint8_t st;
        uint8_t pin = 255;
        uint8_t error = 0;

        ESP_LOGD(TAG, "has get params");
        redirect = 0;
        if ( http_get_key_uint8(req, "pin", &pin) == ESP_OK ) {
            ESP_LOGD(TAG, "pin = %d", pin);
        } else {
            error = 1;
            ESP_LOGD(TAG, "fail get param pin");
        }    

        if ( http_get_key_uint8(req, "st", &st) == ESP_OK ) {
            if ( st > 1) error = 1;
            ESP_LOGD(TAG, "st = %d", st);
        } else {
            error = 1;
            ESP_LOGD(TAG, "fail get param st");
        }    

        if ( http_get_key_uint8(req, "fn", &fn) == ESP_OK ) {
            redirect = 1;
            if (fn == 1) {
                

            } else {
                // unknown function, redirect to main page                    
                ESP_LOGD(TAG, "Unkown function Id %d found", fn);    
            }
        } else {
            // no function found, remote control gpio
            ESP_LOGD(TAG, "Remote control gpio");
            
        }
        
        strcpy(page, "OK");
        if ( error ) {
            strcpy(page, "ERROR");
        } else {
            // control gpios
            ESP_LOGD(TAG, "Send command (st = %d) to GPIO%d", st, pin); 
            if ( set_gpio(pin, st) != ESP_OK) 
                strcpy(page, "ERROR");
            for (int i=0; i<4; i++)   {
                relay_t *relay = (relay_t *)relays[i];
                if ( relay->pin == pin ) {
                    relay_write( relay, st);    
                    break;
                }
            } 
        }
    } 

    if ( redirect == 1)
        //strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"0; URL=/\" /></head>");
        ESP_LOGD(TAG, "Redirecting....");
        char header[40] = "";
        set_redirect_header(0, "/", header);
        ESP_LOGD(TAG, header);
        strcpy(page, header);
        ESP_LOGD(TAG, page);

    httpd_resp_send(req, page, strlen(page));
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
    char page[2048];
    // определить наличие параметров get
    //size_t buf_len;
    //char*  buf;
    uint8_t found = 0;
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

    if ( http_get_has_params(req) == ESP_OK) {
        uint8_t st;
        ESP_LOGV(TAG, "get request has params");
        if ( http_get_key_uint8(req, "st", &st) == ESP_OK ) {
            ESP_LOGV(TAG, "st found %d", st);
            found = (st == 1);        
        }
    }              
    
    if ( found ) {  
        // restart esp and redirect to mainpage after X sec
        ESP_LOGD(TAG, "create reboot task");
        xTaskCreate(&systemRebootTask, "systemRebootTask", 2048, (int *)2000, 5, NULL);
        //strcpy(page, html_restart_header);
        char header[40] = "";
        set_redirect_header(10, "/", header);
        strcpy(page, header);
        restarting_page_data(page+strlen(page));  
    } else {
        restart_page_data(page);          
    }
    httpd_resp_send(req, page, strlen(page));  
    //if ( found ) xTaskCreate(restart_task, "restart_task", 1024, NULL, 10, NULL);
    return ESP_OK;
}

esp_err_t ota_get_handler(httpd_req_t *req){
    char page[2048];
    uint8_t found = 0;
    // check params
    if ( http_get_has_params(req) == ESP_OK) {
        uint8_t st;
        if ( http_get_key_uint8(req, "st", &st) == ESP_OK ) {
            found = (st == 1);      
            ESP_LOGD(TAG, "param st found- value %d", st);
        } else {
            ESP_LOGD(TAG, "param st not found");
        }
    } else {
        ESP_LOGD(TAG, "params not found");
    }

    if ( found ) {  
        // download bin from url
        // show upgrading dialog or page

        xEventGroupClearBits(ota_event_group, OTA_IDLE_BIT);
        char err_text[400];        
        uint32_t start_time = millis();
        
        ota_init();

        
        if ( ota_task_upgrade_from_url(err_text) == ESP_OK ) {
            // upgrading is OK, restart esp and redirect to main page in 10
            //strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
            char header[40] = "";
            set_redirect_header(10, "/", header);
            strcpy(page, header);

            sprintf(page+strlen(page), "File uploaded, it took %d sec. Restarting....", (uint32_t)(millis()-start_time)/1000);
            xTaskCreate(&systemRebootTask, "systemRebootTask", 2048, (int *)3000, 5, NULL);    
        } else {
            // upgrading fail show ota page again
            // show upgrade fail and redirect to ota page in 10 sec
            //strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");

            // handle_error_msg(esp_err_t err)
            char header[40] = "";
            set_redirect_header(10, "/ota", header);
            strcpy(page, header);
            // incorrect redirect header call reset!!!!!!
            strcpy(page+strlen(page), "OTA upgrade failed...\n");
            strcpy(page+strlen(page), err_text);
            httpd_resp_set_status(req, HTTPD_500);
        }
        
        xEventGroupSetBits(ota_event_group, OTA_IDLE_BIT);
    } else {
        ESP_LOGD(TAG, "just show page");
        get_ota_page_data(page);
    }    
      
    httpd_resp_send(req, page, strlen(page));  
    return ESP_OK;
}


esp_err_t ota_post_handler(httpd_req_t *req){
    xEventGroupClearBits(ota_event_group, OTA_IDLE_BIT);
    char err_text[400];
    char page[1024];
    uint32_t start_time = millis(); 

    ota_init();       
    if ( ota_task_upgrade_from_web(req, err_text) == ESP_OK ) {
        // upgrading is OK, restart esp and redirect to main page in 10
        char header[40] = "";
        set_redirect_header(10, "/", header);
        strcpy(page, header);

        sprintf(page+strlen(page), "File uploaded, it took %d sec. Restarting....", (uint32_t)(millis()-start_time)/1000);
        xTaskCreate(&systemRebootTask, "systemRebootTask", 2048, (int *)3000, 5, NULL);      
    } else {
        // upgrading fail show ota page again
        // show upgrade fail and redirect to ota page in 10 sec
        //strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
        char header[40] = "";
        set_redirect_header(10, "/", header);
        strcpy(page, header);

        strcpy(page+strlen(page), "OTA upgrade failed...\n");        
        strcpy(page+strlen(page), err_text);        
        httpd_resp_set_status(req, HTTPD_500);
    }
    xEventGroupSetBits(ota_event_group, OTA_IDLE_BIT);
    httpd_resp_send(req, page, -1);
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

esp_err_t main_ajax_get_handler(httpd_req_t *req)
{
    extern const unsigned char ajax_js_start[] asm("_binary_ajax_js_start");
    extern const unsigned char ajax_js_end[]   asm("_binary_ajax_js_end");
    const size_t ajax_js_size = (ajax_js_end - ajax_js_start);
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)ajax_js_start, ajax_js_size);
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

void add_uri_get_handler(httpd_handle_t _server, const char *uri, httpd_uri_func func) {
ESP_LOGI(TAG, __func__);
ESP_LOGI(TAG, "func addr: %p", func);
    httpd_uri_t my_uri;
      my_uri.uri      = strdup(uri);
      my_uri.method   = HTTP_GET;
      my_uri.handler  = func;
      my_uri.user_ctx = NULL;
    ESP_LOGI(TAG, __func__);
ESP_LOGI(TAG, "my_uri.handler addr: %p", my_uri.handler);
ESP_LOGI(TAG, "_server is %s", (_server != NULL) ? "not NULL" : "NULL");
    esp_err_t err = httpd_register_uri_handler(_server, &my_uri);
    if ( err == ESP_OK ) {
        ESP_LOGI(TAG, "%s registered successfully",my_uri.uri );
    } else {
        ESP_LOGI(TAG, "%s not registered. Error %s", my_uri.uri, esp_err_to_name(err) );
    }
}

void webserver_init(httpd_handle_t* _server) {
    /* Start the web server */
    if ( *_server == NULL) {
        *_server = start_webserver();
        ESP_LOGI(TAG, "webserver_init _server is %s", (_server != NULL) ? "not NULL" : "NULL");
    }
}



httpd_handle_t start_webserver(void){
    httpd_handle_t _server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = WEB_SERVER_STACK_SIZE;
    config.max_uri_handlers = 100; //uri_handlers_no; //WEB_SERVER_MAX_URI_GET_HANDLERS;
    config.recv_wait_timeout = 10;
    //        .max_uri_handlers   = 8,                        
    //    .max_resp_headers   = 8, 
    // Start the httpd server
    ESP_LOGD(TAG, "******** Starting server on port: '%d'", config.server_port);
    if (httpd_start(&_server, &config) == ESP_OK) {
        // Set URI handlers

        register_uri_handlers(_server);
        ESP_LOGI(TAG, "******** start_webserver _server is %s", (_server != NULL) ? "not NULL" : "NULL");
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
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
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

