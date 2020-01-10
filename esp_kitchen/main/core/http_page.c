#include "http_page.h"
#include "http_page_tpl.h"
#include "core.h"

static const char *TAG = "WEB";

void set_redirect_header(uint8_t time, const char *uri, char *data){
    sprintf(data, html_header_redirect, time, uri);
}

void httpd_resp_sendstr_chunk(httpd_req_t *req, const char *buf){
    httpd_resp_send_chunk(req, buf, strlen(buf));
}

void print_html_header_data(char *buf, const char *title) {
    sprintf(buf, html_header, title);
}

void print_html_footer_data(char *buf) {
    char * buf2 = malloc(25);
    get_localtime(buf2);
    sprintf(buf, html_footer, buf2, FW_VER);
    free(buf2);
}

void print_html_devinfo(char *buf) {
    char * buf2 = malloc(20);
    get_uptime(buf2);
    sprintf(buf, html_devinfo, hostname, esp_get_free_heap_size(), wifi_get_rssi(), esp_wifi_get_vdd33(), buf2);
    free(buf2);
}


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
    strcpy(buf, "");
    const char *pir_mode_item = "<option value=\"%d\" %s>%s</option>";
    char pir_mode_items[60*4] = "";

    for (uint8_t i = 0; i<4; i++) {
        
        sprintf(pir_mode_items + strlen(pir_mode_items), pir_mode_item, 
                                                         i, 
                                                         (pir_mode == i) ? "selected=\"selected\" " : "",
                                                         pir_mode_desc[i]);
    }

    const char *html_form_opt = "<div>"
                            "<form method=\"GET\" id=\"opt\">"
                            "<p><span><input type=\"checkbox\" name=\"pir_en\" value=\"%d\" %s></span>"
                                "<span>Датчик движения</span></p>"  // checkbox
                            "<p><span>Режим подсветки рабочей зоны:</span></p>"  // combobox
                            "<p>"
                            "<select id=\"pir-mode\" name=\"pir-mode\" onchange=\"pirmode()\" form=\"opt\">"
                            "%s"
                            "</select>"
                            "</p>"
                                
                            "<p><span>Pir off delay: </span><input size=\"2\" name=\"pir_off_delay\" value=\"%d\"><span>sec</span></p>"
                            "<p><span>Мин уровень освещенности: </span><input size=\"2\" name=\"adclvl\" value=\"%d\"><span>sec</span></p>"
                            "<p><span>Fadeup delay: </span><input size=\"2\" name=\"fadeup\" value=\"%d\"><span>msec</span></p>"
                            "<p><span>Fadedown delay: </span><input size=\"2\" name=\"fadedown\" value=\"%d\"><span>msec</span></p>"
                            "<p><input type=\"hidden\" name=\"st\" value=\"1\"></p>"
                            //"<p><input class=\"on\" type=\"submit\" value=\"\"></p>" 
                            "<button class=\"on\">Set</button>"
                            "</form>"                       
                            "</div>"
                           ;

    sprintf(buf+strlen(buf), html_form_opt
                            , 1 
                            , is_pir_enabled ? "checked" : ""
                            , pir_mode_items
                            , pir_timer_off_delay
                            , adc_lvl
                            , white_led_fadeup_delay
                            , white_led_fadeout_delay
    );

  const char *html_form_opt_pin = "<div>"
                            "<form method=\"GET\" id=\"optpin\">"        
                            "<p><span>Канал R GPIO: </span><input size=\"2\" name=\"ledpin0\" value=\"%d\"></p>"
                            "<p><span>Канал G GPIO: </span><input size=\"2\" name=\"ledpin1\" value=\"%d\"></p>"
                            "<p><span>Канал B GPIO: </span><input size=\"2\" name=\"ledpin2\" value=\"%d\"></p>"
                            "<p><span>Канал W GPIO: </span><input size=\"2\" name=\"ledpin3\" value=\"%d\"></p>"
                            "<p><span>Канал WW GPIO: </span><input size=\"2\" name=\"ledpin4\" value=\"%d\"></p>"
                            
                            "<p>"
                                "<span>Fan GPIO: </span><span><input size=\"2\" name=\"fanpin\" value=\"%d\"></span>"
                                "<span><input type=\"checkbox\" name=\"faninv\" value=\"%d\" %s>Invert</span>"
                            "</p>"
                            "<p><span>PIR GPIO: </span><span><input size=\"2\" name=\"pirpin\" value=\"%d\"></span></p>"
                            "<p>"
                                "<span>IR GPIO: </span><span><input size=\"2\" name=\"irpin\" value=\"%d\"></span>"
                                "<span>Delay: </span><span><input size=\"4\" name=\"irdelay\" value=\"%d\"></span>"
                            "</p>"
                            "<p><input type=\"hidden\" name=\"st\" value=\"2\"></p>"
                            //"<p><input class=\"on\" type=\"submit\" value=\"\"></p>" 
                            "<button class=\"on\">Set</button>"
                            "</form>"                       
                            "</div>"
                            
                           ;
    sprintf(buf+strlen(buf), html_form_opt_pin, main_led_pins[0]
                                              , main_led_pins[1]
                                              , main_led_pins[2]
                                              , main_led_pins[3]
                                              , main_led_pins[4]
                                              , relay_fan_pin
                                              , relay_invert, relay_invert ? "checked" : ""
                                              , pirpin
                                              , ir_pin, ir_delay
    );

    sprintf(buf+strlen(buf), html_restart_button);
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
    print_html_header_data(data, "Main page"); // TODO: взять из context
    print_html_devinfo(data+strlen(data));

    sprintf(data+strlen(data), "<div id=\"sens\" style=\"border-top: 1px solid grey;\">");    

    // ===== print param info ===========================
    sprintf(data+strlen(data), "<div style=\"text-align: left; font-size: 14px; "
                                            "border-bottom: 1px solid grey;\">");

    sprintf(data+strlen(data), "<p>Датчик освещенности: <b>%d</b></p>", get_adc());
    sprintf(data+strlen(data), "<p>Датчик движения: <b>%s</b></p>", is_pir_enabled ? STR_ON : STR_OFF);
  
    sprintf(data+strlen(data), "<p>Режим работы: <b>%s</b></p>", pir_mode_desc[pir_mode] );
    sprintf(data+strlen(data), "<p>Сейчас: <b>%s</b></p>", is_dark ? "темно" : "светло" );
    sprintf(data+strlen(data), "<p>Движение: <b>%s</b></p>", is_motion ? STR_YES : STR_NO );
    if ( count_down_off <= pir_timer_off_delay )
        sprintf(data+strlen(data), "<p>Осталось сек до выключения: <b>%d</b> сек</p>", count_down_off);
    if (count_up_motion > 0) 
        sprintf(data+strlen(data), "<p>Прошло после начала движения: <b>%d</b> сек</p>", count_up_motion);

    sprintf(data+strlen(data), "</div>"); 

    // ================= print fan info =====================
    
    const char *html_relay_item = "<div style=\"text-align: left;\">"
                                  "<span>Вентилятор: </span>"
                                  "<span><a href=\"#\" rel=\"relay\" data-id=\"%d\" data-title=\"Fan\" data-val=\"%d\">"
                                         "<button class=\"relay %s\">Fan</button>"
                                  "</a></span></div>";
    relay_t *fan = (relay_t *)relay_fan_h;
    sprintf(data + strlen( data ),  html_relay_item, 
                                    fan->pin,             // pin        data-id
                                    fan->state,           // val        data-val
                                    fan->state ? "on" : "off"  // class
                                    );     
    // === print white led info =================================
    const char *html_white_led_info = "<div style=\"text-align: left;\">"
                                 "<span>Подсветка рабочей зоны: </span>"
                                 "<span><i id=\"ledc%d\">%d</i></span>"
                                 "<span><input type=\"range\" max=\"255\" name=\"ledc%d\" value=\"%d\"></span>"
                                 "</div>";
    
    ledcontrol_channel_t *ch = ledc->channels + LED_CTRL_WHITE_CH;
    sprintf(data + strlen( data ),  html_white_led_info, 
                                    ch->channel,      // ajax value
                                    ch->duty,          // ajax value
                                    ch->channel,   // input param
                                    ch->duty      // input param
                                ); 

    // ***************************** RGB LED DATA **********************************                            
    // ***************************** ledc **********************************
    sprintf(data + strlen( data ), "<div class=\"ledc\"><h4>RGB лента</h4>");
    const char *html_rgb_item = "<p>"
                                 "<span><b>%s:</b></span>"
                                 "<span><input type=\"range\" max=\"255\" name=\"ledc%d\" value=\"%d\"></span>"
                                 "<span><i id=\"ledc%d\">%d</i></span></p>";

    // RED
    ch = ledc->channels + LED_CTRL_RED_CH;
    sprintf(data + strlen( data ), html_rgb_item, 
                                    "R",
                                    ch->channel,   //name
                                    ch->duty,      // value
                                    ch->channel,   // id
                                    ch->duty);     // title    

    // GREEN
    ch = ledc->channels + LED_CTRL_GREEN_CH;
    sprintf(data + strlen( data ), html_rgb_item, 
                                    "G",
                                    ch->channel,   //name
                                    ch->duty,      // value
                                    ch->channel,   // id
                                    ch->duty);     // title   

    // BLUE
    ch = ledc->channels + LED_CTRL_BLUE_CH;
    sprintf(data + strlen( data ), html_rgb_item, 
                                    "B",
                                    ch->channel,   //name
                                    ch->duty,      // value
                                    ch->channel,   // id
                                    ch->duty);     // title  

    sprintf(data + strlen( data ), "</div>");
    
    // ============================= PRINT COLOR EFFECT INFO ==============================
    //ledc->print_html_data(data);

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

    // *********************************************************************

    rgb_ledc->print_html_data(data);
    
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
}

