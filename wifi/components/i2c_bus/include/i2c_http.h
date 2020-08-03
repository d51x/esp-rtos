#pragma once

#ifndef _I2C_HTTP_H_
#define _I2C_HTTP_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_handlers.h"
#include "httpd.h"
#include "i2c_bus.h"

extern const char *html_page_tools_i2c;      

                                                                                                                 
void i2c_register_http_print_data();
void i2c_register_http_handler(httpd_handle_t _server);
void i2c_register_http_menu();

#ifdef CONFIG_COMPONENT_I2C_SCANNER
esp_err_t i2cscan_get_handler(httpd_req_t *req);
#endif

esp_err_t i2c_get_handler(httpd_req_t *req);


#endif