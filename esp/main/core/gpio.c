


#include "gpio.h"



static const char *TAG = "GPIO";
static QueueHandle_t queue_gpio_state_h = NULL;

void setup_gpio(const gpio_t *gp) {
    gpio_config_t gpio_conf;
    gpio_conf.pin_bit_mask = 1ULL << gp->pin;
    gpio_conf.mode = gp->mode;
    gpio_conf.intr_type = gp->intr_type;
    
    gpio_conf.pull_up_en = 0;
    gpio_conf.pull_down_en = 0;
    gpio_config(&gpio_conf);
}

void reset_gpio(gpio_t *gp) {
    gp->pin = MAX_GPIO;     // for not used gpio
    gp->state = OFF;
    gp->mode = GPIO_MODE_DISABLE;
    gp->level = HIGH_LEVEL;    
    gp->intr_type = GPIO_INTR_DISABLE;
}

void reset_all_gpio(gpio_t *gpio, uint8_t cnt) {
    for (uint8_t i=0;i<cnt;i++) {
        if (i == 6 || i == 7 || i == 8 || i == 9 || i == 10 || i == 11) {
            gpio[i].pin = MAX_GPIO;
        } else {
            reset_gpio(&gpio[i]);
        }
    }
}

void init_gpio(uint8_t pin, gpio_mode_t mode, gpio_level_t level, gpio_state_t state) {
    gpio[pin].pin = pin;
    gpio[pin].state = state;
    gpio[pin].mode = mode;
    gpio[pin].level = level;
    gpio[pin].intr_type = GPIO_INTR_DISABLE;
    setup_gpio(&gpio[pin]);
    gpio[pin].state = get_gpio_state(pin, level);

}

void init_gpios() {
    reset_all_gpio(gpio, GPIO_CNT) ;

    //init_gpio(GPIO_NUM_0, GPIO_MODE_INPUT, HIGH_LEVEL, OFF);
    init_gpio(GPIO_NUM_2, GPIO_MODE_OUTPUT, LOW_LEVEL, OFF);
    //init_gpio(GPIO_NUM_4, GPIO_MODE_INPUT, HIGH_LEVEL, OFF);
    //init_gpio(GPIO_NUM_5, GPIO_MODE_OUTPUT, HIGH_LEVEL, OFF);
    //init_gpio(GPIO_NUM_12, GPIO_MODE_OUTPUT, HIGH_LEVEL, OFF);
    //init_gpio(GPIO_NUM_13, GPIO_MODE_OUTPUT, HIGH_LEVEL, OFF);
    init_gpio(GPIO_NUM_14, GPIO_MODE_OUTPUT, HIGH_LEVEL, OFF);
    //init_gpio(GPIO_NUM_15, GPIO_MODE_OUTPUT, HIGH_LEVEL, OFF);


    // interrupt any gpio - gpio_isr_register

    // interrupt per gpio -  gpio_install_isr_service + gpio_isr_handler_add

    //gpio_set_intr_type(gpio[0].pin, GPIO_INTR_POSEDGE);
    //gpio_isr_register(gpio[0].pin, GPIO_INTR_POSEDGE);
    //gpio_install_isr_service(gpio[0].pin, GPIO_INTR_POSEDGE);

    //gpio_set_intr_type(gpio[5].pin, GPIO_INTR_POSEDGE);
    //gpio_isr_register(gpio[5].pin, GPIO_INTR_POSEDGE);
    //gpio_install_isr_service(gpio[5].pin, GPIO_INTR_POSEDGE);

}

static esp_err_t find_gpio_index(const gpio_t *gp, uint8_t pin, uint8_t *idx) {
    int i = MAX_GPIO;
    for (i=0;i<GPIO_CNT;i++) {
        if (gp[i].pin == pin) break;
    }
    if ( i == GPIO_CNT) {
        return ESP_FAIL;
    } else {
        *idx = i;
        return ESP_OK;
    }  
}

esp_err_t set_gpio(uint8_t pin, uint8_t _st){
    ESP_LOGD(TAG, "%s: started....", __func__);
    // определить level gpio, по массиву

    // check for GPIO_MODE_DISABLE or GPIO_MODE_INPUT - do nothing
    // GPIO_MODE_OUTPUT - do

    // check state and gpio
    if ( pin >= GPIO_NUM_MAX ) {
        //ESP_LOGI(TAG, "%s: FAIL: pin (%d) > %d", __func__, pin, GPIO_NUM_MAX-1);
        return ESP_FAIL;
    }

    if ( _st > 1 ) {
        ESP_LOGD(TAG, "%s: FAIL: state (%d) > 1", __func__, _st);
        return ESP_FAIL;
    }

    uint8_t i = 255;
    if ( find_gpio_index(gpio, pin, &i) == ESP_FAIL ) return ESP_FAIL;
    
    // check current state
    ESP_LOGD(TAG, "%s: check current state....", __func__);
    if ( gpio[i].state == _st) {
        ESP_LOGD(TAG, "%s: gpio%d has the same state. BREAK!!!", __func__, pin);
        return ESP_OK;
    }

    //set_gpio: gpio2 has state 0 ( gpio[i].state) switch to new state 1 (_st).

    // convert state for low_level trigger
    uint8_t st = (gpio[i].level == LOW_LEVEL) ? !_st : _st;
    ESP_LOGD(TAG, "%s: gpio%d has state %d switch to new state %d", __func__, pin, gpio[i].state, _st);
    
    // set new level
    esp_err_t res = gpio_set_level(gpio[i].pin, st);
    if ( res == ESP_OK) {
        gpio[i].state = _st;
        // send mqtt    via task
        //mqtt_publish_gpio_output_state(pin, _st); // создать таску разовую на отправку
        ESP_LOGI(TAG, "Try to put gpio message to queue");
        if ( queue_gpio_state_h != NULL) {
            ESP_LOGI(TAG, "queue is not null");
            gpio_state_msg_t mqtt_gpio;
            mqtt_gpio.pin = pin;
            mqtt_gpio.state = _st;
            mqtt_gpio.mode = gpio[i].mode;
            if ( xQueueSend( queue_gpio_state_h, ( void * ) &mqtt_gpio, ( TickType_t ) 0 ) ) {
                ESP_LOGD(TAG, "Message sended");
            } else {
                ESP_LOGE(TAG, "Error sending message");
            }
        } else {
            ESP_LOGE(TAG, "queue is null");
        }
    }
    return res;
}

void set_queue_gpio_state(const QueueHandle_t *handle){
    queue_gpio_state_h = handle;
}

uint8_t get_gpio_state(uint8_t pin, uint8_t level) {
    uint8_t st = gpio_get_level(pin);
    st = (level == LOW_LEVEL) ? !st : st;
    return st;
}


