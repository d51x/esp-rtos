#include "rgbcontrol.h"

static const char *TAG = "RGBCTRL";

volatile  static rgbcontrol_t *rgb_ctrl = NULL;


void rgbcontrol_set_color_hsv(color_hsv_t hsv);  
void rgbcontrol_set_color_rgb(color_rgb_t rgb);  
void rgbcontrol_set_color_int(uint32_t color32);  
void rgbcontrol_set_color_hex(const char *hex);  

void rgbcontrol_set_brightness(int8_t value);
void rgbcontrol_fade_brightness(int8_t brightness_from, int8_t brightness_to, int16_t brightness_delay);
void rgbcontrol_inc_brightness(int8_t step);
void rgbcontrol_dec_brightness(int8_t step);

void rgbcontrol_set_saturation(int8_t value);
void rgbcontrol_fade_saturation(int8_t saturation_from, int8_t saturation_to, int16_t saturation_delay);
void rgbcontrol_inc_saturation(int8_t step);
void rgbcontrol_dec_saturation(int8_t step);

void rgbcontrol_set_effects(effects_t *effects);

void _rgbcontrol_set_color_rgb(color_rgb_t rgb, bool update);
void _rgbcontrol_set_color_hsv(color_hsv_t hsv, bool update);

void rgbcontrol_print_html_data(char *data);
esp_err_t rgbcontrol_http_get_handler(httpd_req_t *req);

rgbcontrol_t* rgbcontrol_init(ledcontrol_t *ledc, ledcontrol_channel_t *red, ledcontrol_channel_t *green, ledcontrol_channel_t *blue)
{
    if ( ledc == NULL ) {
        ESP_LOGE(TAG, "LED Controller is not initialized yet.");
        return NULL;
    }
    
    if ( red == NULL || green == NULL || blue == NULL ) {
        ESP_LOGE(TAG, "One of color channels is NULL");
        return NULL;
    }

    rgb_ctrl = calloc(1, sizeof(rgbcontrol_t));
    rgb_ctrl->ledc = ledc;

    //memcpy(&rgb_ctrl->red, &red, sizeof(ledcontrol_channel_t));
    //memcpy(&rgb_ctrl->green, &green, sizeof(ledcontrol_channel_t));
    //memcpy(&rgb_ctrl->blue, &blue, sizeof(ledcontrol_channel_t));
    rgb_ctrl->red = *red;
    rgb_ctrl->green = *green;
    rgb_ctrl->blue = *blue;

    rgb_ctrl->hsv.h = 0;
    rgb_ctrl->hsv.s = 0;
    rgb_ctrl->hsv.v = 0;

    rgb_ctrl->fade_delay = RGB_DEFAULT_FADE;
    rgb_ctrl->fadeup_delay = RGB_DEFAULT_FADEUP;
    rgb_ctrl->fadedown_delay = RGB_DEFAULT_FADEDOWN;
    rgb_ctrl->effect_id = COLOR_EFFECTS_MAX-1;

	// указатели на функции
	rgb_ctrl->set_color_hsv = rgbcontrol_set_color_hsv;
	rgb_ctrl->set_color_rgb = rgbcontrol_set_color_rgb;
	rgb_ctrl->set_color_int = rgbcontrol_set_color_int;
	rgb_ctrl->set_color_hex = rgbcontrol_set_color_hex;

    rgb_ctrl->set_brightness = rgbcontrol_set_brightness;
    rgb_ctrl->fade_brightness = rgbcontrol_fade_brightness;
    rgb_ctrl->inc_brightness = rgbcontrol_inc_brightness;
    rgb_ctrl->dec_brightness = rgbcontrol_dec_brightness;

    rgb_ctrl->set_saturation = rgbcontrol_set_saturation;
    rgb_ctrl->fade_saturation = rgbcontrol_fade_saturation;
    rgb_ctrl->inc_saturation = rgbcontrol_inc_saturation;
    rgb_ctrl->dec_saturation = rgbcontrol_dec_saturation;

    rgb_ctrl->set_effects = rgbcontrol_set_effects;

    rgb_ctrl->print_html_data = rgbcontrol_print_html_data;
    strcpy(rgb_ctrl->uri, RGB_URI);
    rgb_ctrl->http_get_handler = rgbcontrol_http_get_handler; 

    return rgb_ctrl;
}

