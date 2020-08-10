#include "mqtt_cl_http.h"

#define HTML_PAGE_CFG_MQTT 2

#define URI_PARAM_MQTT_EN               "mqtt_en"
#define URI_PARAM_MQTT_HOST             "mqtt_host"
#define URI_PARAM_MQTT_LOGIN            "mqtt_login"
#define URI_PARAM_MQTT_PASSW            "mqtt_passw"
#define URI_PARAM_MQTT_TOPIC_BASE       "mqtt_base"
#define URI_PARAM_MQTT_SEND_INTERVAL    "mqtt_sint"

const char *html_page_config_mqtt ICACHE_RODATA_ATTR = "<div class='group rnd'>"
                                    "<h4 class='brd-btm'>MQTT Settings:</h4>"
                                    "<form method='GET'>"
                                        "<p><label class='lf'>Enabled: </label><input type='checkbox' name='"URI_PARAM_MQTT_EN"' %s /></p>"
                                        "<p><label class='lf'>Hostname: </label><input size='20' name='"URI_PARAM_MQTT_HOST"' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Login: </label><input size='20' name='"URI_PARAM_MQTT_LOGIN"' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Password: </label><input size='20' name='"URI_PARAM_MQTT_PASSW"' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Base topic (/): </label><input size='20' name='"URI_PARAM_MQTT_TOPIC_BASE"' class='edit rh' value='%s' /></p>"
                                        "<p><label class='lf'>Send interval: </label><input size='20' name='"URI_PARAM_MQTT_SEND_INTERVAL"' class='edit rh' value='%d' /></p>"
                                        "<p><input type='hidden' name='st' value='2'></p>"  // HTML_PAGE_CFG_MQTT = 2
                                        "<p class='lf2'><input type='submit' value='Сохранить' class='button norm rht'></p>"
                                    "</form>"
                                    "</div>";  


static void mqtt_print_options(char *data, void *args)
{

    mqtt_config_t *mqtt_cfg = malloc(sizeof(mqtt_config_t));
    mqtt_get_cfg(mqtt_cfg);
    sprintf(data + strlen(data), html_page_config_mqtt 
                       , mqtt_cfg->enabled ? "checked" : ""         // enabled
                       , mqtt_cfg->broker_url         // host
                       , mqtt_cfg->login         // login    
                       , mqtt_cfg->password         // password    
                       , mqtt_cfg->base_topic         // base topic    
                       , mqtt_cfg->send_interval          // send interval
                       );
    free(mqtt_cfg);

}

void mqtt_register_http_print_data() 
{
    register_print_page_block( "mqtt_options", PAGES_URI[ PAGE_URI_SETUP ], 2, mqtt_print_options, NULL, mqtt_http_process_params, NULL );
}

void mqtt_http_process_params(httpd_req_t *req, void *args)
{
   // check params

	if ( http_get_has_params(req) == ESP_OK) 
	{

        char param[100];
        mqtt_config_t *mqtt_cfg = malloc(sizeof(mqtt_config_t));
        // TODO: check for empty hostname and ssid
        if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
            if ( atoi(param) != HTML_PAGE_CFG_MQTT ) {
                return;	
            }
        }

        if ( http_get_key_str(req, URI_PARAM_MQTT_EN, param, sizeof(param)) == ESP_OK ) {
            mqtt_cfg->enabled = 1;
        } else {
            mqtt_cfg->enabled = 0;
        }

        if ( http_get_key_str(req, URI_PARAM_MQTT_HOST, param, sizeof(param)) == ESP_OK ) {
            url_decode(param, mqtt_cfg->broker_url);
        }


        if ( http_get_key_str(req, URI_PARAM_MQTT_LOGIN,  param, sizeof( param )) == ESP_OK ) {
            strcpy(mqtt_cfg->login, param);
        } 

        if ( http_get_key_str(req, URI_PARAM_MQTT_PASSW,  param, sizeof( param )) == ESP_OK ) {
            strcpy(mqtt_cfg->password, param);
        }

        if ( http_get_key_str(req, URI_PARAM_MQTT_TOPIC_BASE,  param, sizeof( param )) == ESP_OK ) {
            url_decode(param, mqtt_cfg->base_topic);
        }

        if ( http_get_key_str(req, URI_PARAM_MQTT_SEND_INTERVAL,  param, sizeof( param )) == ESP_OK ) {
            mqtt_cfg->send_interval = atoi(param);
        }

        
        mqtt_save_cfg(mqtt_cfg);
        free(mqtt_cfg);

        mqtt_restart_task();
    }
        
}

void mqtt_http_init(httpd_handle_t _server)
{
    mqtt_register_http_print_data();
}