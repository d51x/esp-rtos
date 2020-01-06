
#include "irrcv.h"



static const char *TAG = "IRRCV";

#define high_byte(val) (uint8_t) ( val >> 8 )
#define low_byte(val) (uint8_t) ( val & 0xFF )
#define PACK4(cmd1,cmd2,cmd3,cmd4) ((cmd1<<24)|(cmd2<<16)|(cmd3<<8)|cmd4)

void ir_receive(void *arg);
void handle_code(irrcv_handle_t ir_handle, uint32_t code);

static esp_err_t ir_rx_nec_code_check(ir_rx_nec_data_t nec_code) {
    if ((nec_code.addr1 != ((~nec_code.addr2) & 0xff))) return ESP_FAIL;
    if ((nec_code.cmd1 != ((~nec_code.cmd2) & 0xff))) 	return ESP_FAIL;
    return ESP_OK;
}

static esp_err_t ir_receiver_get(uint32_t *code){
	ir_rx_nec_data_t ir_data;
	ir_data.val = 0;
	ir_rx_recv_data(&ir_data, 1, portMAX_DELAY);
	if (ESP_OK == ir_rx_nec_code_check(ir_data)) { 
		ESP_LOGD(TAG, "address: 0x%04X, command: 0x%04X", (ir_data.addr1 << 8) + ir_data.addr2, (ir_data.cmd1<<8) + ir_data.cmd2);
		*code = PACK4(ir_data.addr1, ir_data.addr2, ir_data.cmd1, ir_data.cmd2);
		return ESP_OK;
	} else {
		ESP_LOGE(TAG, "Non-standard nec infrared protocol");
		return ESP_FAIL;		
	}
}

irrcv_handle_t irrcv_init(uint8_t pin, uint16_t delay, uint8_t btn_cnt) {
	irrcv_t * _irrcv = calloc(1, sizeof(irrcv_t));
	ir_rx_config_t config;
	config.io_num = pin;
	config.buf_len = IR_RX_BUF_LEN;
	esp_err_t err = ir_rx_init(&config);
	if ( err == ESP_OK ) {
		ESP_LOGE(TAG, esp_err_to_name(err));
		ir_rx_enable();
		_irrcv->pin = pin;
		_irrcv->btn_cnt = btn_cnt;
		_irrcv->receive = ir_receive;
		_irrcv->delay = delay;
		_irrcv->handle_code = handle_code;
		if ( btn_cnt == 0 ) _irrcv->btn = NULL;
		else {
			_irrcv->btn = calloc( _irrcv->btn_cnt, sizeof(ir_btn_t)); 
			for (int i=0; i < btn_cnt; i++) {
				ir_btn_t *btn = (ir_btn_t *) _irrcv->btn + i;
				btn->code = 0;
				btn->cb = NULL;
				btn->user_ctx = NULL;
			}
		}
		return (irrcv_handle_t ) _irrcv;
	} else {
		ESP_LOGE(TAG, esp_err_to_name(err));
		return NULL;
	}
}

void irrcv_start(irrcv_handle_t irrcv){
	if ( irrcv == NULL ) return;
	irrcv_t *ir = (irrcv_t *) irrcv;
	xTaskCreate(ir->receive, "ir_receiver_task", 1024, ir, 10, &ir->task); 
}

void ir_receive(void *arg){
	irrcv_t *ir = (irrcv_t *) arg;
	while (1) {
		ir_rx_enable();
		uint32_t code;
		if ( ir_receiver_get(&code) == ESP_OK ) {
			ir_rx_disable();
			ir->code = code;
			ESP_LOGD(TAG, "ir rx nec data total int:  %d\t\t0x%08X", code, code); 
			ir->handle_code( ir, code);
		}
		vTaskDelay( ir->delay / portTICK_RATE_MS);
	}
	vTaskDelete(NULL);	
}

void handle_code(irrcv_handle_t ir_handle, uint32_t code){
	ESP_LOGD(TAG, "%s  code %d   %08X", __func__, code, code);
	irrcv_t *ir = (irrcv_t *) ir_handle;
	if (ir->btn_cnt == 0) return;

	for (int i=0; i < ir->btn_cnt; i++ ) {
		ir_btn_t *btn = (ir_btn_t *)ir->btn + i;
		if ( btn->code == code ) {
			if ( btn->cb != NULL ) {
				btn->cb( btn->user_ctx );
			}
			break;
		}
	}
}

esp_err_t irrcv_add_button(irrcv_handle_t irrcv, uint8_t id, uint32_t code, void *user_ctx, button_cb cb) {
	irrcv_t *ir = (irrcv_t *) irrcv;
	if ( ir == NULL) return ESP_FAIL;
	if ( id >= ir->btn_cnt) return ESP_FAIL;
	ir_btn_t *btn = (ir_btn_t *)ir->btn + id;
	if ( btn == NULL ) return ESP_FAIL;
	btn->code = code;
	btn->cb = cb;
	btn->user_ctx = user_ctx;
	return ESP_OK;
}