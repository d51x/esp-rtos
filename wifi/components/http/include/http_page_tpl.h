
#pragma once

#ifndef __HTTP_PAGE_TPL_H__
#define __HTTP_PAGE_TPL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define html_header_redirect  "<head><meta http-equiv='refresh' content='%d; URL=%s' /></head>"










//char *menu_uri[MENU_ITEM_COUNT] = {"/", "/setup", "/tools", "/update", "/debug"};   // 10 char
//char *menu_names[MENU_ITEM_COUNT] = {"Main", "Setup", "Tools", "Update", "Debug"};  // 10 char

const char *html_page_start =    "<!DOCTYPE html><html><head><meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
                                    "<title>%s</title><meta name='viewport' content='width=480'>"
                                    "<meta name='mobile-web-app-capable' content='yes'><link rel='stylesheet' type='text/css' href='main.css'/>"
                                    "<script type='text/javascript' src='ajax.js'></script>"
                                    "</head><body>"
                                    "<div id='main'>";

const char *html_page_end = "<div id='clear'></div>"
                          "<div id='footer' class='rnd'>"
				          "<div id='time'><span><b>%s</b></div>"                // local time
				          "<div id='fw'><span><b>v.%s</b></span></div>"         // fw version
			              "</div></div></body></html>";

const char *html_page_content_start = "<div class='data'>";   // data
const char *html_page_content_end = "</div>";   // data

const char *html_page_top_header = "<div id='header' class='bg'>"
				                "<div id='host'>"
					                "<h2>%s</h2>" // hostname
				                "</div>"
				                "<div id='head-right' class='dropdown'>"
					                "<div id='rssi'><h4>%d dBm</h4>"
					            "</div>"
					"<div class='dropdown'>"
					  "<button class='dropbtn'></button>"
					  "<div class='dropdown-content bg'>"
						"<ul>%s</ul>"  // menu items
					  "</div>"
					"</div>"
				"</div>"
			"</div>";

const char *html_page_menu_item =  "<li><a href='%s'>%s</a></li>";

const char *html_page_devinfo = "<div id='dev-info' class='rnd brd-btm'>"	
					                "<div id='memory'><span><b>Free heap: </b> %d</span></div>" // free mem
					                "<div id='uptime'><span><b>Uptime: </b> %s</span></div>"    // uptime
			                        "</div>";


const char *html_page_ota = "<p>Выбрать Firmware</p>"
                                    "<form enctype='multipart/form-data' method='post' action='/update' accept='.bin'>"
                                        "<p><input type='file' name='file' class='button norm rh' /></p>"
                                        "<p><div class='lf2'><input type='submit' value='Загрузить' class='button norm rht' /></p>"
                                    "</form>";

const char *html_page_setup_wifi = "<div class='group rnd'>"
                                        "<h4 class='brd-btm'>WiFi Settings:</h4>"
                                        "<form method='GET'>"
                                            "<p><label class='lf'>Hostname: </label><input size='20' name='hostname' class='edit rh' value='%s' /></p>"
                                            "<p><label class='lf'>SSID: </label><input size='20' name='ssid' class='edit rh' value='%s' /></p>"
                                            "<p><label class='lf'>Password: </label><input size='20' name='pass' class='edit rh' value='%s' /></p>"
                                            "<p><label class='lf'>Mode:</label>"
										        "<label><input type='radio' name='wifi_mode' value='1' %s /> STA</label>"
										        "<label><input type='radio' name='wifi_mode' value='2' %s /> AP</label>"
                                            "</p>"
                                            "<p><input type='hidden' name='st' value='1'></p>"
                                            "<p class='lf2'><input type='submit' value='Сохранить' class='button norm rht'></p>"
                                        "</form>"
                                    "</div>";   

const char *html_page_setup_mqtt = "<div class='group rnd'>"
                                    "<h4 class='brd-btm'>MQTT Settings:</h4>"
                                    "<form method='GET'>"
                                        "<p><label class='lf'>Enabled: </label><input type='checkbox' name='mqtt_en' %s /></p>"
                                        "<p><label class='lf'>Hostname: </label><input size='20' name='mqtt_host' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Login: </label><input size='20' name='mqtt_login' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Password: </label><input size='20' name='mqtt_passw' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Base topic (/): </label><input size='20' name='mqtt_base' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Send interval: </label><input size='20' name='mqtt_sint' class='edit rh' value='%d' /></p>"
                                        "<p><input type='hidden' name='st' value='2'></p>"
                                        "<p class='lf2'><input type='submit' value='Сохранить' class='button norm rht'></p>"
                                    "</form>"
                                    "</div>";


const char *html_page_reboot_button_block = "<div class='group rnd'><div class='lf2'>"
                                            "<button id='reboot' class='button off rht' onclick='reboot()'>Перезагрузить</button>"
                                            "<div id='rbt'>Rebooting... Please, wait!</div>"
                                            "</div></div>";     

#endif 