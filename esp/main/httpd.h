#ifndef __HTTPD_H__
#define __HTTPD_H__


#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include <sys/param.h>
#include "nvs.h"
//#include "utils.h"






esp_err_t _http_event_handler(esp_http_client_event_t *evt);

void webserver_init(httpd_handle_t _server);
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);
void register_uri_handlers(httpd_handle_t _server);

void get_main_page_data(char *data);

#endif /* __HTTPD_H__ */