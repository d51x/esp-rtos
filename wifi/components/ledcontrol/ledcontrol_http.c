#include "ledcontrol_http.h"

#ifdef CONFIG_LED_CONTROL_HTTP

const char *html_block_led_control_start ICACHE_RODATA_ATTR = "<div class='group rnd'>"
                                    "<h4 class='brd-btm'>Led control:</h4>";

const char *html_block_led_control_end ICACHE_RODATA_ATTR = "</div>";

const char *html_block_led_control_data_start ICACHE_RODATA_ATTR = "<div class=\"ledc\">";
                                             

const char *html_block_led_control_item ICACHE_RODATA_ATTR = 
    "<p>"
        "<span><b>%s</b></span>"                 // s - title,
        //"<span><input type=\"range\" max=\"255\" name=\"ledc%d\" value=\"%d\"></span>"          // slider, d - channel id, d - duty
        "<span><input type=\"range\" max=\"255\" name=\"ledc%d\" value=\"%d\" data-uri=\"ledc?ch=%d&duty=\" onchange=\"slider(this.value, this.name, this.dataset.uri);\">"
        "<span><i id=\"ledc%d\" >%d</i></span>"              // d - channel id, d - duty
    "</p>"; 

static const char* TAG = "LEDCHTTP";

// todo - title + указать номера каналов
static void ledcontrol_print_data(char *data, void *args)
{
    ledcontrol_handle_t ledc_h = (ledcontrol_handle_t)args;
    ledcontrol_t *ledc = (ledcontrol_t *)ledc_h;
    
    strcpy(data+strlen(data), html_block_led_control_start);
    strcpy(data+strlen(data), html_block_led_control_data_start);

    for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {   
        ledcontrol_channel_t *ch = ledc->channels + i;
        sprintf( data+strlen(data), html_block_led_control_item
                                      , ch->name
                                      , ch->channel                                   // channel num
                                      , ch->duty              // channel duty    
                                      , ch->channel     // for data-uri                                  
                                      , ch->channel                                   // channel num
                                      , ch->duty              // channel duty
                                      );
    }
    
    strcpy(data+strlen(data), html_block_led_control_end);
}

void ledcontrol_register_http_print_data(ledcontrol_handle_t dev_h)
{
    register_print_page_block( "ledc_data", PAGES_URI[ PAGE_URI_ROOT], 5, ledcontrol_print_data, dev_h, NULL, NULL );
}

esp_err_t ledcontrol_get_handler(httpd_req_t *req)
{
    /*
    ip/ledc?ch=<channel>&duty=<duty>
    ip/ledc?ch=<channel>&on=1
    ip/ledc?ch=<channel>&off=1
    ip/ledc?ch=<channel>&step=<step>   // next duty
    ip/ledc?ch=<channel>&fade=1&from=<duty_from>&to=<duty_to>&delay=<duty_delay>
    ip/ledc?allon=1
    ip/ledc?alloff=1
    ip/ledc?ch=<channel> - print channel duty
    ip/ledc?all=1 - print all channels duty
*/
    esp_err_t err = ESP_FAIL;
    // check params
    char page[512] = ""; 
	if ( http_get_has_params(req) == ESP_OK) 
	{
        user_ctx_t *ctx = req->user_ctx;
        ledcontrol_handle_t ledc_h = (ledcontrol_handle_t)ctx->args;
        ledcontrol_t *ledc = (ledcontrol_t *)ledc_h;


        char param[100];
        if ( http_get_key_str(req, "ch", param, sizeof(param)) == ESP_OK ) 
        {
            err = ESP_OK;
            uint8_t ch = atoi(param);
            if ( ch < 0 || ch > ledc->led_cnt /*LEDCONTROL_CHANNEL_MAX*/ ) 
            {
                err = ESP_FAIL;
                strcpy(page, "ERROR");
                goto end;
            }    

            ledcontrol_channel_t *channel = ledc->channels + ch;
            long val = 0;

            if ( http_get_key_long(req, "duty", &val) == ESP_OK ) 
            {
                // channel->set_duty
                if ( val >= 0 && val <= MAX_DUTY ) 
                {
                    err = ledc->set_duty( channel, val );
                    ledc->update();
                }
            } 
            else if ( http_get_key_long(req, "on", &val) == ESP_OK ) 
            {
                // channel > on
                if ( val == 1) 
                    err = ledc->on(channel);
            } 
            else if ( http_get_key_long(req, "off", &val) == ESP_OK ) 
            {
                // channel > off
                if ( val == 1) 
                    err = ledc->off(channel);
            } 
            else if ( http_get_key_long(req, "step", &val) == ESP_OK ) 
            {
                if ( val > 0 ) 
                    err = ledc->next_duty(channel, val);    
                else if ( val < 0 ) 
                    err = ledc->prev_duty(channel, val*(-1));   
            } 
            else if ( http_get_key_long(req, "fade", &val) == ESP_OK ) 
            {
                // channel > fade
                long from, to, delay;
                if ( http_get_key_long(req, "from", &from) == ESP_OK &&
                     http_get_key_long(req, "to", &to) == ESP_OK &&
                     http_get_key_long(req, "delay", &delay) == ESP_OK )
                {
                    err = ledc->fade( channel, from, to, delay);
                }
            }

            if ( err == ESP_OK )
            {
                itoa( ledc->get_duty( channel), page, 10);
            }
            else
                strcpy(page, "ERROR");
        } 
        else if ( http_get_key_str(req, "allon", param, sizeof(param)) == ESP_OK ) 
        {
            ledc->on_all();
            strcpy(page, "OK");
        } 
        else if ( http_get_key_str(req, "alloff", param, sizeof(param)) == ESP_OK ) 
        {
            ledc->off_all();
            strcpy(page, "OK");
        }            
        else if ( http_get_key_str(req, "all", param, sizeof(param)) == ESP_OK )
        {
            // print all duties and channels
            strcpy(page, "{");
            for (uint8_t i = 0; i < ledc->led_cnt; i++ ) {             
                char s[12];
                
                ledcontrol_channel_t *ch = ledc->channels + i;
                uint8_t val = ledc->get_duty( ch );                
                sprintf(s, "\"ch%d\": %d", i, val);
                strcat(page, s);
                if ( i < ledc->led_cnt-1 ) 
                    strcat(page, ", ");                
            }
            strcpy(page+strlen(page), "}");
        }
        
    }
end:
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
	httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;    
}

void ledcontrol_register_http_handler(httpd_handle_t _server, ledcontrol_handle_t dev_h)
{
    user_ctx_t *ctx = (user_ctx_t *)calloc(1, sizeof(user_ctx_t));
    ctx->args = dev_h;
    add_uri_get_handler( _server, LED_CONTROL_URI, ledcontrol_get_handler, ctx); 
}

void ledcontrol_http_init(httpd_handle_t _server, ledcontrol_handle_t dev_h)
{
    ledcontrol_register_http_print_data(dev_h);  
    ledcontrol_register_http_handler(_server, dev_h);
}

#endif