#include "esp_log.h"
#include "iot_debug.h"



#ifdef CONFIG_DEBUG_UART1
    #include "freertos/FreeRTOS.h"
    #include "driver/uart.h"

#define DEBUG_BUF_SIZE UART_FIFO_LEN + 1

void enable_debug_uart1()
{
	uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE		
    };
	
	uart_param_config(UART_NUM_1, &uart_config);
	uart_set_baudrate( UART_NUM_1, 115200);


	uint32_t br;
	uart_get_baudrate( UART_NUM_1, &br);
		#ifdef CONFIG_DEBUG_UART1
		userlog("%s: baudrate1 %d\n", __func__, br);
		#endif
		ESP_LOGW("DBG", "%s: baudrate1 %d\n", __func__, br);

	uart_driver_install(UART_NUM_1, DEBUG_BUF_SIZE, DEBUG_BUF_SIZE, 10, NULL, 0);	
	os_install_putc1(userlog);
}

void userlog(const char *fmt, ...) 
{
	char *str = (char *) malloc(100);
	memset(str, 0, 100);
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(str, 100, fmt, args);
    va_end(args);
	uart_write_bytes(UART_NUM_1, str, len);
	free(str);
	str = NULL;
}



#endif