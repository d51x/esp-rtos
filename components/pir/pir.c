
#include "pir.h"



static const char *TAG = "PIR";

static void task_cb(void *arg) {

	pir_t *pir = (pir_t *) arg;

	int delay = 10 / portTICK_PERIOD_MS;
	while (1) {
		if ( pir->taskq != NULL && pir->argq != NULL /*&& uxQueueMessagesWaiting(pir->taskq) != 0*/) {
			void (*task)(void *);
			void *arg;		
			if ( xQueueReceive(pir->taskq, &task, 0) == pdPASS &&
				 xQueueReceive(pir->argq, &arg, 0) == pdPASS ) 
				 {
					task(arg);
				 }
		}
		vTaskDelay( delay );
	}
	vTaskDelete(NULL);
}

// callback when pir send HIGH
static void pir_high_cb(void *arg)
{
	pir_t *pir = (pir_t *) arg;
	if ( pir->high_cb) 
		pir->high_cb( pir->cb_high_ctx );
}

// callback when pir send LOW
static void pir_low_cb(void *arg)
{
	pir_t *pir = (pir_t *) arg;
	if ( pir->low_cb )
		pir->low_cb( pir->cb_low_ctx );
}

static void pir_tmr_cb(xTimerHandle tmr)
{
	pir_t *pir = (pir_t *) pvTimerGetTimerID(tmr);
	ESP_LOGI(TAG, "pir->tmr_cb add %p", pir->tmr_cb);
	if ( pir->tmr_cb )
		pir->tmr_cb( pir->cb_tmr_ctx );
}

static void gpio_poll(void *arg){
	pir_t *pir = (pir_t *) arg;

	while (1) {
		int level = gpio_get_level( pir->pin );
	
		if ( level == 1 ) {	
			if ( pir->active_level ==  PIR_LEVEL_HIGH || pir->active_level ==  PIR_LEVEL_ANY) {   // 
				pir_high_cb( pir );
				if (pir->tmr) {  // restart timer if timer already created
					xTimerStop(pir->tmr, 0);
					xTimerReset(pir->tmr, 0);		
				} else {
					// create and start timer
					pir->tmr = xTimerCreate("pir_tmr", pir->interval, pdFALSE, pir, pir_tmr_cb);    // callback timer stop
					xTimerStart(pir->tmr, 0);
				}			
			} else {
				pir_low_cb( pir );
			}
		} else {
			if ( pir->active_level ==  PIR_LEVEL_HIGH || pir->active_level ==  PIR_LEVEL_ANY ) {   // 
				pir_low_cb( pir );
				if (pir->tmr) {  // restart timer if timer already created
					xTimerStop(pir->tmr, 0);
					xTimerReset(pir->tmr, 0);		
				} else {
					// create and start timer
					pir->tmr = xTimerCreate("pir_tmr", pir->interval, pdFALSE, pir, pir_tmr_cb);    // callback timer stop
					xTimerStart(pir->tmr, 0);
				}			
			} else {
				pir_high_cb( pir );
			}		

		}
		vTaskDelay( pir->interval  );
	}
	vTaskDelete( NULL );
}

static void IRAM_ATTR pir_isr_handler(void *arg) {
	portBASE_TYPE HPTaskAwoken = pdFALSE;
	BaseType_t xHigherPriorityTaskWoken;

	pir_t *pir = (pir_t *) arg;
	int level = gpio_get_level( pir->pin );
	
	if (pir->taskq != NULL && pir->argq != NULL ) {
            void *func;
            void *arg;
			if ( level == 1 ) {
				if ( pir->active_level ==  PIR_LEVEL_HIGH || pir->active_level == PIR_LEVEL_ANY ) {   // 
					func = pir_high_cb;
					arg = pir;
					if (pir->tmr) {  // restart timer if timer already created
						xTimerStopFromISR(pir->tmr, &HPTaskAwoken);
						xTimerResetFromISR(pir->tmr, &HPTaskAwoken);		
					} else {
						// create and start timer
						pir->tmr = xTimerCreate("pir_tmr", pir->interval, pdFALSE, pir, pir_tmr_cb);    // callback timer stop
						xTimerStartFromISR(pir->tmr, &HPTaskAwoken);
					}			
				} else {
					func = pir_low_cb;
					arg = pir;					
				}		
			} else {
				// level 0
				if ( pir->active_level ==  PIR_LEVEL_HIGH || pir->active_level == PIR_LEVEL_ANY ) {   // 
					func = pir_low_cb;
					arg = pir;						
					if (pir->tmr) {  // restart timer if timer already created
						xTimerStopFromISR(pir->tmr, &HPTaskAwoken);
						xTimerResetFromISR(pir->tmr, &HPTaskAwoken);		
					} else {
						// create and start timer
						pir->tmr = xTimerCreate("pir_tmr", pir->interval, pdFALSE, pir, pir_tmr_cb);    // callback timer stop
						xTimerStartFromISR(pir->tmr, &HPTaskAwoken);
					}			
				} else {
					func = pir_high_cb;
					arg = pir;						
				}		
			}
            xQueueOverwriteFromISR(pir->taskq, &func, &xHigherPriorityTaskWoken);
			xQueueOverwriteFromISR(pir->argq, &arg, &xHigherPriorityTaskWoken);
    }

	/*
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portEND_SWITCHING_ISR(pdTRUE);
    }	*/
	portEND_SWITCHING_ISR( HPTaskAwoken == pdTRUE );
}

