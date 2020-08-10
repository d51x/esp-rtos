#include "ota_http.h"


const char *html_page_ota ICACHE_RODATA_ATTR = 
  "<p>Выбрать Firmware</p>"
  "<form enctype='multipart/form-data' method='post' action='"HTTP_URI_OTA"' accept='.bin'>" 
    "<p><input type='file' name='file' class='button norm rh' /></p>" 
    "<p><div class='lf2'><input type='submit' value='Загрузить' class='button norm rht' /></p>"
  "</form>"; 

void ota_register_http_print_data();
void ota_http_process_params(httpd_req_t *req, void *args);

static void ota_print_html(char *data, void *args)
{
    strcpy(data + strlen(data), html_page_ota); 
}

void ota_http_get_process_params(httpd_req_t *req, void *args)
{
    // TODO
}

void ota_http_post_process_params(httpd_req_t *req, void *args)
{
    char err_text[400];
    char page[PAGE_DEFAULT_BUFFER_SIZE];
    uint32_t start_time = millis(); 

    if ( ota_task_upgrade_from_web(req, err_text) == ESP_OK ) {
        // upgrading is OK, restart esp and redirect to main page in 10
        char header[40] = "";
        set_redirect_header(10, "/", header);
        strcpy(page, header);

        sprintf(page+strlen(page), "File uploaded, it took %d sec. Restarting....", (uint32_t)(millis()-start_time)/1000);
        xTaskCreate(&systemRebootTask, "systemRebootTask", 1024, (int *)3000, 5, NULL);  
		httpd_resp_set_hdr(req, "Refresh", "10; /");
  
    } else {
        // upgrading fail show ota page again
        // show upgrade fail and redirect to ota page in 10 sec
        //strcpy(page, "<head><meta http-equiv=\"refresh\" content=\"10; URL=/\" /></head>");
        

        //strcpy(page+strlen(page), "OTA upgrade failed...\n");        
        strcat(page, "OTA upgrade failed...\n");        
        //strcpy(page+strlen(page), err_text);        
        strcat(page, err_text);        
        httpd_resp_set_status(req, HTTPD_500);
    }
    
    httpd_resp_send(req, page, -1);
    return ESP_OK;
}

void ota_register_http_print_data() 
{
    register_print_page_block( "ota", PAGES_URI[ PAGE_URI_OTA ], 1, ota_print_html, NULL, ota_http_get_process_params, NULL );
}

void ota_http_init(httpd_handle_t _server)
{
    ota_register_http_print_data();
    add_uri_post_handler(_server, HTTP_URI_OTAPOST, ota_http_post_process_params, NULL);
}