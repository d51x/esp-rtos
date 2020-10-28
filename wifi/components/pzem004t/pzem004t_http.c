#include "pzem004t_http.h"
#include "http_page_tpl.h"

#ifdef CONFIG_SENSOR_PZEM004_T_WEB

    static const char *TAG = "PZEM";

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
#endif

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
    sprintf(param, "%d W/h", (uint32_t)pzem_data.power);
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
    sprintf(param, "%d W*h", (uint32_t)pzem_data.energy);
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
    sprintf(param, "%d W*h", pzem_data.consumption.prev_total);
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
        sprintf(param, "%d W*h", pzem_data.consumption.prev_night);
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
        sprintf(param, "%d W*h", pzem_data.consumption.prev_day );
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
    sprintf(param, "%d W*h", pzem_data.consumption.today_total);
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
        sprintf(param, "%d W*h", pzem_data.consumption.today_night);
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
        sprintf(param, "%d W*h", pzem_data.consumption.today_day );
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
    #endif

    httpd_resp_sendstr_chunk(req, html_block_data_end); 
    free(data);
}

void pzem_register_http_print_data()
{
    http_args_t *p = calloc(1,sizeof(http_args_t));
    register_print_page_block( "pzem_data", PAGES_URI[ PAGE_URI_ROOT], 3, pzem_print_data, p, NULL, NULL);    
}

void pzem_http_init(httpd_handle_t _server)
{
    pzem_register_http_print_data();    
}


#endif