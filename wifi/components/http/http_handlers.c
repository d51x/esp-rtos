
#include "http_handlers.h"
#include "http_page.h"
#include "httpd.h"

/*
pages:

  menu: 
      main - main page
      setup - auth, wifi type and auth for sta 
*/


static const char *TAG = "HTTPH";
static void process_wifi_param(httpd_req_t *req);

static void process_mqtt_param(httpd_req_t *req);

const char *PAGES_URI[PAGE_URI_MAX] = { 
    HTTP_URI_ROOT,                    // PAGE_URI_ROOT
    HTTP_URI_SETUP,               // PAGE_URI_SETUP
    HTTP_URI_DEBUG,               // PAGE_URI_DEBUG
    HTTP_URI_TOOLS,               // PAGE_URI_TOOLS
    HTTP_URI_UPDATE,              // PAGE_URI_OTA
    HTTP_URI_REBOOT,              // PAGE_URI_REBOOT
    "/main.css",            // PAGE_URI_CSS
    "/ajax.js",             // PAGE_URI_AJAX
    "/favicon.ico",         // PAGE_URI_FAVICO
    HTTP_URI_ICON_MENU,            // PAGE_URI_ICON_MENU
    HTTP_URI_ICON_MENU2            // PAGE_URI_ICON_MENU2
    };



user_ctx_t PAGES_HANDLER[PAGE_URI_MAX] = {
    {HTTP_STR_MAIN,    true,   show_page_main,       NULL},
    {HTTP_STR_SETUP,   true,   show_page_setup,      NULL},
    {HTTP_STR_DEBUG,   true,   show_page_debug,      NULL},
    {HTTP_STR_TOOLS,   true,   show_page_tools,      NULL},
    {HTTP_STR_UPDATE,  true,   show_page_update,     NULL},
    {HTTP_STR_REBOOT,  false,  reboot_get_handler,   NULL},
    {"",               false,  NULL,                 NULL},
    {"",               false,  NULL,                 NULL},
    {"",               false,  NULL,                 NULL},
    {"",               false,  NULL,                 NULL},
    {"",               false,  NULL,                 NULL}
};

http_menu_item_t *http_menu = NULL;

uint8_t menu_items_count = MENU_ITEM_COUNT;


void process_params(httpd_req_t *req)
{
    for (uint8_t i = 0; i < http_print_page_block_count; i++)
    {
        char *_uri;
        _uri = http_uri_clean(req);
        if ( strcmp( _uri, http_print_page_block[i].uri) == 0 && http_print_page_block[i].process_cb != NULL) {
            // do
            http_print_page_block[i].process_cb(req); 
        }
        free(_uri);
    }
}

esp_err_t main_get_handler(httpd_req_t *req) {
    

    char page[PAGE_MAIN_BUFFER_SIZE];  
    // const char* resp_str = (const char*) req->user_ctx;
    //get_main_page_data(page);
    strncpy(page, HTTP_STR_MAIN, PAGE_MAIN_BUFFER_SIZE);
    //show_page_main( page );
    show_http_page( req, page);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));

    #ifdef CONFIG_COMPONENT_DEBUG
        print_task_stack_depth(TAG, "httpd task");
    #endif

    return ESP_OK;
}

esp_err_t setup_get_handler(httpd_req_t *req){
  char page[PAGE_DEFAULT_BUFFER_SIZE];  

  // check params
	if ( http_get_has_params(req) == ESP_OK) 
	{
		process_wifi_param(req);
		//process_mqtt_param(req);
        process_params(req);

        char *path = http_uri_clean( req );
        make_redirect(req, 0, path);
        free( path );
        return ESP_OK;
	}
	  
  strncpy(page, HTTP_STR_SETUP, PAGE_DEFAULT_BUFFER_SIZE);
  //show_page_setup( page );
  show_http_page( req, page);
  httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
  httpd_resp_send(req, page, strlen(page));
  return ESP_OK;
}

void process_wifi_param(httpd_req_t *req)
{
	char param[100];
	// TODO: check for empty hostname and ssid
	if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
		if ( atoi(param) != HTML_PAGE_CFG_WIFI ) {
			return;	
		}
	}

 	if ( http_get_key_str(req, "hostname", param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_cfg->hostname, param);
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, wifi_cfg->hostname);
        
        mqtt_set_device_name(wifi_cfg->hostname);
    }

    if ( http_get_key_str(req, "ssid", param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_cfg->ssid, param);
    }

    if ( http_get_key_str(req, "pass", param, sizeof(param)) == ESP_OK ) {
        strcpy(wifi_cfg->password, param);
    }

    wifi_cfg->mode = WIFI_MODE_NULL;
    if ( http_get_key_str(req, "wifi_mode", param, sizeof(param)) == ESP_OK ) {
        wifi_cfg->mode = atoi(param);
    }

	wifi_cfg->first = 0;
	wifi_cfg_save(wifi_cfg);
}



void process_mqtt_param(httpd_req_t *req)
{

}


esp_err_t config_get_handler(httpd_req_t *req){
  char page[PAGE_DEFAULT_BUFFER_SIZE];  



	// show page
  	strncpy(page, HTTP_STR_CONFIG, PAGE_DEFAULT_BUFFER_SIZE);
  	//show_page_config( page );
    show_http_page( req, page);
  	httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
  	httpd_resp_send(req, page, strlen(page));
  	return ESP_OK;
}

