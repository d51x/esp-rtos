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


typedef struct {
    char fname[32];
    uint32_t size;
    char dt[25]; 
} ota_firm_t;

esp_err_t ota_task_upgrade_from_web(httpd_req_t *req, char *err_text);
esp_err_t ota_load_nvs(ota_firm_t *fw);

#endif /* __HTTP_OTA_H__ */