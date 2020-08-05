#include "http_page.h"
#include "http_page_tpl.h"


static const char *TAG = "WEB";

http_print_page_block_t *http_print_page_block = NULL;
uint8_t http_print_page_block_count = 0;

void set_redirect_header(uint8_t time, const char *uri, char *data){
    sprintf(data, html_header_redirect, time, uri);
}

void httpd_resp_sendstr_chunk(httpd_req_t *req, const char *buf){
    httpd_resp_send_chunk(req, buf, strlen(buf));
}



void page_initialize_menu()
{
    http_menu = (http_menu_item_t *)calloc(MENU_ITEM_COUNT, sizeof(http_menu_item_t));
    
    strcpy(http_menu[0].uri,    HTTP_URI_ROOT   );
    strcpy(http_menu[0].name,   HTTP_STR_MAIN   );    
    
    strcpy(http_menu[1].uri,    HTTP_URI_SETUP  );
    strcpy(http_menu[1].name,   HTTP_STR_SETUP  );
    
    strcpy(http_menu[2].uri,    HTTP_URI_TOOLS  );
    strcpy(http_menu[2].name,   HTTP_STR_TOOLS  );   

    strcpy(http_menu[3].uri,    HTTP_URI_UPDATE );
    strcpy(http_menu[3].name,   HTTP_STR_UPDATE );

    strcpy(http_menu[4].uri,    HTTP_URI_DEBUG  );
    strcpy(http_menu[4].name,   HTTP_STR_DEBUG  );
/*
{
    { HTTP_URI_ROOT,   HTTP_STR_MAIN   },
    { HTTP_URI_SETUP,  HTTP_STR_SETUP  },
    { HTTP_URI_TOOLS,  HTTP_STR_TOOLS  },
    { HTTP_URI_UPDATE, HTTP_STR_UPDATE },
    { HTTP_URI_DEBUG,  HTTP_STR_DEBUG  }
};
*/
}

void page_generate_html_start(char *buf, const char *title)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );
    sprintf(buf, html_page_start, title);
}

void page_generate_html_end(char *_buf)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    char * buf = malloc(25);
    get_localtime(buf);

    //ESP_LOGI(TAG, "html_page_end: \n %s", html_page_end);

    sprintf(_buf + strlen(_buf), html_page_end, buf, FW_VER);
    free(buf);
}






void page_generate_top_header(char *buf)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );
    char * uptime = malloc(20);
    get_uptime(uptime);

    char *menu = malloc((strlen(html_page_menu_item) + 10 + 10)* menu_items_count + 1);
    page_show_menu(menu);

    //ESP_LOGI(TAG, "html_page_top_header: \n %s", html_page_top_header);

    sprintf(buf + strlen(buf), html_page_top_header
                , wifi_cfg->hostname  // hostname
                , wifi_get_rssi()       // rssi
                , menu
                );

    //ESP_LOGI(TAG, "html_page_devinfo: \n %s", html_page_devinfo);

    sprintf(buf + strlen(buf), html_page_devinfo
                                , esp_get_free_heap_size()
                                , uptime
                                );
    free(uptime);
    free(menu);
}

void page_generate_data(char *buf, const char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    //ESP_LOGI(TAG, "html_page_content_start: \n %s", html_page_content_start);

    sprintf(buf + strlen(buf), html_page_content_start);
    sprintf(buf + strlen(buf), data);

    //ESP_LOGI(TAG, "html_page_content_end: \n %s", html_page_content_end);
    sprintf(buf + strlen(buf), html_page_content_end);
}



void page_show_menu(char *buf)
{
    // TODO add callback to add custom menu item from component
    uint8_t i;
    strcpy(buf, "");
    for ( i = 0; i < menu_items_count; i++) {
        //sprintf(buf+strlen(buf), html_page_menu_item, menu_uri[i], menu_names[i]);
        sprintf(buf+strlen(buf), html_page_menu_item, http_menu[i].uri, http_menu[i].name);
    }
}


