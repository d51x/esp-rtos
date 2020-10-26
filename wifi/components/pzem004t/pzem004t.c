
#include "pzem004t.h"
#include "utils.h"
#include "freertos/task.h"


#ifdef CONFIG_SENSOR_PZEM004_T

static const char *TAG = "PZEM";

#define UART_READ_TIMEOUT					1500  // влияет на результаты чтения из юсарт
#define PZEM_PAUSE_TASK 	20

#define VOLTAGE_TRESHOLD 400
#define CURRENT_TRESHOLD 100
#define POWER_TRESHOLD 25000

#define CMD_VOLTAGE 		0xB0
#define RESP_VOLTAGE 		0xA0
#define CMD_CURRENT 		0xB1
#define RESP_CURRENT 		0xA1
#define CMD_POWER   		0xB2
#define RESP_POWER   		0xA2
#define CMD_ENERGY  		0xB3
#define RESP_ENERGY  		0xA3
#define CMD_ADDRESS         0xB4
#define RESP_ADDRESS        0xA4

#define RX_BUF_SIZE UART_FIFO_LEN + 1
#define TX_BUF_SIZE UART_FIFO_LEN + 1

#define PZEM_PERIODIC_TASK_PRIORITY 13
#define PZEM_PERIODIC_TASK_STACK_DEPTH 2048

#ifdef CONFIG_SENSOR_PZEM004_T_SOFTUART
uint8_t _uart_num = 0;
#else
uart_port_t _uart_num = UART_NUM_0;
#endif

TaskHandle_t xPzemHandle = NULL;

PZEM_Address _pzem_addr = {192, 168, 1, 1};

typedef struct {
    uint8_t command;
    //uint8_t addr[4];
    PZEM_Address addr;
    uint8_t data;
    uint8_t crc;
} PZEM_Command_t;

#define RESPONSE_SIZE sizeof(PZEM_Command_t)
#define RESPONSE_DATA_SIZE RESPONSE_SIZE - 2

static volatile pzem_data_t _pzem_data;

pzem_read_strategy_t _strategy;

#define PZEM_READ_ERROR_COUNT 20

//UART_NUM_0
void pzem_init(uint8_t uart_num)
{

    _uart_num = uart_num;

	#ifdef CONFIG_SENSOR_PZEM004_T_SOFTUART
	//softuart_open(_uart_num, 9600, 0 /*RX*/, 2 /*TX*/);
	softuart_open(_uart_num, 9600, 2 /*RX*/, 0 /*TX*/, UART_READ_TIMEOUT);
	#else
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
	
    uart_param_config( _uart_num, &uart_config );
	uart_set_baudrate( _uart_num, 9600);
	uint32_t br;
	uart_get_baudrate( _uart_num, &br);
		#ifdef CONFIG_DEBUG_UART1
		userlog("%s: baudrate %d\n", __func__, br);
		#endif
		ESP_LOGW(TAG, "%s: baudrate %d\n", __func__, br);

    uart_driver_install(_uart_num, RX_BUF_SIZE, TX_BUF_SIZE, 10, NULL, 0); 
	#endif
 
    memset(&_pzem_data, 0, sizeof(pzem_data_t));  

	_strategy.voltage_read_count = 1;
	_strategy.current_read_count = 1;
	_strategy.power_read_count = 1;
	_strategy.energy_read_count = 1;
	_pzem_data.ready = ESP_FAIL;

}

static void send_buffer(const uint8_t *buffer, uint8_t len)
{
    
	#ifdef CONFIG_DEBUG_UART1
		//userlog("%08d > %s: len %d\n", millis(), __func__, len);
		if ( len > 0 ) {
			userlog("%s: buf = ", __func__);
			for (uint8_t i = 0; i < len; i++) {
				userlog("%02X ", buffer[i]);
			}
			userlog("\n");
		}
	#endif

	#ifdef CONFIG_SENSOR_PZEM004_T_SOFTUART
	softuart_write_bytes(_uart_num, buffer, len);
	#else
	uart_write_bytes(_uart_num, (const char *) buffer, len);
	#endif
}

static uint8_t read_buffer(uint8_t *buffer, uint8_t cnt)
{
	int8_t result = 0;

	#ifdef CONFIG_SENSOR_PZEM004_T_SOFTUART
	result = softuart_read_buf(_uart_num, (char *)buffer, cnt);	// TOSO   add timeout for reading
	#else
	result = uart_read_bytes(_uart_num, buffer, cnt, UART_READ_TIMEOUT / portTICK_RATE_MS );
	#endif

	#ifdef CONFIG_DEBUG_UART1
		userlog("%08d > %s: len %d\n", millis(), __func__, result);
	#endif	
	if 	( result < 0 ) { result = 0;	}
	return result;
}

