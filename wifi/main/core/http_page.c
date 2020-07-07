#include "http_page.h"
#include "http_page_tpl.h"


static const char *TAG = "WEB";

void set_redirect_header(uint8_t time, const char *uri, char *data){
    sprintf(data, html_header_redirect, time, uri);
}

void httpd_resp_sendstr_chunk(httpd_req_t *req, const char *buf){
    httpd_resp_send_chunk(req, buf, strlen(buf));
}




void page_generate_html_start(char *buf, const char *title)
{
    sprintf(buf, html_page_start, title);
}

void page_generate_html_end(char *_buf)
{
    char * buf = malloc(25);
    get_localtime(buf);
    sprintf(_buf + strlen(_buf), html_page_end, buf, FW_VER);
    free(buf);
}






void page_generate_top_header(char *buf)
{
    char * uptime = malloc(20);
    get_uptime(uptime);

    char *menu = malloc((strlen(html_page_menu_item) + 10 + 10)* MENU_ITEM_COUNT + 1);
    page_generate_menu(menu);

    sprintf(buf + strlen(buf), html_page_top_header
                , wifi_cfg->hostname  // hostname
                , wifi_get_rssi()       // rssi
                , menu
                );

    sprintf(buf + strlen(buf), html_page_devinfo
                                , esp_get_free_heap_size()
                                , uptime
                                );
    free(uptime);
    free(menu);
}

void page_generate_data(char *buf, const char *data)
{
    sprintf(buf + strlen(buf), html_page_content_start);
    sprintf(buf + strlen(buf), data);
    sprintf(buf + strlen(buf), html_page_content_end);
}



void page_generate_menu(char *buf)
{
    uint8_t i;
    strcpy(buf, "");
    for ( i = 0; i < MENU_ITEM_COUNT; i++) {
        sprintf(buf+strlen(buf), html_page_menu_item, menu_uri[i], menu_names[i]);
    }
}


void generate_page(char *page, const char *title, const char *data) 
{   
    page_generate_html_start(page, title);
    page_generate_top_header(page);
    page_generate_data(page, data);
    page_generate_html_end(page);   
}

void show_page_main(char *data)
{
    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    strcpy(page_data, "Main page");
    generate_page(data, "Main page", page_data);
    free(page_data);
}

void show_page_setup(char *data)
{
    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    //strcpy(page_data, "Setup");

    sprintf(page_data, html_page_setup_wifi 
                       , wifi_cfg->hostname         // hostname
                       , wifi_cfg->ssid         // ssid
                       , wifi_cfg->password         // pass    
                       , (wifi_cfg->mode == WIFI_MODE_STA) ? "checked" : ""          // sta checked
                       , (wifi_cfg->mode == WIFI_MODE_AP)  ? "checked" : ""         // ap checked
                       );

    mqtt_config_t *mqtt_cfg = malloc(sizeof(mqtt_config_t));
    mqtt_get_cfg(mqtt_cfg);
    sprintf(page_data + strlen(page_data), html_page_setup_mqtt 
                       , mqtt_cfg->enabled ? "checked" : ""         // enabled
                       , mqtt_cfg->broker_url         // host
                       , mqtt_cfg->login         // login    
                       , mqtt_cfg->password         // password    
                       , mqtt_cfg->send_interval          // send interval
                       );
    free(mqtt_cfg);
    
    sprintf( page_data + strlen(page_data), html_page_reboot_button_block);
    generate_page(data, "Setup", page_data);
    free(page_data);
    
}

void show_page_config(char *data)
{
    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);

    generate_page(data, "Config", page_data);
    free(page_data);
}

void show_page_tools(char *data)
{
    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    /* зарегистрирровать callback отрисовки на странице */
    /* зарегистрировать callback обработки параметров */
    uint8_t sda = 2;
    uint8_t scl = 0;
    sprintf(page_data, html_page_tools_i2c, sda, scl);
    generate_page(data, "Tools", page_data);
    free(page_data);
}

void show_page_update(char *data)
{
    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    strcpy(page_data, html_page_ota);
    generate_page(data, "Update with OTA", page_data);
    free(page_data);
}

void show_page_debug(char *data)
{

}

void show_restart_page_data(char *data)
{

}

void show_restarting_page_data(char *data)
{

}