void _rgbcontrol_set_color_hsv(color_hsv_t hsv, bool update) {
    color_rgb_t *rgb = malloc( sizeof(color_rgb_t));
	hsv_to_rgb(rgb, hsv);
    _rgbcontrol_set_color_rgb(*rgb, update);
    memcpy(&rgb_ctrl->hsv, &hsv, sizeof(color_hsv_t));
	free(rgb);
}

void rgbcontrol_set_color_hsv(color_hsv_t hsv) {
    _rgbcontrol_set_color_hsv(hsv, true);
}

void _rgbcontrol_set_color_rgb(color_rgb_t rgb, bool update) {
    rgb_to_hsv(&rgb, &rgb_ctrl->hsv);
    rgb_ctrl->ledc->set_duty( rgb_ctrl->ledc->channels + rgb_ctrl->red.channel,   rgb.r );
    rgb_ctrl->ledc->set_duty( rgb_ctrl->ledc->channels + rgb_ctrl->green.channel, rgb.g );
    rgb_ctrl->ledc->set_duty( rgb_ctrl->ledc->channels + rgb_ctrl->blue.channel,  rgb.b );
    if ( update ) rgb_ctrl->ledc->update();	  
}

void rgbcontrol_set_color_rgb(color_rgb_t rgb) {
    _rgbcontrol_set_color_rgb(rgb, true); 
}

void rgbcontrol_set_color_int(uint32_t color32) {
	color_rgb_t *rgb = malloc( sizeof(color_rgb_t));
	int_to_rgb( color32, rgb);
	rgbcontrol_set_color_rgb(*rgb);
	free(rgb);

    char topic[12] = MQTT_TOPIC_COLOR_INT;
    char payload[10];
    itoa(color32, payload, 10);
    rgb_ctrl->ledc->mqtt_send(topic, payload);
}

void rgbcontrol_set_color_hex(const char *hex) {
	color_rgb_t *rgb = malloc( sizeof(color_rgb_t));
	hex_to_rgb( hex, rgb);
	rgbcontrol_set_color_rgb(*rgb);
	free(rgb);
}

void rgbcontrol_set_brightness(int8_t value){
    if ( value > VAL_MAX ) rgb_ctrl->hsv.v = VAL_MAX;
    else if ( value < 0 ) rgb_ctrl->hsv.v = 0;
    else  rgb_ctrl->hsv.v = value;
    rgbcontrol_set_color_hsv( rgb_ctrl->hsv );
}

void rgbcontrol_inc_brightness(int8_t step){
    int8_t val = rgb_ctrl->hsv.v;
    if ( val + step > VAL_MAX ) val = VAL_MAX;
    else val += step;
    rgbcontrol_set_brightness( val );
}

void rgbcontrol_dec_brightness(int8_t step){
    int8_t val = rgb_ctrl->hsv.v;
    if ( val - step < 0 ) val = 0;
    else val -= step;
    rgbcontrol_set_brightness( val );
}

void rgbcontrol_set_saturation(int8_t value){
    if ( value > SAT_MAX ) rgb_ctrl->hsv.s = SAT_MAX;
    else if ( value < 0 ) rgb_ctrl->hsv.s = 0;
    else rgb_ctrl->hsv.s = value;
    rgbcontrol_set_color_hsv( rgb_ctrl->hsv );
}

void rgbcontrol_inc_saturation(int8_t step){;
    int8_t val = rgb_ctrl->hsv.s;
    if ( val + step > SAT_MAX ) val = SAT_MAX;
    else val += step;
    rgbcontrol_set_saturation( val );
}

void rgbcontrol_dec_saturation(int8_t step){
    int8_t val = rgb_ctrl->hsv.s;
    if ( val - step < 0 ) val = 0;
    else val -= step;
    rgbcontrol_set_saturation( val );
}

