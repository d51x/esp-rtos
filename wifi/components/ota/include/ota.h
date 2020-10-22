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


esp_err_t ota_task_upgrade_from_web(httpd_req_t *req, char *err_text);


#endif /* __HTTP_OTA_H__ */