
#pragma once

#ifndef __HTTP_PAGE_TPL_H__
#define __HTTP_PAGE_TPL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "html_formatting.h"

#define html_header_redirect  "<head><meta http-equiv='refresh' content='%d; URL=%s' /></head>"





//char *menu_uri[MENU_ITEM_COUNT] = {"/", "/setup", "/tools", "/update", "/debug"};   // 10 char
//char *menu_names[MENU_ITEM_COUNT] = {"Main", "Setup", "Tools", "Update", "Debug"};  // 10 char

const char *html_page_start =    "<!DOCTYPE html><html><head><meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
                                    "<title>%s</title><meta name='viewport' content='width=480'>"
                                    "<meta name='mobile-web-app-capable' content='yes'><link rel='stylesheet' type='text/css' href='main.css'/>"
                                    "<script type='text/javascript' src='ajax.js'></script>"
                                    "</head><body>"
                                    DIV_S_ CSS_ID "main" DIV_S_Q_E; // "<div id='main'>";

const char *html_page_end = DIV_S_ CSS_ID "clear" DIV_S_Q_E DIV_E       // "<div id='clear'></div>"
                              DIV_S_ CSS_ID "footer" HQ CSS_CLASS "rnd" DIV_S_Q_E     // "<div id='footer' class='rnd'>"
                              DIV_S_ CSS_ID "time" DIV_S_Q_E SPAN_S BOLD_S "%s" BOLD_E SPAN_E DIV_E  // "<div id='time'><span><b>%s</b></div>"                // local time
                              DIV_S_ CSS_ID "fw" DIV_S_Q_E SPAN_S BOLD_S "v.%s" BOLD_E SPAN_E DIV_E    // "<div id='fw'><span><b>v.%s</b></span></div>"         // fw version
                              DIV_E DIV_E        // "</div></div></body></html>";
			                        "</body></html>";

const char *html_page_content_start = DIV_S_ CSS_CLASS "data" DIV_S_Q_E;   // "<div class='data'>"; // data

const char *html_page_content_end = DIV_E;   // "</div>"; // data

const char *html_page_top_header = DIV_S_ CSS_ID "header" HQ CSS_CLASS "bg" DIV_S_Q_E            // "<div id='header' class='bg'>"
				                           DIV_S_ CSS_ID "host" DIV_S_Q_E                                // "<div id='host'>"
					                          H2_S "%s" H2_E                                                  // "<h2>%s</h2>" // hostname
                                    DIV_E                                                               // "</div>"

                                  DIV_S_ CSS_ID "head-right" HQ CSS_CLASS "dropdown" DIV_S_Q_E   // "<div id='head-right' class='dropdown'>"
				                
					                        DIV_S_ CSS_ID "rssi" DIV_S_Q_E H4_S "%d dBm" H4_E        // "<div id='rssi'><h4>%d dBm</h4>"
					                        DIV_E                                                                 // "</div>"
					                        DIV_S_ CSS_CLASS "dropdown" DIV_S_Q_E                          // "<div class='dropdown'>"
					                        BUTTON_S_ CSS_CLASS "dropbtn" BUTTON_S_Q_E BUTTON_E          //"<button class='dropbtn'></button>"
					                        DIV_S_ CSS_CLASS "dropdown-content bg" DIV_S_Q_E                                 // "<div class='dropdown-content bg'>"
						                      UL_S "%s" UL_E                                                    // "<ul>%s</ul>"  // menu items
					  DIV_E // "</div>"
					DIV_E // "</div>"
				DIV_E // "</div>"
			DIV_E //"</div>"
      ;

const char *html_page_menu_item =  LI_S ANC_S_ "%s" ANC_S_Q_E "%s" ANC_E LI_E; //"<li><a href='%s'>%s</a></li>";

const char *html_page_devinfo = DIV_S_ CSS_ID "dev-info" HQ CSS_CLASS "rnd brd-btm" DIV_S_Q_E // "<div id='dev-info' class='rnd brd-btm'>"	
					                      DIV_S_ CSS_ID "memory" DIV_S_Q_E  SPAN_S BOLD_S "Free heap: " BOLD_E "%d" SPAN_E DIV_E // "<div id='memory'><span><b>Free heap: </b> %d</span></div>" // free mem
					                      DIV_S_ CSS_ID "uptime" DIV_S_Q_E SPAN_S BOLD_S "Uptime: " BOLD_E "%s" SPAN_E DIV_E //"<div id='uptime'><span><b>Uptime: </b> %s</span></div>"    // uptime
			                          DIV_E ; //"</div>";


