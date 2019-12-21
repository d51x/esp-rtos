#ifndef __HTTP_OTA_H__
#define __HTTP_OTA_H__

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include <esp_http_client.h>
#include "esp_partition.h"
#include "esp_http_server.h"
#include "main.h"


#define FIRMWARE_UPGRADE_URL "http://192.168.2.63/esp.bin"

esp_err_t ota_task_uptade_from_url(const char *url);
#endif /* __HTTP_OTA_H__ */