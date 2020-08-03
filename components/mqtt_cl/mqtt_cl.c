#include "mqtt_cl.h"


static const char *TAG = "MQTT";

#define PARAM_MQTT "mqtt"
#define PARAM_MQTT_URL "url"
#define PARAM_MQTT_SEND_INTERVAL "interval"
#define PARAM_MQTT_ENABLED "enabled"
#define PARAM_MQTT_LOGIN "login"
#define PARAM_MQTT_PASSWORD "passw"
#define PARAM_MQTT_TOPIC_BASE "base"

#define MQTT_DEICE_NAME_LENGTH TCPIP_HOSTNAME_MAX_SIZE + MQTT_CFG_LOGIN_LENGTH + 2
static int retry_num = 0;
static int reconnect_count = 0;

static esp_mqtt_client_handle_t mqtt_client = NULL;
static mqtt_config_t _mqtt_cfg;
static char *_mqtt_dev_name[MQTT_DEICE_NAME_LENGTH];

static TaskHandle_t xHanldePublishAll = NULL;

static mqtt_send_t mqtt_send[MQTT_SEND_CB];
static mqtt_recv_t mqtt_recv[MQTT_RECV_CB];
static uint8_t mqtt_send_cnt = 0;

static void process_data(esp_mqtt_event_handle_t event);
static void mqtt_publish_generic(const char *_topic, const char *payload);
static void mqtt_publish_all_task(void *arg);
static void mqtt_publish_device_uptime();
static void mqtt_publish_device_freemem();
static void mqtt_publish_device_rssi();
static void mqtt_subscribe_topics(esp_mqtt_client_handle_t client);
static void process_custom_topics(const char *_topic, const char *data);
static void mqtt_publish_custom_registered_topics();

void mqtt_load_cfg(mqtt_config_t *cfg)
{
    // broker url
    if ( nvs_param_str_load(PARAM_MQTT, PARAM_MQTT_URL,  cfg->broker_url) != ESP_OK ) {
        strcpy(cfg->broker_url, MQTT_BROKER_URL);
    }

    // mqtt send interval
    if ( nvs_param_u16_load(PARAM_MQTT, PARAM_MQTT_SEND_INTERVAL, &cfg->send_interval) != ESP_OK ) {
        cfg->send_interval = MQTT_SEND_INTERVAL;
    }

    // mqtt enabled
    if ( nvs_param_u8_load(PARAM_MQTT, PARAM_MQTT_ENABLED, &cfg->enabled) != ESP_OK ) {
        cfg->enabled = 0;
    }

    // mqtt login
    if ( nvs_param_str_load(PARAM_MQTT, PARAM_MQTT_LOGIN,  cfg->login) != ESP_OK ) {
        strcpy(cfg->login, CONFIG_MQTT_LOGIN);
    }

    // mqtt password
    if ( nvs_param_str_load(PARAM_MQTT, PARAM_MQTT_PASSWORD,  cfg->password) != ESP_OK ) {
        strcpy(cfg->password, CONFIG_MQTT_PASS);
    }

    // mqtt base topic
    if ( nvs_param_str_load(PARAM_MQTT, PARAM_MQTT_TOPIC_BASE,  cfg->base_topic) != ESP_OK ) {
        strcpy(cfg->base_topic, CONFIG_MQTT_TOPIC_BASE);
    }
}

void mqtt_get_cfg(mqtt_config_t *cfg)
{
    memcpy(cfg, &_mqtt_cfg, sizeof(mqtt_config_t)); 
}