const char *html_page_ota = P_S "Выбрать Firmware" P_E // "<p>Выбрать Firmware</p>"
                            FORM_S_  FORM_ENCTYPE FORM_POST FORM_ACTION "'/update' accept='.bin'" FORM_S_Q_E  //     "<form enctype='multipart/form-data' method='post' action='/update' accept='.bin'>"
                                        P_S INPUT_S_ INPUT_TYPE "file" HQ INPUT_NAME "file" HQ CSS_CLASS "button norm rh" INPUT_SS_E P_E //"<p><input type='file' name='file' class='button norm rh' /></p>"
                                        P_S DIV_S_ CSS_CLASS "lf2" DIV_S_Q_E INPUT_S_ INPUT_TYPE "submit" HQ INPUT_VALUE "Загрузить" HQ CSS_CLASS "button norm rht" INPUT_SS_E P_E //"<p><div class='lf2'><input type='submit' value='Загрузить' class='button norm rht' /></p>"
                                    FORM_E; //"</form>";

const char *html_page_setup_wifi = DIV_S_ CSS_CLASS "group rnd" DIV_S_Q_E //"<div class='group rnd'>"
                                   H4_S_ CSS_CLASS "brd-btm" H4_S_Q_E "WiFi Settings:" H4_E //     "<h4 class='brd-btm'>WiFi Settings:</h4>"
                                        FORM_S_ FORM_GET FORM_S_E //"<form method='GET'>"
                                            
                                            P_S  LABEL_S_ CSS_CLASS "lf" LABEL_S_Q_E "Hostname: " LABEL_E 
                                            INPUT_S_ INPUT_SIZE "20" HQ INPUT_NAME "hostname" HQ CSS_CLASS "edit rh" HQ INPUT_VALUE "%s" HQ INPUT_SS_E 
                                            P_E
                                            //"<p><label class='lf'>Hostname: </label><input size='20' name='hostname' class='edit rh' value='%s' /></p>"
                                            
                                            P_S  LABEL_S_ CSS_CLASS "lf" LABEL_S_Q_E "SSID: " LABEL_E 
                                            INPUT_S_ INPUT_SIZE "20" HQ INPUT_NAME "ssid" HQ CSS_CLASS "edit rh" HQ INPUT_VALUE "%s" HQ INPUT_SS_E 
                                            P_E
                                            //"<p><label class='lf'>SSID: </label><input size='20' name='ssid' class='edit rh' value='%s' /></p>"
                                            
                                            P_S  LABEL_S_ CSS_CLASS "lf" LABEL_S_Q_E "Password: " LABEL_E 
                                            INPUT_S_ INPUT_SIZE "20" HQ INPUT_NAME "pass" HQ CSS_CLASS "edit rh" HQ INPUT_VALUE "%s" HQ INPUT_SS_E 
                                            P_E                                           
                                            //"<p><label class='lf'>Password: </label><input size='20' name='pass' class='edit rh' value='%s' /></p>"

                                            P_S  LABEL_S_ CSS_CLASS "lf" LABEL_S_Q_E "Mode: " LABEL_E  //"<p><label class='lf'>Mode:</label>"
										                        LABEL_S INPUT_S_ INPUT_TYPE "radio" HQ INPUT_NAME "wifi_mode" HQ INPUT_VALUE "1" HQ "%s" INPUT_SS_E " STA" LABEL_E // <label><input type='radio' name='wifi_mode' value='1' %s /> STA</label>"
										                        LABEL_S INPUT_S_ INPUT_TYPE "radio" HQ INPUT_NAME "wifi_mode" HQ INPUT_VALUE "2" HQ "%s" INPUT_SS_E " AP" LABEL_E // <label><input type='radio' name='wifi_mode' value='2' %s /> AP</label>"
                                            P_E //"</p>"
                                            P_S INPUT_S_ INPUT_TYPE "hidden" HQ INPUT_NAME "st" HQ INPUT_VALUE "1" HQ INPUT_SS_E P_E //"<p><input type='hidden' name='st' value='1'></p>"
                                            P_S_ CSS_CLASS "lf2" P_S_Q_E INPUT_S_ INPUT_TYPE "submit" HQ INPUT_VALUE "Сохранить" HQ CSS_CLASS "button norm rht" HQ INPUT_SS_E P_E //"<p class='lf2'><input type='submit' value='Сохранить' class='button norm rht'></p>"
                                        FORM_E // "</form>"
                                    DIV_E; //"</div>";   

const char *html_page_reboot_button_block = DIV_S_ CSS_CLASS "group rnd" DIV_S_Q_E DIV_S_ CSS_CLASS "lf2" DIV_S_Q_E // "<div class='group rnd'><div class='lf2'>"
                                            BUTTON_S_ CSS_ID "reboot" HQ CSS_CLASS "button off rht" HQ "onclick='reboot()'" BUTTON_S_E "Перезагрузить" BUTTON_E // "<button id='reboot' class='button off rht' onclick='reboot()'>Перезагрузить</button>"
                                            DIV_S_ CSS_ID "rbt" DIV_S_Q_E "Rebooting... Please, wait!" DIV_E //"<div id='rbt'>Rebooting... Please, wait!</div>"
                                            DIV_E DIV_E ; //"</div></div>";     

#endif 