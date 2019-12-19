

#include "driver/gpio.h"
#include "mqtt.h"
#include "dsw.h"



static const char *TAG = "DSW";



void ds18b20_init(){

    gpio_config_t gpio_conf;
    gpio_conf.pin_bit_mask = 1ULL << DS18B20_PIN;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&gpio_conf);

    onewire_reset_search();

}




//-==================================================================================
float ICACHE_FLASH_ATTR ds18b20_getTemp(const uint8_t *_addr){
	int i;
	uint8_t data[12];
	onewire_reset(DS18B20_PIN);
	onewire_select(DS18B20_PIN, _addr);

	onewire_write(DS18B20_PIN, ONEWIRE_CONVERT_T, 1); // perform temperature conversion

	os_delay_us(1000*1000); // sleep 1s

	//ESP_LOGI(TAG, "Scratchpad: ");
	onewire_reset(DS18B20_PIN);
	onewire_select(DS18B20_PIN, _addr);
	onewire_write(DS18B20_PIN, ONEWIRE_READ_SCRATCHPAD, 0); // read scratchpad
	
	for(i = 0; i < 9; i++)
	{
		data[i] = onewire_read(DS18B20_PIN);
		//ESP_LOGI(TAG, "%2x ", data[i]);
	}
	//ESP_LOGI(TAG, "-");

	int HighByte, LowByte, TReading, SignBit, Whole, Fract;
	LowByte = data[0];
	HighByte = data[1];
	TReading = (HighByte << 8) + LowByte;
	SignBit = TReading & 0x8000;  // test most sig bit
	if (SignBit) // negative
		TReading = (TReading ^ 0xffff) + 1; // 2's comp
	
	Whole = TReading >> 4;  // separate off the whole and fractional portions
	Fract = (TReading & 0xf) * 100 / 16;

    //dsw->temp = 0;
    //memcpy(dsw->addr, addr, 8);
    char buf[10] = "";
    if ( SignBit ) buf[0] = "-";
    sprintf(buf, "%s%d.%d", SignBit ? "-" : "", Whole, Fract < 10 ? 0 : Fract);
	ESP_LOGI(TAG, "Temperature: %s C", buf);

    float temp = strtod(buf, NULL);
    if ( temp < - 50.f || temp > 125.f || temp == 85.f || (SignBit == '-' && Whole == 0 && Fract < 10) ) return 125.f;
    //dsw->temp = temp;
    return temp;
	//return ESP_OK;
}

static esp_err_t ds18b20_check_crc(uint8_t *addr){
	if(onewire_crc8(addr, 7) != addr[7]) {
		ds18b20_total_crc_error++;
		return ESP_FAIL;
	}
	return ESP_OK;
}

void ds18b20_search_task(void *arg){

	uint8_t addr[8];
	
	memset(&ds18b20, 0, DSW_COUNT*sizeof(ds18b20_t));
	ds18b20_init();
	ds18b20_total_crc_error = 0;
	uint8_t cnt;
	while (1) {
		cnt = 0;
		ESP_LOGI(TAG, "=========================================================");
		if ( onewire_search(DS18B20_PIN, addr) == ESP_OK ) {
			if ( ds18b20_check_crc(addr) == ESP_FAIL ) {
				ESP_LOGE(TAG, "ds18b20 crc error. total crc error count %d", ds18b20_total_crc_error);
				continue;
			}
			memcpy(ds18b20[cnt].addr, addr, 8);

			cnt++;
			ESP_LOGI(TAG, "Addr%d: %02X %02X %02X %02X %02X %02X %02X %02X", cnt, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7] );

			while ( onewire_search(DS18B20_PIN, addr) == ESP_OK ) {
				if ( ds18b20_check_crc(addr) == ESP_FAIL ) {
					ESP_LOGE(TAG, "ds18b20 crc error. total crc error count %d", ds18b20_total_crc_error);
					continue;
				}
				memcpy(ds18b20[cnt].addr, addr, 8);
				cnt++;
				ESP_LOGI(TAG, "Addr%d: %02X %02X %02X %02X %02X %02X %02X %02X", cnt, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7] );
			}		
			onewire_reset_search();	
		}	
		
		vTaskDelay(30000 / portTICK_RATE_MS);
	}
}

void ds18b20_get_temp_task(void *arg){
	
    while (1) {
        for (uint8_t i=0;i<DSW_COUNT;i++) {
			float temp;
			if ( ds18b20[i].addr[0] ) {
				temp =  ds18b20_getTemp(ds18b20[i].addr);
				if ( temp != 125.f) ds18b20[i].temp = temp;	
				ESP_LOGI(TAG, "addr %02x %02x %02x %02x %02x %02x %02x %02x  temp: %.2f C", ds18b20[i].addr[0], 
															ds18b20[i].addr[1], ds18b20[i].addr[2], ds18b20[i].addr[3], ds18b20[i].addr[4], 
															ds18b20[i].addr[5], ds18b20[i].addr[6], ds18b20[i].addr[7], ds18b20[i].temp);
			}

		}
                                                
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}