void mqtt_save_cfg(const mqtt_config_t *cfg)
{
    nvs_param_u8_save(PARAM_MQTT, PARAM_MQTT_ENABLED, cfg->enabled);
    nvs_param_str_save(PARAM_MQTT, PARAM_MQTT_URL,  cfg->broker_url); 
    nvs_param_str_save(PARAM_MQTT, PARAM_MQTT_LOGIN,  cfg->login);
    nvs_param_str_save(PARAM_MQTT, PARAM_MQTT_PASSWORD,  cfg->password);
    nvs_param_str_save(PARAM_MQTT, PARAM_MQTT_TOPIC_BASE,  cfg->base_topic);
    nvs_param_u16_save(PARAM_MQTT, PARAM_MQTT_SEND_INTERVAL, cfg->send_interval); 
    memcpy(&_mqtt_cfg, cfg, sizeof(mqtt_config_t));
}

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    
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

            // mqtt_online_task
            // mqtt_keep_alive_task
            // mqtt_received_task
            if ( xHanldePublishAll == NULL ) {
                xTaskCreate(mqtt_publish_all_task, "mqtt_publish_all_task", 2048, NULL, CONFIG_MQTT_TASK_PRIORITY, &xHanldePublishAll);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:

                mqtt_state = 0;
                ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
                // TODO: save status mqtt and counters
                if ( xHanldePublishAll ) {
                    vTaskDelete(xHanldePublishAll);
                    xHanldePublishAll = NULL;
                }
                //mqtt_deinit();
                //mqtt_init();
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");

            process_data(event);
            break;
        case MQTT_EVENT_ERROR:
            mqtt_error_count++;
            // TODO: save status mqtt and counters and last error
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }

    //ESP_LOGI(TAG, "Event handler remaning space: %d", uxTaskGetStackHighWaterMark( NULL ));
    //print_task_stack_depth(TAG, __func__);
    return ESP_OK;
}

static char * mqtt_client_id()
{
    char *buf = calloc(1, 10);
    buf = wifi_get_mac();
    sprintf(buf, MQTT_CLIENT_ID_MASK, buf[3], buf[4], buf[5]);
    ESP_LOGI(TAG, "mqtt client id: %s", buf);
    return buf;
}

void mqtt_init()
{

    //load data from nvs
    mqtt_load_cfg(&_mqtt_cfg);

   if ( !_mqtt_cfg.enabled ) {
        return;
    }

    memset(mqtt_send, 0, MQTT_SEND_CB*sizeof(mqtt_send_t));
    memset(mqtt_recv, 0, MQTT_RECV_CB*sizeof(mqtt_recv_t));

    EventBits_t bits = xEventGroupWaitBits(xWiFiEventGroup, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Wifi connected, initialize mqtt...");
        if ( mqtt_client == NULL) 
            {
                char *client_id = calloc(1, 10);
                client_id = mqtt_client_id();
                ESP_LOGI(TAG, "client id: %s", client_id);
                esp_mqtt_client_config_t mqtt_cfg = {
                    .event_handle = mqtt_event_handler,
                    .disable_auto_reconnect = false,
                    .task_prio = CONFIG_MQTT_TASK_PRIORITY,
                    .keepalive = CONFIG_MQTT_KEEPALIVE_TIMEOUT,
                    //.username = "wwww",          
                };
                
                //const char *client_id;                  /*!< default client id is ``ESP32_%CHIPID%`` where %CHIPID% are last 3 bytes of MAC address in hex format */
        //const char *lwt_topic;                  /*!< LWT (Last Will and Testament) message topic (NULL by default) */
            //const char *lwt_msg;                    /*!< LWT message (NULL by default) */    
            //    void *user_context;                     /*!< pass user context to this option, then can receive that context in ``event->user_context`` */

                mqtt_cfg.uri = strdup(_mqtt_cfg.broker_url);
                mqtt_cfg.username = strdup( _mqtt_cfg.login);
                mqtt_cfg.password = strdup( _mqtt_cfg.password);
                mqtt_cfg.client_id = strdup( client_id );
                free(client_id);

                ESP_LOGI(TAG, "Mqtt client_id: %s", mqtt_cfg.client_id);

                mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
            }

            esp_mqtt_client_start(mqtt_client); 
            esp_log_level_set("*", ESP_LOG_NONE);
            //esp_log_level_set("MQTT_CLIENT", ESP_LOG_INFO);
            //esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
            //esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
            //esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
            //esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
    }  
}

void mqtt_deinit()
{
    // stop 
    ESP_LOGI(TAG, "MQTT: Disconnecting");
    mqtt_error_count = 0;
    esp_mqtt_client_stop(mqtt_client); 

    if ( mqtt_client != NULL ) 
    {
        esp_mqtt_client_destroy(mqtt_client);
    }
    ESP_LOGI(TAG, "MQTT: Disconnected");
}

