#pragma once

#ifndef _LEDCONTROL_HTTP_H_
#define _LEDCONTROL_HTTP_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ledcontrol.h"
#include "http_handlers.h"
#include "httpd.h"

#ifdef CONFIG_LED_CONTROL_HTTP

#define LED_CONTROL_URI "/ledc"

extern const char *html_block_led_control;    
void ledcontrol_register_http_print_data(ledcontrol_handle_t dev_h);
void ledcontrol_register_http_handler(httpd_handle_t _server, ledcontrol_handle_t dev_h);
void ledcontrol_http_init(httpd_handle_t _server, ledcontrol_handle_t dev_h);

#endif
#endif