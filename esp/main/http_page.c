#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "http_page.h"
static const char *TAG = "WEB";


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

void print_html_dsw(char *buf) {
    sprintf(buf, html_ds18b20_header);
    uint8_t i = 0;   
    for (i=0;i<DSW_COUNT;i++) {
		if ( ds18b20[i].addr[0] ) {
                sprintf(buf+strlen(buf), html_ds18b20_data, i+1, ds18b20[i].temp, 
                                                        ds18b20[i].addr[0], ds18b20[i].addr[1], ds18b20[i].addr[2], ds18b20[i].addr[3], 
                                                        ds18b20[i].addr[4], ds18b20[i].addr[5], ds18b20[i].addr[6], ds18b20[i].addr[7]);
            }
		}    
}

void print_html_dht(char *buf) {
    sprintf(buf, html_dht_data, dht.temp, dht.hum);
}

void print_html_gpio(char *buf) {
    sprintf(buf, html_gpio_header);

    for (int i=0;i<GPIO_CNT;i++) {
        if ( gpio[i].pin == MAX_GPIO ) continue;
        sprintf(buf+strlen(buf), html_gpio_item, gpio[i].pin, !gpio[i].state, gpio[i].state ? "on" : "off", gpio[i].pin);  // TODO: учесть инвертирование в будущем
    }
    sprintf(buf+strlen(buf), "</div>");
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

void get_main_page_data(char *data) {
    //header
    sprintf(data, html_header, "Main page");
    
    //body

    char * buf = malloc(20);
    char * buf2 = malloc(25);
    get_uptime(buf);    
    get_localtime(buf2);
    sprintf(data+strlen(data), html_page_device_info, hostname, wifi_get_rssi(), esp_get_free_heap_size(), esp_wifi_get_vdd33(), buf, buf2);
    free(buf);
    free(buf2);

    #ifdef DHT
    sprintf(data+strlen(data), html_dht_data, dht.temp, dht.hum);
    #endif

    uint8_t i = 0;
    #ifdef DS18B20
    for (i=0;i<DSW_COUNT;i++) {
			if ( ds18b20[i].addr[0] ) {
				//ds18b20[i].temp;
				//ESP_LOGI(TAG, "addr %02x %02x %02x %02x %02x %02x %02x %02x  temp: %.2f C", ds18b20[i].addr[0], 
				//											ds18b20[i].addr[1], ds18b20[i].addr[2], ds18b20[i].addr[3], ds18b20[i].addr[4], 
				//											ds18b20[i].addr[5], ds18b20[i].addr[6], ds18b20[i].addr[7], ds18b20[i].temp);
                sprintf(data+strlen(data), html_ds18b20_data, i+1, ds18b20[i].temp, ds18b20[i].addr[0], 
															ds18b20[i].addr[1], ds18b20[i].addr[2], ds18b20[i].addr[3], ds18b20[i].addr[4], 
															ds18b20[i].addr[5], ds18b20[i].addr[6], ds18b20[i].addr[7]);
			}

		}    
    #endif

    #ifdef GPIO
    for (i=0;i<GPIO_CNT;i++) {
        if ( gpio[i].pin == MAX_GPIO ) continue;
        ESP_LOGI(TAG, "data len = %d", strlen(data));
        sprintf(data+strlen(data), html_main_body, gpio[i].pin, gpio[i].pin, !gpio[i].state, gpio[i].state ? "ON" : "OFF");  // TODO: учесть инвертирование в будущем
    }
    #endif

    sprintf(data+strlen(data), html_form_post_test, sda, scl);
   // "<span>GPIO%d <a href=""/gpio?num=%d&st=%d"">%s</a></span>"

    //footer
    ESP_LOGI(TAG, "data len = %d", strlen(data));
    ESP_LOGI(TAG, "start footer");
    sprintf(data + strlen(data), html_footer);
    ESP_LOGI(TAG, "data len = %d", strlen(data));
    ESP_LOGI(TAG, "end footer");



}

void get_debug_page_data(char *data) {

    system_info_t *sys_info = malloc(sizeof(system_info_t));
    get_system_info(sys_info);

    char * buf = malloc(20);
    get_uptime(buf);

        
    sprintf(data, html_debug, (sys_info->chip_info.chip_model == 0) ? "esp8266" : "esp32",
                                sys_info->chip_info.chip_id,
                                sys_info->chip_info.chip_revision,
                                sys_info->sdk_version,
                                sys_info->mem_info.flash_size / (1024 * 1024),
                                sys_info->mem_info.free_heap_size,
                                sys_info->mem_info.flash_size_map,
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
                                ds18b20_total_crc_error
    );
    
    free(buf);
    free(sys_info);

}

void gpioprint_page_data(char *data) {

    uint8_t i;
    for (i=0;i<GPIO_CNT;i++){
        //gpio
        if ( gpio[i].pin == MAX_GPIO) continue;
        //get_gpio_info(&gpio[i]);
        sprintf(data, "%d:%d;", gpio[i].pin, gpio[i].state);
        data += strlen(data);
    }
}

void tools_page_data(char *data) {
    //header
    print_html_header_data(data, "Tools page"); // TODO: взять из context
    print_html_devinfo(data+strlen(data));

    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context
}

void get_setup_page_data(char *data) {
    print_html_header_data(data, "Setup page"); // TODO: взять из context
    print_html_devinfo(data+strlen(data));

    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context
}

void restart_page_data(char *data) {
    //header
    sprintf(data, html_header, "Restart page");
    
    //body
    sprintf(data+strlen(data), html_restart_body);
    
    //footer
    sprintf(data + strlen(data), html_footer);
}

void restarting_page_data(char *data) {
    //header
    //sprintf(data, html_header, "Restart page");
    
    //body
    sprintf(data+strlen(data), html_restarting_body);
    
    // сделать обратный счетчик на js
    //footer
    //sprintf(data + strlen(data), html_footer);
}

void get_ota_page_data(char *data){
    ESP_LOGI(TAG, __func__);
    print_html_header_data(data, "OTA page");

    const esp_partition_t* part = esp_ota_get_running_partition(); 
    const esp_partition_t *updpart = esp_ota_get_next_update_partition(NULL);  
    //ESP_LOGI(TAG, "ota partition\ntype:\t%d\nsubtype:\t0x%2X\nlabel:\t%s\nsize:\t%d\naddress:\t0x%X", part->type, part->subtype, part->label, part->size, part->address);
    sprintf(data+strlen(data), "<p>OTA (current)</p>");
    //sprintf(data+strlen(data), "<p>type: %d</p>", part->type);
    sprintf(data+strlen(data), "<p>subtype: 0x%2X</p>", part->subtype);
    sprintf(data+strlen(data), "<p>label: %s</p>", part->label);
    //sprintf(data+strlen(data), "<p>size: %d</p>", part->size);
    //sprintf(data+strlen(data), "<p>address: 0x%X</p>", part->address);
    sprintf(data+strlen(data), "<p>Compiled: %s %s</p>", __TIME__, __DATE__);
    sprintf(data+strlen(data), "<p>Updated: %s</p>", "<read from nvs>");

    
    
    //ESP_LOGI(TAG, "free mem: %d", esp_get_free_heap_size());

    //ESP_LOGI(TAG, "ota update_partition\ntype:\t%d\nsubtype:\t0x%2X\nlabel:\t%s\nsize:\t%d\naddress:\t0x%X", updpart->type, updpart->subtype, updpart->label, updpart->size, updpart->address);
    sprintf(data+strlen(data), "<p>OTA (next update)</p>");
    //sprintf(data+strlen(data), "<p>type: %d</p>", updpart->type);
    sprintf(data+strlen(data), "<p>subtype: 0x%2X</p>", updpart->subtype);
    sprintf(data+strlen(data), "<p>label: %s</p>", updpart->label);
    //sprintf(data+strlen(data), "<p>size: %d</p>", updpart->size);
    //sprintf(data+strlen(data), "<p>address: 0x%X</p>", updpart->address);    

    sprintf(data+strlen(data), "<p>Обновить с сервера:</p>");
    sprintf(data+strlen(data), html_ota_body, FIRMWARE_UPGRADE_URL);

    //ESP_LOGI(TAG, "free mem: %d", esp_get_free_heap_size());

    sprintf(data+strlen(data), "<p>Выбрать Firmware</p>");
    sprintf(data+strlen(data), "<form enctype=\"multipart/form-data\" method=\"post\" action=\"/fwupload\" accept=\".bin\">"
                                "<p><input type=\"file\" name=\"file\">"
                                "<input type=\"submit\" value=\"Загрузить\"></p>"
                                "</form>");



    print_html_menu(data+strlen(data));
    print_html_footer_data(data+strlen(data)); // TODO: взять из context

    //ESP_LOGI(TAG, "page size: %d", strlen(data));
}

