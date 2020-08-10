#include "http_page.h"
#include "http_page_tpl.h"


static const char *TAG = "WEB";

http_print_page_block_t *http_print_page_block = NULL;
uint8_t http_print_page_block_count = 0;

void print_page_block(const char *uri, char *data);


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
    sprintf(buf + strlen(buf), html_page_start, title);
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

void page_generate_data(const char *uri, char *data)
{
    sprintf(data + strlen(data), html_page_content_start);
    print_page_block( uri, data);
    sprintf(data + strlen(data), html_page_content_end);
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


void generate_page(httpd_req_t *req, const char *uri, const char *title, const char *data) 
{   
    //ESP_LOGI(TAG, "****** %s", __func__ );
    // may be printed as chunk
    page_generate_html_start(data, title);  // data with html_start

    // may be printed as chunk
    page_generate_top_header(data);  // data with html_start + top_header

    // may be printed as chunk into function
    page_generate_data(uri, data);

    // may be printed as chunk
    page_generate_html_end(data);  
    
    #ifdef CONFIG_CONPONENT_DEBUG
        print_task_stack_depth(TAG, "page: %s", uri);    
    #endif     
}

void show_http_page(httpd_req_t *req, char *data)
{
   user_ctx_t *usr_ctx = (user_ctx_t *) req->user_ctx;

    if ( usr_ctx != NULL ) 
    {
        char *_uri;
        _uri = http_uri_clean(req);
        char *_title = (char *) calloc(1, strlen(usr_ctx->title) + strlen(wifi_cfg->hostname) + 3);
        sprintf(_title, "%s: %s", wifi_cfg->hostname, usr_ctx->title);

        uint8_t found = 0;
        for ( uint8_t i = 0; i < PAGE_URI_MAX; i++) 
        {
            if ( strcmp( _uri, PAGES_URI[i] ) == 0 )
            {
                found = 1;
                if ( PAGES_HANDLER[i].show && PAGES_HANDLER[i].fn != NULL )
                {
                    PAGES_HANDLER[i].fn(req, _title, data);
                }
                break;
            }
        }

        if ( !found ) {
            show_custom_page(req, _uri, usr_ctx->title, data) ;
        }

        free( _title );
        free( _uri );
    } 
}


void print_page_block(const char *uri, char *data)
{
    uint8_t (*indexes)[2] = NULL;
    uint8_t found_cnt = 0;
    uint8_t i = 0;

    for ( i = 0; i < http_print_page_block_count; i++) 
    {
        if (strcmp(http_print_page_block[i].uri, uri) == 0 && http_print_page_block[i].fn_print_block != NULL) 
        {
            found_cnt++;
            indexes = (uint8_t *) realloc(indexes, found_cnt * 2 * sizeof(uint8_t));
            indexes[found_cnt-1][0] =i;
            indexes[found_cnt-1][1] =http_print_page_block[i].index;
        }
    }  

    // sort
    if ( http_print_page_block_count > 1 && found_cnt > 1) {
        for ( i = 0; i < found_cnt; i++)
        {
            for ( uint8_t j = i + 1; j < found_cnt; j++ )
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

    // print data
    for ( i = 0; i < found_cnt; i++) 
    {
        uint8_t idx = indexes[i][0];
        if (strcmp(http_print_page_block[idx].uri, uri) == 0 && http_print_page_block[idx].fn_print_block != NULL) 
        {
            http_print_page_block[ idx ].fn_print_block(data, http_print_page_block[ idx ].args1);            
        }

    }


    if ( strcmp(uri, PAGES_URI[ PAGE_URI_OTA ]) == 0 ) {
        strcpy(data + strlen(data), html_page_ota);    // TODO сделать через блок и зарегистрировать
    }
    if ( strcmp(uri, PAGES_URI[ PAGE_URI_TOOLS ]) == 0 ) {
        sprintf( data + strlen(data), html_page_reboot_button_block);    // TODO сделать через блок и зарегистрировать
    }

    free(indexes);
}

void show_custom_page(httpd_req_t *req, const char *uri, const char *title, char *data)
{
    generate_page(req, uri, title, data);
}

void show_page_main(httpd_req_t *req, const char *title, char *data)
{
    // TODO: uri and title already in req
    generate_page(req, PAGES_URI[ PAGE_URI_ROOT ], title, data);

}

void show_page_setup(httpd_req_t *req, const char *title, char *data)
{
    generate_page(req, PAGES_URI[ PAGE_URI_SETUP ], title, data);
}

void show_page_tools(httpd_req_t *req, const char *title, char *data)
{
    generate_page(req, PAGES_URI[ PAGE_URI_TOOLS ], title, data);
}

void show_page_update(httpd_req_t *req, const char *title, char *data)
{
    generate_page(req, PAGES_URI[ PAGE_URI_OTA ], title, data);
}

void show_page_debug(httpd_req_t *req, const char *title, char *data)
{
    generate_page(req, PAGES_URI[ PAGE_URI_DEBUG ], title, data);
}

void show_restart_page_data(httpd_req_t *req, char *data)
{

}

void show_restarting_page_data(httpd_req_t *req, char *data)
{

}

esp_err_t register_print_page_block(const char *name, const char *uri, uint8_t index, func_http_print_page_block fn_print_block, void *args1, httpd_uri_func fn_cb, void *args2)
{
    //ESP_LOGI(TAG, "function %s started", __func__);

    //ESP_LOGI(TAG, "printpage block count %d", http_print_page_block_count);

    for ( uint8_t i = 0; i < http_print_page_block_count; i++) 
    {
        //ESP_LOGI(TAG, "[%d] compare registered uri %s with page uri %s", i, http_print_page_block[i].uri, uri);
        if (strcmp(http_print_page_block[i].name, name) == 0 && 
            strcmp(http_print_page_block[i].uri, uri) == 0 && 
            http_print_page_block[i].fn_print_block == fn_print_block) 
        {
            //ESP_LOGI(TAG, "[%d] found (%s) uri %s, return...", i, http_print_page_block[i].name, http_print_page_block[i].uri);
            return ESP_FAIL;
        }
    }    

    http_print_page_block_count++; // увеличим размер массива
    http_print_page_block = (http_print_page_block_t *) realloc(http_print_page_block, http_print_page_block_count * sizeof(http_print_page_block_t));
    strcpy( http_print_page_block[ http_print_page_block_count - 1 ].uri, uri); 
    strcpy( http_print_page_block[ http_print_page_block_count - 1 ].name, name); 
    http_print_page_block[ http_print_page_block_count - 1 ].index = index; 
    http_print_page_block[ http_print_page_block_count - 1 ].fn_print_block = fn_print_block;
    http_print_page_block[ http_print_page_block_count - 1 ].args1 = args1;
    http_print_page_block[ http_print_page_block_count - 1 ].process_cb = fn_cb;
    http_print_page_block[ http_print_page_block_count - 1 ].args2 = args2;

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


