#include "http_page.h"
#include "http_page_tpl.h"

static const char *TAG = "WEB";

void set_redirect_header(uint8_t time, const char *uri, char *data){
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "redirect uri: %s", uri);
    sprintf(data, html_header_redirect, time, uri);
}

void httpd_resp_sendstr_chunk(httpd_req_t *req, const char *buf){
    httpd_resp_send_chunk(req, buf, strlen(buf));
}

void print_html_header_data(char *buf, const char *title) {
    ESP_LOGI(TAG, __func__);
    sprintf(buf, html_header, title);
}

void print_html_footer_data(char *buf) {
    char * buf2 = malloc(25);
    get_localtime(buf2);
    sprintf(buf, html_footer, buf2, FW_VER);
    free(buf2);
}

void print_html_devinfo(char *buf) {
    ESP_LOGI(TAG, __func__);
    char * buf2 = malloc(20);
    get_uptime(buf2);
    sprintf(buf, html_devinfo, hostname, esp_get_free_heap_size(), wifi_get_rssi(), esp_wifi_get_vdd33(), buf2);
    free(buf2);
}


#ifdef DS18B20
void print_html_dsw(const ds18b20_t *_dsw, uint8_t cnt, char *buf) {

    sprintf(buf, html_ds18b20_header);
    uint8_t i = 0;   
    for (i=0;i<cnt;i++) {
		if ( _dsw[i].addr[0] ) {
                sprintf(buf+strlen(buf), html_ds18b20_data, i+1, _dsw[i].temp, 
                                                        _dsw[i].addr[0], _dsw[i].addr[1], _dsw[i].addr[2], _dsw[i].addr[3], 
                                                        _dsw[i].addr[4], _dsw[i].addr[5], _dsw[i].addr[6], _dsw[i].addr[7]);
            }
		}       
}
#endif  

#ifdef DHT
void print_html_dht(const dht_t *dht, char *buf) {
    sprintf(buf, html_dht_data, dht->temp, dht->hum);
}
#endif

void print_html_menu(char *buf) {
    sprintf(buf, "<menu>");
    sprintf(buf+strlen(buf), html_menu_item, "/", "Main");
    sprintf(buf+strlen(buf), html_menu_item, "/setup", "Setup");
    sprintf(buf+strlen(buf), html_menu_item, "/tools", "Tools");
    sprintf(buf+strlen(buf), html_menu_item, "/ota", "OTA");
    sprintf(buf+strlen(buf), html_menu_item, "/debug", "Debug");
    sprintf(buf+strlen(buf), "</menu>");
}

void print_html_tools(char *buf){
    sprintf(buf, html_tools_body);
}

void print_html_setup(char *buf){
    sprintf(buf, html_setup_body);

    // load from nvs
    // wifi_ssid, wifi_pass, wifi_mode
    // mqtt_en, mqtt_host, mqtt_login, mqtt_sint
    // ota_uri, ota_bufsz
    char wifi_ssid[20], wifi_pass[20];
    uint8_t wifi_mode = WIFI_MODE_NULL;

    esp_err_t err;
    // TODO

    wifi_nvs_cfg_t *wifi_nvs_cfg = malloc( sizeof( wifi_nvs_cfg_t));
    wifi_load_data_from_nvs( wifi_nvs_cfg);

    ota_nvs_data_t *nvs_ota = malloc( sizeof(ota_nvs_data_t));

    ESP_LOGD(TAG, "setup page: get_ota_nvs_data");
    err = get_ota_nvs_data(nvs_ota);

    ESP_LOGD(TAG, "Loaded ota uri %s", nvs_ota->uri);
    ESP_LOGD(TAG, "Loaded ota size buf %d", nvs_ota->buf_size);

    mqtt_config_t *mqtt_cfg = malloc(sizeof(mqtt_config_t));
    mqtt_get_current_config(mqtt_cfg);

    ESP_LOGD(TAG, "MQTT CFG broker url %s", mqtt_cfg->broker_url);
    ESP_LOGD(TAG, "MQTT CFG login %s", mqtt_cfg->login);
    ESP_LOGD(TAG, "MQTT CFG send_interval %d", mqtt_cfg->send_interval);
    ESP_LOGD(TAG, "MQTT CFG enabled %d", mqtt_cfg->enabled);

    sprintf(buf+strlen(buf), html_setup_form_post, 
                                wifi_nvs_cfg->ssid, //wifi_ssid, 
                                wifi_nvs_cfg->password, //wifi_pass,
                                //(wifi_mode == WIFI_MODE_STA) ? "checked=\"checked\"" : "",
                                (wifi_nvs_cfg->mode == WIFI_MODE_STA) ? "checked=\"checked\"" : "",
                                //(wifi_mode == WIFI_MODE_AP) ? "checked=\"checked\"" : "",
                                (wifi_nvs_cfg->mode == WIFI_MODE_AP) ? "checked=\"checked\"" : "",
                                //mqtt_cfg->enabled, 
                                mqtt_cfg->enabled ? "checked" : "",
                                //mqtt_en ? "checked" : "",
                                mqtt_cfg->broker_url, 
                                //mqtt_host, 
                                mqtt_cfg->login, 
                                //mqtt_login, 
                                mqtt_cfg->send_interval,
                                //mqtt_sint,
                                nvs_ota->uri, //(err) ? "" : nvs_ota->uri, 
                                nvs_ota->buf_size );
                             
    free(nvs_ota);
    free(mqtt_cfg);
}

