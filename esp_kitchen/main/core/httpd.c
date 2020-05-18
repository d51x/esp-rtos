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


static void process_mqtt_param(httpd_req_t *req) {
    mqtt_config_t *mqtt_cfg = malloc(sizeof(mqtt_config_t));
    char param[100];

    if ( http_get_key_str(req, "mqtt_en", param, sizeof(param)) == ESP_OK ) {
        mqtt_cfg->enabled = 1;
    } else {
        mqtt_cfg->enabled = 0;
    }

    if ( http_get_key_str(req, "mqtt_host", param, sizeof(param)) == ESP_OK ) {
        url_decode(param, mqtt_cfg->broker_url);
    }


    if ( http_get_key_str(req, "mqtt_login",  param, sizeof( param )) == ESP_OK ) {
        strcpy(mqtt_cfg->login, param);
    } else {
        ESP_LOGE(TAG, "mqtt login not found");    
    }

    if ( http_get_key_str(req, "mqtt_sint",  param, sizeof( param )) == ESP_OK ) {
        mqtt_cfg->send_interval = atoi(param);
    }

    mqtt_save_data_to_nvs(mqtt_cfg);
    free(mqtt_cfg);
}

static void process_wifi_param(httpd_req_t *req){
    char param[100];
    wifi_nvs_cfg_t *wifi_nvs_cfg = malloc( sizeof(wifi_nvs_cfg_t));

    if ( http_get_key_str(req, "hostname", param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_nvs_cfg->hostname, param);
        set_hostname( wifi_nvs_cfg->hostname );
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, wifi_hostname);
    }

    
    if ( http_get_key_str(req, "ssid", param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_nvs_cfg->ssid, param);
    }

    if ( http_get_key_str(req, "pass", param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_nvs_cfg->password, param);
    }

    wifi_nvs_cfg->mode = WIFI_MODE_NULL;
    if ( http_get_key_str(req, "wifi_mode", param, sizeof(param)) == ESP_OK ) {
        wifi_nvs_cfg->mode = atoi(param);
    } 

    wifi_save_data_to_nvs(wifi_nvs_cfg);
    free(wifi_nvs_cfg);

}

static void process_ota_param(httpd_req_t *req){
    char param[100];
    ota_nvs_data_t *nvs_ota = malloc( sizeof(ota_nvs_data_t));
    if ( http_get_key_str(req, "ota_uri", param, sizeof(param)) == ESP_OK ) {
        url_decode(param, nvs_ota->uri);
    } 
    
    if ( http_get_key_str(req, "ota_bufsz", param, sizeof(param)) == ESP_OK ) {
        nvs_ota->buf_size = atoi(param);
    } else {
        nvs_ota->buf_size = UPLOAD_BUFFER_SIZE;
    }

    set_ota_nvs_data( nvs_ota );
    free(nvs_ota);
}

esp_err_t setup_get_handler(httpd_req_t *req){
    
    char page[PAGE_DEFAULT_BUFFER_SIZE];
    
    if ( http_get_has_params(req) == ESP_OK) {
        process_wifi_param(req);
        process_mqtt_param(req);
        process_ota_param(req);
        make_redirect(req, 0, req->user_ctx);
    } 
    
    get_setup_page_data(page);            
    
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
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


static void process_pir_enabled(httpd_req_t *req, const char *param, size_t sz){
    uint8_t val = http_get_key_str(req, "pir_en", param, sz) == ESP_OK ;
    if ( is_pir_enabled == val ) return;
        // save pir_en
    is_pir_enabled = val;
    nvs_param_u8_save("pir", "enabled", is_pir_enabled);

    pir_t *pir = (pir_t *)pir_h;
    if ( is_pir_enabled )   pir->enable( pir_h );
    else                    pir->disable( pir_h );
}

static void process_pir_mode(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "pir-mode", param, sizeof(param)) != ESP_OK ) return;
    uint8_t val = atoi( param );
    if ( pir_mode == val ) return;
    pir_mode = val;
    nvs_param_u8_save("pir", "mode", pir_mode);
}

static void process_pir_off_delay(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "pir_off_delay", param, sizeof(param)) != ESP_OK ) return;
    uint8_t val = atoi( param );
    if ( pir_timer_off_delay == val ) return;
    pir_timer_off_delay = val;
    nvs_param_u16_save("pir", "piroffdelay", pir_timer_off_delay);
    
    pir_t *pir = (pir_t *)pir_h;
    pir->set_interval_low( pir_h, pir_timer_off_delay);
}