static void process_data(esp_mqtt_event_handle_t event)
{
    char *topic = malloc(event->topic_len+1);
    memset(topic, 0, event->topic_len+1);
    strncpy(topic, event->topic, event->topic_len);

    char *data = malloc(event->data_len+1);
    memset(data, 0, event->data_len+1);
    strncpy(data, event->data, event->data_len);

    //ESP_LOGI(TAG, "TOPIC=%s", topic);
    //ESP_LOGI(TAG, "DATA=%s", data);

    // cut _mqtt_dev_name from topic
    char _topic[TOPIC_END_NAME_LENGTH];
    strcpy(_topic, topic + strlen(_mqtt_dev_name));

    process_custom_topics(_topic, data);  // mqtt_add_receive_callback

    free(topic);
    free(data);    
}

static void process_custom_topics(const char *_topic, const char *data) 
{
    for ( uint8_t i = 0; i < MQTT_RECV_CB; i++ ) {
        if ( strcmp( mqtt_recv[i].topic, _topic) == ESP_OK ) 
        {
            mqtt_recv[i].fn_cb(data);
        }
    }
}


static void mqtt_publish_generic(const char *_topic, const char *payload) {
    char topic[32];
    strcpy( topic, _mqtt_dev_name /*MQTT_DEVICE*/ );
    //strcpy( topic + strlen( topic ), _topic);
    strcat( topic, _topic);
    esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 1);
}

void mqtt_publish_all_task(void *arg){
    uint32_t delay_ms = _mqtt_cfg.send_interval*1000;
    while (1) {

        mqtt_publish_device_uptime();
        mqtt_publish_device_freemem();
        mqtt_publish_device_rssi();

        mqtt_publish_custom_registered_topics();

        vTaskDelay( delay_ms / portTICK_RATE_MS);
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

void mqtt_set_device_name(const char *dev_name){
    memset(_mqtt_dev_name, 0, MQTT_DEICE_NAME_LENGTH);
    strcpy(_mqtt_dev_name, _mqtt_cfg.base_topic);
    //strcat(_mqtt_dev_name, "/");
    strcat(_mqtt_dev_name, dev_name);
    strcat(_mqtt_dev_name, "/");
}

void mqtt_publish(const char *_topic, const char *payload)
{
    mqtt_publish_generic( _topic, payload);
}

void mqtt_add_periodic_publish_callback( const char *topic, func_mqtt_send_cb fn_cb)
{
    uint8_t found = 0;
    uint8_t i = 0;
    for ( i = 0; i < MQTT_SEND_CB; i++) 
    {
        if (strcmp(mqtt_send[i].topic, topic) == 0 && mqtt_send[i].fn_cb == fn_cb) 
        {
            found = 1;
            break;
        }
    }

    if ( !found ) {
        for ( i = 0; i < MQTT_SEND_CB; i++) 
        {
            if ( strlen(mqtt_send[i].topic) ==  0) {
                strcpy(mqtt_send[i].topic, topic);
                mqtt_send[i].fn_cb = fn_cb;
                break;
            }
        }       
    }
}

void mqtt_add_receive_callback( const char *topic, func_mqtt_recv_cb fn_cb)
{
    uint8_t found = 0;
    uint8_t i = 0;
    for ( i = 0; i < MQTT_RECV_CB; i++) 
    {
        if (strcmp(mqtt_recv[i].topic, topic) == 0 && mqtt_recv[i].fn_cb == fn_cb) 
        {
            found = 1;
            break;
        }
    }

    if ( !found ) {
        for ( i = 0; i < MQTT_RECV_CB; i++) 
        {
            if ( strlen(mqtt_recv[i].topic) ==  0) {
                strcpy(mqtt_recv[i].topic, topic);
                mqtt_recv[i].fn_cb = fn_cb;
                break;
            }
        }       
    }
}


static void mqtt_subscribe_topics(esp_mqtt_client_handle_t client)
{
    char topic[32];
    strcpy(topic, _mqtt_dev_name ); /*MQTT_DEVICE*/
    //strcpy(topic+strlen(topic), "#");
    strcat(topic, "#");
    //snprintf(topic, 32, MQTT_TOPIC_SUBSCRIBE, "test", "esp");
    esp_mqtt_client_subscribe(client, topic, 0);
}

static void mqtt_publish_custom_registered_topics()
{
    char payload[10];
    for ( uint8_t i = 0; i < MQTT_SEND_CB; i++ ) {
        if ( strlen(mqtt_send[i].topic) > 0 ) {
            memset(payload, 0, 10);
            mqtt_send[i].fn_cb(payload);
            mqtt_publish_generic( mqtt_send[i].topic, payload); 
        }
    }
}
