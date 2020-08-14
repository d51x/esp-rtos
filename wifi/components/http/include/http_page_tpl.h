
#pragma once

#ifndef __HTTP_PAGE_TPL_H__
#define __HTTP_PAGE_TPL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ICACHE_RODATA_ATTR __attribute__((section(".irom.text")))

extern const char *html_header_redirect ICACHE_RODATA_ATTR;
extern const char *html_page_start ICACHE_RODATA_ATTR; 
extern const char *html_page_end ICACHE_RODATA_ATTR;

extern const char *html_page_content_start ICACHE_RODATA_ATTR;
extern const char *html_page_content_end ICACHE_RODATA_ATTR;

extern const char *html_page_top_header ICACHE_RODATA_ATTR;

extern const char *html_page_menu_item ICACHE_RODATA_ATTR;

extern const char *html_page_devinfo ICACHE_RODATA_ATTR;

extern const char *html_page_reboot_button_block ICACHE_RODATA_ATTR;


// ====== data block  ======================================================================
extern const char *html_block_data_start ICACHE_RODATA_ATTR;   
extern const char *html_block_data_end ICACHE_RODATA_ATTR;  

// ====== forms  ======================================================================
extern const char *html_block_data_form_start ICACHE_RODATA_ATTR;  

extern const char *html_block_data_form_submit ICACHE_RODATA_ATTR;

extern const char *html_block_data_form_end ICACHE_RODATA_ATTR;

extern const char *html_block_data_form_item_label_edit ICACHE_RODATA_ATTR;
extern const char *html_block_data_form_item_checkbox  ICACHE_RODATA_ATTR;

#endif 