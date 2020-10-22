#include "http_page.h"
#include "http_page_tpl.h"


static const char *TAG = "WEB";

http_print_page_block_t *http_print_page_block = NULL;
uint8_t http_print_page_block_count = 0;

void print_page_block(httpd_req_t *req, const char *uri);


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

    strcpy(http_menu[2].uri,    HTTP_URI_CONFIG  );
    strcpy(http_menu[2].name,   HTTP_STR_CONFIG  );  

    strcpy(http_menu[3].uri,    HTTP_URI_TOOLS  );
    strcpy(http_menu[3].name,   HTTP_STR_TOOLS  );   

    strcpy(http_menu[4].uri,    HTTP_URI_OTA );
    strcpy(http_menu[4].name,   HTTP_STR_OTA );

    strcpy(http_menu[5].uri,    HTTP_URI_DEBUG  );
    strcpy(http_menu[5].name,   HTTP_STR_DEBUG  );
/*
{
    { HTTP_URI_ROOT,   HTTP_STR_MAIN   },
    { HTTP_URI_SETUP,  HTTP_STR_SETUP  },
    { HTTP_URI_TOOLS,  HTTP_STR_TOOLS  },
    { HTTP_URI_OTA,     HTTP_STR_OTA },
    { HTTP_URI_DEBUG,  HTTP_STR_DEBUG  }
};
*/
}

void page_generate_html_start(httpd_req_t *req, const char *title)
{
    httpd_resp_sendstr_chunk(req, html_page_start1);
    httpd_resp_sendstr_chunk(req, title);
    httpd_resp_sendstr_chunk(req, html_page_start2);
}

void page_generate_html_end(httpd_req_t *req)
{
    httpd_resp_sendstr_chunk(req, html_page_end1);

    char * buf = malloc(25);
    get_localtime(buf);
    httpd_resp_sendstr_chunk(req, buf);
    free(buf);

    httpd_resp_sendstr_chunk(req, html_page_end2);
    httpd_resp_sendstr_chunk(req, FW_VER);
    httpd_resp_sendstr_chunk(req, html_page_end3);
}

void page_generate_top_header(httpd_req_t *req)
{
    httpd_resp_sendstr_chunk(req, html_page_top_header1);
    httpd_resp_sendstr_chunk(req, wifi_cfg->hostname);
    httpd_resp_sendstr_chunk(req, html_page_top_header2);

    char *buf = malloc(5);
    itoa(wifi_get_rssi(), buf, 10);
    httpd_resp_sendstr_chunk(req, buf);
    free(buf);

    httpd_resp_sendstr_chunk(req, html_page_top_header3);

    page_show_menu(req); //char *menu = malloc((strlen(html_page_menu_item) + 10 + 10)* menu_items_count + 1);
    
    httpd_resp_sendstr_chunk(req, html_page_top_header4);
    httpd_resp_sendstr_chunk(req, html_page_devinfo1);

    char *buf_mem = malloc(8);
    itoa(esp_get_free_heap_size(), buf_mem, 10);
    httpd_resp_sendstr_chunk(req, buf_mem);
    free(buf_mem);

    httpd_resp_sendstr_chunk(req, html_page_devinfo2);
    char * uptime = malloc(20);
    get_uptime(uptime);
    httpd_resp_sendstr_chunk(req, uptime);
    free(uptime);
    httpd_resp_sendstr_chunk(req, html_page_devinfo3);
}

void page_generate_data(httpd_req_t *req, const char *uri)
{
    ESP_LOGI(TAG, "%s: %s", __func__, uri);
    //TODO: для AP не отображаются страницы, останавливается вывод на hostname, далее пусто
    httpd_resp_sendstr_chunk(req, html_page_content_start);
    print_page_block( req, uri);
    httpd_resp_sendstr_chunk(req, html_page_content_end);
}

void page_show_menu(httpd_req_t *req)
{
    for ( uint8_t i = 0; i < menu_items_count; i++) {
         httpd_resp_sendstr_chunk(req, html_page_menu_item1);
         httpd_resp_sendstr_chunk(req, http_menu[i].uri);
         httpd_resp_sendstr_chunk(req, html_page_menu_item2);
         httpd_resp_sendstr_chunk(req, http_menu[i].name);
         httpd_resp_sendstr_chunk(req, html_page_menu_item3);
    }
}


