
#pragma once

#ifndef __HTTP_PAGE_TPL_H__
#define __HTTP_PAGE_TPL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define html_header_redirect  "<head><meta http-equiv='refresh' content='%d; URL=%s' /></head>"





//char *menu_uri[MENU_ITEM_COUNT] = {"/", "/setup", "/tools", "/update", "/debug"};   // 10 char
//char *menu_names[MENU_ITEM_COUNT] = {"Main", "Setup", "Tools", "Update", "Debug"};  // 10 char

const char *html_page_start ICACHE_RODATA_ATTR =    
  "<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
      "<title>%s</title>"
      "<meta name='viewport' content='width=480'>"
      "<meta name='mobile-web-app-capable' content='yes'><link rel='stylesheet' type='text/css' href='main.css'/>"
      "<script type='text/javascript' src='ajax.js'></script>"
    "</head>"
    "<body>"
      "<div id='main'>"; 

const char *html_page_end ICACHE_RODATA_ATTR = 
      "<div id='clear'></div>"
        "<div id='footer' class='rnd'>"
          "<div id='time'><span><b>%s</b></div>"            // local time
          "<div id='fw'><span><b>v.%s</b></span></div>"     // fw version
        "</div>"
      "</div>"
	  "</body>"
  "</html>";

const char *html_page_content_start ICACHE_RODATA_ATTR = "<div class='data'>";

const char *html_page_content_end ICACHE_RODATA_ATTR = "</div>";

const char *html_page_top_header ICACHE_RODATA_ATTR = 
  "<div id='header' class='bg'>"
    "<div id='host'>"
      "<h2>%s</h2>"   // hostname
    "</div>"                                                              // 
    "<div id='head-right' class='dropdown'>"		                
      "<div id='rssi'>"
        "<h4>%d dBm</h4>"
      "</div>"
	    "<div class='dropdown'>"
        "<button class='dropbtn'></button>"
	      "<div class='dropdown-content bg'>"
	        "<ul>%s</ul>" // menu items
	      "</div>"
	    "</div>"
	  "</div>"
	"</div>";

const char *html_page_menu_item ICACHE_RODATA_ATTR =  "<li><a href='%s'>%s</a></li>";

const char *html_page_devinfo ICACHE_RODATA_ATTR = 
  "<div id='dev-info' class='rnd brd-btm'>"
	  "<div id='memory'><span><b>Free heap: </b> %d</span></div>"  // free mem
    "<div id='uptime'><span><b>Uptime: </b> %s</span></div>"     // uptime
  "</div>";


const char *html_page_reboot_button_block ICACHE_RODATA_ATTR = 
  "<div class='group rnd'><div class='lf2'>" 
    "<button id='reboot' class='button off rht' onclick='reboot()'>Перезагрузить</button>" 
    "<div id='rbt'>Rebooting... Please, wait!</div>" 
    "</div>"
  "</div>";

#endif 