void get_main_page_data(char *data) {
    ESP_LOGI(TAG, __func__);
    print_html_header_data(data, "Main page"); // TODO: взять из context
    print_html_devinfo(data+strlen(data));

    sprintf(data+strlen(data), "<div id=\"sens\">");    
    /*    
    #ifdef DS18B20
        print_html_dsw(&ds18b20, DSW_COUNT, data+strlen(data));
    #endif
    #ifdef DHT
        sprintf(data+strlen(data), html_dht_header);
        print_html_dht(&dht, data+strlen(data));
        //print_html_dht(&dht2, data+strlen(data));
    #endif
    #ifdef GPIO
        print_html_gpio( data + strlen( data ));
    #endif
    */

    // ============================= PRINT COLOR EFFECT INFO ==============================
    //ledc->print_html_data(data);
  
    /*
    sprintf(data + strlen( data ), html_gpio_header);

    const char *html_relay_item = "<span><a href=\"#\" rel=\"relay\" data-id=\"%d\" data-title=\"GPIO%02d\" data-val=\"%d\">"
                                         "<button class=\"relay %s\">GPIO%02d</button></a></span>";

    for (int i=0; i<4; i++) {
        relay_t *relay = (relay_t *)relays[i];
        sprintf(data + strlen( data ), html_relay_item, 
                                        relay->pin,             // pin
                                        relay->pin,    // title
                                        relay->state,           // val
                                        relay->state ? "on" : "off",  // class
                                        relay->pin);  
          
    }
    sprintf(data + strlen( data ), html_gpio_end);
    */
    // ====================== checkbox ====================================================
    /*
    const char *html_checkbox = "<span><input type=\"checkbox\" rel=\"relay\" name=\"relay%d\" value=\"%d\" %s ></span> <span id=\"relay%d\">%s</span>";
    for (int i=0; i<4; i++) {
        relay_t *relay = (relay_t *)relays[i];
        sprintf(data + strlen( data ), html_checkbox, 
                                        relay->pin,
                                        relay->state,
                                        relay->state ? "checked" : "",
                                        relay->pin,
                                        relay->state ? "ON" : "OFF");
    }
    */
    // ***************************** ledc **********************************
    ESP_LOGI(TAG, "print ledc data");
    sprintf(data + strlen( data ), "<div class=\"ledc\"><h4>Led controller channels</h4>");

    const char *html_ledc_item = "<div><input type=\"range\" max=\"255\" name=\"ledc%d\" value=\"%d\"><i id=\"ledc%d\">%d</i></div>";

    for (int i=0; i < ledc->led_cnt; i++) {
        sprintf(data + strlen( data ), html_ledc_item, 
                                        ledc->channels[i].channel,   //name
                                        ledc->channels[i].duty,      // value
                                        ledc->channels[i].channel,   // id
                                        ledc->channels[i].duty);     // title
    }
    
    sprintf(data + strlen( data ), "</div>");
    // *********************************************************************

    // rgb_ledc->print_html_data(data);
    
    // ==============================================================================
    sprintf(data + strlen( data ), "<script type=\"text/javascript\" src=\"ajax.js\"></script>");
    sprintf(data+strlen(data), "</div>");
    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context  
}

