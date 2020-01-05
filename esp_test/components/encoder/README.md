# Component: encoder

1. define encoder handle
    encoder_handle_t enc_h;

2. Fill encoder init structure

    encoder_config_t enc_cfg = {
        .pin_btn = 13,
        .pin_clk = 4,	
        .pin_dt = 0,
        .left = enc_left_cb,
	    .cb_left_ctx = "left",
	    .right = enc_right_cb,
	    .cb_right_ctx = "right",
	    .press = enc_press_cb,
	    .cb_press_ctx = "press",
    };

3. Create and initialize encoder

    enc_h = encoder_init(enc_cfg);

callback functions

void enc_left_cb() {
    ESP_LOGI(TAG, __func__);

}

void enc_right_cb() {
    ESP_LOGI(TAG, __func__);

}

void enc_press_cb() {
    ESP_LOGI(TAG, __func__);

}