static uint8_t pzem_crc(uint8_t *data, uint8_t sz)
{
    uint16_t crc = 0;
    for(uint8_t i=0; i<sz; i++)
        crc += *data++;
    return (uint8_t)(crc & 0xFF);
}

static void pzem_send (uint8_t *addr, uint8_t cmd)
{
	PZEM_Command_t pzem;
	pzem.command = cmd;
	for ( uint8_t i = 0; i < sizeof(pzem.addr); i++) pzem.addr[i] = addr[i];
    //memcpy(pzem.addr, addr, sizeof(PZEM_Address));
	pzem.data = 0;
	uint8_t *bytes = (uint8_t*)&pzem;
	pzem.crc = pzem_crc(bytes, sizeof(PZEM_Command_t) - 1);
	//pzem.crc = pzem_crc((uint8_t*)&pzem, sizeof(PZEM_Command_t) - 1);

		char wlog[128];
		sprintf(wlog, "%s: len %d buf: ", __func__, sizeof(PZEM_Command_t));
		char r[8];  
		for (uint8_t i = 0; i < sizeof(PZEM_Command_t); i++) {
			sprintf(r, "%02X ", bytes[i]);
			strcat(wlog + strlen(wlog), r);
		}
		ESP_LOGW(TAG, wlog);  

	send_buffer(bytes, sizeof(PZEM_Command_t));
	//send_buffer((uint8_t*)&pzem, sizeof(PZEM_Command_t));
}

static esp_err_t pzem_read(uint8_t resp, uint8_t *data)
{
	esp_err_t res = ESP_FAIL;
	uint8_t *buf = (uint8_t *) malloc(RESPONSE_SIZE);
	uint8_t len = read_buffer(buf, RESPONSE_SIZE);
	

	#ifdef CONFIG_DEBUG_UART1
		userlog("%08d > %s: len %d\n", millis(), __func__, len);
	  	if ( len > 0 ) {
	  		userlog("%s: len: %d \t buf = ", __func__, len);
	  		for (uint8_t i = 0; i < len; i++) {
	  			userlog("%02X ", buf[i]);
	  		}
	  		userlog("\n");
	  	}
	#else
		char wlog[128];
		sprintf(wlog, "%s: len %d", __func__, len);
	  	if ( len > 0 ) 
		{
			strcat(wlog + strlen(wlog), " buf: ");
			char r[8];  
	  		for (uint8_t i = 0; i < len; i++) {
	  			sprintf(r, "%02X ", buf[i]);
				strcat(wlog + strlen(wlog), r);
	  		}
	  	}		  
		ESP_LOGW(TAG, "%s", wlog);  
		ESP_LOGW(TAG, " ");  
	#endif

	if ( len == 0 ) res = ESP_FAIL;
	else 
		if ( len ==  RESPONSE_SIZE 
			&& buf[0] == resp 
			&& buf[6] == pzem_crc(buf, len-1)
			) 
		{
			for ( uint8_t i = 0; i < RESPONSE_DATA_SIZE; i++) data[i] = buf[1 + i];
			res = ESP_OK;
		}	
	free(buf);
	WDT_FEED();
	buf = NULL;
	return res;
}

static float pzem_voltage(uint8_t *addr) {
	uint8_t data[RESPONSE_DATA_SIZE];
	pzem_send(addr, CMD_VOLTAGE);
	esp_err_t err = pzem_read( RESP_VOLTAGE, &data);
	pauseTask(10);
	float value = (err == ESP_OK ) ? (data[0] << 8) + data[1] + ( data[2] / 10.0) : PZEM_ERROR_VALUE;
	return value;
}

static float pzem_current(uint8_t *addr) {
	uint8_t data[RESPONSE_DATA_SIZE];
	pzem_send(addr, CMD_CURRENT);
	esp_err_t err = pzem_read( RESP_CURRENT, &data);
	pauseTask(10);
	float value = (err == ESP_OK ) ? (data[0] << 8) + data[1] + (data[2] / 100.0) : PZEM_ERROR_VALUE;
	return value;
}

static float pzem_power(uint8_t *addr) {
	uint8_t data[RESPONSE_DATA_SIZE];
	pzem_send(addr, CMD_POWER);
	esp_err_t err = pzem_read( RESP_POWER, &data);
	pauseTask(10);
	float value = (err == ESP_OK) ? (data[0] << 8) + data[1] : PZEM_ERROR_VALUE;
	return value;
}

static float pzem_energy(uint8_t *addr) {
	uint8_t data[RESPONSE_DATA_SIZE];
	pzem_send(addr, CMD_ENERGY);
	esp_err_t err = pzem_read( RESP_ENERGY, &data);
	pauseTask(10);
	float value = (err == ESP_OK ) ? ((uint32_t)data[0] << 16) + ((uint16_t)data[1] << 8) + data[2] : PZEM_ERROR_VALUE;
	return value;
}

