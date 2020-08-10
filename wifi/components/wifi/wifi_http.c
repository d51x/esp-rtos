#include "wifi_http.h"

#define HTML_PAGE_CFG_WIFI 1

#define URI_PARAM_WIFI_HOST             "hostname"
#define URI_PARAM_WIFI_SSID             "ssid"
#define URI_PARAM_WIFI_PASSW            "pass"
#define URI_PARAM_WIFI_MODE             "wifi_mode"

const char *html_page_config_wifi ICACHE_RODATA_ATTR = 
  "<div class='group rnd'>"
    "<h4 class='brd-btm'>WiFi Settings:</h4>" 
    "<form method='GET'>" 
      "<p><label class='lf'>Hostname: </label><input size='20' name='"URI_PARAM_WIFI_HOST"' class='edit rh' value='%s' /></p>"       
      "<p><label class='lf'>SSID: </label><input size='20' name='"URI_PARAM_WIFI_SSID"' class='edit rh' value='%s' /></p>"
      "<p><label class='lf'>Password: </label><input size='20' name='"URI_PARAM_WIFI_PASSW"' class='edit rh' value='%s' /></p>"
      "<p><label class='lf'>Mode:</label>"
        "<label><input type='radio' name='"URI_PARAM_WIFI_MODE"' value='1' %s /> STA</label>" 
        "<label><input type='radio' name='"URI_PARAM_WIFI_MODE"' value='2' %s /> AP</label>"
      "</p>"
      "<p><input type='hidden' name='st' value='1'></p>" 
      "<p class='lf2'><input type='submit' value='Сохранить' class='button norm rht'></p>" 
    "</form>"
  "</div>";

static void wifi_print_options(char *data, void *args)
{
    sprintf(data + strlen(data), html_page_config_wifi 
                    , wifi_cfg->hostname         // hostname
                    , wifi_cfg->ssid         // ssid
                    , wifi_cfg->password         // pass    
                    , (wifi_cfg->mode == WIFI_MODE_STA) ? "checked" : ""          // sta checked
                    , (wifi_cfg->mode == WIFI_MODE_AP)  ? "checked" : ""         // ap checked
    );
}

void wifi_register_http_print_data() 
{
    register_print_page_block( "wifi_options", PAGES_URI[ PAGE_URI_SETUP ], 1, (func_http_print_page_block) wifi_print_options, NULL, (httpd_uri_func) wifi_http_process_params, NULL );
}

void wifi_http_process_params(httpd_req_t *req, void *args)
{
   // check params

	char param[100];
	// TODO: check for empty hostname and ssid
	if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
		if ( atoi(param) != HTML_PAGE_CFG_WIFI ) {
			return;	
		}
	}

 	if ( http_get_key_str(req, URI_PARAM_WIFI_HOST, param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_cfg->hostname, param);
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, wifi_cfg->hostname);
        
        mqtt_set_device_name(wifi_cfg->hostname);
    }

    if ( http_get_key_str(req, URI_PARAM_WIFI_SSID, param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_cfg->ssid, param);
    }

    if ( http_get_key_str(req, URI_PARAM_WIFI_PASSW, param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_cfg->password, param);
    }

    wifi_cfg->mode = WIFI_MODE_NULL;
    if ( http_get_key_str(req, URI_PARAM_WIFI_MODE, param, sizeof(param)) == ESP_OK ) {
        wifi_cfg->mode = atoi(param);
    }

	wifi_cfg->first = 0;
	wifi_cfg_save(wifi_cfg);
}

void wifi_http_init(httpd_handle_t _server)
{
  wifi_register_http_print_data();
}