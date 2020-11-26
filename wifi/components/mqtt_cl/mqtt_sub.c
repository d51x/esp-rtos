#include "mqtt_sub.h"
#include "string.h"

static const char *TAG = "MQTTSUBS";

static uint8_t base_topics_count = 0;
static uint8_t end_points_count = 0;

static mqtt_sub_endpoint_t *end_points = NULL;
static mqtt_sub_base_topic_t *base_topics = NULL;
static mqtt_sub_endpoint_value_t *endpoint_values = NULL;

#define MQTT_SUBS_NVS_SECTION "mqttsubs"
#define MQTT_SUBS_NVS_KEY_BASE_COUNT "basecnt"
#define MQTT_SUBS_NVS_KEY_BASE_DATA  "basedata"

#define MQTT_SUBS_NVS_KEY_ENDPOINT_COUNT "endpntcnt"
#define MQTT_SUBS_NVS_KEY_ENDPOINT_DATA  "endpntdata"


static void debug_print_endpoints()
{
    ESP_LOGI(TAG, "base topic count %d", base_topics_count);
    ESP_LOGI(TAG, "end points count %d", end_points_count);

    if ( base_topics_count > 0 )
    {
        for ( uint8_t i = 0; i < base_topics_count; i++)
        {
            ESP_LOGI(TAG, "base topic %d: %s", i, base_topics[i].base);

            if ( end_points_count > 0 )
            {
                for ( uint8_t k = 0; k < end_points_count; k++)
                {
                    if ( end_points[k].base_id == i ) 
                    {
                        ESP_LOGI(TAG, "\t\t endpoint (%02d): %s", i, end_points[k].endpoint);
                    }
                }
            }
        }
    }    
}

void mqtt_subscriber_clear_all()
{
    nvs_param_erase(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_COUNT);
    nvs_param_erase(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_DATA);
    nvs_param_erase(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_COUNT);
    nvs_param_erase(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_DATA);

    base_topics_count = 0;
    end_points_count = 0;

    if ( base_topics ) free( base_topics );
    if ( end_points ) free( end_points );
    if ( endpoint_values ) free( endpoint_values );
}
static void mqtt_subscriber_load_nvs()
{
    esp_err_t err = nvs_param_u8_load(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_COUNT, &base_topics_count);
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s %s", __func__, esp_err_to_name(err), MQTT_SUBS_NVS_KEY_BASE_COUNT);
        base_topics_count = 0;
        end_points_count = 0;
        return;
    }    

    base_topics = (mqtt_sub_base_topic_t *) calloc(base_topics_count, sizeof(mqtt_sub_base_topic_t));
    ////f_mqtt_sub_base_topic_t *t = (f_mqtt_sub_base_topic_t *) calloc(base_topics_count, sizeof(f_mqtt_sub_base_topic_t));

    err = nvs_param_load(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_DATA, base_topics);
    ////err = nvs_param_load(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_DATA, t);
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s %s", __func__, esp_err_to_name(err), MQTT_SUBS_NVS_KEY_BASE_DATA);
        memset(base_topics, 0, base_topics_count * sizeof(mqtt_sub_base_topic_t));
        ////free(t);
        return;
    }     

    ////for ( uint8_t i = 0; i < base_topics_count; i++)
    ////{
    ////     base_topics[i].base = strdup(t[i].base);
    ////     base_topics[i].id = t[i].id;
        
        ////ESP_LOGI(TAG, "base topic %d: %s", i, base_topics[i].base);

    ////}
    ////free(t);

    //ESP_LOGI(TAG, "try load end_points_count");
    err = nvs_param_u8_load(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_COUNT, &end_points_count);
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s %s", __func__, esp_err_to_name(err), MQTT_SUBS_NVS_KEY_ENDPOINT_COUNT);
        end_points_count = 0;
        return;
    } 

    //ESP_LOGI(TAG, "end_points_count = %d", end_points_count);
    end_points = (mqtt_sub_endpoint_t *) calloc(end_points_count, sizeof(mqtt_sub_endpoint_t));
    endpoint_values = (mqtt_sub_endpoint_value_t *) calloc(end_points_count, sizeof(mqtt_sub_endpoint_value_t));
    memset(endpoint_values, 0, end_points_count * sizeof(mqtt_sub_endpoint_value_t));
    ////f_mqtt_sub_endpoint_t *e = (f_mqtt_sub_endpoint_t *) calloc(end_points_count, sizeof(f_mqtt_sub_endpoint_t));
    err = nvs_param_load(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_DATA, end_points);
    ////err = nvs_param_load(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_DATA, e);
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s %s", __func__, esp_err_to_name(err), MQTT_SUBS_NVS_KEY_ENDPOINT_DATA);
        memset(end_points, 0, end_points_count * sizeof(mqtt_sub_endpoint_t));
        ////free(e);
        return;
    }    
    ////for (uint8_t i = 0; i < end_points_count; i++)
    ////{
    ////    end_points[i].endpoint = strdup(e[i].endpoint);
    ////    end_points[i].id = e[i].id;
    ////    end_points[i].base_id = e[i].base_id;
    ////    //ESP_LOGI(TAG, "base topic %d: %s, endpoint %d: %s", end_points[i].base_id, base_topics[end_points[i].base_id].base, i, end_points[i].endpoint);
    ////}
    ////free(e);
}