void generate_page(httpd_req_t *req, const char *uri, const char *title) 
{   
    page_generate_html_start(req, title);  // data with html_start
    page_generate_top_header(req);  // data with html_start + top_header
    page_generate_data(req, uri); 
    page_generate_html_end(req);  

    #ifdef CONFIG_CONPONENT_DEBUG
        print_task_stack_depth(TAG, "page: %s", uri);    
    #endif     
}

void show_http_page(httpd_req_t *req)
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
                    PAGES_HANDLER[i].fn(req, _title);
                }
                break;
            }
        }

        if ( !found ) {
            show_custom_page(req, _uri, usr_ctx->title) ;
        }

        free( _title );
        free( _uri );
    } 
}

void print_page_block(httpd_req_t *req, const char *uri)
{
    ESP_LOGI(TAG, "%s: %s, block count: %d", __func__, uri, http_print_page_block_count);
    uint8_t (*indexes)[2] = NULL;
    uint8_t found_cnt = 0;
    uint8_t i = 0;

    for ( i = 0; i < http_print_page_block_count; i++) 
    {
        // строгое соответствие uri, параметры запроса не учитываются!
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
        // строгое соответствие uri, параметры запроса не учитываются
        if (strcmp(http_print_page_block[idx].uri, uri) == 0 && http_print_page_block[idx].fn_print_block != NULL) 
        {
            http_args_t *arg = http_print_page_block[ idx ].args1;
            arg->req = req;
            ESP_LOGW(TAG, "page req: %p", req);
            ESP_LOGW(TAG, "arg->req req: %p", arg->req);
            http_print_page_block[ idx ].fn_print_block(arg);       
        }

    }


    if ( strcmp(uri, PAGES_URI[ PAGE_URI_TOOLS ]) == 0 || strcmp(uri, PAGES_URI[ PAGE_URI_CONFIG ]) == 0 || strcmp(uri, PAGES_URI[ PAGE_URI_SETUP ]) == 0)
    {
        httpd_resp_sendstr_chunk(req, html_page_reboot_button_block);
    }

    free(indexes);
}

void show_custom_page(httpd_req_t *req, const char *uri, const char *title)
{
    generate_page(req, uri, title);
}

void show_page_main(httpd_req_t *req, const char *title)
{
    // TODO: uri and title already in req
    generate_page(req, PAGES_URI[ PAGE_URI_ROOT ], title);

}

void show_page_setup(httpd_req_t *req, const char *title)
{
    generate_page(req, PAGES_URI[ PAGE_URI_SETUP ], title);
}

void show_page_config(httpd_req_t *req, const char *title)
{
    generate_page(req, PAGES_URI[ PAGE_URI_CONFIG ], title);
}

void show_page_tools(httpd_req_t *req, const char *title)
{
    generate_page(req, PAGES_URI[ PAGE_URI_TOOLS ], title );
}

void show_page_ota(httpd_req_t *req, const char *title)
{
    generate_page(req, PAGES_URI[ PAGE_URI_OTA ], title);
}

void show_page_debug(httpd_req_t *req, const char *title)
{
    generate_page(req, PAGES_URI[ PAGE_URI_DEBUG ], title);
}

void show_restart_page_data(httpd_req_t *req)
{

}

void show_restarting_page_data(httpd_req_t *req)
{

}

esp_err_t register_print_page_block(const char *name, const char *uri, uint8_t index, func_http_print_page_block fn_print_block, http_args_t *args1, httpd_uri_func fn_cb, void *args2)
{
    for ( uint8_t i = 0; i < http_print_page_block_count; i++) 
    {
        if (strcmp(http_print_page_block[i].name, name) == 0 && 
            strcmp(http_print_page_block[i].uri, uri) == 0 &&       // строгое соответствие uri, параметры запроса не учитываются
            http_print_page_block[i].fn_print_block == fn_print_block) 
        {
            //ESP_LOGI(TAG, "[%d] found (%s) uri %s, return...", i, http_print_page_block[i].name, http_print_page_block[i].uri);
            return ESP_FAIL;
        }
    }    

    http_print_page_block_count++; // увеличим размер массива
    ESP_LOGW(TAG, "http_print_page_block_count %d", http_print_page_block_count);

    http_print_page_block = (http_print_page_block_t *) realloc(http_print_page_block, http_print_page_block_count * sizeof(http_print_page_block_t));

    //TODO переделать на strncpy с указнием кол-ва байт копирования
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