esp_err_t tools_get_handler(httpd_req_t *req){

  // check params
	if ( http_get_has_params(req) == ESP_OK) 
	{
        process_params(req);
	}

  char page[PAGE_DEFAULT_BUFFER_SIZE];  
  strncpy(page, HTTP_STR_TOOLS, PAGE_DEFAULT_BUFFER_SIZE);
  //show_page_tools( page );
  show_http_page( req, page);
  httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
  httpd_resp_send(req, page, strlen(page));
  return ESP_OK;
}

esp_err_t update_get_handler(httpd_req_t *req){
  char page[PAGE_DEFAULT_BUFFER_SIZE];  
  strncpy(page, HTTP_STR_UPDATE, PAGE_DEFAULT_BUFFER_SIZE);
  //show_page_update( page );
  show_http_page( req, page);
  httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
  httpd_resp_send(req, page, strlen(page));
  return ESP_OK;
}

esp_err_t update_post_handler(httpd_req_t *req){
    
    char err_text[400];
    char page[PAGE_DEFAULT_BUFFER_SIZE];
    uint32_t start_time = millis(); 

    if ( ota_task_upgrade_from_web(req, err_text) == ESP_OK ) {
        // upgrading is OK, restart esp and redirect to main page in 10
        char header[40] = "";
        set_redirect_header(10, "/", header);
        strcpy(page, header);

        sprintf(page+strlen(page), "File uploaded, it took %d sec. Restarting....", (uint32_t)(millis()-start_time)/1000);
        xTaskCreate(&systemRebootTask, "systemRebootTask", 1024, (int *)3000, 5, NULL);  
		httpd_resp_set_hdr(req, "Refresh", "10; /");
  
    } else {
        // upgrading fail show ota page again
        // show upgrade fail and redirect to ota page in 10 sec
        //strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
        

        //strcpy(page+strlen(page), "OTA upgrade failed...\n");        
        strcat(page, "OTA upgrade failed...\n");        
        //strcpy(page+strlen(page), err_text);        
        strcat(page, err_text);        
        httpd_resp_set_status(req, HTTPD_500);
    }
    
    httpd_resp_send(req, page, -1);
    return ESP_OK;
}

esp_err_t debug_get_handler(httpd_req_t *req){
  char page[PAGE_DEFAULT_BUFFER_SIZE];  
  strncpy(page, HTTP_STR_DEBUG, PAGE_DEFAULT_BUFFER_SIZE);
  //show_page_debug( page );
    show_http_page( req, page);
  httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
  httpd_resp_send(req, page, strlen(page));
  return ESP_OK;
}

esp_err_t reboot_get_handler(httpd_req_t *req)
{
	uint8_t found = 0;
	

	if ( http_get_has_params(req) == ESP_OK) {
        uint8_t st;
        if ( http_get_key_uint8(req, "st", &st) == ESP_OK ) {
            ESP_LOGV(TAG, "st found %d", st);
            found = (st == 1);        
        }
    } 


    
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

             
    
    if ( found ) {  
        xTaskCreate(&systemRebootTask, "systemRebootTask", 1024, 2000, 5, NULL);
        httpd_resp_set_hdr(req, "Refresh", "5; /");
        httpd_resp_send(req, NULL, 0);
    
    } else {
        //restart_page_data(page);  
		char page[512];        
        strcpy(page, "Please restart ESP");
		httpd_resp_send(req, page, strlen(page)); 
    }
     
    return ESP_OK;
}


esp_err_t favicon_get_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
    const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);
    httpd_resp_set_type(req, HTTP_MEDIA_TYPE_ICON);
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
    return ESP_OK;
}

esp_err_t icons_get_handler(httpd_req_t *req)
{
    size_t icon_size = 0;
    char *icon_start = NULL;

	if ( strcmp(req->uri, HTTP_URI_ICON_MENU) == ESP_OK) 
    {
        icon_size = (menu_png_end - menu_png_start);
        icon_start = (char *)menu_png_start;
    }
    else if ( strcmp(req->uri, HTTP_URI_ICON_MENU2) == ESP_OK) 
    {
        icon_size = (menu2_png_end - menu2_png_start);
        icon_start = (char *)menu2_png_start;
    }        

    httpd_resp_set_type(req, HTTP_MEDIA_TYPE_ICON);
    httpd_resp_send(req, (const char *)icon_start, icon_size);
    return ESP_OK;
}


esp_err_t main_css_get_handler(httpd_req_t *req)
{
    extern const unsigned char main_css_start[] asm("_binary_main_min_css_start");
    extern const unsigned char main_css_end[]   asm("_binary_main_min_css_end");
    const size_t main_css_size = (main_css_end - main_css_start);
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)main_css_start, main_css_size);
    return ESP_OK;
}

esp_err_t main_ajax_get_handler(httpd_req_t *req)
{
    extern const unsigned char ajax_js_start[] asm("_binary_ajax_min_js_start");
    extern const unsigned char ajax_js_end[]   asm("_binary_ajax_min_js_end");
    const size_t ajax_js_size = (ajax_js_end - ajax_js_start);
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)ajax_js_start, ajax_js_size);
    return ESP_OK;
}


// TODO show custom page handler from component
//show_http_page( req, page);

esp_err_t register_http_process_page_data(const char *uri, httpd_uri_func fn_cb)
{
    return ESP_OK;
}