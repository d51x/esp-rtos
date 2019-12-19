#include "esp_system.h"

//#include "httpd.h"
#include "mqtt.h"



static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t mqtt_client;
static void process_data(esp_mqtt_event_handle_t event);

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
//    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            mqtt_state = 1;
            mqtt_reconnects++;
            mqtt_subscribe_topics(client);
            // TODO: save status mqtt and counters
            break;
        case MQTT_EVENT_DISCONNECTED:
            mqtt_state = 0;
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            // TODO: save status mqtt and counters
            break;

        case MQTT_EVENT_SUBSCRIBED:
            //ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            //ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            //ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            //ESP_LOGI(TAG, "MQTT_EVENT_DATA");

            process_data(event);
            break;
        case MQTT_EVENT_ERROR:
            mqtt_error_count++;
            // TODO: save status mqtt and counters and last error
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

void mqtt_start(){
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URL,
        .event_handle = mqtt_event_handler,
        // .user_context = (void *)your_context
    };
/*
        .uri = "mqtts://api.emitter.io:443",    // for mqtt over ssl
        // .uri = "mqtt://api.emitter.io:8080", //for mqtt over tcp
        // .uri = "ws://api.emitter.io:8080", //for mqtt over websocket
        // .uri = "wss://api.emitter.io:443", //for mqtt over websocket secure
*/
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client); 
    //esp_mqtt_client_stop

    //esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_NONE);
}

void mqtt_subscribe_topics(esp_mqtt_client_handle_t client){
    char topic[32];

    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), "#");
    //snprintf(topic, 32, MQTT_TOPIC_SUBSCRIBE, "test", "esp");
    esp_mqtt_client_subscribe(client, topic, 0);

}

void mqtt_publish_gpio_input_state(const gpio_t *gpio){
    char topic[32];
    //snprintf(topic, 32, MQTT_DEVICE_GPIO_INPUT, "test", "esp", gpio->pin);
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_GPIO_INPUT);
    itoa(gpio->pin, topic+strlen(topic), 10);
    char payload[10];
    sprintf(payload,"%d", gpio->state);
    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }
}

void mqtt_publish_gpio_output_state(uint8_t pin, uint8_t state){
    //return;
    //ESP_LOGI(TAG, "%s: gpio%d state %d", __func__, pin, state);
    char topic[32];
    //snprintf(topic, 32, MQTT_DEVICE_GPIO_OUTPUT, "test", "esp", gpio->pin);
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_GPIO_OUTPUT);    
    itoa(pin, topic+strlen(topic), 10);
    char payload[10];
    sprintf(payload,"%d", state);
    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }    
}

void mqtt_publish_gpio_state_task(void *arg){
    
    //mqtt_gpio_t *gp = (mqtt_gpio_t *)(arg);
    mqtt_gpio_t *gp = malloc(sizeof(mqtt_gpio_t));
    memcpy(gp, arg, sizeof(mqtt_gpio_t));
    ESP_LOGI(TAG, "%s: gpio%d state %d", __func__, gp->pin, gp->state);
    vTaskDelay(300 / portTICK_RATE_MS);
    //ESP_LOGI(TAG, "%s: after delay gpio%d state %d", __func__, gp->pin, gp->state);
    mqtt_publish_gpio_output_state(gp->pin, gp->state);
    free(gp);
    vTaskDelete(NULL);
}

void mqtt_publish_device_uptime(){
    char topic[32];
    //snprintf(topic, 32, MQTT_DEVICE_UPTIME, "test", "esp");
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_UPTIME);  
    char payload[10];
    sprintf(payload,"%lu", millis()/1000UL);
    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }    
}

void mqtt_publish_device_freemem(){
    char topic[32];
    //snprintf(topic, 32, MQTT_DEVICE_FREEMEM, "test", "esp");
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_FREEMEM);      
    char payload[10];
    sprintf(payload,"%d", esp_get_free_heap_size());
    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }    
}

void mqtt_publish_device_rssi(){
    char topic[32];
    //snprintf(topic, 32, MQTT_DEVICE_RSSI, "test", "esp");
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_RSSI);  

    char payload[10];
    sprintf(payload,"%d", wifi_get_rssi());
    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }    
}