void get_debug_page_data(char *data) {
    print_html_header_data(data, "Debug page"); // TODO: взять из context

    system_info_t *sys_info = malloc(sizeof(system_info_t));
    get_system_info(sys_info);

    char * buf = malloc(20);
    get_uptime(buf);

    char *ota_updated = malloc(25);
    if ( get_ota_upgraded_dt(ota_updated) != ESP_OK ) strcpy(ota_updated, "unknown");
    sprintf(data+strlen(data), html_debug, (sys_info->chip_info.chip_model == 0) ? "esp8266" : "esp32",
                                sys_info->chip_info.chip_id,
                                sys_info->chip_info.chip_revision,
                                sys_info->sdk_version,
                                sys_info->mem_info.flash_size / (1024 * 1024),
                                sys_info->mem_info.free_heap_size,
                                sys_info->mem_info.flash_size_map,
                                ota_updated,
                                sys_info->vdd33,
                                wifi_info.wifi_reconnect,
                                wifi_info.status ? "connected" : "disconnected",
                                wifi_get_rssi(),
                                print_wifi_mode(wifi_info.wifi_mode),
                                //IP_2_STR(&wifi_info.ip),
                                ip4addr_ntoa(&wifi_info.ip),
                                MAC2STR(wifi_info.mac),      
                                mqtt_error_count, mqtt_reconnects,                        
                                buf,
                                0
    );
    
    free(buf);
    free(ota_updated);
    free(sys_info);
    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context  
}

/*
void gpioprint_page_data(char *data) {

    uint8_t i;
    // for (i=0;i<GPIO_CNT;i++){
    //     //gpio
    //     if ( gpio[i].pin == MAX_GPIO) continue;
    //     //get_gpio_info(&gpio[i]);
    //     sprintf(data, "%d:%d;", gpio[i].pin, gpio[i].state);
    //     data += strlen(data);
    // }
    // TODO: handle relays gpio and other user configurablу output gpios or just detect output gpios within bitmask
    for (int i=0; i<GPIO_PIN_COUNT;i++) {
        if ( GPIO_MODE_OUTPUT & GPIO_MODE_DEF_OUTPUT) {
            sprintf(data, "%d:%d;", i, gpio_get_level(i));
            data += strlen(data);
        }
    }
}
*/

void tools_page_data(char *data) {
    print_html_header_data(data, "Tools page"); // TODO: взять из context
    print_html_tools(data+strlen(data));
    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context
}

void get_setup_page_data(char *data) {
    print_html_header_data(data, "Setup page"); // TODO: взять из context
    print_html_setup(data+strlen(data));
    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context
}

void restart_page_data(char *data) {
    print_html_header_data(data, "Restart page");
    
    sprintf(data+strlen(data), html_restart_body);

    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); 
}

void restarting_page_data(char *data) {
    print_html_header_data(data, "Restart page");
    
    sprintf(data+strlen(data), html_restarting_body);
    
    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); 
}

void get_ota_page_data(char *data){
    ESP_LOGD(TAG, __func__);
    print_html_header_data(data, "OTA page");

    const esp_partition_t* part = esp_ota_get_running_partition(); 
    const esp_partition_t *updpart = esp_ota_get_next_update_partition(NULL);  
    ESP_LOGD(TAG, "ota partition\ntype:\t%d\nsubtype:\t0x%2X\nlabel:\t%s\nsize:\t%d\naddress:\t0x%X", part->type, part->subtype, part->label, part->size, part->address);
    ESP_LOGD(TAG, "ota update_partition\ntype:\t%d\nsubtype:\t0x%2X\nlabel:\t%s\nsize:\t%d\naddress:\t0x%X", updpart->type, updpart->subtype, updpart->label, updpart->size, updpart->address);
    

    ota_nvs_data_t *nvs_ota = malloc( sizeof(ota_nvs_data_t));
    get_ota_nvs_data(nvs_ota);

    char * updated = malloc(25);
    if ( get_ota_upgraded_dt(updated) != ESP_OK ) strcpy(updated, "unknown");
    sprintf(data+strlen(data), html_ota_body, part->label, __TIME__, __DATE__, updated, nvs_ota->uri  );

    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context
    free(updated);
    free(nvs_ota);
    //ESP_LOGI(TAG, "page size: %d", strlen(data));
}