void generate_page(char *page, const char *title, const char *data) 
{   
    //ESP_LOGI(TAG, "****** %s", __func__ );

    page_generate_html_start(page, title);
    page_generate_top_header(page);
    page_generate_data(page, data);
    page_generate_html_end(page);   
}

void show_http_page(httpd_req_t *req, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );
        // TODO передавать http_req в функцию
        // из http_req брать user_ctx
        // в нем указаель на функцию отрисовки
        // тайтл страницы

   //ESP_LOGI(TAG, "%s", __func__);
   //ESP_LOGI(TAG, "uri  %s",  req->uri);
   //ESP_LOGI(TAG, "get user ctx %p",  req->user_ctx);
   user_ctx_t *usr_ctx = (user_ctx_t *) req->user_ctx;

    if ( usr_ctx != NULL ) 
    {
        //ESP_LOGI(TAG, "Custom title: %s", usr_ctx->title);

        char *_uri;
        _uri = http_uri_clean(req);
	    ///if ( http_get_has_params(req) == ESP_OK) 
	    ///{
            // remove params
            //_uri = strstr(req->uri, "?");
            
            //_uri = strchr(req->uri, '?');
            //uint8_t pos = _uri - req->uri;
            //_uri = (char *) calloc(1, pos + 1);
            //strncpy(_uri, req->uri, pos);
            ///_uri = cut_str_from_str( req->uri, "?");
        ///} else {
            ///_uri = (char *) calloc(1, strlen( req->uri));
            ///strcpy(_uri, req->uri);
        ///}
        //char *_title = (char *) calloc(1, strlen(title) + strlen(wifi_cfg->hostname) + 3);
        char *_title = (char *) calloc(1, strlen(usr_ctx->title) + strlen(wifi_cfg->hostname) + 3);
        //sprintf(_title, "%s: %s", wifi_cfg->hostname, title);
        sprintf(_title, "%s: %s", wifi_cfg->hostname, usr_ctx->title);

        uint8_t found = 0;
        for ( uint8_t i = 0; i < PAGE_URI_MAX; i++) 
        {
            if ( strcmp( _uri, PAGES_URI[i] ) == 0 )
            {
                found = 1;
                if ( PAGES_HANDLER[i].show && PAGES_HANDLER[i].fn != NULL )
                {
                    PAGES_HANDLER[i].fn(_title, data);
                }
                break;
            }
        }

        if ( !found ) {
            show_custom_page(usr_ctx->title, data) ;
        }

        free( _title );
        free( _uri );
    } 
}


