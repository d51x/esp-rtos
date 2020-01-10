#ifndef __WIFI_H__
#define __WIFI_H__

#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_system.h"


#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "utils.h"
#include "nvsparam.h"

EventGroupHandle_t wifi_event_group;  /* FreeRTOS event group to signal when we are connected*/
int WIFI_CONNECTED_BIT;  /* The event group allows multiple bits for each event, but we only care about one event - are we connected  to the AP with an IP? */

    #define ESP_WIFI_MODE_AP   1
    
        #define ESP_WIFI_SSID      "Dminty"
        #define ESP_WIFI_PASS      "110funther26"
    

    
    #define ESP_WIFI_AP_SSID      "ZppDasdwerfds"
    #define ESP_WIFI_AP_PASS      "110funther26"

#define MAX_STA_CONN       5

typedef struct {
    char ssid[32];
    char password[64];
    wifi_mode_t mode;
    uint8_t first;
    char hostname[TCPIP_HOSTNAME_MAX_SIZE];
    
} wifi_nvs_cfg_t;

char wifi_hostname[TCPIP_HOSTNAME_MAX_SIZE];

esp_err_t wifi_event_handler(void *ctx, system_event_t *event);
void wifi_init(wifi_mode_t wifi_mode);
void wifi_init_sta(wifi_config_t *wifi_cfg);
void wifi_init_softap(wifi_config_t *wifi_cfg);
int8_t wifi_get_rssi();

void wifi_load_data_from_nvs(wifi_nvs_cfg_t *cfg);
void wifi_save_data_to_nvs(const wifi_nvs_cfg_t *cfg);

void set_hostname(const char *_hostname);

#endif /* __WIFI_H__ */

