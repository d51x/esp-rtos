#include "irrcv_http.h"

#ifdef CONFIG_IR_RECV_HTTP

static const char* TAG = "IRRCV";

const char *IRRCV_TITLE ICACHE_RODATA_ATTR = "IR Receiver";
const char *IRRCV_URI ICACHE_RODATA_ATTR = "/ir";
const char *block_1 ICACHE_RODATA_ATTR = "ir_data1";
const char *block_2 ICACHE_RODATA_ATTR = "ir_data2";
const char *block_3 ICACHE_RODATA_ATTR = "ir_data3";

const char *param_st ICACHE_RODATA_ATTR = "ir";
const char *param_iren ICACHE_RODATA_ATTR = "iren";
const char *param_irpin ICACHE_RODATA_ATTR = "irpin";
const char *param_irdelay ICACHE_RODATA_ATTR = "irdelay";
const char *label_ir_en ICACHE_RODATA_ATTR = "Включен";
const char *label_ir_gpio ICACHE_RODATA_ATTR = "IR GPIO";
const char *label_ir_delay ICACHE_RODATA_ATTR = "Rcv delay (ms)";

const char *string_ir_code ICACHE_RODATA_ATTR = "IR code: 0x%08X";

static void irrcv_print_data(char *data, void *args)
{
    irrcv_handle_t dev_h = (irrcv_handle_t)args;
    irrcv_t *dev = (irrcv_t *) dev_h;    
    sprintf(data + strlen(data), html_block_data_start, IRRCV_TITLE);
     
    strcat(data, html_block_data_end);

}

static void irrcv_print_cfg(char *data, void *args)
{
    irrcv_handle_t dev_h = (irrcv_handle_t)args;
    irrcv_t *dev = (irrcv_t *) dev_h;    
    sprintf(data + strlen(data), html_block_data_start, IRRCV_TITLE);
    strcat(data, html_block_data_form_start);
    
    // TODO:enable checkbox
    sprintf(data + strlen(data), html_block_data_form_item_checkbox
                                , label_ir_en // %s label
                                , param_iren   // %s name
                                , dev->enabled  // %d value
                                , dev->enabled ? "checked" : ""
                                );
    sprintf(data + strlen(data), html_block_data_form_item_label_edit
                                , label_ir_gpio // %s label
                                , param_irpin   // %s name
                                , dev->pin  // %d value
                                );

    sprintf(data + strlen(data), html_block_data_form_item_label_edit
                                , label_ir_delay // %s label
                                , param_irdelay   // %s name
                                , dev->delay  // %d value
                                );
    sprintf(data + strlen(data), html_block_data_form_submit
                                , param_st // %s st
                                );
    strcat(data, html_block_data_form_end);
    strcat(data, html_block_data_end);
}

static void irrcv_print_code(char *data, void *args)
{
    irrcv_handle_t dev_h = (irrcv_handle_t)args;
    irrcv_t *dev = (irrcv_t *) dev_h;    
    sprintf(data + strlen(data), string_ir_code, dev->code);
}

static void irrcv_process_param(httpd_req_t *req, void *args)
{
    ESP_LOGW(TAG, __func__);
    // check params
    irrcv_handle_t dev_h = (irrcv_handle_t)args;
    irrcv_t *dev = (irrcv_t *) dev_h; 

    ESP_LOGW(TAG, "pin: %d", dev->pin);
    ESP_LOGW(TAG, "delay: %d", dev->delay);

	if ( http_get_has_params(req) == ESP_OK) 
	{
        char param[100];
        if ( http_get_key_str(req, "st", param, sizeof(param)) == ESP_OK ) {
            if ( strcmp(param, param_st) != 0 ) {
                return;	
            }
        } 
        
        if ( http_get_key_str(req, param_iren, param, sizeof(param)) == ESP_OK ) {
            dev->enabled = 1;
        }  else {
            dev->enabled = 0;
        }        

        if ( http_get_key_str(req, param_irpin, param, sizeof(param)) == ESP_OK ) {
            dev->pin = atoi(param);
        }  else {
            dev->pin = IR_RECEIVE_GPIO;
        }

        if ( http_get_key_str(req, param_irdelay, param, sizeof(param)) == ESP_OK ) {
            dev->delay = atoi(param);
        }  else {
            dev->delay = IR_RECEIVE_DELAY;
        }   

        irrcv_save_cfg( dev_h );
        if ( dev->enabled )
            irrcv_restart(dev_h);
        else    
            irrcv_stop(dev_h);
        
    }     
}

static void irrcv_register_http_print_data(irrcv_handle_t irrcv) 
{
    register_print_page_block( block_1, IRRCV_URI, 3, irrcv_print_data, irrcv, NULL, NULL );
    register_print_page_block( block_2, PAGES_URI[ PAGE_URI_TOOLS ], 4, irrcv_print_cfg, irrcv, irrcv_process_param, irrcv );
    register_print_page_block( block_3, PAGES_URI[ PAGE_URI_DEBUG ], 4, irrcv_print_code, irrcv, NULL, NULL );
}

static esp_err_t irrcv_get_handler(httpd_req_t *req)
{
    char page[PAGE_DEFAULT_BUFFER_SIZE] = "";    

    show_http_page( req, page);

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
	httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;   
}

static void irrcv_register_http_handler(httpd_handle_t _server, irrcv_handle_t irrcv)
{
    user_ctx_t *ctx = (user_ctx_t *) calloc(1, sizeof(user_ctx_t));
    strncpy(ctx->title, IRRCV_TITLE, 20);
    ctx->show = true;       
    ctx->args = irrcv;

    add_uri_get_handler( _server, IRRCV_URI, irrcv_get_handler, ctx); 
    //free(ctx); // не освобождаем, т.к. идет присвоение указателя на этот
}

void irrcv_http_init(httpd_handle_t _server, irrcv_handle_t irrcv)
{
    irrcv_register_http_print_data(irrcv);  
    irrcv_register_http_handler(_server, irrcv);
    register_http_page_menu( IRRCV_URI, "IR");
}

#endif