void print_page_block(const char *uri, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    //ESP_LOGI(TAG, " function %s started", __func__);

    uint8_t (*indexes)[2] = NULL;
    uint8_t found_cnt = 0;
    uint8_t i = 0;

    //ESP_LOGI(TAG, "find custom print page blocks for uri %s", uri);
    //ESP_LOGI(TAG, "found_cnt %d", found_cnt);

    for ( i = 0; i < http_print_page_block_count; i++) 
    {
        //ESP_LOGI(TAG, "compare block uri %s with page uri %s", http_print_page_block[i].uri, uri);
        if (strcmp(http_print_page_block[i].uri, uri) == 0 && http_print_page_block[i].fn_print_block != NULL) 
        {
            //ESP_LOGI(TAG, "found block for uri %s with index %d (%d) and func addr %p", 
            //http_print_page_block[i].uri, 
            //http_print_page_block[i].index,
            //i,
            //http_print_page_block[i].fn_print_block);
            found_cnt++;

            //ESP_LOGI(TAG, "found_cnt %d", found_cnt);
            indexes = (uint8_t *) realloc(indexes, found_cnt * 2 * sizeof(uint8_t));
            indexes[found_cnt-1][0] =i;
            indexes[found_cnt-1][1] =http_print_page_block[i].index;

            //ESP_LOGI(TAG, "indexes[%d] = %d, %d", found_cnt-1, indexes[found_cnt-1][0], indexes[found_cnt-1][1]);
        }
    }  

    // TODO sort indexes
    if ( http_print_page_block_count > 1 && found_cnt > 1) {
        for ( i = 0; i < http_print_page_block_count; i++)
        {
            for ( uint8_t j = i + 1; j < http_print_page_block_count; j++ )
            {
                if ( indexes[i][1] > indexes[j][1] )
                {
                    uint8_t t[2];
                    memcpy(&t, indexes[i], 2*sizeof(uint8_t));
                    memcpy(indexes[i], indexes[j], 2*sizeof(uint8_t));
                    memcpy(indexes[j], t, 2*sizeof(uint8_t));
                }
            } 
        }
    }
    //for ( i = 0; i < found_cnt; i++) {
    //    ESP_LOGI(TAG, "indexes[%d] = %d, %d", i, indexes[i][0], indexes[i][1]);
    //}
    // print data
    for ( i = 0; i < found_cnt; i++) 
    {
        uint8_t idx = indexes[i][0];
        //ESP_LOGI(TAG, "print block with index %d (%d) func addr %p", idx, indexes[i][1], http_print_page_block[ idx ].fn_print_block);
        if (strcmp(http_print_page_block[idx].uri, uri) == 0 && http_print_page_block[idx].fn_print_block != NULL) 
        {
            http_print_page_block[ idx ].fn_print_block(data);            
        }

    }

    free(indexes);
}

void show_custom_page(const char *title, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    strcpy(page_data, title);
    generate_page(data, title, page_data);
    free(page_data);
}

void show_page_main(const char *title, char *data)
{

    //ESP_LOGI(TAG, "****** %s", __func__ );

    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    strcpy(page_data, "");

    print_page_block( PAGES_URI[ PAGE_URI_ROOT ], page_data);

    generate_page(data, title, page_data);
    free(page_data);

    #ifdef CONFIG_CONPONENT_DEBUG
        print_task_stack_depth(TAG, "main page");    
    #endif
}

void show_page_setup(const char *title, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    //strcpy(page_data, "Setup");

    //ESP_LOGI(TAG, "html_page_setup_wifi: \n %s", html_page_setup_wifi);

    sprintf(page_data, html_page_setup_wifi 
                       , wifi_cfg->hostname         // hostname
                       , wifi_cfg->ssid         // ssid
                       , wifi_cfg->password         // pass    
                       , (wifi_cfg->mode == WIFI_MODE_STA) ? "checked" : ""          // sta checked
                       , (wifi_cfg->mode == WIFI_MODE_AP)  ? "checked" : ""         // ap checked
                       );

    print_page_block( PAGES_URI[ PAGE_URI_SETUP ], page_data);
/*
    mqtt_config_t *mqtt_cfg = malloc(sizeof(mqtt_config_t));
    mqtt_get_cfg(mqtt_cfg);
    sprintf(page_data + strlen(page_data), html_page_setup_mqtt 
                       , mqtt_cfg->enabled ? "checked" : ""         // enabled
                       , mqtt_cfg->broker_url         // host
                       , mqtt_cfg->login         // login    
                       , mqtt_cfg->password         // password    
                       , mqtt_cfg->base_topic         // base topic    
                       , mqtt_cfg->send_interval          // send interval
                       );
    free(mqtt_cfg);
*/

    sprintf( page_data + strlen(page_data), html_page_reboot_button_block);
    generate_page(data, title, page_data);
    free(page_data);
    
}

void show_page_config(const char *title, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);

    generate_page(data, title, page_data);
    free(page_data);
}