static void mqtt_subscriber_save_nvs_base_topics()
{
    esp_err_t err = nvs_param_u8_save(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_COUNT, base_topics_count);
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s", __func__, esp_err_to_name(err));
    }

    ////f_mqtt_sub_base_topic_t *t = (f_mqtt_sub_base_topic_t *) calloc(base_topics_count, sizeof(f_mqtt_sub_base_topic_t));
    ////for (uint8_t i = 0; i < base_topics_count; i++)
    ////{
    ////    strcpy(t[i].base, base_topics[i].base);
    ////    t[i].id = base_topics[i].id;
    ////    //ESP_LOGW(TAG, "%s: %s", __func__, t[i].base);
    ////}

    err = nvs_param_save(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_DATA, base_topics, base_topics_count*sizeof(mqtt_sub_base_topic_t));
    ////err = nvs_param_save(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_BASE_DATA, t, base_topics_count*sizeof(f_mqtt_sub_base_topic_t));
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s", __func__, esp_err_to_name(err));
    }
    ////free(t);
}

static void mqtt_subscriber_save_nvs_end_points()
{
    esp_err_t err = nvs_param_u8_save(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_COUNT, end_points_count);
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s", __func__, esp_err_to_name(err));
    }

    ////f_mqtt_sub_endpoint_t *t = (f_mqtt_sub_endpoint_t *) calloc(end_points_count, sizeof(f_mqtt_sub_endpoint_t));
    ////for (uint8_t i = 0; i < end_points_count; i++)
    ////{
    ////    strcpy(t[i].endpoint, end_points[i].endpoint);
    ////    t[i].id = end_points[i].id;
    ////    t[i].base_id = end_points[i].base_id;
    ////}

    err = nvs_param_save(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_DATA, end_points, end_points_count*sizeof(mqtt_sub_endpoint_t));
    ////err = nvs_param_save(MQTT_SUBS_NVS_SECTION, MQTT_SUBS_NVS_KEY_ENDPOINT_DATA, t, end_points_count*sizeof(f_mqtt_sub_endpoint_t));
    if ( err != ESP_OK ) {
        ESP_LOGE(TAG, "%s: %s", __func__, esp_err_to_name(err));
    }
    ////free(t);
}

static int mqtt_subscriber_get_endpoint_id(uint8_t base_id, char *_endpoint)
{
    //ESP_LOGI(TAG, "%s: base id: %d, endpoint: %s", __func__, base_id, _endpoint);

    for (uint8_t i = 0; i < end_points_count; i++)
    {
        //ESP_LOGI(TAG, "%s: endpoint (%d): %s, base_id: %d", __func__, i, end_points[i].endpoint, end_points[i].base_id);

        if ( end_points[i].base_id == base_id && strcmp( end_points[i].endpoint, _endpoint) == 0) {
            //found = true;
            //break;
            //ESP_LOGI(TAG, "%s: endpoint %s found", __func__, _endpoint);
            return i;
        }
    }

    // не нашли
    //ESP_LOGI(TAG, "%s: endpoint not found", __func__);
    return -1;
}

