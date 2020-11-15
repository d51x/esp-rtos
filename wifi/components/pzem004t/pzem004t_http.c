#include "pzem004t_http.h"
#include "http_page_tpl.h"

#ifdef CONFIG_SENSOR_PZEM004_T_WEB

    static const char *TAG = "PZEM";
#define PZEM_FLOAT_DIVIDER 1000.0f

const char *html_block_pzem004t_title ICACHE_RODATA_ATTR = "PZEM-004T";
const char *html_block_pzem004t_title_voltage ICACHE_RODATA_ATTR = "Voltage";
const char *html_block_pzem004t_title_current ICACHE_RODATA_ATTR = "Current";
const char *html_block_pzem004t_title_power ICACHE_RODATA_ATTR = "Power";
const char *html_block_pzem004t_title_energy_total ICACHE_RODATA_ATTR = "Energy total";
const char *html_block_pzem004t_title_energy_today ICACHE_RODATA_ATTR = "Energy today";
const char *html_block_pzem004t_title_energy_yesterday ICACHE_RODATA_ATTR = "Energy yesterday";
const char *html_block_pzem004t_title_errors ICACHE_RODATA_ATTR = "Errors";

#ifdef CONFIG_SENSOR_PZEM004_T_CALC_CONSUMPTION
const char *html_block_pzem004t_title_consump_today ICACHE_RODATA_ATTR = "Расход сегодня";
const char *html_block_pzem004t_title_consump_prev ICACHE_RODATA_ATTR = "Расход вчера";

const char *html_block_pzem004t_title_consump_day ICACHE_RODATA_ATTR = "день";
const char *html_block_pzem004t_title_consump_night ICACHE_RODATA_ATTR = "ночь";

const char *html_block_pzem004t_title_energy_now ICACHE_RODATA_ATTR = "Сейчас";
const char *html_block_pzem004t_title_energy_midnight_today ICACHE_RODATA_ATTR = "Сегодня в 00:00";
const char *html_block_pzem004t_title_energy_t1_today ICACHE_RODATA_ATTR = "Сегодня в 07:00";
const char *html_block_pzem004t_title_energy_t2_today ICACHE_RODATA_ATTR = "Сегодня в 23:00";
const char *html_block_pzem004t_title_energy_midnight_prev ICACHE_RODATA_ATTR = "Вчера в 00:00";
const char *html_block_pzem004t_title_energy_t1_prev ICACHE_RODATA_ATTR = "Вчера в 07:00";
const char *html_block_pzem004t_title_energy_t2_prev ICACHE_RODATA_ATTR = "Вчера в 23:00";

#endif

#define PZEM_URI "/pzem"

