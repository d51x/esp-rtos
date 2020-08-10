
#pragma once

#ifndef __HTTP_PAGE_H__
#define __HTTP_PAGE_H__



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_http_server.h"
#include "esp_err.h"
#include "rom/ets_sys.h"


#include "utils.h"
#include "wifi.h"
#include "mqtt_cl.h"
#include "http_utils.h"
#include "http_handlers.h"



#define STR_ON "ВКЛ"
#define STR_OFF "ВЫКЛ"
#define STR_YES "ДА"
#define STR_NO "НЕТ"

#define PAGE_MAIN_BUFFER_SIZE       1024*8
#define PAGE_DEFAULT_BUFFER_SIZE    1024*8

typedef void (* func_http_print_page_block)(const char *data, void *args);  
typedef esp_err_t (*httpd_uri_process_fn)(httpd_req_t *req, void *args);

extern uint8_t http_print_page_block_count;

typedef struct {
    const char uri[HTTPD_MAX_URI_LEN + 1];
    uint8_t index;
    func_http_print_page_block fn_print_block;
    void *args1;
    char name[20];
    httpd_uri_process_fn process_cb;
    void *args2;
} http_print_page_block_t;

extern http_print_page_block_t *http_print_page_block;


void show_page_main(const char *title, char *data);
void show_page_setup(const char *title, char *data);
void show_page_config(const char *title, char *data);
void show_page_tools(const char *title, char *data);
void show_page_update(const char *title, char *data);
void show_page_debug(const char *title, char *data);

void show_custom_page(const char *title, char *data);
void page_generate_html_start(char *buf, const char *title);
void page_generate_html_end(char *buf);
void page_generate_top_header(char *buf);
void page_show_menu(char *buf);
void page_initialize_menu();

void page_generate_data(char *buf, const char *data);





void show_http_page(httpd_req_t *req, char *data);



void show_restart_page_data(char *data);
void show_restarting_page_data(char *data);



void httpd_resp_sendstr_chunk(httpd_req_t *req, const char *buf);




void set_redirect_header(uint8_t time, const char *uri, char *data);

// зарегистрировать функцию колбека, которая будет вызвана при выводе информации на страницу
// uri - на какой странице выводить
// index - очередность вывода
// fn_cb - функция коллбека для формирования буфера
esp_err_t register_print_page_block(const char *name, const char *uri, uint8_t index, func_http_print_page_block fn_cb, void *args1, httpd_uri_func fn_cb2, void *args2);


esp_err_t register_http_page_menu(const char *uri, const char *name);

#endif 