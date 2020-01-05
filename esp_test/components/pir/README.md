# Component: pir sensor


Create and init PIR:
1. Define pir handle
        pir_handle_t pir_h;

2. Fill PIR config structure

    pir_conf_t pir_cfg = {
        .pin = GPIO_NUM_4,                      // pin
        .interval = 10,                         // interval for callback timer after HIGH / LOW trigger occurs
        .type = PIR_ISR,                        // use interrupt (PIR_ISR) or polling gpio (ISR_POLL and .interval of loop ( msec ))
        .active_level = PIR_LEVEL_HIGH,         // type of interrupt
        .cb_high_ctx = "high ctx",              // context (void *arg ) for high_cb function 
        .high_cb = pir_high,                    // high_cb function when interrupt triggering with HIGH level GPIO
        .cb_low_ctx = "low ctx",                // context (void *arg ) for low_cb function 
        .low_cb = pir_low,                      // low_cb function when interrupt triggering with LOW level GPIO
        .cb_tmr_ctx = "tmr ctx",                // context (void *arg ) for tmr_cb function
        .tmr_cb = pir_tmr,                      // tmr_cb function occurs when timer end after interrupt
    };

  3. Create and initialize pir handle
  
     pir_h = pir_init(pir_cfg);


4. Enable or disable PIR working, get PIR status

    pir_t *pir = (pir_t *)pir_h;
    pir->enable( pir_h );
    pir->disable( pir_h );

    int i = pir->status;



void pir_high(void *arg) {
   ESP_LOGI(TAG, "%s %s", __func__, (char *)arg);
   ESP_LOGI(TAG, "PIR active HIGH");
}

void pir_low(void *arg) {
   ESP_LOGI(TAG, "%s %s", __func__, (char *)arg);
   ESP_LOGI(TAG, "PIR active LOW");
}

void pir_tmr(void *arg) {
   ESP_LOGI(TAG, "%s %s", __func__, (char *)arg);
   ESP_LOGI(TAG, "PIR timer end");
}
