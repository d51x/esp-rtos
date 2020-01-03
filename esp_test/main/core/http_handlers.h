#ifndef __HTTP_HANDLERS_H__
#define __HTTP_HANDLERS_H__

#include "esp_http_server.h"
/*
pages:

  menu: 
      main - main page
      setup - auth, wifi type and auth for sta 
      hardware 
      devices 
      tools - restart, update


*/

esp_err_t main_get_handler(httpd_req_t *req);
esp_err_t setup_get_handler(httpd_req_t *req);
esp_err_t debug_get_handler(httpd_req_t *req);
esp_err_t tools_get_handler(httpd_req_t *req);
esp_err_t gpio_get_handler(httpd_req_t *req);
esp_err_t gpioprint_get_handler(httpd_req_t *req);
esp_err_t restart_get_handler(httpd_req_t *req);
esp_err_t ota_get_handler(httpd_req_t *req);
esp_err_t ota_post_handler(httpd_req_t *req);
esp_err_t favicon_get_handler(httpd_req_t *req);
esp_err_t main_css_get_handler(httpd_req_t *req);

httpd_uri_t uri_handlers[] = {
    { .uri      = "/",
      .method   = HTTP_GET,
      .handler  = main_get_handler,
      .user_ctx = "Main page",
    },   
    { .uri      = "/setup",
      .method   = HTTP_GET,
      .handler  = setup_get_handler,
      .user_ctx = "Setup page",
    },
    {   .uri   = "/debug",
        .method    = HTTP_GET,
        .handler   = debug_get_handler,
        .user_ctx  = "Debug page",
    },
    {   .uri   = "/tools",
        .method    = HTTP_GET,
        .handler   = tools_get_handler,
        .user_ctx  = "Tools page",
    },
    {   .uri   = "/gpio",    // get request from internal system
        .method    = HTTP_GET,
        .handler   = gpio_get_handler,
        .user_ctx  = NULL,   
    },
    {   .uri   = "/gpioprint",    
        .method    = HTTP_GET,
        .handler   = gpioprint_get_handler,
        .user_ctx  = NULL,   
    },
    {   .uri   = "/restart",    
        .method    = HTTP_GET,
        .handler   = restart_get_handler,
        .user_ctx  = NULL,   
    },
    {   .uri   = "/ota",    
        .method    = HTTP_GET,
        .handler   = ota_get_handler,
        .user_ctx  = NULL,   
    },
    {   .uri   = "/fwupload",    
        .method    = HTTP_POST,
        .handler   = ota_post_handler,
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
};

int uri_handlers_no = sizeof(uri_handlers)/sizeof(httpd_uri_t);

#endif /* __HTTP_HANDLERS_H__ */