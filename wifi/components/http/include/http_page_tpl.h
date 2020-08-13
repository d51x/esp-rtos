
#pragma once

#ifndef __HTTP_PAGE_TPL_H__
#define __HTTP_PAGE_TPL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define html_header_redirect  "<head><meta http-equiv='refresh' content='%d; URL=%s' /></head>"





//char *menu_uri[MENU_ITEM_COUNT] = {"/", "/setup", "/tools", "/update", "/debug"};   // 10 char
//char *menu_names[MENU_ITEM_COUNT] = {"Main", "Setup", "Tools", "Update", "Debug"};  // 10 char

const char *html_page_start1 ICACHE_RODATA_ATTR =    
  "<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
      "<title>"; 
      
const char *html_page_start2 ICACHE_RODATA_ATTR =    
  "</title>"
      "<meta name='viewport' content='width=480'>"
      "<meta name='mobile-web-app-capable' content='yes'><link rel='stylesheet' type='text/css' href='main.css'/>"
    "</head>"
    "<body>"
      "<div id='main'>"; 

const char *html_page_end1 ICACHE_RODATA_ATTR = 
      "<script type='text/javascript' src='ajax.js'></script>"
      "<div id='clear'></div>"
        "<div id='footer' class='rnd'>"
          "<div id='time'><span><b>";

const char *html_page_end2 ICACHE_RODATA_ATTR = 
      "</b></div>"            // local time
          "<div id='fw'><span><b>v.";
  
const char *html_page_end3 ICACHE_RODATA_ATTR = 
      "</b></span></div>"     // fw version
        "</div>"
      "</div>"
	  "</body>"
  "</html>";

const char *html_page_content_start ICACHE_RODATA_ATTR = "<div class='data'>";

const char *html_page_content_end ICACHE_RODATA_ATTR = "</div>";

const char *html_page_top_header1 ICACHE_RODATA_ATTR = 
  "<div id='header' class='bg'>"
    "<div id='host'>"
      "<h2>";

const char *html_page_top_header2 ICACHE_RODATA_ATTR = 
 "</h2>"   // hostname
    "</div>"                                                              // 
    "<div id='head-right' class='dropdown'>"		                
      "<div id='rssi'>"
        "<h4>";

const char *html_page_top_header3 ICACHE_RODATA_ATTR = 
 " dBm</h4>"
      "</div>"
	    "<div class='dropdown'>"
        "<button class='dropbtn'></button>"
	      "<div class='dropdown-content bg'>"
	        "<ul>";

const char *html_page_top_header4 ICACHE_RODATA_ATTR = 
 "</ul>" // menu items
	      "</div>"
	    "</div>"
	  "</div>"
	"</div>";


const char *html_page_menu_item1 ICACHE_RODATA_ATTR =  "<li><a href='";
const char *html_page_menu_item2 ICACHE_RODATA_ATTR =  "'>";
const char *html_page_menu_item3 ICACHE_RODATA_ATTR =  "</a></li>";

const char *html_page_devinfo1 ICACHE_RODATA_ATTR = 
  "<div id='dev-info' class='rnd brd-btm'>"
	  "<div id='memory'><span><b>Free heap: </b> ";

const char *html_page_devinfo2 ICACHE_RODATA_ATTR = 
  "</span></div>"  // free mem
    "<div id='uptime'><span><b>Uptime: </b> ";

const char *html_page_devinfo3 ICACHE_RODATA_ATTR = 
  "</span></div>"     // uptime
  "</div>";

const char *html_page_reboot_button_block ICACHE_RODATA_ATTR = 
  "<div class='group rnd'><div class='lf2'>" 
    "<button id='reboot' class='button off rht' onclick='reboot()'>Перезагрузить</button>" 
    "<div id='rbt'>Rebooting... Please, wait!</div>" 
    "</div>"
  "</div>";

#endif 