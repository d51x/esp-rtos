#include "user.h"

void user_setup(void *args)
{
    ESP_LOGW("user", __func__ );

}

void user_loop(uint32_t sec)
{

}

void user_web_main(char *data, void *args)
{
 strcat(data, "HelloQ");
}

void user_web_options(char *data, void *args)
{
    ESP_LOGW("user", __func__ );
    sprintf(data + strlen(data), "Hello User Options!");
}

void user_process_param(httpd_req_t *req, void *args)
{
    ESP_LOGW("user", __func__ );
    ESP_LOGW("user", "Hello User process param!");
}
