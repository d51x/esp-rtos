#include "mcp23017_http.h"
#include "http_page_tpl.h"

#ifdef CONFIG_MCP23017_HTTP
const char *html_block_mcp23107_title ICACHE_RODATA_ATTR = "MCP23017";
const char *btn_id_tpl ICACHE_RODATA_ATTR = "mcp23017-%d";
const char *mcp23017_uri ICACHE_RODATA_ATTR = MCP23017_URI "?st=mcp&pin=%d&val=";

static const char* TAG = "MCP23017HTTP";

static void mcp23017_print_data(http_args_t *args)
{
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    mcp23017_handle_t dev_h = (mcp23017_handle_t)arg->dev;
    mcp23017_t *dev = (mcp23017_t *) dev_h;

    size_t sz = get_buf_size(html_block_data_start, html_block_mcp23107_title);
    char *data = malloc( sz);   
    sprintf(data, html_block_data_start, html_block_mcp23107_title);
    httpd_resp_sendstr_chunk(req, data);

    for ( uint8_t i = 0; i < 16; i++)
    {
        //uint8_t val = (BIT_CHECK(dev->pins_values, i) != 0 ) ;
        uint16_t values;
        mcp23017_read_io(dev_h, &values);
        uint8_t val = (BIT_CHECK(values, i) != 0 ) ;
        uint8_t visible = (BIT_CHECK(dev->http_buttons, i) != 0 ) ;
        if ( visible ) {
            char *btn_id = malloc( strlen(btn_id_tpl) + 2);
            sprintf(btn_id, btn_id_tpl, i);

            char *btn_name = malloc(16);
            snprintf(btn_name, 16, "PIN%02d", i);

            char *uri = malloc( strlen(mcp23017_uri) + 2);
            sprintf(uri, mcp23017_uri, i);

            sz = get_buf_size(html_button
                                    , btn_id
                                    , "lht"                        // id
                                    , val ? "on" : "off" 
                                    , "lht"       // class
                                    , uri
                                    , !val                      // data-val
                                    , ( dev->names[i] != NULL ) ? dev->names[i] : btn_name                  // data-text
                                    , 0                        
                                    , 1                       
                                    , ( dev->names[i] != NULL ) ? dev->names[i] : btn_name                  // текст кнопки
                                    );

            char *buf_btn = malloc( sz );
            sprintf(buf_btn, html_button
                                    , btn_id
                                    , "lht"                        // id
                                    , val ? "on" : "off"        // class
                                    , "lht"
                                    , uri
                                    , !val                      // data-val
                                    , ( dev->names[i] != NULL ) ? dev->names[i] : btn_name                  // data-text
                                    , 0                        
                                    , 1                       
                                    , ( dev->names[i] != NULL ) ? dev->names[i] : btn_name                  // текст кнопки
                                    );                                
            httpd_resp_sendstr_chunk(req, buf_btn);
            free(buf_btn);            
            free(btn_id);
            free(uri);
            free(btn_name);
        }
    }
    
    httpd_resp_sendstr_chunk(req, html_block_data_end);
    free(data);
}

void mcp23017_register_http_print_data(mcp23017_handle_t dev_h) 
{
    http_args_t *p = calloc(1,sizeof(http_args_t));
    p->dev = dev_h;
    register_print_page_block( "mcp23017_data", PAGES_URI[ PAGE_URI_ROOT], 4, mcp23017_print_data, p, NULL, NULL );
    
}

esp_err_t mcp23017_get_handler(httpd_req_t *req)
{
    // check params
    char page[200] = ""; 
	if ( http_get_has_params(req) == ESP_OK) 
	{
        user_ctx_t *ctx = req->user_ctx;
        mcp23017_handle_t dev_h = (mcp23017_handle_t)ctx->args;
        mcp23017_t *dev = (mcp23017_t *) dev_h;

        char param[20];
        if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
           
            if ( strcmp(param, "mcp") != 0 ) {
                return ESP_FAIL;	
            }
        } 

        if ( http_get_key_str(req, "pin", param, sizeof(param)) == ESP_OK ) 
        {
            uint8_t pin = atoi(param);
            if ( http_get_key_str(req, "val", param, sizeof(param)) == ESP_OK )
            {
                uint8_t value = atoi(param);

                if ( mcp23017_write_pin(dev_h, pin, value) == ESP_OK )
                    itoa(value, page, 10);
                else
                    strcpy(page, "ERROR");
            } else {
                uint8_t value = 0;
                if ( mcp23017_read_pin(dev_h, pin, &value) == ESP_OK )
                    //sprintf(page, "OK");
                    itoa(value != 0, page, 10);
                else
                    strcpy(page, "ERROR");               
            }
                 
        }  
        else 
        {
            //all pins
            uint16_t value = 0;
            if ( mcp23017_read_io(dev_h, &value) == ESP_OK  )
            {
                strcpy(page, "{");
                for (uint8_t i=0; i<16;i++)
                {
                    char s[12];
                    sprintf(s, "\"pin%d\": %d", i, BIT_CHECK(value, i) != 0);
                    strcat(page, s);
                    if ( i < 15 ) 
                        strcat(page, ", ");
                }
                strcat(page, "}");
            }
            else
                strcpy(page, "ERROR");              
        }
    } 

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
	httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;
}


void mcp23017_register_http_handler(httpd_handle_t _server, mcp23017_handle_t dev_h)
{

    user_ctx_t *ctx = (user_ctx_t *)calloc(1, sizeof(user_ctx_t));
    ctx->args = dev_h;
    add_uri_get_handler( _server, MCP23017_URI, mcp23017_get_handler, ctx); 
}

void mcp23017_http_init(httpd_handle_t _server, mcp23017_handle_t dev_h)
{
    mcp23017_register_http_print_data(dev_h);  
    mcp23017_register_http_handler(_server, dev_h);
}

void mcp23017_http_set_btn_name(mcp23017_handle_t dev_h, uint8_t idx, const char *name)
{
    mcp23017_t *dev = (mcp23017_t *) dev_h;   
    dev->names[idx] = name; 
}
#endif