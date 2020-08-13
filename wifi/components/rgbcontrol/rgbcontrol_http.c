#include "rgbcontrol_http.h"

#ifdef CONFIG_RGB_CONTROLLER_HTTP

static const char* TAG = "RGBHTTP";

extern const char *html_block_rgb_control_start ICACHE_RODATA_ATTR = 
    "<div class='group rnd'>"
        "<h4 class='brd-btm'>Color Effects:</h4>";      

const char *html_block_rgb_control_end ICACHE_RODATA_ATTR = 
    "</div>";    

const char *effects_data_start ICACHE_RODATA_ATTR = 
    "<div class='ef'>"
            "<p><span><b>HSV color:</b> %d %d %d</span></p>"
            "<p><span><b>RGB color:</b> %d %d %d</span></p>"  
        "<div style=\"height: 50pxfloat: right;width:  50%%;background: rgb(%d,%d,%d)\"></div>";

const char *effects_data_select_item ICACHE_RODATA_ATTR = "</div>";

 #ifdef CONFIG_RGB_EFFECTS
const char *effects_select_start ICACHE_RODATA_ATTR =         
            "<p><span><b>Color effect:</b></span>"
                "<select id=\"effects\" onchange=\"effects()\">";

const char *effects_select_end ICACHE_RODATA_ATTR = "</select></p>";

const char *effects_item ICACHE_RODATA_ATTR = "<option value=\"%d\" %s>%s</option>";
#endif



const char *effects_data_end ICACHE_RODATA_ATTR = "</div>";



static void rgbcontrol_print_data(char *data, void *args)
{
    rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)args;

    #ifdef CONFIG_RGB_EFFECTS
    effects_t *ee = rgb_ctrl->effects;
    ESP_LOGW(TAG, "rgb_ctrl->effects %p", rgb_ctrl->effects);
    if ( ee == NULL ) return;    
    #endif

    color_rgb_t rgb;

    rgb.r = rgb_ctrl->ledc->get_duty( rgb_ctrl->ledc->channels + rgb_ctrl->red.channel);
    rgb.g = rgb_ctrl->ledc->get_duty( rgb_ctrl->ledc->channels + rgb_ctrl->green.channel);
    rgb.b = rgb_ctrl->ledc->get_duty( rgb_ctrl->ledc->channels + rgb_ctrl->blue.channel);

    color_hsv_t **hsv;
    rgb_to_hsv(&rgb, &rgb_ctrl->hsv);


    strcat(data, html_block_rgb_control_start);
    sprintf(data+strlen(data), effects_data_start
                                , rgb_ctrl->hsv.h
                                , rgb_ctrl->hsv.s
                                , rgb_ctrl->hsv.v
                                , rgb.r
                                , rgb.g
                                , rgb.b
                                , rgb.r
                                , rgb.g
                                , rgb.b
                                ); 

    #ifdef CONFIG_RGB_EFFECTS
    strcat(data, effects_select_start);
    for (int i=0; i < COLOR_EFFECTS_MAX; i++ ) 
    {
        effect_t *e = ee->effect + i;
        sprintf(data+strlen(data), effects_item
                                        , i
                                        , (ee->effect_id == i || ( i == COLOR_EFFECTS_MAX-1 && ee->effect_id == -1) ) ? "selected=\"selected\" " : ""
                                        , e->name);
    }
    strcat(data, effects_select_end);
    effect_t *e = ee->effect + ee->effect_id;
    #endif

    strcat(data, effects_data_end);
    strcat(data, html_block_rgb_control_end);

}

void rgbcontrol_register_http_print_data(rgbcontrol_handle_t dev_h)
{
    register_print_page_block( "rgb", PAGES_URI[ PAGE_URI_ROOT], 7, rgbcontrol_print_data, dev_h, NULL, NULL );
}

static esp_err_t http_process_rgb(httpd_req_t *req, char *param, size_t size)
{
    user_ctx_t *ctx = req->user_ctx;
    rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)ctx->args;

    esp_err_t err = http_get_key_str(req, "rgb", param, size);
    if (  err != ESP_OK ) {
        ESP_LOGE(TAG, "get key param of rgb ERROR");
        return err;
    }
    char *istr = strtok (param,",");
    color_rgb_t *rgb = malloc(sizeof(color_rgb_t));
    rgb->r = atoi(istr);
    istr = strtok (NULL,",");
    rgb->g = atoi(istr); 
    istr = strtok (NULL,",");
    rgb->b = atoi(istr);

    #ifdef CONFIG_RGB_EFFECTS
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();
    #endif

    rgb_ctrl->set_color_rgb(*rgb);
    free(rgb);                
    return ESP_OK;
}

static esp_err_t http_process_rgb2(httpd_req_t *req)
{
    user_ctx_t *ctx = req->user_ctx;
    rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)ctx->args;

    esp_err_t err = ESP_FAIL;
    color_rgb_t *rgb = malloc(sizeof(color_rgb_t));
    if ( http_get_key_uint8(req, "r", &rgb->r) == ESP_OK &&
         http_get_key_uint8(req, "g", &rgb->g) == ESP_OK &&
         http_get_key_uint8(req, "b", &rgb->b) == ESP_OK) 
    {

        #ifdef CONFIG_RGB_EFFECTS
        effects_t *ef = (effects_t *) rgb_ctrl->effects;
        if ( ef != NULL ) ef->stop();
        #endif

        rgb_ctrl->set_color_rgb(*rgb);
        err = ESP_OK;
    }
    free(rgb);
    return err;
}

