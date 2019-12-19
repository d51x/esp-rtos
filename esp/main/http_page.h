
#ifndef __HTTP_PAGE_H__
#define __HTTP_PAGE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rom/ets_sys.h"

#include "wifi.h"
#include "gpio_utils.h"
#include "http_ota.h"
#include "utils.h"
#include "dsw.h"
#include "mqtt.h"       // убрать зависимости


#define HTML_CSS  "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"\
                            ".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;"\
                            "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"\
                            ".button2 {background-color: #77878A;}</style>"

const static char *html_header =    "<!doctype html><html><head>"
                                    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
                                    "<title>%s</title>"
                                    "<meta http-equiv=\"REFRESH\" content=\"60\">"
                                    "<meta name=\"viewport\" content=\"width=480\">"
                                    "<meta name=\"mobile-web-app-capable\" content=\"yes\">"
                                    //"<link rel=\"stylesheet\" href=\"main.css\">"
                                    HTML_CSS
                                    "</head><body>";

const static char *html_page_device_info = 
                            "<h1>Hostname: %s</h1>"
                            "<p>"
                            "Wifi RSSI: %d dBm<br>"
                            "Free heap size: %d<br>"
                            "Vcc: %d<br>"
                            "uptime: %s<br>"
                            "local time: %s<br>"
                            "</p>";

const static char *html_footer = "<ul>"
                                "<li><a href=""/"">Main</a></li>"
                                "<li><a href=""/setup"">Setup</a></li>"
                                "<li><a href=""/tools"">Tools</a></li>"
                                "<li><a href=""/ota"">OTA</a></li>"
                                "<li><a href=""/debug"">Debug</a></li>"
                                "</ul>"
                                "</body></html>";


const static char *html_tools_body = "<p><span><a href=\"/restart\"><button class=\"button\">Restart</button></a></span></p>";

const static char *html_setup_body = "<p>"
                            "Hostname: %s<br>"
                            "Wifi RSSI: %d dBm<br>"
                            "Free heap size: %d<br>"
                            "Vcc: %d<br>"
                            "uptime: %s<br>"
                            "local time: %s<br>"
                            "</p>";


const static char *html_restart_header = "<head><meta http-equiv=\"refresh\" content=\"15; URL=/\" /></head>";
const static char *html_restart_body = "<p>"
                            "<a href=\"/restart?st=1\">Restart</a>"
                            "</p>";

const static char *html_restarting_body = "<p>"
                            "ESP restarting...."
                            "</p>";

const static char *html_debug = "<html><head><title>Debug</title></head>"
                            "<body>"
                            "CHIP INFO<br>"
                            "Model: %s<br>"
                            "chip_id %d<br>"
                            "rev. %d<br>"
                            "IDF version: %s<br>"
                            "Flash size: %d Mb<br>"
                            "Free heap size: %d<br>"
                            "Flash size map: %d<br>"
                            "Vcc: %d<br>"
                            "Wifi reconnect: %d<br>"
                            "Wifi status: %s<br>"
                            "Wifi RSSI: %d<br>"
                            "Wifi mode: %s<br>"
                            "IP address: %s<br>"
                            "MAC address: "MACSTR"<br>"
                            "MQTT Errors: %d; Reconnects: %d<br>"
                            "uptime: %s<br>"
                            "DS18B20 CRC Error: %d<br>"
                            "</body></html>";


const static char *html_ds18b20_data = ""
                                #ifdef DS18B20
                                    "<p>"
                                    "<span>DSW%d Temp: %.2fС&nbsp;&nbsp;&nbsp;(%02X %02X %02X %02X %02X %02X %02X %02X)</span>"
                                    "</p>"
                                #endif
                                    ""; 

const static char *html_dht_data = ""
                                #ifdef DHT
                                    "<p>"
                                    "<span>DHT Temp: %.2fС</span>"
                                    "<span>DHT Humy: %.2fС</span>"
                                    "</p>"
                                #endif
                                    "";        

const static char *html_main_body = "<p>"
                                    "<span>GPIO%d <a href=""/gpio?pin=%d&st=%d&rdct=1""><button class=\"button\">%s</button></a></span>"
                                    "</p>";

const static char *html_form_post_test = "<form method=\"GET\">"
                                        "<br>GPIO SDA <input size=\"2\" name=\"sda\" value=\"%d\">" 
                                        "<br>GPIO SCL <input size=\"2\" name=\"scl\" value=\"%d\">"
                                        "<br><input type=\"hidden\" name=\"st\" value=\"5\">"
                                        "<br><input type=\"submit\" value=\"Set\">";

const static char *html_ota_body =  "<p>"
                                    "OTA url: %s" 
                                    "</p>";

void gpioprint_page_data(char *data);
void tools_page_data(char *data);
void get_setup_page_data(char *data);
void get_debug_page_data(char *data);
void restart_page_data(char *data);
void restarting_page_data(char *data);
void restart_task(void *arg);   // TODO: нужно ли это здесь?
void get_main_page_data(char *data);
void get_ota_page_data(char *data);
#endif 