void rgbcontrol_fade_brightness(int8_t brightness_from, int8_t brightness_to, int16_t brightness_delay){
    direction_e direction = (brightness_from < brightness_to) ? UP : DOWN;
    int8_t brightness = brightness_from;

    while ( 
            ((direction == UP) && ( brightness <= brightness_to)) ||
            ((direction == DOWN) && ( brightness >= brightness_to))
          )  
    {
        rgbcontrol_set_brightness( brightness );
        if ( direction == UP )
            brightness++;   // TODO: учесть brightness table
        else
            brightness--;  // TODO: учесть brightness table

        vTaskDelay( brightness_delay / portTICK_RATE_MS );
    }  
}

void rgbcontrol_fade_saturation(int8_t saturation_from, int8_t saturation_to, int16_t saturation_delay){
    direction_e direction = (saturation_from < saturation_to) ? UP : DOWN;
    int8_t saturation = saturation_from;
    while ( 
            ((direction == UP) && ( saturation <= saturation_to)) ||
            ((direction == DOWN) && ( saturation >= saturation_to))
          )  
    {
        rgbcontrol_set_saturation( saturation );
        if ( direction == UP )
            saturation++;   // TODO: учесть brightness table
        else
            saturation--;  // TODO: учесть brightness table

        vTaskDelay( saturation_delay / portTICK_RATE_MS );
    }  
}

void rgbcontrol_set_effects(effects_t *effects){
    rgb_ctrl->effects = effects;
}

void rgbcontrol_print_html_data(char *data){

    effects_t *ee = rgb_ctrl->effects;
    if ( ee == NULL ) return;

    const char *effects_data = "<div class=\"effect\" style=\"display: flow-root;\">"
                                
                                "<div style=\"display: inline-block; width: 50%%;\">"
                                  "<p><span><b>HSV color:</b> %d %d %d</span></p>"
                                    "<p><span><b>RGB color:</b> %d %d %d</span></p>"  
                                "</div>"            
                                "<div style=\"height: 50px;border: 1px solid grey;float: right;display: inline-block;width:  50%%;background: rgb(%d,%d,%d)\"></div>"

                                "<div><p><span><b>Color effect:</b></span>"
                                //"<span id=\"color\">%s (%d)</span>"
                                "<select id=\"effects\" onchange=\"effects()\">"
                                "%s"
                                "</select>"
                                "</p></div>"
                                "</div>";

    const char *effects_item = "<option value=\"%d\" %s>%s</option>";
    char select[600] = "";
    for (int i=0; i < COLOR_EFFECTS_MAX; i++ ) {
        effect_t *e = ee->effect + i;
        sprintf(select+strlen(select), effects_item, i, 
                                                     (ee->effect_id == i || ( i == COLOR_EFFECTS_MAX-1 && ee->effect_id == -1) ) ? "selected=\"selected\" " : "",
                                                     e->name);
    }
    
    effect_t *e = ee->effect + ee->effect_id;
    color_rgb_t rgb;
    hsv_to_rgb(&rgb, rgb_ctrl->hsv);
    sprintf(data+strlen(data), effects_data, rgb_ctrl->hsv.h
                                           , rgb_ctrl->hsv.s
                                           , rgb_ctrl->hsv.v
                                           , rgb.r
                                           , rgb.g
                                           , rgb.b
                                           , rgb.r, rgb.g, rgb.b  // rgb()
                                             //(ee->effect_id == -1) ? "color" : e->name, ee->effect_id
                                           , select
    );
                                    
}


esp_err_t http_process_rgb(httpd_req_t *req, char *param, size_t size);
esp_err_t http_process_rgb2(httpd_req_t *req);
esp_err_t http_process_hsv(httpd_req_t *req, char *param, size_t size);
esp_err_t http_process_hsv2(httpd_req_t *reqe);
esp_err_t http_process_hex(httpd_req_t *req, char *param, size_t size);
esp_err_t http_process_int(httpd_req_t *req);