static esp_err_t http_process_hsv(httpd_req_t *req, char *param, size_t size) 
{
    user_ctx_t *ctx = req->user_ctx;
    rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)ctx->args;

    esp_err_t err = http_get_key_str(req, "hsv", param, size);
    if (  err != ESP_OK ) {
        ESP_LOGE(TAG, "get key param of hsv ERROR");
        return err;
    }
    char *istr = strtok (param,",");
    color_hsv_t *hsv = malloc( sizeof(color_hsv_t));
    hsv->h = atoi(istr);
    istr = strtok (NULL,",");
    hsv->s = atoi(istr);
    istr = strtok (NULL,",");
    hsv->v = atoi(istr);

    #ifdef CONFIG_RGB_EFFECTS
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();
    #endif

    rgb_ctrl->set_color_hsv(*hsv);
    err = ESP_OK;
    free(hsv);
    return ESP_OK;
}

static esp_err_t http_process_hsv2(httpd_req_t *req) 
{
    user_ctx_t *ctx = req->user_ctx;
    rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)ctx->args;

    esp_err_t err = ESP_FAIL;
    color_hsv_t *hsv = malloc( sizeof(color_hsv_t));
    if ( http_get_key_uint16(req, "h", &hsv->h) == ESP_OK &&
            http_get_key_uint8(req, "s", &hsv->s) == ESP_OK &&
            http_get_key_uint8(req, "v", &hsv->v) == ESP_OK) 
    {
        #ifdef CONFIG_RGB_EFFECTS
        effects_t *ef = (effects_t *) rgb_ctrl->effects;
        if ( ef != NULL ) ef->stop();
        #endif

        rgb_ctrl->set_color_hsv(*hsv);
        err = ESP_OK;
    }
    free(hsv);
    return err;
}

static esp_err_t http_process_hex(httpd_req_t *req, char *param, size_t size) 
{
    
    user_ctx_t *ctx = req->user_ctx;
    rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)ctx->args;
    
    esp_err_t err = http_get_key_str(req, "val", param, size);
    if ( err != ESP_OK ) return err;
    
    #ifdef CONFIG_RGB_EFFECTS
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();     
    #endif

    rgb_ctrl->set_color_hex(param);
    return ESP_OK;
}

static esp_err_t http_process_int(httpd_req_t *req) 
{
    user_ctx_t *ctx = req->user_ctx;
    rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)ctx->args;

    long color;
    esp_err_t err = http_get_key_long(req, "val", &color);
    if ( err != ESP_OK ) return err;

    #ifdef CONFIG_RGB_EFFECTS
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();       
    #endif
    
    rgb_ctrl->set_color_int(color);
    return ESP_OK;
}

esp_err_t rgbcontrol_get_handler(httpd_req_t *req)
{
    // handle http get request
    char page[200] = "";
    esp_err_t err = ESP_FAIL;

    if ( http_get_has_params(req) == ESP_OK) 
    {
        user_ctx_t *ctx = req->user_ctx;
        rgbcontrol_t *rgb_ctrl = (rgbcontrol_t *)ctx->args;

        char param[30];

        if ( http_get_key_str(req, "rgb", param, sizeof(param)) == ESP_OK ) 
        {
            //  rgb?rgb=r,g,b  
            err = http_process_rgb(req, param, sizeof(param));
        }
        else if ( http_get_key_str(req, "hsv", param, sizeof(param)) == ESP_OK ) 
        {
            // rgb?hsv=h,s,v
            err = http_process_hsv(req, param, sizeof(param));
        }    
        else if ( http_get_key_str(req, "type", param, sizeof(param)) == ESP_OK ) 
        {
            if ( strcmp(param, "rgb") == ESP_OK ) 
            {
                err = http_process_rgb2(req); // rgb?type=rgb&r=r&g=g&b=b
            } 
            else if ( strcmp(param, "hsv") == ESP_OK ) 
            {
                err = http_process_hsv2(req); // rgb?type=hsv&h=h&s=s&v=v
            } 
            else if ( strcmp(param, "int") == ESP_OK ) 
            {
                err = http_process_int(req); // rgb?type=int&val=value
            } 
            else if ( strcmp(param, "hex") == ESP_OK ) 
            {
                err = http_process_hex(req, param, sizeof(param));  // rgb?type=hex&val=value
            } 
            #ifdef CONFIG_RGB_EFFECTS
            else if ( strcmp(param, "effect") == ESP_OK ) 
            {
                if ( http_get_key_str(req, "id", param, sizeof(param)) == ESP_OK ) 
                {
                    effects_t *ef = (effects_t *) rgb_ctrl->effects;
                    if ( ef != NULL ) {
                        ef->set( atoi(param) );
                    }    
                } 
                else if ( http_get_key_str(req, "name", param, sizeof(param)) == ESP_OK ) 
                {
                    effects_t *ef = (effects_t *) rgb_ctrl->effects;
                    if ( ef != NULL )                   
                        ef->set_by_name( param );
                }
            }
            #endif
        }

        strcpy( page, (err == ESP_OK ) ? "OK" : "ERROR");  

    } 

    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;        
}

void rgbcontrol_register_http_handler(httpd_handle_t _server, rgbcontrol_handle_t dev_h)
{
    user_ctx_t *ctx = (user_ctx_t *)calloc(1, sizeof(user_ctx_t));
    ctx->args = dev_h;
    add_uri_get_handler( _server, RGB_CONTROL_URI, rgbcontrol_get_handler, ctx); 
}

void rgbcontrol_http_init(httpd_handle_t _server, rgbcontrol_handle_t dev_h)
{
    rgbcontrol_register_http_print_data(dev_h);
    rgbcontrol_register_http_handler(_server, dev_h);
}


#endif