esp_err_t pzem_set_addr(PZEM_Address *_addr)
{
	ESP_LOGW(TAG, __func__);	
	memcpy(&_pzem_addr, _addr, 4);
    uint8_t data[RESPONSE_DATA_SIZE];
    pzem_send(_pzem_addr, CMD_ADDRESS);
    esp_err_t err = pzem_read(RESP_ADDRESS, &data);

	#ifdef CONFIG_DEBUG_UART1
		userlog("%s result %s \n", __func__, esp_err_to_name(err) );
	#endif

    pauseTask(10);
    return err;
}

float pzem_read_voltage()
{
	ESP_LOGW(TAG, __func__);
	float v = pzem_voltage(_pzem_addr);
	_pzem_data.voltage = ( v == 0 || v > VOLTAGE_TRESHOLD) ? _pzem_data.voltage : v;

	// #ifdef CONFIG_DEBUG_UART1
	// 	userlog("%08d > voltage \t %0.2f \n", millis(), _pzem_data.voltage);
	// #endif

	if ( v == 0 ) _pzem_data.errors++;
    pauseTask(PZEM_PAUSE_TASK );
    return _pzem_data.voltage;
}

float pzem_read_current()
{
	ESP_LOGW(TAG, __func__);
	float v = pzem_current(_pzem_addr);
	_pzem_data.current = ( v == 0 || v > CURRENT_TRESHOLD) ? _pzem_data.current : v;

	// #ifdef CONFIG_DEBUG_UART1
	// 	userlog("%08d > current \t %0.2f \n", millis(), _pzem_data.current);
	// #endif

    pauseTask(PZEM_PAUSE_TASK );
    return _pzem_data.current;    
}

float pzem_read_power()
{
	ESP_LOGW(TAG, __func__);
	float v = pzem_power(_pzem_addr);
	_pzem_data.power = ( v == 0 || v > POWER_TRESHOLD) ? _pzem_data.power : v;	

	// #ifdef CONFIG_DEBUG_UART1
	// 	userlog("%08d > power \t %0.2f \n", millis(), _pzem_data.power);
	// #endif

    pauseTask(PZEM_PAUSE_TASK );
    return _pzem_data.power;        
}

float pzem_read_energy()
{
	ESP_LOGW(TAG, __func__);
	float v = pzem_energy(_pzem_addr);	
	_pzem_data.energy = ( v == 0) ? _pzem_data.energy : v;

	// #ifdef CONFIG_DEBUG_UART1
	// 	userlog("%08d > energy \t %0.2f \n", millis(), _pzem_data.energy);
	// #endif

    pauseTask(PZEM_PAUSE_TASK );
    return _pzem_data.energy;     
}

pzem_data_t pzem_get_data()
{
	return _pzem_data;
}

void pzem_set_read_strategy(pzem_read_strategy_t strategy)
{
	_strategy = strategy;
}

static void pzem_periodic_task(void *arg)
{
	uint32_t delay = (uint32_t)arg;
	uint32_t i;

	_pzem_data.errors = 0;

    while (1) 
	{	

		#ifdef CONFIG_DEBUG_UART1
			userlog("%s result %s \n", __func__, esp_err_to_name(_pzem_data.ready) );
		#endif

		if ( _pzem_data.ready != ESP_OK ) 
		{
			#ifdef CONFIG_DEBUG_UART1
				userlog("try to set pzem address \n");
			#endif			
			_pzem_data.ready = pzem_set_addr(&_pzem_addr);
			pauseTask(1000);
		} 
		else 
		{
			#ifdef CONFIG_DEBUG_UART1
			userlog("get pzem data\n");
			#endif

			for (i = 0; i < _strategy.voltage_read_count; i++)
				pzem_read_voltage();

			for (i = 0; i < _strategy.current_read_count; i++)	
			 	pzem_read_current();

			for (i = 0; i < _strategy.power_read_count; i++)	
			 	pzem_read_power();

			for (i = 0; i < _strategy.energy_read_count; i++)	
			 	pzem_read_energy();

			if ( _pzem_data.errors >= PZEM_READ_ERROR_COUNT )
			{
				pzem_set_addr(&_pzem_addr);
				//esp_restart();
				pauseTask(1000);
			}
			pauseTask(delay * 1000);
			
		}
    }

    vTaskDelete( NULL );	
}

void pzem_task_start(uint32_t delay_sec)
{
	xTaskCreate(pzem_periodic_task, "pzem_task", PZEM_PERIODIC_TASK_STACK_DEPTH, delay_sec, PZEM_PERIODIC_TASK_PRIORITY, &xPzemHandle);
}

#endif