void show_page_tools(const char *title, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    /* зарегистрирровать callback отрисовки на странице */
    /* зарегистрировать callback обработки параметров */
    strcpy(page_data, "");
    //#ifdef CONFIG_COMPONENT_I2C
    //uint8_t sda = 2;
    //uint8_t scl = 0;
    //sprintf(page_data, html_page_tools_i2c, sda, scl);
    //#endif
    
    print_page_block( PAGES_URI[ PAGE_URI_TOOLS ], page_data);

    //ESP_LOGI(TAG, "html_page_reboot_button_block: \n %s", html_page_reboot_button_block);

    sprintf( page_data + strlen(page_data), html_page_reboot_button_block);
    
    generate_page(data, title, page_data);
    free(page_data);
}

void show_page_update(const char *title, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);

    //ESP_LOGI(TAG, "html_page_ota: \n %s", html_page_ota);

    strcpy(page_data, html_page_ota);
    generate_page(data, title, page_data);
    free(page_data);
}

void show_page_debug(const char *title, char *data)
{
    //ESP_LOGI(TAG, "****** %s", __func__ );

    char *page_data = malloc(PAGE_DEFAULT_BUFFER_SIZE);
    strcpy(page_data, "");
    print_page_block( PAGES_URI[ PAGE_URI_DEBUG ], page_data);
    
    generate_page(data, title, page_data);
    free(page_data);    
}

void show_restart_page_data(char *data)
{

}

void show_restarting_page_data(char *data)
{

}

esp_err_t register_print_page_block(const char *uri, uint8_t index, func_http_print_page_block fn_print_block, httpd_uri_func fn_cb)
{
    //ESP_LOGI(TAG, "function %s started", __func__);

    //ESP_LOGI(TAG, "printpage block count %d", http_print_page_block_count);

    for ( uint8_t i = 0; i < http_print_page_block_count; i++) 
    {
        //ESP_LOGI(TAG, "[%d] compare registered uri %s with page uri %s", i, http_print_page_block[i].uri, uri);
        if (strcmp(http_print_page_block[i].uri, uri) == 0 && http_print_page_block[i].fn_print_block == fn_print_block) 
        {
            //ESP_LOGI(TAG, "[%d] found uri %s, return...", i, http_print_page_block[i].uri);
            return ESP_FAIL;
        }
    }    

    //ESP_LOGI(TAG, "register new print page block");
    
    http_print_page_block_count++; // увеличим размер массива
    //ESP_LOGI(TAG, "printpage block count %d", http_print_page_block_count);

    http_print_page_block = (http_print_page_block_t *) realloc(http_print_page_block, http_print_page_block_count * sizeof(http_print_page_block_t));
    strcpy( http_print_page_block[ http_print_page_block_count - 1 ].uri, uri); 
    http_print_page_block[ http_print_page_block_count - 1 ].index = index; 
    http_print_page_block[ http_print_page_block_count - 1 ].fn_print_block = fn_print_block;
    http_print_page_block[ http_print_page_block_count - 1 ].process_cb = fn_cb;

    //ESP_LOGI(TAG, "registered printpage block %s\t\t%d\t\t%p", 
    //        http_print_page_block[ http_print_page_block_count - 1 ].uri, 
    //        http_print_page_block[ http_print_page_block_count - 1 ].index,
    //        http_print_page_block[ http_print_page_block_count - 1 ].fn_print_block);

    return ESP_OK;
}

esp_err_t register_http_page_menu(const char *uri, const char *name)
{
    //menu_uri
    for (uint8_t i = 0; i < menu_items_count; i++)
    {
        if ( strcmp( http_menu[i].uri, uri ) == 0 ) {
            return ESP_FAIL;
        }
    }

    menu_items_count++;
    http_menu = (http_menu_item_t *) realloc( http_menu, menu_items_count * sizeof(http_menu_item_t));
    strcpy(http_menu[menu_items_count - 1].uri, uri);
    strcpy(http_menu[menu_items_count - 1].name, name);
    return ESP_OK;
}