static int mqtt_subscriber_get_endpoint_id_by_topic(const char *topic)
{
    int res = -1;
    char *t = calloc(1, MQTT_SUBSCRIBER_BASE_TOPIC_MAX_LENGTH + MQTT_SUBSCRIBER_END_POINT_MAX_LENGTH + 1 ); 
    for (uint8_t i = 0; i < end_points_count; i++)
    {
        memset(t, 0, MQTT_SUBSCRIBER_BASE_TOPIC_MAX_LENGTH + MQTT_SUBSCRIBER_END_POINT_MAX_LENGTH + 1 );
        strcpy(t, base_topics[ end_points[i].base_id ].base);
        strcat(t + strlen(t), "/");
        strcat(t + strlen(t), end_points[i].endpoint);

        if ( strcmp(topic, t) == 0) {
            res = i;  // нашли endpoint id
            break;
        }
    }
    free(t);
    return res;
}

static void mqtt_subscriber_del_endpoints(uint8_t base_id)
{
    //ESP_LOGI(TAG, __func__ );

    int8_t _endpoints_count = 0;
    for (uint8_t i = 0; i < end_points_count; i++)
    {
       if ( end_points[i].base_id == base_id)  _endpoints_count++;
    }
    ESP_LOGI(TAG, "%s: _endpoints_count = %d", __func__, _endpoints_count);

    if ( _endpoints_count > 0) 
    {
        mqtt_sub_endpoint_t *_end_points = (mqtt_sub_endpoint_t *)calloc( end_points_count - _endpoints_count, sizeof(mqtt_sub_endpoint_t));
        mqtt_sub_endpoint_value_t *_end_point_values = (mqtt_sub_endpoint_value_t *)calloc( end_points_count - _endpoints_count, sizeof(mqtt_sub_endpoint_value_t));

        uint8_t k = 0;
        for (uint8_t i = 0; i < end_points_count; i++)
        {
            if ( end_points[i].base_id != base_id ) 
            {
                _end_points[k].base_id = end_points[i].base_id;
                _end_points[k].id = k;

                ////_end_points[k].endpoint = strdup(end_points[i].endpoint);
                strcpy(_end_points[k].endpoint, end_points[i].endpoint);
                
                _end_point_values[k].id = k;
                strcpy(_end_point_values[k].value, endpoint_values[i].value);
                k++;
            }

            // ?????
            ////free(end_points[i].endpoint);
        }

        end_points_count -= _endpoints_count; 
        end_points = (mqtt_sub_endpoint_t *)realloc( end_points, end_points_count * sizeof(mqtt_sub_endpoint_t)); 
        endpoint_values = (mqtt_sub_endpoint_value_t *)realloc( endpoint_values, end_points_count * sizeof(mqtt_sub_endpoint_value_t)); 

        for (uint8_t i = 0; i < end_points_count; i++)
        {
            end_points[i].base_id = _end_points[i].base_id;
            end_points[i].id = i;
            ////end_points[i].endpoint = strdup(_end_points[i].endpoint);  
            strcpy(end_points[i].endpoint, _end_points[i].endpoint);
            // ?????
            ////free(_end_points[i].endpoint);   
            endpoint_values[i].id = i;
            strcpy(endpoint_values[i].value, _end_point_values[i].value);
        }       

        free(_end_points);
        free(_end_point_values);
    }
    
}

static void mqtt_subscriber_del_base_topic(uint8_t base_id)
{
    mqtt_sub_base_topic_t *_base_topics = (mqtt_sub_base_topic_t *) calloc( base_topics_count - 1, sizeof(mqtt_sub_base_topic_t) );
    uint8_t k = 0;
    for (uint8_t i = 0; i < base_topics_count; i++)
    {
        if ( base_topics[i].id != base_id )
        {
            _base_topics[k].id = base_topics[i].id;
            ////_base_topics[k].base = strdup( base_topics[i].base );
            strcpy(_base_topics[k].base,base_topics[i].base );
            ////free(base_topics[i].base);
            k++;
        }
    }

    base_topics_count -= 1;
    base_topics = (mqtt_sub_base_topic_t *) realloc( base_topics, base_topics_count * sizeof(mqtt_sub_base_topic_t) );

    for (uint8_t i = 0; i < base_topics_count; i++)
    {
        base_topics[i].id = _base_topics[i].id;
        //base_topics[i].base = strdup( _base_topics[i].base );
        strcpy(base_topics[i].base, _base_topics[i].base );
        ////free(_base_topics[i].base);
    }
    free( _base_topics );
}

