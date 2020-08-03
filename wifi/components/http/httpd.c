#include "httpd.h"
#include "http_handlers.h"


static const char *TAG = "HTTPD";


/********************* Basic Handlers Start *******************/




void register_uri_handlers(httpd_handle_t _server) {
    
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_ROOT], main_get_handler, &PAGES_HANDLER[PAGE_URI_ROOT]); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_SETUP], setup_get_handler, &PAGES_HANDLER[PAGE_URI_SETUP]); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_DEBUG], debug_get_handler, &PAGES_HANDLER[PAGE_URI_DEBUG]); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_TOOLS], tools_get_handler, &PAGES_HANDLER[PAGE_URI_TOOLS]); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_OTA], update_get_handler, &PAGES_HANDLER[PAGE_URI_OTA]); 
    add_uri_post_handler( _server, "/update", update_post_handler); 

    
    //add_uri_get_handler( _server, "/reboot", reboot_get_handler); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_REBOOT], reboot_get_handler, NULL); 

    //add_uri_get_handler( _server, "/favicon.ico", favicon_get_handler); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_FAVICO], favicon_get_handler, NULL); 

    //add_uri_get_handler( _server, "/main.css", main_css_get_handler); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_CSS], main_css_get_handler, NULL); 

    //add_uri_get_handler( _server, "/ajax.js", main_ajax_get_handler); 
    add_uri_get_handler( _server, PAGES_URI[PAGE_URI_AJAX], main_ajax_get_handler, NULL); 


    
    //add_uri_get_handler( _server, "/device.png", icons_get_handler); 
    //add_uri_get_handler( _server, "/wifi.png", icons_get_handler); 
    //add_uri_get_handler( _server, "/memory.png", icons_get_handler); 
    //add_uri_get_handler( _server, "/uptime.png", icons_get_handler); 
    add_uri_get_handler( _server, "/menu.png", icons_get_handler, NULL); 
    add_uri_get_handler( _server, "/menu2.png", icons_get_handler, NULL); 

    

/*
    for (int i = 0; i < uri_handlers_no; i++) {
        if (httpd_register_uri_handler(_server, &uri_handlers[i]) != ESP_OK) {
            //ESP_LOGW(TAG, "register uri failed for %d", i);
            return;
        }
    }
*/
}

void add_uri_get_handler(httpd_handle_t _server, const char *uri, httpd_uri_func func, void *ctx) {

    user_ctx_t *_ctx = (user_ctx_t *) ctx;

    httpd_uri_t my_uri;
      my_uri.uri      = strdup(uri);
      my_uri.method   = HTTP_GET;
      my_uri.handler  = func;


    if ( ctx != NULL ) 
    {
      my_uri.user_ctx = ctx;
    }

    esp_err_t err = httpd_register_uri_handler(_server, &my_uri);
    
    /*
     if ( err == ESP_OK ) {
         ESP_LOGI(TAG, "%s registered successfully",my_uri.uri );
     } else {
         ESP_LOGI(TAG, "%s not registered. Error %s", my_uri.uri, esp_err_to_name(err) );
     }
     */
}


void add_uri_post_handler(httpd_handle_t _server, const char *uri, httpd_uri_func func) {
    httpd_uri_t my_uri;
      my_uri.uri      = strdup(uri);
      my_uri.method   = HTTP_POST;
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
        *_server = webserver_start();
        //ESP_LOGI(TAG, "webserver_init _server is %s", (_server != NULL) ? "not NULL" : "NULL");
    }
}



httpd_handle_t webserver_start(void){
    httpd_handle_t _server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = WEB_SERVER_STACK_SIZE;
    config.max_uri_handlers = WEB_SERVER_MAX_URI_HANDLERS; //100; //uri_handlers_no; //WEB_SERVER_MAX_URI_GET_HANDLERS;
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

void webserver_stop(httpd_handle_t _server){
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