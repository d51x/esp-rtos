#include "wifi.h"



static const char *TAG = "WIFI";

void wifi_init_softap(wifi_config_t *wifi_cfg){

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_AP_SSID,
            .ssid_len = strlen(ESP_WIFI_AP_SSID),
            .password = ESP_WIFI_AP_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .ssid_hidden = 0,
        },
    };
    

    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    
    memcpy(wifi_cfg, &wifi_config, sizeof(wifi_config_t));

    
      
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, wifi_cfg);
    

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s", ESP_WIFI_AP_SSID, ESP_WIFI_PASS);
}

void wifi_init_sta(wifi_config_t *wifi_cfg)
{
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    
    //wifi_nvs_cfg_t *wifi_nvs_cfg = malloc(sizeof( wifi_nvs_cfg_t));
    //wifi_load_data_from_nvs( wifi_nvs_cfg );
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
        },
    };

    //ESP_LOGI(TAG, "STA:: Using SSID: %s", wifi_nvs_cfg->ssid);
    //ESP_LOGI(TAG, "STA:: Using passw: %s", wifi_nvs_cfg->password);


    //strcpy(&wifi_config.sta.ssid, wifi_nvs_cfg->ssid);
    //strcpy(&wifi_config.sta.password, wifi_nvs_cfg->password);
    

    //ESP_LOGI(TAG, "SSDI: %s", ESP_WIFI_SSID);
    memcpy(wifi_cfg, &wifi_config, sizeof(wifi_config_t));
    

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_cfg->sta.ssid);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_cfg);

    //ESP_LOGI(TAG, "wifi_init_sta finished.");
    //ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    //free(wifi_nvs_cfg);
}

esp_err_t wifi_event_handler(void *ctx, system_event_t *event){
    //httpd_handle_t *_server = (httpd_handle_t *) ctx;
    system_event_info_t *info = &event->event_info;
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");

            
            char* _hostname = malloc(TCPIP_HOSTNAME_MAX_SIZE);
            _hostname = set_hostname(NULL);
            ESP_LOGI(TAG, _hostname);
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, _hostname);  //TCPIP_HOSTNAME_MAX_SIZE    32
            // ????
            //esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
            // ????
            esp_wifi_connect();
            wifi_info.wifi_reconnect = 0;
            free(_hostname);
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
            ESP_LOGI(TAG, "got ip: %s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            wifi_info.status = 1;
            memcpy(&wifi_info.ip, &event->event_info.got_ip.ip_info.ip, 4);
            
            //esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B );//| WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
            xEventGroupSetBits(ota_event_group, OTA_IDLE_BIT);
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            //ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STACONNECTED");
            wifi_info.status = 1;
            ESP_LOGI(TAG, "station: "MACSTR" join, AID=%d", MAC2STR(event->event_info.sta_connected.mac), event->event_info.sta_connected.aid);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:       
            //ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STADISCONNECTED");
            wifi_info.status = 0;
            ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d", MAC2STR(event->event_info.sta_disconnected.mac), event->event_info.sta_disconnected.aid);          
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            wifi_info.wifi_reconnect++;
            wifi_info.status = 0;
            //ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            //ESP_LOGE(TAG, "wifi_reconnect: %d", wifi_info.wifi_reconnect);
            //ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);
            if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
                /*Switch to 802.11 bgn mode */
                esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
            }
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;    
}

void wifi_init(wifi_mode_t wifi_mode) {
    // get gpio1 and gpio3 states, if 1 and 1, set AP, else STA
    // start wifi connection and webserver

    WIFI_CONNECTED_BIT = BIT0;
    wifi_info.status = 0;
    wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    esp_event_loop_init(wifi_event_handler, NULL);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_nvs_cfg_t *wifi_nvs_cfg = malloc(sizeof( wifi_nvs_cfg_t));
    wifi_load_data_from_nvs( wifi_nvs_cfg );
    
    //wifi_info.wifi_mode = wifi_mode;
    //if ( wifi_nvs_cfg->first != 0) {
        //wifi_info.wifi_mode = WIFI_MODE_AP;
       //wifi_nvs_cfg->first = 0;
       /// wifi_nvs_cfg->mode = WIFI_MODE_AP;
        //wifi_save_data_to_nvs(wifi_nvs_cfg);
    //} else {
       //wifi_info.wifi_mode = wifi_nvs_cfg->mode;
    //}
    

///ESP_LOGI(TAG, "Using SSID: %s", wifi_nvs_cfg->ssid);
///ESP_LOGI(TAG, "Using passw: %s", wifi_nvs_cfg->password);
///ESP_LOGI(TAG, "Using mode: %s", (wifi_nvs_cfg->mode == WIFI_MODE_STA) ? "STA" : ( (wifi_nvs_cfg->mode == WIFI_MODE_AP) ? "AP" : "Disable"));

    wifi_config_t wifi_config;
    switch ( wifi_mode ) {
    //switch ( wifi_info.wifi_mode ) {
        case WIFI_MODE_STA:
            wifi_init_sta(&wifi_config);
            esp_read_mac(wifi_info.mac, ESP_MAC_WIFI_STA);

            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
            break;
        case WIFI_MODE_AP:
            wifi_init_softap(&wifi_config);
            esp_read_mac(wifi_info.mac, ESP_MAC_WIFI_SOFTAP);
            break;
        case WIFI_MODE_APSTA:

            break;
        default:
            break;    
    }
    
    esp_wifi_start();
    //free(wifi_nvs_cfg);
}

