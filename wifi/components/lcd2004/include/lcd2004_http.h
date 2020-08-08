#pragma once

#ifndef _LCD2004_HTTP_H_
#define _LCD2004_HTTP_H_



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_handlers.h"
#include "httpd.h"
#include "lcd2004.h"

#ifdef CONFIG_COMPONENT_LCD2004_HTTP

extern const char *html_block_lcd2004;    
void lcd2004_register_http_print_data(); 
void lcd2004_register_http_handler(httpd_handle_t _server);

#endif

#endif