#include "user.h"

static const char *TAG = "USER";


void user_setup(void *args)
{
    ESP_LOGW(TAG, __func__ );
    
    // if ( softuart_open(0, 9600, 0 /*RX*/, 2 /*TX*/) ) {
    //     ESP_LOGW(TAG, "SoftUart0 opened" );
    // } else {
    //     ESP_LOGE(TAG, "SoftUart0 open ERROR" );
    // }
    ESP_LOGW(TAG, "http_handlers_count = %d", http_handlers_count );
    http_handlers_count++;
    ESP_LOGW(TAG, "http_handlers_count = %d", http_handlers_count );
}

void user_loop(uint32_t sec)
{
    // // Set Addr: B4 C0 A8 01 01 00 1E
    // uint8_t cmd[7] = { 0xB4, 0xC0, 0xA8, 0x01, 0x01, 0x00, 0x1E};
    
    // //if ( softuart_puts(0, cmd)) 
    // if ( softuart_write_bytes(0, cmd, 7)) 
    // {
    //     ESP_LOGW(TAG, "SoftUart0 softuart_puts OK" );
    // } 
    // else 
    // {
    //     ESP_LOGE(TAG, "SoftUart0 softuart_puts ERROR" );
    // }
    
    // //pauseTask(50);
    
    // uint8_t readed = 0;
    // char buf[7];
    // readed = softuart_read_buf(0, buf, 7);


    // if ( readed == 0 ) 
    // {
    //     ESP_LOGE(TAG, "SoftUart0 no data available" );
    // } 
    // else 
    // {
    //     ESP_LOGW(TAG, "SoftUart0 readed %d, buf: ", readed );
    //     for ( uint8_t i=0; i < readed; i++)
    //     {
    //         ESP_LOGW(TAG, "0x%02X ", buf[i] );
    //     }
        
    // }
}

void user_web_main(http_args_t *args)
{
    ESP_LOGW(TAG, __func__ );
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    httpd_resp_sendstr_chunk(req, html_block_data_no_header_start);
    httpd_resp_sendstr_chunk(req, "Hello!");

    //char data[20];
    //sprintf(data, "<br>Temp: %2.1f", sht21_get_temp());
    //httpd_resp_sendstr_chunk(req, data);
    //httpd_resp_sendstr_chunk(req, "<br>");

    //mcp23017_print_button(mcp23017_h, req, "mcp13", 13);
    //relay_print_button(req, "btn1", 1);
    //relay_print_button(req, "btn2", 2);
    //relay_print_button(req, "btn3", 3);

    httpd_resp_sendstr_chunk(req, html_block_data_end);
}

void user_web_options(http_args_t *args)
{
    ESP_LOGW("user", __func__ );
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;
        
    char *buf = malloc( strlen(html_block_data_header_start) + 20 );
    sprintf(buf, html_block_data_header_start, "User options");
    httpd_resp_sendstr_chunk(req, buf);
    free(buf);        
    httpd_resp_sendstr_chunk(req, "Hello User Options!");
    httpd_resp_sendstr_chunk(req, html_block_data_end);  

}

void user_process_param(httpd_req_t *req, void *args)
{
    ESP_LOGW("user", __func__ );
    ESP_LOGW("user", "Hello User process param!");
}

void sensors_print(http_args_t *args)
{
    ESP_LOGW(TAG, __func__ );
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;
        
      
    
}

esp_err_t sensors_get_handler(httpd_req_t *req)
{
    bool vsens = false;
    //if ( http_get_has_params(req) == ESP_OK) 
	//{
    char param[2];
    vsens = ( http_get_key_str(req, "m", param, sizeof(param)) == ESP_OK );
    //}
    if ( !vsens )
    {
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    }


    char *buf = malloc( 200 );
    sprintf(buf, "hostname:%s;", wifi_cfg->hostname);
    //httpd_resp_sendstr_chunk(req, buf);

    pzem_data_t pzem_data = pzem_get_data();
    sprintf(buf+strlen(buf), "pmv:%.1f;", pzem_data.voltage);
    //httpd_resp_sendstr_chunk(req, buf);

    sprintf(buf+strlen(buf), "pmc:%.1f;", pzem_data.current);
    //httpd_resp_sendstr_chunk(req, buf);

    sprintf(buf+strlen(buf), "pmw:%d;", (uint32_t)pzem_data.power);
    //httpd_resp_sendstr_chunk(req, buf);

    sprintf(buf+strlen(buf), "pmwh:%d;", (uint32_t)pzem_data.energy);
    //httpd_resp_sendstr_chunk(req, buf);
    
    if ( vsens )
        httpd_send(req, buf, strlen(buf));
    else
        httpd_resp_send(req, buf, strlen(buf));
    
    //httpd_resp_end(req);

    free(buf);  

    return ESP_OK;
}