// добавляет новый endpoint для указанного base topic id
// если endpoint уже есть для указанного base topic id, то ничего не делает
static esp_err_t mqtt_subscriber_add_endpoints(uint8_t base_id, char *_endpoints)
{
    //ESP_LOGI(TAG, "%s: try add base id: %d, endpoints: %s", __func__, base_id, _endpoints);
    esp_err_t err = ESP_OK;

    char *s = strdup(_endpoints);
    char *e = malloc(1); //cut_str_from_str(_endpoints, ";");
    //ESP_LOGI(TAG, "%s: fist endpoint: %s", __func__, e);
    
    // удалить все endpoints для base_id
    mqtt_subscriber_del_endpoints(base_id);

    //ESP_LOGI(TAG, "%s: endpoints deleted", __func__ );
    while ( e != NULL )
    {
        e = cut_str_from_str(s, ";");

        
        if ( e == NULL ) break;
        //ESP_LOGI(TAG, "%s: endpoint: %s, other endpoints: %s", __func__, e, s);
        // ищем endpoint в endpoints
        //if ( mqtt_subscriber_get_endpoint_id(base_id, e) > -1 ) {
        //    // endpoint уже присутствует
        //    ESP_LOGW(TAG, "New endpoint \"%s\" already exists for base topic \"%s\"", e, base_topics[ base_id ].base);
        //    continue;
        //} else {
            // добавляем новый endpoint 
            if ( end_points_count < MQTT_SUBSCRIBER_MAX_END_POINTS ) 
            {
                end_points_count++;
                
                end_points = (mqtt_sub_endpoint_t *) realloc(end_points, end_points_count * sizeof(mqtt_sub_endpoint_t));
                endpoint_values = (mqtt_sub_endpoint_value_t *) realloc(endpoint_values, end_points_count * sizeof(mqtt_sub_endpoint_value_t));

                end_points[ end_points_count - 1 ].id = end_points_count - 1;
                end_points[ end_points_count - 1 ].base_id = base_id;
                ////end_points[ end_points_count - 1 ].endpoint = strdup( e );
                strcpy(end_points[ end_points_count - 1 ].endpoint, e );
                //ESP_LOGI(TAG, "New endpoint \"%s\" added for base topic \"%s\"", e, base_topics[ base_id ].base);
                endpoint_values[ end_points_count - 1 ].id = end_points_count - 1;
                strcpy(endpoint_values[ end_points_count - 1 ].value, "");
                
            } else {
                ESP_LOGE(TAG, "Not slots (%d) available for new endpoint %s", MQTT_SUBSCRIBER_MAX_END_POINTS, e);
                err = ESP_FAIL;
                break;
            }
        //}
    }
    free(e);
    free(s);
    return err;
}

static int mqtt_subscriber_get_base_topic_id(const char *base)
{
    for ( uint8_t i = 0; i < base_topics_count; i++)
    {
        if ( strcmp(base, base_topics[i].base) == 0) {
            return i;  // нашли base topic
        }
    }

    // не нашли
    return -1;
}

// добавляет список endpoints в указанный base topic
// 
esp_err_t mqtt_subscriber_add(const char* base_topic, const char *_endpoints)
{
    //ESP_LOGI(TAG, "%s: try add %s: %s", __func__, base_topic, _endpoints);

    int base_id = mqtt_subscriber_get_base_topic_id(base_topic);

    //ESP_LOGI(TAG, "%s: base id: %d", __func__, base_id);

    if ( base_id == -1 )
    {
        // можно добавлять новый топик
        if  ( base_topics_count < MQTT_SUBSCRIBER_MAX_BASE_TOPICS )
        { 
            base_topics_count++;
            base_id = base_topics_count-1;

            // увеличим массив
            base_topics = (mqtt_sub_base_topic_t *) realloc(base_topics, base_topics_count * sizeof(mqtt_sub_base_topic_t));
            base_topics[ base_id ].id = base_id;
            //base_topics[ base_id ].base = strdup( base_topic );
            strcpy(base_topics[ base_id ].base, base_topic );
            //ESP_LOGI(TAG, "New base topic \"%s\" added ", base_topics[ base_id ].base);  

            //mqtt_subscriber_save_nvs_base_topics();

        } else {
            ESP_LOGE(TAG, "Not slots (%d) available for new base topic %s", MQTT_SUBSCRIBER_MAX_BASE_TOPICS,  base_topic);
            return ESP_FAIL;           
        }
    } 

    // продолжаем, если base_id > -1
    esp_err_t err = mqtt_subscriber_add_endpoints( base_id, _endpoints);
    // if ( err == ESP_OK )
    //     mqtt_subscriber_save_nvs_end_points();
    return err;
}


