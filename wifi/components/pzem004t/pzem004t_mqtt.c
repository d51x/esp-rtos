#include "pzem004t_mqtt.h"

#ifdef CONFIG_SENSOR_PZEM004_T

#define VOLTAGE_MQTT_TOPIC_PARAM	"pmv"
#define CURRENT_MQTT_TOPIC_PARAM	"pmc"
#define POWER_MQTT_TOPIC_PARAM		"pmw"
#define ENERGY_MQTT_TOPIC_PARAM		"pmwh"

static void pzem_mqtt_send_voltage(char *payload, void *args)
{
    pzem_data_t pzem_data = pzem_get_data();
    sprintf(payload, "%0.1f", pzem_data.voltage);
}

static void pzem_mqtt_send_current(char *payload, void *args)
{
    pzem_data_t pzem_data = pzem_get_data();
    sprintf(payload, "%0.1f", pzem_data.current);
}

static void pzem_mqtt_send_power(char *payload, void *args)
{
    pzem_data_t pzem_data = pzem_get_data();
    sprintf(payload, "%0.1f", pzem_data.power);
}

static void pzem_mqtt_send_energy(char *payload, void *args)
{
    pzem_data_t pzem_data = pzem_get_data();
    sprintf(payload, "%0.1f", pzem_data.energy);
}

void pzem_mqtt_init()
{
    mqtt_add_periodic_publish_callback( VOLTAGE_MQTT_TOPIC_PARAM, pzem_mqtt_send_voltage, NULL );
    mqtt_add_periodic_publish_callback( CURRENT_MQTT_TOPIC_PARAM, pzem_mqtt_send_current, NULL  );    
    mqtt_add_periodic_publish_callback( POWER_MQTT_TOPIC_PARAM, pzem_mqtt_send_power, NULL  );    
    mqtt_add_periodic_publish_callback( ENERGY_MQTT_TOPIC_PARAM, pzem_mqtt_send_energy, NULL  );    
}
#endif