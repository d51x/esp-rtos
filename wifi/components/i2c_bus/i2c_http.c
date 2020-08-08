#include "i2c_http.h"


#ifdef CONFIG_COMPONENT_I2C

static const char *TAG = "I2C";

const char *html_page_tools_i2c = "<div class='group rnd'>"
                                    "<h4 class='brd-btm'>I2C Settings:</h4>"
                                    "<form method='GET'>"
                                      "<div class='lf3'><p><label class='lf'>SDA: </label><input size='20' name='sda' class='edit rh' value='%d' /></p>"
                                      "<p><label class='lf'>SCL: </label><input size='20' name='scl' class='edit rh' value='%d' /></p>"
                                      "</div>"
                                      "<div class='rh2'><p><input type='hidden' name='st' value='i2c'></p>"
                                      "<p><input type='submit' value='Сохранить' class='button norm rht'></p>"
                                      
                                      "</div>"
                                    "</form>"
                                    #ifdef CONFIG_COMPONENT_I2C_SCANNER
                                    "<button id='i2cscan' class='button off rht' onclick='i2cscan()'>Сканировать</button>"
                                    "<div id='i2cres'></div>"
                                    #endif
                                  "</div>";      


void i2c_print_options(char *data)
{
    i2c_config_t *cfg = (i2c_config_t *) calloc(1, sizeof(i2c_config_t));
    i2c_load_cfg( cfg );
    sprintf(data+strlen(data), html_page_tools_i2c, cfg->sda_io_num,  cfg->scl_io_num);
    free(cfg);
}

/*
void i2c_register_http_menu()
{
    //register_http_page_menu( "/i2c", "I2C");
}
*/

void i2c_register_http_print_data() 
{
    register_print_page_block( "i2c_options", PAGES_URI[ PAGE_URI_TOOLS], 3, i2c_print_options, i2c_http_process_params );
}


void i2c_register_http_handler(httpd_handle_t _server)
{
    //ESP_LOGI(TAG, "function %s started", __func__);
    #ifdef CONFIG_COMPONENT_I2C_SCANNER
    add_uri_get_handler( _server, "/i2cscan", i2cscan_get_handler, NULL); 
    #endif    
    
    //user_ctx_t *ctx = (user_ctx_t *) calloc(1, sizeof(user_ctx_t));
    //strncpy(ctx->title, "i2c page", 20);
    //ctx->show = true;    
    //add_uri_get_handler( _server, "/i2c", i2c_get_handler, ctx); 
    //add_uri_get_handler( _server, "/i2c", i2c_get_handler, NULL); 
    //free(ctx);
}


void i2c_http_process_params(httpd_req_t *req)
{

   // check params
	if ( http_get_has_params(req) == ESP_OK) 
	{
        char param[100];
        if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
            if ( strcmp(param, "i2c") != 0 ) {
                return;	
            }
        } 
        
        i2c_config_t *cfg = (i2c_config_t *) calloc(1, sizeof(i2c_config_t));
        if ( http_get_key_str(req, "sda", param, sizeof(param)) == ESP_OK ) {
            cfg->sda_io_num = atoi(param);
        }  else {
            cfg->sda_io_num = I2C_SDA_DEFAULT;
        }

        if ( http_get_key_str(req, "scl", param, sizeof(param)) == ESP_OK ) {
            cfg->scl_io_num = atoi(param);
        }  else {
            cfg->scl_io_num = I2C_SCL_DEFAULT;
        }   

        i2c_save_cfg( cfg );
        free( cfg );
    } 
}

/*
esp_err_t i2c_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "function %s started...", __func__ );


    ESP_LOGI(TAG, "show page");

    char page[PAGE_DEFAULT_BUFFER_SIZE] = "";    



    strncpy(page, "i2c", PAGE_DEFAULT_BUFFER_SIZE);
    sprintf(page + strlen(page), "Страница i2c");

    show_http_page( req, page);

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
	httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;
}
*/

#ifdef CONFIG_COMPONENT_I2C_SCANNER
esp_err_t i2cscan_get_handler(httpd_req_t *req)
{
	uint8_t found = 0;
	
    i2c_bus_handle_t bus_handle = i2c_bus_init();
		char page[512] = "";        
        

    uint8_t *devices = malloc(128);
    uint8_t count = 0;
    count = i2c_bus_scan(bus_handle, devices);

    if ( count == 0) {
        sprintf(page + strlen(page), "Устройства не найдены");
    }

    for ( uint8_t i = 0; i<count;i++) {
            //ESP_LOGI(TAG, "-> found device %02d with address 0x%02x", 
            //        i+1, 
            //        devices[i]);
        sprintf(page + strlen(page), "%d: 0x%02x<br>", i+1, devices[i]);
    }

    free(devices);
    devices = NULL;

   
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
		httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;
}
#endif

#endif //#ifdef CONFIG_COMPONENT_I2C