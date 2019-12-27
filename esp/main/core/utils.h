#ifndef __UTILS_H__
#define __UTILS_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <limits.h>
#include <errno.h>
#include "sys/time.h"
//#include <ctype.h>

#include "esp_log.h"
#include "esp_idf_version.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "driver/adc.h"
#include "nvs.h"





#define millis() (unsigned long) (esp_timer_get_time() / 1000ULL)
#define cur_sec() (uint32_t) (esp_timer_get_time() / 1000ULL / 1000U)
#define IP_2_STR(a) ip4addr_ntoa(a)

#define UPTIME2STR "%d days %02dh %02dm %02ds"
#define UPTIMESTRLENMAX 20

#define LOCALTIME2STR "%a, %d.%m.%Y %X"  //http://all-ht.ru/inf/prog/c/func/strftime.html
#define LOCALTIMESTRLENMAX 30


/*
typedef enum {
    STR2INT_SUCCESS,        // 0
    STR2INT_OVERFLOW,       // 1
    STR2INT_UNDERFLOW,      // 2
    STR2INT_INCONVERTIBLE   // 3
} str2int_errno;
*/

typedef struct {
    uint32_t chip_id;
    esp_chip_model_t chip_model;
    uint8_t chip_revision;
} m_chip_info_t;

typedef struct {
    uint32_t flash_size;
    uint32_t free_heap_size;
    uint32_t flash_size_map;    
} m_mem_info_t;

typedef struct {
    wifi_mode_t wifi_mode;
    uint32_t wifi_reconnect;
    ip4_addr_t ip;
    //uint8_t ip[4];
    uint8_t mac[6];
    uint8_t status;
} m_wifi_info_t;

typedef struct {
    m_chip_info_t chip_info;
    m_mem_info_t mem_info;
    char sdk_version[20];
    uint32_t vdd33;
} system_info_t;



m_wifi_info_t wifi_info;
char hostname[TCPIP_HOSTNAME_MAX_SIZE];

uint8_t str_to_int(int *out, char *s, int base);
uint8_t str_to_long(long *out, char *s, int base);
uint8_t str_to_uint(uint *out, char *s, int base);
uint8_t str_to_uint8(uint8_t *out, char *s, int base);

//uint16_t get_vdd33();
void print_chip_info();
void get_system_info(system_info_t *sys_info);
char* print_wifi_mode(wifi_mode_t mode);

uint32_t get_chip_id(uint8_t *mac);
char* set_hostname(char *_hostname);

void get_uptime(char*  buf);
void get_localtime(char*  buf);

void trim(char *s);

void systemRebootTask(void * parameter);
int url_decode(const char *s, char *dec);

#endif /* __UTILS_H__ */