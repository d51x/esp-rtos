#include "user.h"

void user_setup(void *args)
{
    ESP_LOGW("user", __func__ );

}

void user_loop(uint32_t sec)
{

}

void user_web_main(http_args_t *args)
{
    ESP_LOGW("user", __func__ );
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    httpd_resp_sendstr_chunk(req, html_block_data_no_header_start);
    httpd_resp_sendstr_chunk(req, "Hello!");

    char data[20];
    sprintf(data, "<br>Temp: %2.1f", sht21_get_temp());
    httpd_resp_sendstr_chunk(req, data);
    httpd_resp_sendstr_chunk(req, "<br>");

    mcp23017_print_button(mcp23017_h, req, "mcp13", 13);
    relay_print_button(req, "btn1", 1);
    relay_print_button(req, "btn2", 2);
    relay_print_button(req, "btn3", 3);

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
