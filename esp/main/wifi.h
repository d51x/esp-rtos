#ifndef __WIFI_H__
#define __WIFI_H__

#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

#include "main.h"
#include "utils.h"


#define MAX_STA_CONN       5

static char *device_name = "esp_test";




esp_err_t wifi_event_handler(void *ctx, system_event_t *event);
void wifi_init(wifi_mode_t wifi_mode);
void wifi_init_sta(wifi_config_t *wifi_cfg);
void wifi_init_softap(wifi_config_t *wifi_cfg);
int8_t wifi_get_rssi();

#endif /* __WIFI_H__ */