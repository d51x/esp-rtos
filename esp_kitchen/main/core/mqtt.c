#include "mqtt.h"



static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t mqtt_client = NULL;
static mqtt_config_t _mqtt_cfg;


static TaskHandle_t xHanldeAll = NULL;

char *_mqtt_dev_name[30];

static void mqtt_uninitialize();

static void process_data(esp_mqtt_event_handle_t event);

void mqtt_set_device_name(const char *dev_name, const char *login){
    strcpy(_mqtt_dev_name, login);
    strcat(_mqtt_dev_name, "/");
    strcat(_mqtt_dev_name, dev_name);
    strcat(_mqtt_dev_name, "/");
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

            if ( xHanldeAll == NULL )
                xTaskCreate(mqtt_publish_all_task, "mqtt_publish_all_task", 2048, NULL, 10, &xHanldeAll);
            break;
        case MQTT_EVENT_DISCONNECTED:

                mqtt_state = 0;
                ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
                // TODO: save status mqtt and counters
                if ( xHanldeAll ) {
                    vTaskDelete(xHanldeAll);
                }
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

            process_data(event);
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

    mqtt_load_data_from_nvs(&_mqtt_cfg);

    mqtt_set_device_name(wifi_hostname, _mqtt_cfg.login);


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
    //ESP_LOGI(TAG, "%s started", __func__);
    char topic[32];
    
    strcpy( topic, _mqtt_dev_name /*MQTT_DEVICE*/ );
    strcpy( topic + strlen( topic ), _topic);
    
    ESP_LOGI(TAG, "mqtt publish topic %s with payload %s", topic, payload);

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

        // TODO: make array with func_cb and data and register method for callbacks
        mqtt_publish_fan_state();
        mqtt_publish_effect_name();
        mqtt_publish_effect_id();
        mqtt_publish_ledc_duty( LED_CTRL_RED_CH );
        mqtt_publish_ledc_duty( LED_CTRL_GREEN_CH );
        mqtt_publish_ledc_duty( LED_CTRL_BLUE_CH );
        mqtt_publish_ledc_duty( LED_CTRL_WHITE_CH );
        mqtt_publish_adc_thld();
        vTaskDelay( delay_ms / portTICK_RATE_MS);
    }

}

void mqtt_load_data_from_nvs(mqtt_config_t *cfg){

    // broker url
    if ( nvs_param_str_load("mqtt", "url",  cfg->broker_url) != ESP_OK ) {
        strcpy(cfg->broker_url, MQTT_BROKER_URL);
        ESP_LOGE(TAG, "FAIL to load mqtt broker url. Using defaul %s", cfg->broker_url);
    }

    // mqtt send interval
    if ( nvs_param_u32_load("mqtt", "interval", &cfg->send_interval) != ESP_OK ) {
        cfg->send_interval = MQTT_SEND_INTERVAL;
        ESP_LOGE(TAG, "FAIL to load mqtt send initerval. Using default: %d", cfg->send_interval);
    }

    // mqtt enabled
    if ( nvs_param_u8_load("mqtt", "enabled", &cfg->enabled) != ESP_OK ) {
        cfg->enabled = 0;
        ESP_LOGE(TAG, "FAIL to load mqtt enabled. Using default: %d", cfg->enabled);
    }

    // mqtt login
    if ( nvs_param_str_load("mqtt", "login",  cfg->login) != ESP_OK ) {
        strcpy(cfg->login, "");
        ESP_LOGE(TAG, "FAIL to load mqtt broker login. Using defaul <empty>");
    }

    // mqtt password
    if ( nvs_param_str_load("mqtt", "passw",  cfg->passw) != ESP_OK ) {
        strcpy(cfg->passw, "");
        ESP_LOGE(TAG, "FAIL to load mqtt broker passw. Using defaul <empty>");
    }
}

void mqtt_save_data_to_nvs(const mqtt_config_t *cfg){
    nvs_param_str_save("mqtt", "url",  cfg->broker_url);  
    nvs_param_u32_save("mqtt", "interval", cfg->send_interval); 
    nvs_param_u8_save("mqtt", "enabled", cfg->enabled);
    ESP_LOGI(TAG, "save login %s", cfg->login);
    if ( nvs_param_str_save("mqtt", "login",  cfg->login) != ESP_OK ) 
        ESP_LOGE(TAG, "fail to save login");
    nvs_param_str_save("mqtt", "passw",  cfg->passw);

    memcpy(&_mqtt_cfg, cfg, sizeof(mqtt_config_t));
}

void mqtt_extern_publish(const char *_topic, const char *payload){

    mqtt_publish_generic( _topic, payload);
}


