#ifndef __HTTP_HANDLERS_H__
#define __HTTP_HANDLERS_H__

#include "esp_http_client.h"
#include "utils.h"
#include "http_utils.h"
#include "http_page.h"

#include "ota.h"
#include "wifi.h"
#include "mqtt_cl.h"
#include "driver/i2c.h"
#include "i2c_bus.h"
/*
pages:

  menu: 
      main - main page
      setup - auth, wifi type and auth for sta 
*/
#define WEB_SERVER_MAX_URI_HANDLERS 40

typedef enum {
    HTML_PAGE_CFG_WIFI = 1,
    HTML_PAGE_CFG_MQTT,
    HTML_PAGE_CFG_OTA
    
} html_page_cfg_num_t;

    //extern const unsigned char device_png_start[] asm("_binary_device_png_start");
    //extern const unsigned char device_png_end[]   asm("_binary_device_png_end");

    //extern const unsigned char memory_png_start[] asm("_binary_memory_png_start");
    //extern const unsigned char memory_png_end[]   asm("_binary_memory_png_end");

    extern const unsigned char menu_png_start[] asm("_binary_menu_png_start");
    extern const unsigned char menu_png_end[]   asm("_binary_menu_png_end");

    extern const unsigned char menu2_png_start[] asm("_binary_menu2_png_start");
    extern const unsigned char menu2_png_end[]   asm("_binary_menu2_png_end");


    //extern const unsigned char uptime_png_start[] asm("_binary_uptime_png_start");
    //extern const unsigned char uptime_png_end[]   asm("_binary_uptime_png_end");


   // extern const unsigned char wifi_png_start[] asm("_binary_wifi_png_start");
    //extern const unsigned char wifi_png_end[]   asm("_binary_wifi_png_end");

esp_err_t main_get_handler(httpd_req_t *req);
esp_err_t setup_get_handler(httpd_req_t *req);
esp_err_t config_get_handler(httpd_req_t *req);
esp_err_t tools_get_handler(httpd_req_t *req);
esp_err_t debug_get_handler(httpd_req_t *req);
esp_err_t update_get_handler(httpd_req_t *req);
esp_err_t update_post_handler(httpd_req_t *req);
esp_err_t reboot_get_handler(httpd_req_t *req);
esp_err_t favicon_get_handler(httpd_req_t *req);
esp_err_t icons_get_handler(httpd_req_t *req);

esp_err_t i2cscan_get_handler(httpd_req_t *req);


/*
esp_err_t icon_device_get_handler(httpd_req_t *req);
esp_err_t icon_memory_get_handler(httpd_req_t *req);
esp_err_t icon_menu_get_handler(httpd_req_t *req);
esp_err_t icon_menu2_get_handler(httpd_req_t *req);
esp_err_t icon_uptime_get_handler(httpd_req_t *req);
esp_err_t icon_wifi_get_handler(httpd_req_t *req);
*/

esp_err_t main_css_get_handler(httpd_req_t *req);
esp_err_t main_ajax_get_handler(httpd_req_t *req);

/*
httpd_uri_t uri_handlers[] = {
    { .uri      = "/",
      .method   = HTTP_GET,
      .handler  = main_get_handler,
      .user_ctx = "Main page",
    },   
    { .uri      = "/setup",
      .method   = HTTP_GET,
      .handler  = setup_get_handler,
      .user_ctx = "/setup",
    },
    {   .uri   = "/debug",
        .method    = HTTP_GET,
        .handler   = debug_get_handler,
        .user_ctx  = "Debug page",
    },
    {   .uri   = "/restart",    
        .method    = HTTP_GET,
        .handler   = restart_get_handler,
        .user_ctx  = NULL,   
    },
    {   .uri   = "/favicon.ico",    
        .method    = HTTP_GET,
        .handler   = favicon_get_handler,
        .user_ctx  = NULL,   
    },
    {   .uri   = "/main.css",    
        .method    = HTTP_GET,
        .handler   = main_css_get_handler,
        .user_ctx  = NULL,   
    },
    {   .uri   = "/ajax.js",    
        .method    = HTTP_GET,
        .handler   = main_ajax_get_handler,
        .user_ctx  = NULL,   
    },
};

*/


#endif /* __HTTP_HANDLERS_H__ */