#include "utils.h"



static const char *TAG = "utils";
/*
uint16_t get_vdd33() {
    uint16_t adc = 0;
    adc_config_t adc_cfg;
    adc_cfg.mode = ADC_READ_VDD_MODE;
    adc_cfg.clk_div = 8;
    if ( adc_init(&adc_cfg) == ESP_OK ) {
        if ( adc_read(&adc) == ESP_OK ) {
        } else {
            ESP_LOGI(TAG_UTILS, "FAIL: adc_read error\n"); 
        }
        adc_deinit();
    } else {
       ESP_LOGI(TAG_UTILS, "FAIL: adc_init error\n"); 
    }
    return adc;
}
*/
uint32_t get_chip_id(uint8_t *mac){
 return mac[5] + mac[4]*256 + mac[3]*256*256;
}

void get_system_info(system_info_t *sys_info) {
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    m_chip_info_t m_chip_info;
    m_chip_info.chip_id = get_chip_id(wifi_info.mac);
    m_chip_info.chip_model = chip_info.model;
    m_chip_info.chip_revision = chip_info.revision;
    memcpy(&sys_info->chip_info, &m_chip_info, sizeof(m_chip_info_t)); 

    m_mem_info_t m_mem_info;
    m_mem_info.flash_size = spi_flash_get_chip_size();
    m_mem_info.free_heap_size = esp_get_free_heap_size();
    m_mem_info.flash_size_map = system_get_flash_size_map();
    memcpy(&sys_info->mem_info, &m_mem_info, sizeof(m_mem_info_t)); 

//  Free Mem - sendContentBlocking
//  Free Stack - 3623 (2320 - sendWebPage)
//  local time
// uptime from millis to day hour minutes seconds
// rssi

    //char *sdk_version = malloc(20);
    //sdk_version = esp_get_idf_version();
    memcpy(&sys_info->sdk_version, esp_get_idf_version(), 20);
    //free(sdk_version);

    uint32_t vdd33 = esp_wifi_get_vdd33();
    memcpy(&sys_info->vdd33, &vdd33, sizeof(uint32_t));
    
}

char* print_wifi_mode(wifi_mode_t mode){
    switch (mode) {
        case WIFI_MODE_STA:
            return "STA";
            break;
        case WIFI_MODE_AP:
            return "AP";
            break;
        case WIFI_MODE_APSTA:
            return "STA+AP";
            break;
        default:
            return "";
            break;
    }
}

void print_chip_info() {
    system_info_t *sys_info = malloc(sizeof(system_info_t));
    get_system_info(sys_info);

    printf("CHIP INFO\n");
    printf("Model: %s\n", (sys_info->chip_info.chip_model == 0) ? "esp8266" : "esp32");
    printf("rev. %d\n", sys_info->chip_info.chip_revision);
    
    printf("IDF version: %s\n", sys_info->sdk_version);

    printf("Flash size: %d Mb\n", sys_info->mem_info.flash_size / (1024 * 1024));
    printf("Free heap size: %d\n", sys_info->mem_info.free_heap_size);
    printf("Flash size map: %d\n", sys_info->mem_info.flash_size_map);

    printf("VCC: %d\n", sys_info->vdd33);

    printf("Wifi reconnect: %d\n", wifi_info.wifi_reconnect);
    printf("Wifi status: %s\n", (wifi_info.status) ? "connected" : "disconnected");
    printf("Wifi mode: %s\n", print_wifi_mode(wifi_info.wifi_mode));
    printf("IP address: %s\n", IP_2_STR(&wifi_info.ip));
    printf("MAC address: "MACSTR"\n", MAC2STR(wifi_info.mac));
    
    free(sys_info);
        
            //uint32 system_get_userbin_addr(void);

        // OTA
        // uint8 system_upgrade_userbin_check(void); // 0x00 : UPGRADE_FW_BIN1, i.e. user1.bin
        // 0x01 : UPGRADE_FW_BIN2, i.e. user2.bin
        // rssi
        // spiffs info
    
}


void get_uptime(char* buf){
    //char* buf = malloc(20);
    time_t now;
    struct tm timeinfo;
    now = millis()/1000UL;
    setenv("TZ", "UTC-0", 1);
    tzset();    
    localtime_r(&now, &timeinfo);
    snprintf(buf, UPTIMESTRLENMAX, UPTIME2STR,  timeinfo.tm_yday,
                                                        timeinfo.tm_hour,
                                                        timeinfo.tm_min,
                                                        timeinfo.tm_sec);
   //return buf;
}

void get_localtime(char* buf){
    //char *buf = malloc(LOCALTIMESTRLENMAX);
    time_t now;
    struct tm timeinfo;
    setenv("TZ", "UTC-3", 1);
    tzset();    
    time(&now);
    localtime_r(&now, &timeinfo);

    strftime(buf, LOCALTIMESTRLENMAX, LOCALTIME2STR, &timeinfo);
/*
    snprintf(buf, LOCALTIMESTRLENMAX, LOCALTIME2STR,  
                                                        timeinfo.tm_wday,
                                                        timeinfo.tm_mday,
                                                        timeinfo.tm_mon,
                                                        timeinfo.tm_year,
                                                        timeinfo.tm_hour,
                                                        timeinfo.tm_min,
                                                        timeinfo.tm_sec);
*/
    //return buf;
}

