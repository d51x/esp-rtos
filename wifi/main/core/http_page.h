
#ifndef __HTTP_PAGE_H__
#define __HTTP_PAGE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_http_server.h"
#include "rom/ets_sys.h"


#include "utils.h"
#include "wifi.h"
#include "mqtt_cl.h"
#include "http_utils.h"



#define STR_ON "ВКЛ"
#define STR_OFF "ВЫКЛ"
#define STR_YES "ДА"
#define STR_NO "НЕТ"

#define PAGE_MAIN_BUFFER_SIZE       1024*4
#define PAGE_DEFAULT_BUFFER_SIZE    1024*4



void page_generate_html_start(char *buf, const char *title);
void page_generate_html_end(char *buf);
void page_generate_top_header(char *buf);
void page_generate_menu(char *buf);

void page_generate_data(char *buf, const char *data);





void show_page_main(char *data);
void show_page_setup(char *data);
void show_page_config(char *data);
void show_page_tools(char *data);
void show_page_update(char *data);
void show_page_debug(char *data);

void show_restart_page_data(char *data);
void show_restarting_page_data(char *data);



void httpd_resp_sendstr_chunk(httpd_req_t *req, const char *buf);




void set_redirect_header(uint8_t time, const char *uri, char *data);

#endif 