static void pir_enable(pir_handle_t _pir) {
	pir_t *pir = (pir_t *) _pir;
	pir->status = PIR_ENABLED;

	if ( pir->type == PIR_ISR  ) {
		if ( pir->task == NULL ) 
			xTaskCreate( pir->task_cb, "pir_task", 1024, pir, 10, &pir->task);

		if (pir->taskq == NULL ) 
			pir->taskq = xQueueCreate(1, sizeof(void *));

		if (pir->argq == NULL) 
			pir->argq =  xQueueCreate(1, sizeof(void *));
					
		if ( pir->tmr == NULL )
			pir->tmr = xTimerCreate("pir_tmr", pir->interval, pdFALSE, pir, pir_tmr_cb);

		gpio_install_isr_service(0);
    	gpio_isr_handler_add( pir->pin, pir_isr_handler, pir);   // обработчик прерывания, в качестве аргумента передается указатель на сам объект pir
	} else {
		// polling gpio task
		// _pir->task_cb
		if ( pir->type == PIR_POLL )
			xTaskCreate( gpio_poll, "pir_task", 1024, pir, 10, &pir->task_poll);
	} 
}

static void pir_disable(pir_handle_t _pir) {
	pir_t *pir = (pir_t *) _pir;

	pir->status = PIR_DISABLED;

	if ( pir->taskq != NULL ) {
		vQueueDelete( pir->taskq );
		pir->taskq = NULL;
	}

	if ( pir->argq != NULL ) {
		vQueueDelete( pir->argq );
		pir->argq = NULL;
	}

	if ( pir->task != NULL ) {
		vTaskDelete( pir->task );
		pir->task = NULL;
	}

	if ( pir->type == PIR_ISR  ) {
		if ( pir->tmr != NULL ) {
			xTimerStop( pir->tmr, 0 );
			xTimerDelete( pir->tmr, 0 );
		}
    	gpio_isr_handler_remove( pir->pin );   // обработчик прерывания, в качестве аргумента передается указатель на сам объект pir
	} else {
		// polling gpio task
		// _pir->task_cb
		if ( pir->type == PIR_POLL && pir->task_poll != NULL) {
			if ( pir->task_poll != NULL) { 
				vTaskDelete( pir->task_poll );
				pir->task_poll = NULL;
			}	
		}	
	} 
}

pir_handle_t pir_init(pir_conf_t pir_conf){
	pir_t * _pir = calloc(1, sizeof(pir_t));
	
	_pir->pin = pir_conf.pin;
	_pir->active_level = pir_conf.active_level;
	_pir->type = pir_conf.type;
	_pir->status = PIR_DISABLED;
	_pir->cb_high_ctx = pir_conf.cb_high_ctx;
	_pir->high_cb = pir_conf.high_cb;	
	
	_pir->cb_low_ctx = pir_conf.cb_low_ctx;
	_pir->low_cb = pir_conf.low_cb;
	
	_pir->tmr = NULL;
	if ( _pir->type == PIR_ISR)
		_pir->interval = pir_conf.interval * 1000 / portTICK_PERIOD_MS;
	else	
		_pir->interval = pir_conf.interval / portTICK_PERIOD_MS;

	_pir->cb_tmr_ctx = pir_conf.cb_tmr_ctx;
	_pir->tmr_cb = pir_conf.tmr_cb;
	
	_pir->argq = NULL;
 	_pir->taskq = NULL;
	_pir->task = NULL;
	_pir->task_poll = NULL;

	_pir->enable = pir_enable;
	_pir->disable = pir_disable;

	_pir->task_cb = task_cb;

    // настройка  gpio
    
    gpio_config_t gpio_conf;
	if ( pir_conf.active_level == PIR_LEVEL_HIGH)
    	gpio_conf.intr_type = GPIO_INTR_POSEDGE;	// GPIO_INTR_HIGH_LEVEL
	else if ( pir_conf.active_level == PIR_LEVEL_LOW )	
		gpio_conf.intr_type = GPIO_INTR_NEGEDGE;  	// GPIO_INTR_LOW_LEVEL
	else if ( pir_conf.active_level == PIR_LEVEL_ANY )
		gpio_conf.intr_type = GPIO_INTR_ANYEDGE;  	
	else	
		gpio_conf.intr_type = GPIO_INTR_DISABLE;  		
		
	//gpio_conf.intr_type = GPIO_INTR_ANYEDGE;  	// GPIO_INTR_LOW_LEVEL

    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask = (1ULL << _pir->pin);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_conf);

	pir_enable( _pir );

	return (pir_handle_t ) _pir;
}