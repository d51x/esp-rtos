
#ifndef __HTTP_PAGE_TPL_H__
#define __HTTP_PAGE_TPL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *html_header_redirect = "<head><meta http-equiv=\"refresh\" content=\"%d; URL=%s\" /></head>";

const char *html_header =    "<!DOCTYPE html><html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
                                    "<title>%s</title><meta name=\"viewport\" content=\"width=480\">"
                                    "<meta name=\"mobile-web-app-capable\" content=\"yes\"><link rel=\"stylesheet\" type=\"text/css\" href=\"main.css\"/>"
                                    "</head><body><div id=\"main\">";

const char *html_footer = "<footer><p><span>%s</span></p><p>Firmware: %s</p></footer></div></body></html>";

const char *html_menu_item =  "<span><a href=\"%s\">%s</a></span>";

const char *html_page_device_info = 
                            "<h1>Hostname: %s</h1>"
                            "<p>"
                            "Wifi RSSI: %d dBm<br>"
                            "Free heap size: %d<br>"
                            "Vcc: %d<br>"
                            "uptime: %s<br>"
                            "local time: %s<br>"
                            "</p>";

const char *html_restart_button = "<p><span><a href=\"/restart\"><button class=\"off\">Restart</button></a></span></p>";

const char *html_devinfo =   "<div id=\"dev-info\">"
                                    "<h2>Hostname: <b>%s</b></h2>"
                                    "<p>"
                                    "<span>Freemem: %d Kb</span>"
                                    "<span>RSSI: %d dBm</span>"
                                    "<span>Vcc: %d mV</span>"
                                    "</p>"
                                    "<p class=\"uptime\"><span>uptime: %s</span></p>"
                                    "</div>";

const char *html_setup_body =    "<h2>Options:</h2>";

const char *html_restart_body = "<p>"
                            "<a href=\"/restart?st=1\">Restart</a>"
                            "</p>";

const char *html_restarting_body = "<p>"
                            "ESP restarting...."
                            "</p>";

const char *html_debug = 
                            "CHIP INFO<br>"
                            "Model: %s<br>"
                            "chip_id %d<br>"
                            "rev. %d<br>"
                            "IDF version: %s<br>"
                            "Flash size: %d Mb<br>"
                            "Free heap size: %d<br>"
                            "Flash size map: %d<br>"
                            "ota updated %s<br>"
                            "Vcc: %d<br>"
                            "Wifi reconnect: %d<br>"
                            "Wifi status: %s<br>"
                            "Wifi RSSI: %d<br>"
                            "Wifi mode: %s<br>"
                            "IP address: %s<br>"
                            "MAC address: "MACSTR"<br>"
                            "MQTT Errors: %d; Reconnects: %d<br>"
                            "uptime: %s<br>"
                            "DS18B20 CRC Error: %d<br>";
                            


const char *html_ds18b20_header = "<p>Dallas Temperature</p>";
const char *html_ds18b20_data = "<span>%d: <b>%.1fС°</b>(%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X)</span>"; 

const char *html_dht_header = "<p>DHT 22</p>";        
const char *html_dht_data = "<span>Temp: <b>%.1fС°</b></span><span>Hum: <b>%.1f%%</b></span>";        

const char *html_gpio_header= "<div id=\"gpio\"><p>GPIO Buttons</p>";
const char *html_gpio_item = "<span><a href=\"/gpio?pin=%d&st=%d&fn=1\"><button class=\"%s\">GPIO%d</button></a></span>";
const char *html_gpio_end = "</div>";

const char *html_setup_form_post = "<form method=\"GET\">"
                                        "<h3>Wifi Settings</h3>"
                                        "<p><small>not used yet</small></p>"
                                        "<p>Hostname: <input size=\"20\" name=\"hostname\" value=\"%s\"></p>" 
                                        "<p>SSID: <input size=\"20\" name=\"ssid\" value=\"%s\"></p>" 
                                        "<p>Password: <input size=\"20\" name=\"pass\" value=\"%s\"></p>"
                                        "<p>Mode:"
                                        //"<input type=\"radio\" name=\"wifi_mode\" value=\"0\" %s> Disable"
                                        "<input type=\"radio\" name=\"wifi_mode\" value=\"1\" %s> STA"
                                        "<input type=\"radio\" name=\"wifi_mode\" value=\"2\"  %s> AP"
                                        "</p>"
                                        "<hr>"
                                        "<h3>MQTT Settings:</h3>"
                                        "<p><input type=\"checkbox\" name=\"mqtt_en\" %s > enabled</p>"
                                        "<p>host: <input size=\"40\" name=\"mqtt_host\" value=\"%s\"></p>"
                                        "<p>login: <input size=\"20\" name=\"mqtt_login\" value=\"%s\"></p>"
                                        "<p>send interval: <input size=\"20\" name=\"mqtt_sint\" value=\"%d\"></p>"
                                        "<hr>"
                                        "<h3>OTA Settings:</h3>"
                                        "<p>Remote url: <input size=\"50\" name=\"ota_uri\" value=\"%s\"></p>" 
                                        "<p>buf size: <input size=\"20\" name=\"ota_bufsz\" value=\"%d\"></p>" 
                                        "<hr>"
                                        "<p><input type=\"hidden\" name=\"st\" value=\"1\"></p>"
                                        "<p><input type=\"submit\" value=\"Set\"></p>"
                                        "</form>";

const char *html_ota_body =     "<p>Firmware:</p>"
                                "<p>Loaded: %s.bin</p>"
                                "<p>Compiled: %s %s</p>"
                                "<p>Updated: %s</p>"
                                "<p>Обновить с сервера:</p>"
                                    "<p>"
                                    "<a href=\"/ota?st=1\">OTA url: %s</a>" 
                                    "</p>"
                                    "<p>Выбрать Firmware</p>"
                                    "<form enctype=\"multipart/form-data\" method=\"post\" action=\"/fwupload\" accept=\".bin\">"
                                        "<p><input type=\"file\" name=\"file\">"
                                        "<input type=\"submit\" value=\"Загрузить\"></p>"
                                    "</form>";



#endif 