#include "ota_http.h"
#include "http_page_tpl.h"

static const char *ota_block_title ICACHE_RODATA_ATTR = "Firmware update"; 

const char *html_page_failed ICACHE_RODATA_ATTR = "OTA upgrade failed...\n";
const char *html_page_uploaded ICACHE_RODATA_ATTR = "File uploaded, it took %d sec. Restarting....";

const char *html_page_ota ICACHE_RODATA_ATTR = 
  //"<p>Выбрать Firmware</p>"
  "<form enctype='multipart/form-data' method='post' action='"HTTP_URI_OTA"'>" 
    "<p><input type='file' name='file' class='button norm rh'  accept='.bin'/></p>" 
    "<p><div class='lf2'><input type='submit' value='Загрузить' class='button norm rht' /></p>"
  "</form>"; 

void ota_register_http_print_data();
void ota_http_process_params(httpd_req_t *req, void *args);

static void ota_print_html(http_args_t *args)
{
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    size_t sz = get_buf_size(html_block_data_header_start, ota_block_title);
    char *data = malloc( sz );   
    sprintf(data, html_block_data_header_start, ota_block_title);
    httpd_resp_sendstr_chunk(req, data); 
    free(data);
    httpd_resp_sendstr_chunk(req, html_page_ota);
    httpd_resp_sendstr_chunk(req, html_block_data_end);  
    httpd_resp_sendstr_chunk(req, html_block_data_end);  
}

static void ota_debug_print(http_args_t *args)
{
    http_args_t *arg = (http_args_t *)args;
    httpd_req_t *req = (httpd_req_t *)arg->req;

    esp_app_desc_t *app_desc = esp_ota_get_app_description();
    const esp_partition_t* esp_part = esp_ota_get_running_partition();

    ota_firm_t *fw = malloc( sizeof(ota_firm_t));
    ota_load_nvs(fw);

    httpd_resp_sendstr_chunk_fmt(req, 
    "<br>Firmware (OTA): <br>"
    "name: %s<br>"
    "version: %s<br>"
    "idf_ver: %s<br>"
    "file: %s<br>"
    "fw size: %d<br>"
    "updated: %s<br>"
    "compiled: %s %s<br>"
    "running partition: %s (%d bytes)<br>"
    , app_desc->project_name
    , app_desc->version
    , app_desc->idf_ver
    , fw->fname
    , fw->size
    , fw->dt
    , app_desc->date
    , app_desc->time
    , esp_part->label
    , esp_part->size
    );
    free(app_desc);
    free(fw);
}

void ota_http_get_process_params(httpd_req_t *req, void *args)
{
    // TODO
}

void ota_http_post_process_params(httpd_req_t *req, void *args)
{
    char err_text[400];
    char page[500];
    uint32_t start_time = millis(); 

    if ( ota_task_upgrade_from_web(req, err_text) == ESP_OK ) {
        // upgrading is OK, restart esp and redirect to main page in 10
        //char header[40] = "";
        //set_redirect_header(10, "/", header);
        httpd_resp_set_hdr(req, "Refresh", "10; /");
        //httpd_resp_sendstr_chunk(req, header);

        char *buf = malloc( strlen(html_page_uploaded) + 10);
        sprintf(buf, html_page_uploaded, (uint32_t)(millis()-start_time)/1000);
        httpd_resp_sendstr_chunk(req, buf);
        free(buf);
        

        ESP_LOGW("OTAHTTP", "Create Restart Task...");
        xTaskCreate(systemRebootTask, "sysreboot", 1024, 5000, 5, NULL); 
        ESP_LOGW("OTAHTTP", "Restart...");
    } else {
        httpd_resp_set_status(req, HTTPD_500);
        httpd_resp_sendstr_chunk(req, html_page_failed);
        httpd_resp_sendstr_chunk(req, err_text);
    }
    
    //httpd_resp_send_chunk(req, NULL, 0);
    httpd_resp_end(req);
    return ESP_OK;
}

void ota_register_http_print_data() 
{
    http_args_t *p = calloc(1,sizeof(http_args_t));
    register_print_page_block( "ota", PAGES_URI[ PAGE_URI_OTA ], 1, ota_print_html, p, ota_http_get_process_params, NULL );
    register_print_page_block( "debug", PAGES_URI[ PAGE_URI_DEBUG], 2, ota_debug_print, p, NULL, NULL  ); 

}

void ota_http_init(httpd_handle_t _server)
{
    ota_register_http_print_data();
    add_uri_post_handler(_server, HTTP_URI_OTAPOST, ota_http_post_process_params, NULL);
}