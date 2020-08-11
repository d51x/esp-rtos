#include "ledcontrol_mqtt.h"


static const char* TAG = "LEDCMQTT";

ledcontrol_handle_t _dev_h;

void ledcontrol_mqtt_periodic_send_cb(char *buf, void *args)
{
    // для отправки в buf положить значение пина
    ledcontrol_mqtt_t *p = (ledcontrol_mqtt_t *)args;
    ledcontrol_handle_t dev_h = p->dev_h;
    ledcontrol_t *ledc = (ledcontrol_t *)dev_h;
    ledcontrol_channel_t *ch = ledc->channels + p->channel;

    uint8_t value = 0;
    value = ledc->get_duty( ch );
    itoa(value, buf, 10);
}

void ledcontrol_mqtt_recv_cb(char *buf, void *args)
{
    ledcontrol_mqtt_t *p = (ledcontrol_mqtt_t *)args;
    uint8_t value = atoi( buf );

    #ifdef CONFIG_MQTT_TOPIC_SEND_RECV
    if ( p->prev != value )
    {
    #endif
        
    esp_err_t err =  ESP_FAIL;
    if ( value >= 0 && value <= MAX_DUTY ) 
    {
        ledcontrol_handle_t dev_h = p->dev_h;
        ledcontrol_t *ledc = (ledcontrol_t *)dev_h;
        ledcontrol_channel_t *ch = ledc->channels + p->channel;
        err = ledc->set_duty( ch, value );
        ledc->update();
    }  

    #ifdef CONFIG_MQTT_TOPIC_SEND_RECV    
        if ( err == ESP_OK )
            p->prev = value;       
    }
    #endif 
}

void ledcontrol_mqtt_init(ledcontrol_handle_t dev_h)
{
    ledcontrol_t *ledc = (ledcontrol_t *)dev_h;

    for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {   
        ledcontrol_mqtt_t *p = (ledcontrol_mqtt_t *) calloc(1, sizeof(ledcontrol_mqtt_t));
        p->dev_h = dev_h;
        p->channel = i;

        char t[20];
        sprintf(t, "%s%d", LEDCONTROL_MQTT_SEND_TOPIC, i);
        mqtt_add_periodic_publish_callback( t, ledcontrol_mqtt_periodic_send_cb, (ledcontrol_mqtt_t *)p); 

        sprintf(t, "%s%d", LEDCONTROL_MQTT_RECV_TOPIC, i);
        mqtt_add_receive_callback(t, ledcontrol_mqtt_recv_cb, (ledcontrol_mqtt_t *)p);                   
    }
}
