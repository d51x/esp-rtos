#ifndef __OTA_H__
#define __OTA_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_http_client.h"
#include "esp_partition.h"
#include "esp_http_server.h"
#include "utils.h"
#include "nvsparam.h"


typedef enum esp_ota_firm_state {
    ESP_OTA_IDLE = 0,
    ESP_OTA_PREPARE,
    ESP_OTA_START,
    ESP_OTA_RECVED,
    ESP_OTA_FINISH,
} esp_ota_state_t;

typedef struct esp_ota_firm {
    esp_ota_state_t    state;
    uint8_t             progress;
    size_t              write_bytes;
    size_t              ota_size;
} esp_ota_t;

typedef struct {
    char uri[100];
    uint16_t buf_size;
} ota_nvs_data_t;



esp_ota_t esp_ota;

void ota_init();


esp_err_t ota_task_upgrade_from_url(char *err_text);
esp_err_t ota_task_upgrade_from_web(httpd_req_t *req, char *err_text);

esp_ota_t get_ota_state();
void set_ota_state(esp_ota_state_t st);
void set_ota_size(size_t ota_size);
void set_ota_progress(size_t bytes);

void save_ota_upgrade_dt();
void get_ota_upgraded_dt(char *buf);

void get_ota_nvs_data(ota_nvs_data_t *ota_nvs);
void set_ota_nvs_data(const ota_nvs_data_t *ota_nvs);

#endif /* __HTTP_OTA_H__ */