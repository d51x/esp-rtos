#include "mcp23017_mqtt.h"


static const char* TAG = "MCP23017";

mcp23017_handle_t _dev_h;

//mqtt_add_periodic_publish_callback( const char *topic, func_mqtt_send_cb fn_cb);
void mcp23017_mqtt_periodic_send_cb(char *buf, void *args)
{
    // для отправки в buf положить значение пина
    //mcp23017_mqtt_t *p = (mcp23017_mqtt_t *)args;
    uint8_t pin = (uint8_t)args;
    //ESP_LOGI(TAG, "%s, pin %d", __func__, p->pin);
    //ESP_LOGI(TAG, "%s, pin %d", __func__, pin);
    uint8_t value = 0;
    //mcp23017_read_pin( p->dev_h, p->pin, &value);
    mcp23017_read_pin( _dev_h, pin, &value);
    value = (value != 0);
    itoa(value, buf, 10);
}

// void mqtt_add_receive_callback( const char *topic, func_mqtt_recv_cb fn_cb); 
void mcp23017_mqtt_recv_cb(char *buf, void *args)
{
    
    //mcp23017_mqtt_t *p = (mcp23017_mqtt_t *)args;
    uint8_t pin = (uint8_t)args;
    //ESP_LOGI(TAG, "%s, pin %d", __func__, p->pin);
    

// проверка на предыдущее значение
    static uint8_t prev = 0;
    uint8_t value = atoi( buf );

    ESP_LOGI(TAG, "%s, pin %d, val %d", __func__, pin, value);
    //if ( prev == value) return;
    
    //if ( mcp23017_write_pin(p->dev_h, p->pin, value) == ESP_OK )  
    if ( mcp23017_write_pin(_dev_h, pin, value) == ESP_OK )  
        prev = value;  
}

void mcp23017_mqtt_recv_queue_cb(void *arg)
{
    const portTickType xTicksToWait = 100 / portTICK_RATE_MS;
    uint8_t data[2];

    mcp23017_handle_t mqtt_h = (mcp23017_handle_t)arg;

    for( ;; )
    {
        if ( xQueueReceive( mcp23017_status_queue, &data, xTicksToWait ) == pdPASS )
        {
            char t[20], v[2];
            sprintf( t, "%s%d", MCP23017_MQTT_SEND_TOPIC, data[0]);
            itoa(data[1], v, 10);
            //ESP_LOGE(TAG, "recv from queue %s = %s", t, v);
            mqtt_publish(t, v);
        }  
    }
}

void mcp23017_mqtt_init(mcp23017_handle_t dev_h)
{
    xTaskCreate(mcp23017_mqtt_recv_queue_cb, "mcp23017_recv", 1024, dev_h, 10, NULL);

    _dev_h = dev_h;

    for ( uint8_t i = 0; i < 16; i++)
    {
        char t[20];
        
        mcp23017_mqtt_t *p = (mcp23017_mqtt_t *) calloc(1, sizeof(mcp23017_mqtt_t));
        p->dev_h = dev_h;
        p->pin = i;

        sprintf(t, "%s%d", MCP23017_MQTT_SEND_TOPIC, i);
        mqtt_add_periodic_publish_callback( t, mcp23017_mqtt_periodic_send_cb, i);

        sprintf(t, "%s%d", MCP23017_MQTT_RECV_TOPIC, i);
        mqtt_add_receive_callback(t, mcp23017_mqtt_recv_cb, i);
        free(p);
    }    
}