static void process_data(esp_mqtt_event_handle_t event){
    char *topic = malloc(event->topic_len+1);
    memset(topic, 0, event->topic_len+1);
    strncpy(topic, event->topic, event->topic_len);

    char *data = malloc(event->data_len+1);
    memset(data, 0, event->data_len+1);
    strncpy(data, event->data, event->data_len);

    //ESP_LOGI(TAG, "TOPIC=%s", topic);
    //ESP_LOGI(TAG, "DATA=%s", data);

    // cut _mqtt_dev_name from topic
    char _topic[20];
    strcpy(_topic, topic + strlen(_mqtt_dev_name));
    //ESP_LOGI(TAG, "new topic %s", _topic);

    if ( strstr( _topic, "output" ) != NULL ) {
        // process gpio
        char spin[3];
        strcpy(spin, _topic + 6 );
        //ESP_LOGI(TAG, "gpio pin %s state %s", spin, data);
        // КОСТЫЛЬ!!!
        uint8_t pin = atoi( spin );
        uint8_t state = atoi( data );

        //if ( pin == RELAY_FAN_PIN ) {
        if ( pin == relay_fan_pin ) {
            if ( state != relay_read(relay_fan_h) )
                relay_write(relay_fan_h, state);
        }

    } else if ( strstr( _topic, "effect" ) != NULL ) {
        // process effect
        char effect[15];
        strcpy(effect, _topic + 6 + 1 /* 1 = "/"" */ );
        if ( strstr(effect, "name" ) != NULL ) {
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL )  {                 
                effect_t *e = ef->effect + ef->effect_id;
                if ( strcmp(e->name, data) != ESP_OK) {
                    ef->set_by_name( data );   
                }
            }
        } else if ( strstr(effect, "id") != NULL ) {
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL )  {                 
                uint8_t val = atoi(data);
                if ( ef->effect_id != val ) {
                    ef->set( val );   
                }
            }
        }

    
    } else if ( strstr( _topic, "color" ) != NULL ) {
        // process color
        char color[15];
        strcpy(color, _topic + 5 + 1 /* 1 = "/"" */ );
        if ( strstr( color, "rgb" ) != NULL ) {
            // process rgb color
            char *istr = strtok (data,",");
            color_rgb_t *rgb = malloc(sizeof(color_rgb_t));
            rgb->r = atoi(istr);
            istr = strtok (NULL,",");
            rgb->g = atoi(istr); 
            istr = strtok (NULL,",");
            rgb->b = atoi(istr);
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL ) ef->stop();
            rgb_ledc->set_color_rgb(*rgb);
            free(rgb);  
        } else if ( strstr( color, "hsv" ) != NULL ) {
            // process hsv color
            char *istr = strtok (data,",");
            color_hsv_t *hsv = malloc( sizeof(color_hsv_t));
            hsv->h = atoi(istr);
            istr = strtok (NULL,",");
            hsv->s = atoi(istr);
            istr = strtok (NULL,",");
            hsv->v = atoi(istr);
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL ) ef->stop();
            rgb_ledc->set_color_hsv(*hsv);
            free(hsv);
        } else if ( strstr( color, "hex" ) != NULL ) {
            // process hex color
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL ) ef->stop();     
            rgb_ledc->set_color_hex(data);          
        } else if ( strstr( color, "int" ) != NULL ) {
            // process hex color
            effects_t *ef = (effects_t *) rgb_ledc->effects;
            if ( ef != NULL ) ef->stop();     
            uint32_t val = atoi(data);
            rgb_ledc->set_color_int(val);          
        }       
    } else if ( strstr( _topic, "sunset" ) != NULL ) {
        uint8_t val = atoi( data );
        is_sunset = val;
        is_dark = get_dark_mode( pir_mode );
    } else if ( strstr( _topic, MQTT_TOPIC_LEDC_CHANNEL ) != NULL ) {
        char s_ch[2];
        strcpy(s_ch, _topic + strlen(MQTT_TOPIC_LEDC_CHANNEL) );
        uint8_t ch = atoi( s_ch );
        uint32_t duty = atoi( data );
        if ( duty != ledc->get_duty( ledc->channels + ch ) ) {
            ledc->set_duty( ledc->channels + ch, duty );
            ledc->update();
        }
    } else if ( strstr( _topic, "adcthld") != NULL ) {
        uint16_t val = atoi( data );
        if (val > 1023) val = 1023;
        if ( val != adc_lvl ) {
            adc_lvl = val;
            nvs_param_u16_save("main", "adclvl", adc_lvl);
        }
    }


    free(topic);
    free(data);

}

 void mqtt_publish_fan_state(){
    uint8_t st =  relay_read(relay_fan_h);
    char payload[2];
    itoa(st, payload, 10);
    char topic[10];
    //sprintf(topic, "output%d", RELAY_FAN_PIN);
    sprintf(topic, "output%d", relay_fan_pin);
    mqtt_publish_generic( topic, payload);
 }

 void mqtt_publish_effect_name(){
    effects_t *ef = (effects_t *) rgb_ledc->effects;
    effect_t *e = ef->effect + ef->effect_id;
    char payload[12];
    strcpy(payload, e->name);
    mqtt_publish_generic( MQTT_TOPIC_EFFECT_NAME, payload);
 }

  void mqtt_publish_effect_id(){
    effects_t *ef = (effects_t *) rgb_ledc->effects;
    effect_t *e = ef->effect + ef->effect_id;
    char payload[3];
    itoa(ef->effect_id, payload, 10);
    mqtt_publish_generic( MQTT_TOPIC_EFFECT_ID, payload);
 }

void mqtt_publish_ledc_duty(uint8_t channel){
    char payload[3];
    char topic[10];
    char s_id[2];
    itoa(channel, s_id, 10);
    strcpy(topic, MQTT_TOPIC_LEDC_CHANNEL);
    strcat(topic, s_id);
    itoa(ledc->get_duty( ledc->channels + channel), payload, 10);
    mqtt_publish_generic( topic, payload);
 }

 void mqtt_publish_adc_thld() {
    char payload[5];
    itoa(adc_lvl, payload, 10);
    mqtt_publish_generic( "adcthld", payload);    
 }