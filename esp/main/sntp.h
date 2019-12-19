#ifndef __SNTP_H__
#define __SNTP_H__

#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"







#define NTP_TASK_DELAY 5*1000*60  // 5 min
#define NTP_TASK_TIMEOUT 2000  //sec

void initialize_sntp(void);
void sntp_task(void *arg);
void obtain_time(void);

#endif /* __SNTP_H__ */