static void pzem_print_data(http_args_t *args)
{
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    size_t sz = get_buf_size(html_block_data_header_start, html_block_pzem004t_title);
    char *data = malloc( sz );   
    sprintf(data, html_block_data_header_start, html_block_pzem004t_title);
    httpd_resp_sendstr_chunk(req, data);

    pzem_data_t pzem_data = pzem_get_data();

    // ==========================================================================
    char param[10];
    sprintf(param, "%0.2f V", pzem_data.voltage);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_voltage // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_voltage // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    // ==========================================================================
    sprintf(param, "%0.2f A", pzem_data.current);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_current // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_current // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    // ==========================================================================
    sprintf(param, "%0.2f kW/h", pzem_data.power / PZEM_FLOAT_DIVIDER);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_power // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_power // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    // ==========================================================================
    sprintf(param, "%0.2f kW*h", pzem_data.energy / PZEM_FLOAT_DIVIDER);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_total // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_total // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    // ==========================================================================
    sprintf(param, "%d", pzem_data.errors);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_errors // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_errors // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    #ifdef CONFIG_SENSOR_PZEM004_T_CALC_CONSUMPTION
    // ==========================================================================
    // расход вчера: общий
    sprintf(param, "%0.2f kW*h", pzem_data.consumption.prev_total / PZEM_FLOAT_DIVIDER);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_consump_prev // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_consump_prev // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    if ( PZEM_ENERGY_ZONE_T1_HOUR > 0 && PZEM_ENERGY_ZONE_T2_HOUR > 0 )
    {
        // расход вчера: ночь
        sprintf(param, "%0.2f kW*h", pzem_data.consumption.prev_night / PZEM_FLOAT_DIVIDER);
        sz = get_buf_size(html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_night // %s label
                                    , param   // %s name
                                    );
        data = realloc(data, sz);
        sprintf(data, html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_night // %s label
                                    , param   // %s name
                                    );
        httpd_resp_sendstr_chunk(req, data);


        // расход вчера: день
        sprintf(param, "%0.2f kW*h", pzem_data.consumption.prev_day / PZEM_FLOAT_DIVIDER);
        sz = get_buf_size(html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_day // %s label
                                    , param   // %s name
                                    );
        data = realloc(data, sz);
        sprintf(data, html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_day // %s label
                                    , param   // %s name
                                    );
        httpd_resp_sendstr_chunk(req, data);
    }

    // ==========================================================================
    // расход сегодня: общий
    sprintf(param, "%0.2f kW*h", pzem_data.consumption.today_total / PZEM_FLOAT_DIVIDER);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_consump_today // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_consump_today // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    if ( PZEM_ENERGY_ZONE_T1_HOUR > 0 && PZEM_ENERGY_ZONE_T2_HOUR > 0 )
    {
        // расход сегодня: ночь
        uint32_t today_consump = 0;
        sprintf(param, "%0.2f kW*h", pzem_data.consumption.today_night / PZEM_FLOAT_DIVIDER);
        sz = get_buf_size(html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_night // %s label
                                    , param   // %s name
                                    );
        data = realloc(data, sz);
        sprintf(data, html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_night // %s label
                                    , param   // %s name
                                    );
        httpd_resp_sendstr_chunk(req, data);


        // расход сегодня: день
        sprintf(param, "%0.2f kW*h", pzem_data.consumption.today_day / PZEM_FLOAT_DIVIDER);
        sz = get_buf_size(html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_day // %s label
                                    , param   // %s name
                                    );
        data = realloc(data, sz);
        sprintf(data, html_block_data_form_item_label_label
                                    , html_block_pzem004t_title_consump_day // %s label
                                    , param   // %s name
                                    );
        httpd_resp_sendstr_chunk(req, data);
    }

    
    // ==========================================================================
    strcpy(data, "<hr>");
    httpd_resp_sendstr_chunk(req, data);

    // показания по зонам
    //сегодня сейчас
    sprintf(param, "%d", (uint32_t)pzem_data.energy);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_now // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_now // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    //сегодня 0:00
    sprintf(param, "%d", pzem_data.energy_values.today_midnight);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_midnight_today // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_midnight_today // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    //сегодня 07:00
    sprintf(param, "%d", pzem_data.energy_values.today_t1);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t1_today // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t1_today // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    //сегодня 23:00
    sprintf(param, "%d", pzem_data.energy_values.today_t2);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t2_today // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t2_today // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    //вчера 00:00
    sprintf(param, "%d", pzem_data.energy_values.prev_midnight);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_midnight_prev // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_midnight_prev // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);

    //вчера 07:00
    sprintf(param, "%d", pzem_data.energy_values.prev_t1);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t1_prev // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t1_prev // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data);    

    //вчера 23:00
    sprintf(param, "%d", pzem_data.energy_values.prev_t2);
    sz = get_buf_size(html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t2_prev // %s label
                                , param   // %s name
                                );
    data = realloc(data, sz);
    sprintf(data, html_block_data_form_item_label_label
                                , html_block_pzem004t_title_energy_t2_prev // %s label
                                , param   // %s name
                                );
    httpd_resp_sendstr_chunk(req, data); 

    #endif

    httpd_resp_sendstr_chunk(req, html_block_data_end); 
    free(data);
}

void pzem_register_http_print_data()
{
    http_args_t *p = calloc(1,sizeof(http_args_t));
    register_print_page_block( "pzem_data", PAGES_URI[ PAGE_URI_ROOT], 3, pzem_print_data, p, NULL, NULL);    
}

static esp_err_t pzem_get_handler(httpd_req_t *req)
{
    // check params
    char page[10] = ""; 
	if ( http_get_has_params(req) == ESP_OK) 
	{
        char param[20];
        //  /pzem?reset=1&prev=1
        if ( http_get_key_str(req, "reset", param, sizeof(param)) == ESP_OK ) {
            uint8_t state = atoi(param);
            if ( state ) {
                state = 0;
                if ( http_get_key_str(req, "today", param, sizeof(param)) == ESP_OK ) {
                    state = atoi(param);
                }
                pzem_reset_consumption(state);
                strcpy(page, "OK");
            }
        } else {
            strcpy(page, "ERROR");
        }
    }
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
	httpd_resp_send(req, page, strlen(page)); 
     
    return ESP_OK;    
}

static void pzem_register_http_handler(httpd_handle_t _server)
{
    add_uri_get_handler( _server, PZEM_URI, pzem_get_handler, NULL); 
}

void pzem_http_init(httpd_handle_t _server)
{
    
    pzem_register_http_print_data();    
    pzem_register_http_handler(_server);
}


#endif