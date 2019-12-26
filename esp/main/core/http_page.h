
#ifndef __HTTP_PAGE_H__
#define __HTTP_PAGE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_http_server.h"
#include "rom/ets_sys.h"

#include "core.h"
#include "wifi.h"
#include "gpio.h"
#include "ota/http_ota.h"
#include "utils.h"

#ifdef DS18B20
#include "sensors/dsw.h"
#endif

#ifdef DHT
#include "sensors/dht.h"
#endif

#include "mqtt.h"


void print_html_header_data(char *buf, const char *title);
void print_html_footer_data(char *buf);
void print_html_devinfo(char *buf);

#ifdef DS18B20
void print_html_dsw(const ds18b20_t *_dsw, uint8_t cnt, char *buf);
#endif

#ifdef DHT
void print_html_dht(const dht_t *dht, char *buf);
#endif

void print_html_gpio(char *buf);
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