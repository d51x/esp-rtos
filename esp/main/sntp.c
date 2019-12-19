#include "esp_system.h"

#include "lwip/apps/sntp.h"
#include "sntp.h"

#include "wifi.h"

static const char *TAG = "SNTP";

void sntp_task(void *arg){
    //ESP_LOGI(TAG, "%s: started\n", __func__);
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];

    time(&now);
    localtime_r(&now, &timeinfo);

    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        //ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
    }

    
    setenv("TZ", "UTC-3", 1);
    tzset();

    while (1) {
        // update 'now' variable with current time
        time(&now);
        localtime_r(&now, &timeinfo);

        
        if (timeinfo.tm_year < (2016 - 1900)) {

            //ESP_LOGE(TAG, "The current date/time error");
            //ESP_LOGE(TAG, "%s: year: %d < (2016 - 1900)", __func__, timeinfo.tm_year);
        } else {
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
            //ESP_LOGI(TAG, "The current date/time in Moscow is: %s", strftime_buf);
        }

        //ESP_LOGI(TAG, "Free heap size: %d\n", esp_get_free_heap_size());
        vTaskDelay(NTP_TASK_DELAY / portTICK_RATE_MS);
    }
}

void obtain_time(void)
{
    //ESP_LOGI(TAG, "%s: started\n", __func__);
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        //ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        //ESP_LOGE(TAG, "%s: year: %d < (2016 - 1900)", __func__, timeinfo.tm_year);
        vTaskDelay(NTP_TASK_TIMEOUT / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}

void initialize_sntp(void)
{
    //ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

}