static void rebase_endpoint_ids()
{
    for ( uint8_t i = 0; i < base_topics_count; i++ )
    {
        for ( uint8_t k = 0; k < end_points_count; k++)
        {
            if ( end_points[k].base_id == base_topics[i].id )
                {
//                    ESP_LOGI(TAG, "%s:  %s --> %s | old base id: %d | new base id: %d", __func__, base_topics[i].base, end_points[k].endpoint, end_points[k].base_id, i);
                    end_points[k].base_id = i; // ????? не будет ли пересечений
                }
        }
    }

}

static void rebase_base_topic_ids()
{
    //ESP_LOGW(TAG, __func__ );

    //debug_print_endpoints();

    for ( uint8_t i = 0; i < base_topics_count; i++ )
    {
        //ESP_LOGI(TAG, "%s:  %s | old base id: %d | new base id: %d", __func__, base_topics[i].base, base_topics[i].id, i);
        base_topics[i].id = i;
    }    
    //ESP_LOGW(TAG, "=================================" );
}


// удаляет base topic и все его endpoints
esp_err_t mqtt_subscriber_del(const char* base_topic)
{
    int base_id = mqtt_subscriber_get_base_topic_id(base_topic);

    if ( base_id == -1 )
    {
        ESP_LOGE(TAG, "Unable find base topic \"%s\"", base_topic);
        return ESP_FAIL;
    }

    // удалим все endpoints найденного basetopic
    mqtt_subscriber_del_endpoints(base_id);
    
    // теперь удалим сам base_topic
    mqtt_subscriber_del_base_topic(base_id);


    // переназначить base_id для endpoints и id для base
    rebase_endpoint_ids();
    rebase_base_topic_ids();

    //mqtt_subscriber_save_nvs_base_topics();
    //mqtt_subscriber_save_nvs_end_points();

    return ESP_OK;
}

static void mqtt_subscriber_receive_cb(char *buf, void *args)
{
    // в args положить topic
    char *topic = (char *)args;
//    ESP_LOGI(TAG, "received topic %s with data: %s", topic, buf);
    //free(topic);
    // ищем endpoint id
    int endpoint_id = mqtt_subscriber_get_endpoint_id_by_topic(topic);
    if ( endpoint_id > -1 )
    {
        strcpy(endpoint_values[ endpoint_id ].value, buf);

        ESP_LOGI(TAG, "received topic %s with data: %s", topic, endpoint_values[ endpoint_id ].value);
    }
}

void mqtt_subscriber_init()
{
    //mqtt_subscriber_clear_all();
    ESP_LOGI(TAG, __func__ );

    mqtt_subscriber_load_nvs();
    debug_print_endpoints();

    if (base_topics_count == 0 && end_points_count == 0)
    {
        mqtt_subscriber_add("dacha/bathroom", "dhtt1;dhth1");
        mqtt_subscriber_add("dacha/bedroom", "dhtt1;dhth1");
        mqtt_subscriber_add("dacha/livingroom1", "dhtt1;dhth1");
        mqtt_subscriber_add("dacha/livingroom2", "dhtt1;dhth1");
    }
    

    mqtt_subscriber_save_nvs_base_topics();
    mqtt_subscriber_save_nvs_end_points();

    char *t = calloc(1, MQTT_SUBSCRIBER_BASE_TOPIC_MAX_LENGTH + MQTT_SUBSCRIBER_END_POINT_MAX_LENGTH + 1 );
    for (uint8_t i = 0; i < end_points_count; i++)
    {
        strcpy(t, base_topics[ end_points[i].base_id ].base);
        strcat(t + strlen(t), "/");
        strcat(t + strlen(t), end_points[i].endpoint);
        //ESP_LOGI(TAG, "Subscribe to: %s", t);
        mqtt_add_receive_callback( t, 0, mqtt_subscriber_receive_cb, NULL);
    }
    
    

    //debug_print_endpoints();
    //mqtt_subscriber_clear_all();
    //debug_print_endpoints();
}
