
#ifndef __HTTP_PAGE_H__
#define __HTTP_PAGE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_http_server.h"
#include "rom/ets_sys.h"


#include "wifi.h"
#include "ota.h"
#include "utils.h"
#include "http_utils.h"

#include "mqtt.h"

#define STR_ON "ВКЛ"
#define STR_OFF "ВЫКЛ"
#define STR_YES "ДА"
#define STR_NO "НЕТ"

void print_html_header_data(char *buf, const char *title);
void print_html_footer_data(char *buf);
void print_html_devinfo(char *buf);

void print_html_menu(char *buf);
void httpd_resp_sendstr_chunk(httpd_req_t *req, const char *buf);

void gpioprint_page_data(char *data);
void tools_page_data(char *data);
void get_setup_page_data(char *data);
void get_debug_page_data(char *data);
void restart_page_data(char *data);
void restarting_page_data(char *data);

void get_main_page_data(char *data);
void get_ota_page_data(char *data);

void set_redirect_header(uint8_t time, const char *uri, char *data);
#endif 