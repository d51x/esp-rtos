#include "mqtt.h"



static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t mqtt_client = NULL;
static mqtt_config_t _mqtt_cfg;


static TaskHandle_t xHanldeAll = NULL;

static char *_mqtt_dev_name = MQTT_DEVICE;

static void mqtt_uninitialize();


void mqtt_set_device_name(const char *dev_name){
    strcpy(_mqtt_dev_name, dev_name);
}

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    ESP_LOGV(TAG, "%s.........................", __func__);
    esp_mqtt_client_handle_t client = event->client;
//    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt_state = 1;
            mqtt_reconnects++;
            mqtt_subscribe_topics(client);
            // TODO: save status mqtt and counters

            break;
        case MQTT_EVENT_DISCONNECTED:

                mqtt_state = 0;
                ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
                // TODO: save status mqtt and counters

                //mqtt_uninitialize();
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGD(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGD(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGD(TAG, "MQTT_EVENT_DATA");

            //process_data(event);
            break;
        case MQTT_EVENT_ERROR:
            mqtt_error_count++;
            // TODO: save status mqtt and counters and last error
            ESP_LOGD(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

void mqtt_get_current_config(mqtt_config_t *cfg){
    memcpy(cfg, &_mqtt_cfg, sizeof(mqtt_config_t));
}




void mqtt_start(){/*const char *broker_url, uint16_t send_interval*/

    // load data from nvs
    ESP_LOGV(TAG, "%s started...", __func__);    
    strcpy(_mqtt_dev_name, MQTT_DEVICE);
    strcpy(_mqtt_cfg.broker_url, MQTT_BROKER_URL);
    _mqtt_cfg.enabled = 1;
    strcpy(_mqtt_cfg.login, "");
    strcpy(_mqtt_cfg.passw, "");
    _mqtt_cfg.send_interval = MQTT_SEND_INTERVAL;

    //mqtt_load_data_from_nvs(&_mqtt_cfg);

    if ( !_mqtt_cfg.enabled ) {
        ESP_LOGI(TAG, "MQTT disabled. Return.");
        return;
    } else {
        ESP_LOGI(TAG, "MQTT enabled. Start...");
    }
    ESP_LOGI(TAG, "Initialize mqtt with broker_url %s", _mqtt_cfg.broker_url);    
    if ( mqtt_client == NULL) {
        ESP_LOGI(TAG, "MQTT client is NULL");
        esp_mqtt_client_config_t mqtt_cfg = {
            //.uri = _broker_url,
            .uri = MQTT_BROKER_URL,
            //.client_id   now ESP32_MAC3
            .event_handle = mqtt_event_handler,
            // .user_context = (void *)your_context
        };
        
        //esp_mqtt_client_config_t mqtt_cfg;
        //strcpy(mqtt_cfg.uri, _mqtt_cfg.broker_url);
        //mqtt_cfg.event_handle = mqtt_event_handler;
        //_send_interval = send_interval;     
        
        mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
        //esp_mqtt_client_set_uri(mqtt_client, _mqtt_cfg.broker_url);    
    } else {
        ESP_LOGI(TAG, "MQTT client is not NLUL");
    }

    esp_mqtt_client_start(mqtt_client); 
    

    //esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_NONE);
}

void mqtt_stop(){
    // stop 
    mqtt_error_count = 0;
    mqtt_uninitialize();
    esp_mqtt_client_stop(mqtt_client); 
}


static void mqtt_uninitialize(){
    // delete tasks
    ESP_LOGI(TAG, "%s started...", __func__);

    if( xHanldeAll != NULL ) {
        ESP_LOGI(TAG, "xHanldeAll is not NULL, delete task");
        vTaskDelete( xHanldeAll );
        xHanldeAll = NULL;
    }  else {
        ESP_LOGI(TAG, "xHanldeAll is  NULL");
    }

}


void mqtt_subscribe_topics(esp_mqtt_client_handle_t client){
    char topic[32];
    strcpy(topic, _mqtt_dev_name ); /*MQTT_DEVICE*/
    strcpy(topic+strlen(topic), "#");
    //snprintf(topic, 32, MQTT_TOPIC_SUBSCRIBE, "test", "esp");
    esp_mqtt_client_subscribe(client, topic, 0);

}


static void mqtt_publish_generic(const char *_topic, const char *payload) {
    ESP_LOGV(TAG, "%s started", __func__);
    char topic[32];
    
    strcpy( topic, _mqtt_dev_name /*MQTT_DEVICE*/ );
    strcpy( topic + strlen( topic ), _topic);
    
    ESP_LOGV(TAG, "mqtt publish topic %s with payload %s", topic, payload);

    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }
}


void mqtt_publish_device_uptime(){
    char payload[10];
    sprintf(payload,"%lu", millis()/1000UL);
    mqtt_publish_generic( MQTT_DEVICE_UPTIME, payload);    
}

void mqtt_publish_device_freemem(){
    char payload[10];
    sprintf(payload,"%d", esp_get_free_heap_size());
    mqtt_publish_generic( MQTT_DEVICE_FREEMEM, payload);     
}

void mqtt_publish_device_rssi(){
    char payload[10];
    sprintf(payload,"%d", wifi_get_rssi());
    mqtt_publish_generic( MQTT_DEVICE_RSSI, payload);     
}





void mqtt_publish_all_task(void *arg){
    ESP_LOGD(TAG, "%s started...", __func__);
 
    //uint32_t delay_ms = _send_interval*1000;
    uint32_t delay_ms = _mqtt_cfg.send_interval*1000;
    while (1) {
        xEventGroupWaitBits(ota_event_group, OTA_IDLE_BIT, false, true, portMAX_DELAY);
        mqtt_publish_device_uptime();
        mqtt_publish_device_freemem();
        mqtt_publish_device_rssi();

        vTaskDelay( delay_ms / portTICK_RATE_MS);
    }

}

void mqtt_load_data_from_nvs(mqtt_config_t *cfg){
    ESP_LOGI(TAG, __func__);
    nvs_handle mqtt_handle;
    if ( nvs_open("mqtt", NVS_READONLY, &mqtt_handle) == ESP_OK ) {
        size_t size_buf = strlen(cfg->broker_url)-1;
        nvs_get_str(mqtt_handle, "url", NULL, &size_buf);
        if ( nvs_get_str(mqtt_handle, "url", cfg->broker_url, &size_buf) != ESP_OK ) {
            strcpy( cfg->broker_url, MQTT_BROKER_URL );        
            ESP_LOGE(TAG, "FAIL to load mqtt broker url. Using defaul %s", cfg->broker_url);
        }

        if ( nvs_get_u32(mqtt_handle, "interval", &cfg->send_interval) != ESP_OK ) {
            cfg->send_interval = MQTT_SEND_INTERVAL;
            ESP_LOGE(TAG, "FAIL to load mqtt send initerval. Using default: %d", cfg->send_interval);
        }      

        if ( nvs_get_u8(mqtt_handle, "enabled", (uint8_t *)&cfg->enabled) != ESP_OK ) {
            cfg->enabled = 0;
            ESP_LOGE(TAG, "FAIL to load mqtt enabled. Using default: %d", cfg->enabled);
        }   

        size_buf = strlen(cfg->login)-1;
        nvs_get_str(mqtt_handle, "login", NULL, &size_buf);
        if ( nvs_get_str(mqtt_handle, "login", cfg->login, &size_buf) != ESP_OK ) {
            strcpy( cfg->login, "" );        
            ESP_LOGE(TAG, "FAIL to load mqtt broker login. Using defaul <empty>");
        }

        size_buf = strlen(cfg->passw)-1;
        nvs_get_str(mqtt_handle, "passw", NULL, &size_buf);
        if ( nvs_get_str(mqtt_handle, "passw", cfg->passw, &size_buf) != ESP_OK ) {
            strcpy( cfg->passw, "" );        
            ESP_LOGE(TAG, "FAIL to load mqtt broker passw. Using defaul <empty>");
        }

        nvs_close(mqtt_handle);         
    } else {
        ESP_LOGE(TAG, "cannot open mqtt section in nvs");
    }
}

void mqtt_save_data_to_nvs(const mqtt_config_t *cfg){
    ESP_LOGI(TAG, __func__);
    nvs_handle mqtt_handle;
    if ( nvs_open("mqtt", NVS_READWRITE, &mqtt_handle) == ESP_OK ) {
        if ( nvs_set_str(mqtt_handle, "url", cfg->broker_url) != ESP_OK ) ESP_LOGE(TAG, "ERROR: cannot save mqtt url to nvs");
        if ( nvs_set_u32(mqtt_handle, "interval", cfg->send_interval) != ESP_OK) ESP_LOGE(TAG, "ERROR: cannot save mqtt interval to nvs");
        

        ESP_LOGI(TAG, "save mqtt send enabled to nvs - %d", cfg->enabled);
        if ( nvs_set_u8(mqtt_handle, "enabled", cfg->enabled) == ESP_OK) {
            ESP_LOGI(TAG, "mqtt enabled saved to nvs successfully");
        } else {
            ESP_LOGE(TAG, "ERROR: cannot save mqtt enabled to nvs");
        }

        if ( nvs_set_str(mqtt_handle, "login", cfg->login) != ESP_OK) ESP_LOGE(TAG, "ERROR: cannot save mqtt login to nvs");
        if ( nvs_set_str(mqtt_handle, "passw", cfg->passw) != ESP_OK) ESP_LOGE(TAG, "ERROR: cannot save mqtt passw to nvs");
    } else {
        ESP_LOGE(TAG, "Cannot open MQTT nvs for write");
    }
    nvs_close(mqtt_handle);
}
