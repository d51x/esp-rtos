#include "lcd2004_http.h"

#ifdef CONFIG_COMPONENT_LCD2004_HTTP


const char *html_block_lcd2004 ICACHE_RODATA_ATTR = "<div class='group rnd'>"
                                    "<h4 class='brd-btm'>LCD2004:</h4>"
                                       "<form method='GET'>"
                                            "<div class='lf3'><p><label class='lf'>Address: </label><input size='20' name='lcdaddr' class='edit rh' value='0x%2X' /></p>"
                                            "</div>"
                                            "<div class='rh2'><p><input type='hidden' name='st' value='lcd'></p>"
                                            "<p><input type='submit' value='Сохранить' class='button norm rht'></p>"
                                            
                                            "</div>"
                                       "</form>"
                                       
                                       "<button id='lcd' class='button rht %s' "            // on или off - текущее состояние
                                                        "data-class='button rht' "
                                                        "data-uri='/lcd?st=lcd&led=' "
                                                        "data-val='%d' "                    // 0 или 1 - нужное состояние кнопки,которое будет передано в запрос для изменения
                                                        "data-text='%s'"                    // текст для кнопки, который подставится после нажатия, парметр замены {0}
                                                                            //id     id2    v   st
                                                        "onclick='btnclick(\"lcd\", \"lcd\", 0, 1)'>"     // lcd - id, v: 0 - без подстановки результата, 1 - с подстановкой в конец, 2 - с подстановкой во внутрь вместо {0}
                                                                                                            // st: менять состояние кнопки 1, не менять состояние кнопки 0
                                                        "%s"
                                        "</button>" 
                                       "<button id='lcd2' class='button rht norm' "            // 
                                                        "data-class='button rht' "
                                                        "data-uri='/lcd?st=lcd&clr=' "
                                                        "data-val='1' "                    
                                                        "data-text='%s'"                    // текст для кнопки, который подставится после нажатия, парметр замены {0}
                                                        "onclick='btnclick(\"lcd2\", \"lcd2\", 0, 0)'>"     // lcd - id, 0 - без подстановки результата, 1 - с подстановкой в конец, 2 - с подстановкой во внутрь вместо {0}
                                                        "%s"
                                        "</button>"   
                                       "<button id='lcd3' class='button rht %s' "            // on или off - текущее состояние
                                                        "data-class='button rht' "
                                                        "data-uri='/lcd?st=lcd&on=' "
                                                        "data-val='%d' "                    // 0 или 1 - нужное состояние кнопки,которое будет передано в запрос для изменения
                                                        "data-text='%s'"                    // текст для кнопки, который подставится после нажатия, парметр замены {0}
                                                                            //id     id2    v   st
                                                        "onclick='btnclick(\"lcd3\", \"lcd3\", 2, 1)'>"     // lcd - id, v: 0 - без подстановки результата, 1 - с подстановкой в конец, 2 - с подстановкой во внутрь вместо {0}
                                                                                                            // st: менять состояние кнопки 1, не менять состояние кнопки 0
                                                        "%s"
                                        "</button>"                                                                               
                                  "</div>"; 

void lcd2004_print_options(char *data, void *args)
{
    lcd2004_conf_t *cfg = (lcd2004_conf_t *)calloc(1, sizeof(lcd2004_conf_t));
    lcd2004_get_cfg( cfg );

    uint8_t state = lcd2004_backlight_state();
    uint8_t state2 = lcd2004_state();
    sprintf(data+strlen(data), html_block_lcd2004
                    , cfg->addr  // 0x3F // lcd2004 addr
                    // button led
                    , cfg->backlight ? " on" : " off"    //  подстановка в class
                    , !cfg->backlight                     // data-val    
                    , "Подсветка"                        // data-text = текст подстановки для кнопки
                    , "Подсветка"                 // текст кнопки

                    // button clear
                    , "Очистить"
                    , "Очистить"

                    // button on off
                    , cfg->state ? " on" : " off"
                    , !cfg->state
                    , "{0}"
                    , cfg->state ? "ON" : "OFF"
                    );
    
    free(cfg);
}

void lcd2004_http_process_params(httpd_req_t *req, void *args)
{
    ESP_LOGI("LCD2004_HTTP", "%s", __func__);
   // check params
	if ( http_get_has_params(req) == ESP_OK) 
	{
        char param[100];
        if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
            if ( strcmp(param, "lcd") != 0 ) {
                return;	
            }
        } 
        // TODO: обработать принятые данные  
        // опции дисплея    

        if ( http_get_key_str(req, "lcdaddr", param, sizeof(param)) == ESP_OK ) {
            ESP_LOGI("LCD2004_HTTP", "lcdaddr %s", param);
            lcd2004_conf_t *cfg = (lcd2004_conf_t *)calloc(1, sizeof(lcd2004_conf_t));
            lcd2004_get_cfg( cfg );
            cfg->addr = hex2int(param);     
            lcd2004_save_cfg(cfg);
            free(cfg);  
        } 
    } 
}

void lcd2004_register_http_print_data() 
{
    register_print_page_block( "lcd2004_options", PAGES_URI[ PAGE_URI_TOOLS], 3, lcd2004_print_options, NULL, lcd2004_http_process_params, NULL );
}

esp_err_t lcd2004_get_handler(httpd_req_t *req)
{
    // check params
    char page[512] = ""; 
	if ( http_get_has_params(req) == ESP_OK) 
	{
        char param[100];
        if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
           
            if ( strcmp(param, "lcd") != 0 ) {
                return ESP_FAIL;	
            }
        } 

        // peram led
        if ( http_get_key_str(req, "led", param, sizeof(param)) == ESP_OK ) 
        {
           uint8_t state = atoi(param);
            // turn on/off lcd led
            lcd2004_backlight( state );  
            state = lcd2004_backlight_state();  
            //itoa( state, page, 10);
            strcpy(page, state ? "ON" : "OFF");
            
            lcd2004_conf_t *cfg = (lcd2004_conf_t *)calloc(1, sizeof(lcd2004_conf_t));
            lcd2004_get_cfg( cfg );
            cfg->backlight = state;     
            lcd2004_save_cfg(cfg);
            free(cfg);             
        }  
        else if ( http_get_key_str(req, "clr", param, sizeof(param)) == ESP_OK ) 
        {
            // clear
            lcd2004_clear();  
            strcpy(page, "OK");
        }   
        else if ( http_get_key_str(req, "on", param, sizeof(param)) == ESP_OK ) 
        {
           uint8_t state = atoi(param);
            // turn on/off lcd state
            lcd2004_set_state( state );  
            state = lcd2004_state();  
            //itoa( state, page, 10);
            strcpy(page, state ? "ON" : "OFF");

            lcd2004_conf_t *cfg = (lcd2004_conf_t *)calloc(1, sizeof(lcd2004_conf_t));
            lcd2004_get_cfg( cfg );
            cfg->state = state;     
            lcd2004_save_cfg(cfg);
            free(cfg);                        
        }       
        else 
        {
            return ESP_FAIL;
        }
    } 

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
	httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;
}

void lcd2004_register_http_handler(httpd_handle_t _server)
{
    add_uri_get_handler( _server, "/lcd", lcd2004_get_handler, NULL); 
}

#endif