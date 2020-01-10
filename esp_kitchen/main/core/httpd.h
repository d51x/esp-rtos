#ifndef __HTTPD_H__
#define __HTTPD_H__


#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include <sys/param.h>


#include "http_utils.h"
#include "core.h"
#include "http_page.h"
#include "nvsparam.h"
#include "mqtt.h"

#ifdef LEDCTRL
    #include "led_ctrl.h"
#endif




esp_err_t _http_event_handler(esp_http_client_event_t *evt);

void webserver_init(httpd_handle_t* _server);
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);
void register_uri_handlers(httpd_handle_t _server);
void add_uri_get_handler(httpd_handle_t _server, const char *uri, httpd_uri_func func);
void make_redirect(httpd_req_t *req, uint8_t timeout, const char *path);

#endif /* __HTTPD_H__ */