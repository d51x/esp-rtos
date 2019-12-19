#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ir_receiver.h"



static const char *TAG = "IR_RX";

#define IR_RX_BUF_LEN 128

static esp_err_t ir_rx_nec_code_check(ir_rx_nec_data_t nec_code) {
    if ((nec_code.addr1 != ((~nec_code.addr2) & 0xff))) return ESP_FAIL;
    if ((nec_code.cmd1 != ((~nec_code.cmd2) & 0xff))) 	return ESP_FAIL;
    return ESP_OK;
}

static void ir_receiver_init(uint8_t pin) {
	ir_rx_config_t config;
	config.io_num = pin;
	config.buf_len = IR_RX_BUF_LEN;
	if ( ir_rx_init(&config) )
			ir_rx_enable();
}

#define high_byte(val) (uint8_t) ( val >> 8 )
#define low_byte(val) (uint8_t) ( val & 0xFF )
#define PACK4(cmd1,cmd2,cmd3,cmd4) ((cmd1<<24)|(cmd2<<16)|(cmd3<<8)|cmd4)

static esp_err_t ir_receiver_get(uint32_t *code){
	ir_rx_nec_data_t ir_data;
	ir_data.val = 0;
	uint16_t address = 0;
	uint8_t command = 0;
	uint8_t command_invert = 0;

	ir_rx_recv_data(&ir_data, 1, portMAX_DELAY);
	if (ESP_OK == ir_rx_nec_code_check(ir_data)) { 
		
		ESP_LOGI(TAG, "address: 0x%04X, command: 0x%04X", (ir_data.addr1 << 8) + ir_data.addr2, (ir_data.cmd1<<8) + ir_data.cmd2);

//		ESP_LOGI(TAG, "addr1: 0x%x, addr2: 0x%x, cmd1: 0x%x, cmd2: 0x%x", ir_data.addr1, ir_data.addr2, ir_data.cmd1, ir_data.cmd2);

		*code = PACK4(ir_data.addr1, ir_data.addr2, ir_data.cmd1, ir_data.cmd2);
		//ESP_LOGI(TAG, "ir rx nec data total int:  %d\t\t0x%08X", (int)code, (int)code); 


// addr 16 bit  - low High
// command - 8 bit
// inverted command - 8 bit
		//ESP_LOGI(TAG, "ir rx nec data:  0x%x", ir_data.cmd1); 
		//code = , ir_data.cmd1, sizeof(uint32_t));
		return ESP_OK;
	} else {
		ESP_LOGI(TAG, "Non-standard nec infrared protocol");
		return ESP_FAIL;		
	}
}

void ir_receiver_task(void *arg) {
	
	ir_receiver_init(5);
	
	uint32_t cmd = 0;
	while (1) {
		ir_rx_enable();
		if ( ir_receiver_get(&cmd) == ESP_OK ) {
			ir_rx_disable();
			ESP_LOGI(TAG, "ir rx nec data total int:  %d\t\t0x%08X", (int)cmd, (int)cmd); 
			last_code = cmd;

		}
		vTaskDelay(100 / portTICK_RATE_MS);

	}
	//vTaskDelete(NULL);
}