char* set_hostname(char *_hostname){
    if ( _hostname == NULL || strlen(_hostname) == 0 ) {
        // default name like "ESP_XXXXXX"
        snprintf(hostname, TCPIP_HOSTNAME_MAX_SIZE, "ESP_%02X%02X%02X", wifi_info.mac[3], wifi_info.mac[4], wifi_info.mac[5]);
        return hostname;
    } else {
        return _hostname;
    }
}

    
uint8_t str_to_long(long *out, char *s, int base) {
    char *end;
    //if (s[0] == '\0' || isspace(s[0]))
    //    return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > LONG_MAX || (errno == ERANGE && l == LONG_MAX))
        return 1;
    if (l < LONG_MIN || (errno == ERANGE && l == LONG_MIN))
        return 2;
    if (*end != '\0')
        return 3;
    *out = l;
    return 0;
}

uint8_t str_to_int(int *out, char *s, int base) {
    char *end;
    //if (s[0] == '\0' || isspace(s[0]))
    //    return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return 1;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return 2;
    if (*end != '\0')
        return 3;
    *out = l;
    return 0;
}

uint8_t str_to_uint(uint *out, char *s, int base) {
    char *end;
    //if (s[0] == '\0' || isspace(s[0]))
    //    return STR2INT_INCONVERTIBLE;
    errno = 0;
    unsigned long l = strtoul(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > UINT_MAX || (errno == ERANGE && l == ULONG_MAX))
        return 1;
    if (*end != '\0')
        return 3;
    *out = l;
    return 0;
}

uint8_t str_to_uint8(uint8_t *out, char *s, int base) {
    char *end;
    //if (s[0] == '\0' || isspace(s[0]))
    //    return STR2INT_INCONVERTIBLE;
    errno = 0;
    unsigned long l = strtoul(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > UCHAR_MAX || (errno == ERANGE && l == UCHAR_MAX))
        return 1;
    if (*end != '\0')
        return 3;
    *out = l;
    return 0;
}

void trim(char *s){
    // удаляем пробелы и табы с начала строки:
    int i = 0, j;
    while ( (s[i] == ' ') || (s[i] == '\t') ) i++;
    if ( i > 0 ) {
        for ( j = 0; j < strlen(s); j++) s[j] = s[j+i];
        s[j] = '\0';
    }
 
    // удаляем пробелы и табы с конца строки:
    i = strlen(s) - 1;
    while ( (s[i]==' ') || (s[i]=='\t') ) i--;
    if ( i < (strlen(s) - 1 ) ) s[i+1]='\0';
}

void rtrim( char * string, char * trim )
{
    // делаем обрезку справа
    int i;
    for( i = strlen (string) - 1; i >= 0 && strchr (trim, string[i]) != NULL; i-- )
    {  
        // переставляем терминатор строки 
        string[i] = '\0';
    }
}
 
void ltrim( char * string, char * trim )
{
    // делаем обрезку слева
    while ( string[0] != '\0' && strchr ( trim, string[0] ) != NULL )
    {
        memmove( &string[0], &string[1], strlen(string) );
    }
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void systemRebootTask(void * parameter)
{

	// Init the event group
	//reboot_event_group = xEventGroupCreate();
	
	// Clear the bit
	//xEventGroupClearBits(reboot_event_group, REBOOT_BIT);

	
	//for (;;)
	//{
		// Wait here until the bit gets set for reboot
		//EventBits_t staBits = xEventGroupWaitBits(reboot_event_group, REBOOT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
		
		// Did portMAX_DELAY ever timeout, not sure so lets just check to be sure
		//if ((staBits & REBOOT_BIT) != 0)
		//{
			ESP_LOGI("OTA", "Reboot Command, Restarting within %d", (int)parameter);
			vTaskDelay((int)parameter / portTICK_PERIOD_MS);

			esp_restart();
		//}
	//}

}




inline int ishex(int x)
{
	return	(x >= '0' && x <= '9')	||
		(x >= 'a' && x <= 'f')	||
		(x >= 'A' && x <= 'F');
}
	
int url_decode(const char *s, char *dec)
{
	char *o;
	const char *end = s + strlen(s);
	int c;
 
	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+') c = ' ';
		else if (c == '%' && (	!ishex(*s++)	||
					!ishex(*s++)	||
					!sscanf(s - 2, "%2x", &c)))
			return -1;
 
		if (dec) *o = c;
	}
 
	return o - dec;
}

uint32_t hex2int(char *hex) {
    uint32_t val = 0;
    while (*hex) {
        // get current character then increment
        char byte = *hex++; 
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
        // shift 4 to make space for new digit, and add the 4 bits of the new digit 
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}
