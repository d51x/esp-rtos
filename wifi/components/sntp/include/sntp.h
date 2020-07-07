#ifndef __SNTP_H__
#define __SNTP_H__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "lwip/apps/sntp.h"
#include "wifi.h"
#include "utils.h"






#define NTP_TASK_DELAY 5*1000*60  // 5 min
#define NTP_TASK_TIMEOUT 5000  //msec

void sntp_task(void *arg);
void obtain_time(void);
void sntp_start();

#endif /* __SNTP_H__ */