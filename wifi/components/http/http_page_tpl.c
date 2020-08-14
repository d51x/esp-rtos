#include "http_page_tpl.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


const char *html_header_redirect   = "<head><meta http-equiv='refresh' content='%d; URL=%s' /></head>";





//char *menu_uri[MENU_ITEM_COUNT] = {"/", "/setup", "/tools", "/update", "/debug"};   // 10 char
//char *menu_names[MENU_ITEM_COUNT] = {"Main", "Setup", "Tools", "Update", "Debug"};  // 10 char

const char *html_page_start  =    
  "<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
      "<title>%s</title>"
      "<meta name='viewport' content='width=480'>"
      "<meta name='mobile-web-app-capable' content='yes'><link rel='stylesheet' type='text/css' href='main.css'/>"
    "</head>"
    "<body>"
      "<div id='main'>"; 

const char *html_page_end  = 
      "<script type='text/javascript' src='ajax.js'></script>"
      "<div id='clear'></div>"
        "<div id='footer' class='rnd'>"
          "<div id='time'><span><b>%s</b></div>"            // local time
          "<div id='fw'><span><b>v.%s</b></span></div>"     // fw version
        "</div>"
      "</div>"
	  "</body>"
  "</html>";

const char *html_page_content_start  = "<div class='data'>";

const char *html_page_content_end  = "</div>";

const char *html_page_top_header  = 
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

const char *html_page_menu_item  =  "<li><a href='%s'>%s</a></li>";

const char *html_page_devinfo  = 
  "<div id='dev-info' class='rnd brd-btm'>"
	  "<div id='memory'><span><b>Free heap: </b> %d</span></div>"  // free mem
    "<div id='uptime'><span><b>Uptime: </b> %s</span></div>"     // uptime
  "</div>";


const char *html_page_reboot_button_block  = 
  "<div class='group rnd'><div class='lf2'>" 
    "<button id='reboot' class='button off rht' onclick='reboot()'>Перезагрузить</button>" 
    "<div id='rbt'>Rebooting... Please, wait!</div>" 
    "</div>"
  "</div>";


// ====== data block  ======================================================================
const char *html_block_data_start  = 
    "<div class='group rnd'>"
        "<h4 class='brd-btm'>%s</h4>";   

const char *html_block_data_end  = 
    "</div>";  

// ====== forms  ======================================================================
const char *html_block_data_form_start  = 
    "<form method='GET'>"
    "<div class='lf3'>";  

const char *html_block_data_form_submit  = 
      "</div>"
      "<div class='rh2'><p><input type='hidden' name='st' value='%s'></p>"
      "<p><input type='submit' value='Сохранить' class='button norm rht'></p>";

const char *html_block_data_form_end  = 
      "</div>"
    "</form>";

const char *html_block_data_form_item_label_edit  = 
    "<p><label class='lf'>%s: </label><input size='20' name='%s' class='edit rh' value='%d' /></p>";