static void process_pir_adc(httpd_req_t *req, const char *param, size_t sz){
    /*
    if ( http_get_key_str(req, "adclvl", param, sizeof(param)) != ESP_OK ) return;     
    uint16_t val = atoi( param );
    if ( adc_lvl == val ) return;
    adc_lvl = val;
    nvs_param_u16_save("main", "adclvl", adc_lvl);
    mqtt_publish_adc_thld();
    */

    if ( http_get_key_str(req, "adclvlmin", param, sizeof(param)) == ESP_OK ) {
        uint16_t val = atoi( param );
        if ( adc_lvl_min != val ) {
            adc_lvl_min = val;  
            nvs_param_u16_save("main", "adclvlmin", adc_lvl_min);   
            mqtt_publish_adc_thld_min();   
        }
    } 
    
    if ( http_get_key_str(req, "adclvlmax", param, sizeof(param)) == ESP_OK ) {
        uint16_t val = atoi( param );
        if ( adc_lvl_max != val ) {
            adc_lvl_max = val;  
            nvs_param_u16_save("main", "adclvlmax", adc_lvl_max);   
            mqtt_publish_adc_thld_max();   
        }
    } 
}

static void process_pir_dark_time(httpd_req_t *req, const char *param, size_t sz) 
{

    char param1[40];
    if ( http_get_key_str(req, "darktimestart", param1, sizeof(param1)) == ESP_OK ) {
        uint16_t val = atoi( param1 );
        if ( dark_time_start != val ) {
            dark_time_start = val;  
            nvs_param_u16_save("pir", "darktimestart", dark_time_start);   
            mqtt_publish_dark_time_start();
        }  
    } 
    
    if ( http_get_key_str(req, "darktimeend", param1, sizeof(param1)) == ESP_OK ) {
        uint16_t val = atoi( param1 );
        if ( dark_time_end != val ) {
            dark_time_end = val;  
            nvs_param_u16_save("pir", "darktimeend", dark_time_end);   
            mqtt_publish_dark_time_end();
        }   
    } 
    
    if ( http_get_key_str(req, "dutymaxdark", param, sizeof(param)) == ESP_OK ) {
        uint8_t val = atoi( param );
        if ( white_led_max_duty_dark != val ) {
            white_led_max_duty_dark = val;  
            nvs_param_u8_save("pir", "dutymaxdark", white_led_max_duty_dark);   
            //mqtt_publish_adc_thld_max();   
        }
    } 
  
}


static void process_pir_fadeup(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "fadeup", param, sizeof(param)) != ESP_OK ) return;
    uint16_t val = atoi( param );
    if ( white_led_fadeup_delay == val ) return;
    white_led_fadeup_delay = val;
    nvs_param_u16_save("pir", "fadeupdelay", white_led_fadeup_delay);
}

static void process_pir_fadedown(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "fadedown", param, sizeof(param)) != ESP_OK ) return;
    uint16_t val = atoi( param );
    if ( white_led_fadeout_delay == val ) return;
    white_led_fadeout_delay = val;
    nvs_param_u16_save("pir", "fadedowndelay", white_led_fadeout_delay);
}

static void process_ir_gpio(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "irpin", param, sizeof(param)) != ESP_OK ) return;   
    uint8_t val = atoi( param );
    if ( ir_pin == val ) return;
    ir_pin = val;
    nvs_param_u8_save("main", "irpin", ir_pin);
}

static void process_ir_delay(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "irdelay", param, sizeof(param)) != ESP_OK ) return;
    uint8_t val = atoi( param );
    if ( ir_delay == val ) return;
    ir_delay = val;
    nvs_param_u16_save("main", "irdelay", ir_delay); 
}

static void process_pir_gpio(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "pirpin", param, sizeof(param)) != ESP_OK ) return;
    uint8_t val = atoi( param );
    if ( pirpin == val ) return;
    pirpin = val;
    nvs_param_u8_save("main", "pirpin", pirpin);
}

static void process_fan_gpio(httpd_req_t *req, const char *param, size_t sz){
    if ( http_get_key_str(req, "fanpin", param, sizeof(param)) != ESP_OK ) return;
    uint8_t val = atoi( param );
    if ( relay_fan_pin == val ) return;
    relay_fan_pin = val;
    nvs_param_u8_save("main", "fanpin", relay_fan_pin);
}

static void process_fan_gpio_invert(httpd_req_t *req, const char *param, size_t sz){
    uint8_t val = http_get_key_str(req, "faninv", param, sizeof(param)) == ESP_OK;             
    if ( relay_invert == val ) return;
    relay_invert = val;
    nvs_param_u8_save("main", "faninv", relay_invert);
}

static void process_led_channels_gpio(httpd_req_t *req, const char *param, size_t sz){
    for (uint8_t i = 0; i < LED_CTRL_MAX; i++) {
        char tmp[7];
        sprintf(tmp, "ledpin%d", i);
        if ( http_get_key_str(req, tmp, param, sizeof(param)) == ESP_OK ) {
            main_led_pins[i] = atoi( param );
        }
        nvs_param_save("main", "channels_pin", main_led_pins, LED_CTRL_MAX*sizeof(uint8_t));
    } 
}