esp_err_t rgbcontrol_http_get_handler(httpd_req_t *req){
    // handle http get request
    char page[200] = "";
    if ( http_get_has_params(req) == ESP_OK) {
        // has http params
        esp_err_t err = ESP_FAIL;
        char param[30];
        if ( http_get_key_str(req, "rgb", param, sizeof(param)) == ESP_OK ) {
            //  rgb?rgb=r,g,b  
            err = http_process_rgb(req, param, sizeof(param));
        } else if ( http_get_key_str(req, "hsv", param, sizeof(param)) == ESP_OK ) {
            // rgb?hsv=h,s,v
            err = http_process_hsv(req, param, sizeof(param));
        } else if ( http_get_key_str(req, "type", param, sizeof(param)) == ESP_OK ) {
            if ( strcmp(param, "rgb") == ESP_OK ) {
                err = http_process_rgb2(req); // rgb?type=rgb&r=r&g=g&b=b
            } else if ( strcmp(param, "hsv") == ESP_OK ) {
                err = http_process_hsv2(req); // rgb?type=hsv&h=h&s=s&v=v
            } else if ( strcmp(param, "int") == ESP_OK ) {
                err = http_process_int(req); // rgb?type=int&val=value
            } else if ( strcmp(param, "hex") == ESP_OK ) {
                err = http_process_hex(req, param, sizeof(param));  // rgb?type=hex&val=value
            } else if ( strcmp(param, "effect") == ESP_OK ) {
                if ( http_get_key_str(req, "id", param, sizeof(param)) == ESP_OK ) {
                    effects_t *ef = (effects_t *) rgb_ctrl->effects;
                    if ( ef != NULL ) {
                        ef->set( atoi(param) );
                    }    
                } else if ( http_get_key_str(req, "name", param, sizeof(param)) == ESP_OK ) {
                    effects_t *ef = (effects_t *) rgb_ctrl->effects;
                    if ( ef != NULL )                   
                        ef->set_by_name( param );
                }
            }
        }
        strcpy( page, (err == ESP_OK ) ? "OK" : "ERROR");   
    }
    // show page data
    httpd_resp_send(req, page, strlen(page));
    return ESP_OK;     

}

esp_err_t http_process_hex(httpd_req_t *req, char *param, size_t size) {
    esp_err_t err = http_get_key_str(req, "val", param, size);
    if ( err != ESP_OK ) return err;
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();     
    rgb_ctrl->set_color_hex(param);
    return ESP_OK;
}

esp_err_t http_process_int(httpd_req_t *req) {
    long color;
    esp_err_t err = http_get_key_long(req, "val", &color);
    if ( err != ESP_OK ) return err;
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();       
    rgb_ctrl->set_color_int(color);
    return ESP_OK;
}

esp_err_t http_process_rgb(httpd_req_t *req, char *param, size_t size){
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
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();
    rgb_ctrl->set_color_rgb(*rgb);
    free(rgb);                
    return ESP_OK;
}
   
esp_err_t http_process_rgb2(httpd_req_t *req){
    esp_err_t err = ESP_FAIL;
    color_rgb_t *rgb = malloc(sizeof(color_rgb_t));
    if ( http_get_key_uint8(req, "r", &rgb->r) == ESP_OK &&
         http_get_key_uint8(req, "g", &rgb->g) == ESP_OK &&
         http_get_key_uint8(req, "b", &rgb->b) == ESP_OK) 
    {
        effects_t *ef = (effects_t *) rgb_ctrl->effects;
        if ( ef != NULL ) ef->stop();
        rgb_ctrl->set_color_rgb(*rgb);
        err = ESP_OK;
    }
    free(rgb);
    return err;
}
           
esp_err_t http_process_hsv(httpd_req_t *req, char *param, size_t size) {
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
    effects_t *ef = (effects_t *) rgb_ctrl->effects;
    if ( ef != NULL ) ef->stop();
    rgb_ctrl->set_color_hsv(*hsv);
    err = ESP_OK;
    free(hsv);
    return ESP_OK;
}

esp_err_t http_process_hsv2(httpd_req_t *req) {
    esp_err_t err = ESP_FAIL;
    color_hsv_t *hsv = malloc( sizeof(color_hsv_t));
    if ( http_get_key_uint16(req, "h", &hsv->h) == ESP_OK &&
            http_get_key_uint8(req, "s", &hsv->s) == ESP_OK &&
            http_get_key_uint8(req, "v", &hsv->v) == ESP_OK) 
    {
        effects_t *ef = (effects_t *) rgb_ctrl->effects;
        if ( ef != NULL ) ef->stop();
        rgb_ctrl->set_color_hsv(*hsv);
        err = ESP_OK;
    }
    free(hsv);
    return err;
}
