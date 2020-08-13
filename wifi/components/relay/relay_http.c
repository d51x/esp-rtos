#include "relay_http.h"

#ifdef CONFIG_RELAY_HTTP

static const char* TAG = "RELAY";

const char *html_block_relay_start ICACHE_RODATA_ATTR = 
    "<div class='group rnd'>"
        "<h4 class='brd-btm'>Relays:</h4>";    

const char *html_block_relay_end ICACHE_RODATA_ATTR = 
    "</div>";   

const char *html_relay_btn ICACHE_RODATA_ATTR =

                                       "<button id='relay%d' class='button lht %s' "            // on или off - текущее состояние
                                                        "data-class='button lht' "
                                                        "data-uri='" RELAY_URI "?pin=%d&st=' "
                                                        "data-val='%d' "                    // 0 или 1 - нужное состояние кнопки,которое будет передано в запрос для изменения
                                                        "data-text='%s'"                    // текст для кнопки, который подставится после нажатия, парметр замены {0}
                                                                            //id     id2    v   st
                                                        "onclick='btnclick(this.id, this.id, 0, 1)'>"     // lcd - id, v: 0 - без подстановки результата, 1 - с подстановкой в конец, 2 - с подстановкой во внутрь вместо {0}
                                                                                                            // st: менять состояние кнопки 1, не менять состояние кнопки 0
                                                        "%s"
                                        "</button>" ;

static void relay_print_data(char *data, void *args)
{
    strcat(data, html_block_relay_start);
    for (uint8_t i = 0; i < relay_count; i++)
    {
        sprintf(data + strlen(data), html_relay_btn
                                        , relays[i].pin
                                        , relays[i].state ? "on" : "off"
                                        , relays[i].pin
                                        , !relays[i].state
                                        , relays[i].name
                                        , relays[i].name
                                        );
    }
    strcat(data, html_block_relay_end);
}

static void relay_register_http_print_data() 
{
    register_print_page_block( "relay_data", PAGES_URI[ PAGE_URI_ROOT], 3, relay_print_data, NULL, NULL, NULL );
}

static esp_err_t mcp23017_get_handler(httpd_req_t *req)
{
    // check params
    char page[100] = ""; 
    esp_err_t err = ESP_FAIL;
	if ( http_get_has_params(req) == ESP_OK) 
	{

        /*
            <ip>/relay?pin=12&st=1
            <ip>/relay?pin=12
            <ip>/relay?all=1
        */

        char param[20];
        if ( http_get_key_str(req, "pin", param, sizeof(param)) == ESP_OK ) 
        {
            uint8_t pin = atoi(param);
            if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK )
            {
                uint8_t st = atoi(param);
                strcpy(page,  "ERROR");
                for (uint8_t i = 0; i < relay_count; i++)
                {
                    if ( relays[i].pin == pin )
                    {
                        err =  relay_write( (relay_handle_t)&relays[i], st);
                        itoa(st, page, 10);
                        break;
                    }
                }
                
            }
            else
            {
                strcpy(page,  "ERROR"); 
                for (uint8_t i = 0; i < relay_count; i++)
                {
                    if ( relays[i].pin == pin )
                    {
                        relay_state_t st =  relay_read( (relay_handle_t)&relays[i]);
                        itoa(st, page, 10);
                        break;
                    }
                }             
            }            
        }
        else if ( http_get_key_str(req, "all", param, sizeof(param)) == ESP_OK ) 
        {
            strcpy(page,  "{" );
            for (uint8_t i = 0; i < relay_count; i++)
            {
                relay_state_t st =  relay_read( (relay_handle_t)&relays[i]);
                sprintf(page+strlen(page), "\"relay%d\": %d",  relays[i].pin, st );
                if ( i < relay_count-1 )
                    strcat(page, ", ");
            }
            strcat(page,  "}" );
        }
        else
        {
            strcpy(page, "ERROR");
        }
    }

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
	httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;    
}

static void relay_register_http_handler(httpd_handle_t _server)
{
    add_uri_get_handler( _server, RELAY_URI, mcp23017_get_handler, NULL); 
}

void relay_http_init(httpd_handle_t _server)
{
    relay_register_http_print_data();  
    relay_register_http_handler(_server);
}

#endif
