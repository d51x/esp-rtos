

#include "sht21_http.h"

#ifdef CONFIG_SENSOR_SHT21

const char *html_block_sht21 ICACHE_RODATA_ATTR = 
    "<div class='group rnd'>"
        "<h4 class='brd-btm'>SHT21 sensor:</h4>"
        "<p><label class='lf'>Temperature: </label><label>%0.2f°C</label></p>"
        "<p><label class='lf'>Humidity: </label><label>%0.2f%%%%</label></p>"
    "</div>";      

static void sht21_print_data(char *data, void *args)
{
    char *buf = (char *) calloc(512, sizeof(char));
    sprintf(buf, html_block_sht21, sht21_get_temp(), sht21_get_hum());
    strcpy(data+strlen(data), buf);
    free(buf);
}

void sht21_register_http_print_data() 
{
    
    register_print_page_block( "sht21_data", PAGES_URI[ PAGE_URI_ROOT], 3, sht21_print_data, NULL, NULL, NULL);
    
}

void sht21_http_init(httpd_handle_t _server)
{
    sht21_register_http_print_data();
}
#endif

