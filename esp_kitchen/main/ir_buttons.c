#include "ir_buttons.h"

static const char *TAG = "IRBTN";

void ir_btn_all_off(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_pir_ctrl(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_fan_ctrl(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_color_effect(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_adc_mode(void *arg) {    // sunset or adc value
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_speed_ctrl(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_brightness_ctrl(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_set_color(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}

void ir_btn_led_white_ctrl(void *arg) {
    ESP_LOGI(TAG, "%s\targ: %s", __func__, (char *)arg);
}