int8_t wifi_get_rssi(){
    wifi_ap_record_t info;
    if( !esp_wifi_sta_get_ap_info(&info) ) {
        return info.rssi;
    }
    return 0;
}

void wifi_load_data_from_nvs(wifi_nvs_cfg_t *cfg ){
    ESP_LOGI(TAG, __func__);
    nvs_handle wifi_handle;
    if ( nvs_open("wifi", NVS_READONLY, &wifi_handle) == ESP_OK ) {
        size_t size_buf = strlen(cfg->ssid)-1;
        nvs_get_str(wifi_handle, "ssid", NULL, &size_buf);
        if ( nvs_get_str(wifi_handle, "ssid", cfg->ssid, &size_buf) != ESP_OK ) {
            strcpy( cfg->ssid, ESP_WIFI_SSID );        
            ESP_LOGE(TAG, "FAIL to load wifi ssid. Using defaul %s", cfg->ssid);
        }
        size_buf = strlen(cfg->password)-1;
        nvs_get_str(wifi_handle, "passw", NULL, &size_buf);
        if ( nvs_get_str(wifi_handle, "passw", cfg->password, &size_buf) != ESP_OK ) {
            strcpy( cfg->password, ESP_WIFI_PASS );        
            ESP_LOGE(TAG, "FAIL to load wifi password. Using defaul %s", cfg->password);
        }
        if ( nvs_get_u8(wifi_handle, "mode", (uint8_t *)&cfg->mode) != ESP_OK ) {
            cfg->mode = WIFI_MODE_AP;
            ESP_LOGE(TAG, "FAIL to load wifi mode. Using default: %d", cfg->mode);
        }        
        if ( nvs_get_u8(wifi_handle, "first", (uint8_t *)&cfg->first) != ESP_OK ) {
            cfg->first = 1;
            ESP_LOGE(TAG, "FAIL to load wifi first. Using default: %d", cfg->first);
        } else {
            ESP_LOGE(TAG, "Loaded wifi first %d", cfg->first);
        }                
        nvs_close(wifi_handle); 
    }  else {
        ESP_LOGE(TAG, "cannot open wifi section in nvs");
    }        
}

void wifi_save_data_to_nvs(const wifi_nvs_cfg_t *cfg){
    ESP_LOGI(TAG, __func__);
    nvs_handle wifi_handle;
    if ( nvs_open("wifi", NVS_READWRITE, &wifi_handle) == ESP_OK ) {
        if ( nvs_set_str(wifi_handle, "ssid", cfg->ssid) != ESP_OK) {
            ESP_LOGE(TAG, "ERROR: cannot save wifi ssid to nvs");
        }
        if ( nvs_set_str(wifi_handle, "passw", cfg->password) != ESP_OK) {
            ESP_LOGE(TAG, "ERROR: cannot save wifi password to nvs");
        }
        if ( nvs_set_u8(wifi_handle, "mode", cfg->mode) != ESP_OK) {
            ESP_LOGE(TAG, "ERROR: cannot save wifi mode to nvs");
        }
        if ( nvs_set_u8(wifi_handle, "first", cfg->first) != ESP_OK) {
            ESP_LOGE(TAG, "ERROR: cannot save wifi first to nvs");
        } 
    } else {
        ESP_LOGE(TAG, "Cannot open WIFI nvs for write");
    }
    nvs_close(wifi_handle);
}