#ifdef DHT
void mqtt_publish_dhtt(){
    char topic[32];
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_DHTT);  

    char payload[10];
    sprintf(payload,"%.2f",  dht.temp);
    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }    
}

void mqtt_publish_dhth(){
    char topic[32];
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_DHTH);  

    char payload[10];
    sprintf(payload,"%.2f",  dht.hum);
    int res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
    if ( res == -1 ) {
        // not connected or not published
        // mqtt_client->state is MQTT_STATE_CONNECTED
    } else if ( res == 0 ) {
        // OK
    }    
}
#endif

void mqtt_publish_dsw(){
    char topic[32];
    strcpy(topic, MQTT_DEVICE);
    strcpy(topic+strlen(topic), MQTT_DEVICE_DSW);  

    char payload[10];
    int res;
    for (uint8_t i=0;i<DSW_COUNT;i++){
        if ( ds18b20[i].addr[0] ) {
            sprintf(topic,topic, i+1);
            sprintf(payload,"%.2f",  ds18b20[i].temp);
            res = esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
            if ( res == -1 ) {
                // not connected or not published
                // mqtt_client->state is MQTT_STATE_CONNECTED
            } else if ( res == 0 ) {
                // OK
            }            
        }
    }
    
 
}

void mqtt_publish_all_task(void *arg){
    //ESP_LOGI(TAG, "%s: started\n", __func__);

    while (1) {
        mqtt_publish_device_uptime();
        mqtt_publish_device_freemem();
        mqtt_publish_device_rssi();

        uint8_t i;
        for ( i = 0; i < GPIO_NUM_MAX; i++) {
            if ( gpio[i].pin == 255) continue;
            //mqtt_gpio_t mqtt_gpio = {gpio[i].pin, gpio[i].state};
            //xTaskCreate(mqtt_publish_gpio_state_task, "mqtt_publish_gpio_state_task", 2048, &mqtt_gpio, 10, NULL);            
            mqtt_publish_gpio_output_state(gpio[i].pin, gpio[i].state);
        }

        #ifdef DHT
        mqtt_publish_dhtt();
        mqtt_publish_dhth();
        #endif

        mqtt_publish_dsw();

        vTaskDelay(MQTT_SEND_INTERVAL / portTICK_RATE_MS);
    }

}

static void process_data(esp_mqtt_event_handle_t event){
    char *topic = malloc(event->topic_len+1);
    memset(topic, 0, event->topic_len+1);
    strncpy(topic, event->topic, event->topic_len);

    char *data = malloc(event->data_len+1);
    memset(data, 0, event->data_len+1);
    strncpy(data, event->data, event->data_len);

    //printf("TOPIC=%s\n", topic);
    //printf("DATA=%s\n", data);

    char *istr;
    istr = strstr(topic, MQTT_DEVICE_GPIO_OUTPUT);
    if ( istr != NULL ) {
        //ESP_LOGI(TAG, "%s: ---------------------------------------------", __func__);
        //ESP_LOGI(TAG, "%s: gpio output topic found,  pos %s", __func__, istr);
        char num[3]="";
        uint8_t pos = istr - topic + 1 + strlen(MQTT_DEVICE_GPIO_OUTPUT);
        uint8_t len = event->topic_len - pos + 1;
        strncpy(num, istr+strlen(MQTT_DEVICE_GPIO_OUTPUT), len);  
        trim(num);
        //ESP_LOGI(TAG, "%s: gpio index %s", __func__, num);
        trim(data);
        //ESP_LOGI(TAG, "%s: gpio state %s", __func__, data);
        
        uint8_t pin, st;
        if ( str_to_uint8(&pin, num, 10) == 0 && str_to_uint8(&st, data, 10) == 0) {
            //ESP_LOGI(TAG, "%s: set GPIO pin %d to %d", __func__, pin, st);
            set_gpio(pin, st, 0);
        } else {
            //ESP_LOGE(TAG, "%s: FAIL get pin or st", __func__);
        }          
    } 
    free(topic);
    free(data);
}
