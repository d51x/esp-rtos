#include "mcp23017_http.h"

#ifdef CONFIG_MCP23017_HTTP
const char *html_block_mcp23107_start ICACHE_RODATA_ATTR = 
    "<div class='group rnd'>"
        "<h4 class='brd-btm'>MCP23017:</h4>";      

const char *html_block_mcp23107_end ICACHE_RODATA_ATTR = 
    "</div>";    

const char *html_mcp23017_btn ICACHE_RODATA_ATTR =

                                       "<button id='mcp23017-%d' class='button lht %s' "            // on или off - текущее состояние
                                                        "data-class='button lht' "
                                                        "data-uri='" MCP23017_URI "?st=mcp&pin=%d&val=' "
                                                        "data-val='%d' "                    // 0 или 1 - нужное состояние кнопки,которое будет передано в запрос для изменения
                                                        "data-text='%s'"                    // текст для кнопки, который подставится после нажатия, парметр замены {0}
                                                                            //id     id2    v   st
                                                        "onclick='btnclick(\"mcp23017-%d\", \"mcp23017-%d\", 0, 1)'>"     // lcd - id, v: 0 - без подстановки результата, 1 - с подстановкой в конец, 2 - с подстановкой во внутрь вместо {0}
                                                                                                            // st: менять состояние кнопки 1, не менять состояние кнопки 0
                                                        "%s"
                                        "</button>" ;

static const char* TAG = "MCP23017HTTP";

static void mcp23017_print_data(char *data, void *args)
{
    mcp23017_handle_t dev_h = (mcp23017_handle_t)args;
    mcp23017_t *dev = (mcp23017_t *) dev_h;

    char *buf = (char *) calloc(250 * 16, sizeof(char));
    strcpy(buf, "");
    for ( uint8_t i = 0; i < 16; i++)
    {
        //uint8_t val = (BIT_CHECK(dev->pins_values, i) != 0 ) ;
        uint16_t values;
        mcp23017_read_io(dev_h, &values);
        uint8_t val = (BIT_CHECK(values, i) != 0 ) ;
        uint8_t visible = (BIT_CHECK(dev->http_buttons, i) != 0 ) ;
        if ( visible ) {
            
            char btn_name[16];
            snprintf(btn_name, 16, "PIN%02d", i);

            char *buf_btn = (char *) calloc(250, sizeof(char));
            snprintf(buf_btn, 512, html_mcp23017_btn
                                    , i                         // id
                                    , val ? "on" : "off"        // class
                                    , i                         // uri pin
                                    , !val                      // data-val
                                    , ( dev->names[i] != NULL ) ? dev->names[i] : btn_name                  // data-text
                                    , i                         // btnclick id
                                    , i                         // btnclick id2
                                    , ( dev->names[i] != NULL ) ? dev->names[i] : btn_name                  // текст кнопки
                                    );
            strncat(buf, buf_btn, 512);
            free(buf_btn);
        }
    }


    strcpy(data+strlen(data), html_block_mcp23107_start);
    strcpy(data+strlen(data), buf);
    strcpy(data+strlen(data), html_block_mcp23107_end);
    free(buf);
}

void mcp23017_register_http_print_data(mcp23017_handle_t dev_h) 
{
    register_print_page_block( "mcp23017_data", PAGES_URI[ PAGE_URI_ROOT], 4, mcp23017_print_data, dev_h, NULL, NULL );
    
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