esp_err_t tools_get_handler(httpd_req_t *req){
    char page[2048*2];

    // check get params
    if ( http_get_has_params(req) == ESP_OK) {
        // pir_en=1&pir_off_delay=15&fadeup=100&fadedown=150&st=1
        char param[40];
        int val = 0;

        if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {  
            val = atoi( param );

            if ( val == 1 ) {
                // process block1 params
                process_pir_enabled(req, param, sizeof(param));
                process_pir_mode(req, param, sizeof(param));
                process_pir_off_delay(req, param, sizeof(param));
                process_pir_adc(req, param, sizeof(param));
                process_pir_dark_time(req, param, sizeof(param));
                process_pir_fadeup(req, param, sizeof(param));
                process_pir_fadedown(req, param, sizeof(param));
                
            } else if (val == 2) {
                // process block2 params
                process_ir_gpio(req, param, sizeof(param));
                process_ir_delay(req, param, sizeof(param));
                process_pir_gpio(req, param, sizeof(param));
                process_fan_gpio(req, param, sizeof(param));
                process_fan_gpio_invert(req, param, sizeof(param));
                process_led_channels_gpio(req, param, sizeof(param));
            }
        }
        make_redirect(req, 0, (char *)req->user_ctx);
    }

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
            // TODO: handle all relays and other user configurable output gpios
            // // TODO: handle relays gpio and other user configurablу output gpios or just detect output gpios within bitmask
            // for (int i=0; i<4; i++)   {
            //     relay_t *relay = (relay_t *)relays[i];
            //     if ( relay->pin == pin ) {
            //         relay_write( relay, st);    
            //         break;
            //     }
            // } 
            // КОСТЫЛЬ!!!
            //if ( pin == RELAY_FAN_PIN ) {
            if ( pin == relay_fan_pin ) {
                relay_write(relay_fan_h, st);
            }
        }
    } 

    if ( redirect == 1)
        //strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"0; URL=/\" /></head>");
        ESP_LOGD(TAG, "Redirecting....");
        make_redirect(req, 0, "/");

    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}

/*
esp_err_t gpioprint_get_handler(httpd_req_t *req){
    char page[1024];
    gpioprint_page_data(page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;
}
*/

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
        xTaskCreate(&systemRebootTask, "systemRebootTask", 2048, 2000, 5, NULL);
        httpd_resp_set_hdr(req, "Refresh", "3; /");
        //httpd_resp_set_hdr(req, "Location", "/");
        httpd_resp_send(req, NULL, 0);
        return ESP_OK;
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
            httpd_resp_set_hdr(req, "Refresh", "3; /ota");
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
    httpd_uri_t my_uri;
      my_uri.uri      = strdup(uri);
      my_uri.method   = HTTP_GET;
      my_uri.handler  = func;
      my_uri.user_ctx = NULL;
    // ESP_LOGI(TAG, __func__);
    // ESP_LOGI(TAG, "my_uri.handler addr: %p", my_uri.handler);
    // ESP_LOGI(TAG, "_server is %s", (_server != NULL) ? "not NULL" : "NULL");
    esp_err_t err = httpd_register_uri_handler(_server, &my_uri);
    // if ( err == ESP_OK ) {
    //     ESP_LOGI(TAG, "%s registered successfully",my_uri.uri );
    // } else {
    //     ESP_LOGI(TAG, "%s not registered. Error %s", my_uri.uri, esp_err_to_name(err) );
    // }
}

void webserver_init(httpd_handle_t* _server) {
    /* Start the web server */
    if ( *_server == NULL) {
        *_server = start_webserver();
        //ESP_LOGI(TAG, "webserver_init _server is %s", (_server != NULL) ? "not NULL" : "NULL");
    }
}



httpd_handle_t start_webserver(void){
    httpd_handle_t _server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = WEB_SERVER_STACK_SIZE;
    config.max_uri_handlers = 15; //100; //uri_handlers_no; //WEB_SERVER_MAX_URI_GET_HANDLERS;
    config.recv_wait_timeout = 10;
    //        .max_uri_handlers   = 8,                        
    //    .max_resp_headers   = 8, 
    // Start the httpd server
    ESP_LOGD(TAG, "******** Starting server on port: '%d'", config.server_port);
    if (httpd_start(&_server, &config) == ESP_OK) {
        // Set URI handlers

        register_uri_handlers(_server);
        //ESP_LOGI(TAG, "******** start_webserver _server is %s", (_server != NULL) ? "not NULL" : "NULL");
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

void make_redirect(httpd_req_t *req, uint8_t timeout, const char *path) {
    char t[3];
    itoa(timeout, t, 10);
    char *hdr = calloc(1, strlen(t) + 2 + strlen(path) + 1);
    strcpy(hdr, t);
    strcat(hdr, "; ");
    strcat(hdr, path);
    httpd_resp_set_hdr(req, "Refresh", hdr);
    httpd_resp_send(req, NULL, 0);
    free(hdr);
}