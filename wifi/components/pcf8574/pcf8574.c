// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.



#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "i2c_bus.h"
#include "pcf8574.h"


#ifdef CONFIG_COMPONENT_PCF8574


static const char* TAG = "PCF8574";

pcf8574_handle_t pcf8574_create(uint8_t addr)
{
    ESP_LOGI(TAG, __func__);
    // TODO: check address 0x20 .. 0x27 and 0x38 .. 0x3F
    pcf8574_t *pcf8574 = (pcf8574_t *) calloc(1, sizeof(pcf8574_t));
    pcf8574->status = PCF8574_DISABLED;
    //POINT_ASSERT(TAG, pcf8574, NULL);

    pcf8574->addr = addr;
    pcf8574->input_value = 0x00;
    pcf8574->output_value = 0xFF;
    pcf8574->gpio_num_isr = 255;
    pcf8574->inverse = PCF8574_PORT_NORMAL;

    pcf8574_enable( (pcf8574_handle_t ) pcf8574);

    return (pcf8574_handle_t ) pcf8574;
}

esp_err_t pcf8574_delete(pcf8574_handle_t pcf8574_h)
{
    ESP_LOGI(TAG, __func__);
    return ESP_OK;
}

esp_err_t pcf8574_available(pcf8574_handle_t pcf8574_h)
{
    ESP_LOGI(TAG, __func__);
    return ESP_OK;
}

esp_err_t pcf8574_enable(pcf8574_handle_t pcf8574_h)
{
    ESP_LOGI(TAG, __func__);
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;

    ESP_LOGI(TAG, "pcf8574 is %p", pcf8574);
    pcf8574->i2c_bus_handle = i2c_bus_init();
    
    ESP_LOGI(TAG, "pcf8574->i2c_bus_handle is %p", pcf8574->i2c_bus_handle);

    if ( pcf8574->i2c_bus_handle == NULL ) return ESP_FAIL;

    pcf8574->status = PCF8574_ENABLED;
    return ESP_OK;
}

esp_err_t pcf8574_disable(pcf8574_handle_t pcf8574_h)
{
    ESP_LOGI(TAG, __func__);
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    pcf8574->i2c_bus_handle = NULL;
    pcf8574->status = PCF8574_DISABLED;
    pcf8574->gpio_num_isr = 255;
    pcf8574->input_value = 0x00;
    pcf8574->output_value = 0xFF;
    return ESP_OK;
}

// читаем биты напрямую
esp_err_t pcf8574_read(pcf8574_handle_t pcf8574_h, uint8_t *value)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    if ( xSemaphoreI2C == NULL ) return ESP_FAIL;
    if( xSemaphoreTake( xSemaphoreI2C, I2C_SEMAPHORE_WAIT ) == pdTRUE )
    {
        taskENTER_CRITICAL();
        esp_err_t err = i2c_read_data(pcf8574->addr, value, 1);
        if ( err == ESP_OK ) 
        {
            pcf8574->input_value = *value;
        }
        taskEXIT_CRITICAL();
        xSemaphoreGive( xSemaphoreI2C );
        return err;
    }   
    return ESP_FAIL;
}

esp_err_t pcf8574_read_bit(pcf8574_handle_t pcf8574_h, uint8_t bit, uint8_t *value)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    if ( bit > 7) return ESP_FAIL;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    if ( pcf8574_read(pcf8574_h, &pcf8574->input_value) == ESP_OK )
    {
        *value = (pcf8574->input_value & (1 << bit)) > 0;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

// пишем биты напрямую
esp_err_t pcf8574_write(pcf8574_handle_t pcf8574_h, uint8_t value)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    if ( xSemaphoreI2C == NULL ) return ESP_FAIL;
    if( xSemaphoreTake( xSemaphoreI2C, I2C_SEMAPHORE_WAIT ) == pdTRUE )
    {
        taskENTER_CRITICAL();
        esp_err_t err = i2c_send_command(pcf8574->addr, value);
        if ( err == ESP_OK ) {
            pcf8574->output_value = value; // запомнили, что отправили    
        }
        taskEXIT_CRITICAL();
        xSemaphoreGive( xSemaphoreI2C );
        return err;    
    } 
    return ESP_FAIL;
}

esp_err_t pcf8574_write_bit(pcf8574_handle_t pcf8574_h, uint8_t bit, uint8_t value)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    if ( bit > 7) return ESP_FAIL;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    uint8_t new_value = 0;
    if ( value == 0 ) {
        pcf8574->output_value &= ~(1 << bit);
    } else {
        pcf8574->output_value |= (1 << bit);
    }
    if ( pcf8574_write(pcf8574_h, new_value) == ESP_OK ) 
    {
        pcf8574->output_value = new_value;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t pcf8574_toggle(pcf8574_handle_t pcf8574_h, uint8_t mask)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    uint8_t value = pcf8574->output_value ^ mask;

    if ( pcf8574_write(pcf8574_h, value) == ESP_OK ) 
    {
        pcf8574->output_value = value;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t pcf8574_toggle_bit(pcf8574_handle_t pcf8574_h, uint8_t bit)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    if ( bit > 7) return ESP_FAIL;

    return pcf8574_toggle(pcf8574_h, 1 << bit);
}

esp_err_t pcf8574_shift_right(pcf8574_handle_t pcf8574_h, const uint8_t n)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    if (n == 0 || n > 7) return ESP_FAIL;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    uint8_t value = pcf8574->output_value >> n;
    if ( pcf8574_write(pcf8574_h, value) == ESP_OK ) 
    {
        pcf8574->output_value = value;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }    
}

esp_err_t pcf8574_shift_left(pcf8574_handle_t pcf8574_h, const uint8_t n)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    if (n == 0 || n > 7) return ESP_FAIL;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    uint8_t value = pcf8574->output_value << n;
    if ( pcf8574_write(pcf8574_h, value) == ESP_OK ) 
    {
        pcf8574->output_value = value;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }  
}

esp_err_t pcf8574_rotate_right(pcf8574_handle_t pcf8574_h, const uint8_t n)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    uint8_t r = n & 7;
    pcf8574_t * pcf8574 = (pcf8574_t *)pcf8574_h;
    uint8_t value = (pcf8574->output_value >> r) | (pcf8574->output_value << (8-r));
    if ( pcf8574_write(pcf8574_h, value) == ESP_OK ) 
    {
        pcf8574->output_value = value;
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }   
}

esp_err_t pcf8574_rotate_left(pcf8574_handle_t pcf8574_h, const uint8_t n)
{
    ESP_LOGI(TAG, __func__);
    if ( pcf8574_h == NULL ) return ESP_FAIL;
    return pcf8574_rotate_right(pcf8574_h, 8- (n & 7));
}

// регистрация callback для софт-прерывания (polling pcf8574 с интервалом refresh_time_ms)
void pcf8574_register_soft_isr_handler(pcf8574_handle_t pcf8574_h, uint32_t refresh_time_ms, pcf8574_isr_cb cb)
{

}

void pcf8574_register_isr_handler(pcf8574_handle_t pcf8574_h, uint8_t gpio_num, pcf8574_isr_cb cb)
{

}

void pcf8574_test_task_cb(void *arg)
{
    ESP_LOGI(TAG, __func__);
    pcf8574_handle_t pcf8574_h = (pcf8574_handle_t)arg;

    while ( 1 ) 
    {

        static uint8_t bit = 0;
        static uint8_t value = 0;
        uint8_t readed = 0;
        if ( pcf8574_write(pcf8574_h, value) == ESP_OK )
        {
           if ( pcf8574_read(pcf8574_h, &readed) == ESP_OK ) 
           {
               ESP_LOGI(TAG, "write %d and read %d", value, readed);
               readed = 0;
               value++;
               if ( value > 7 ) value = 0;
           } else {
               ESP_LOGE(TAG, "read error");
           }
        } else {
            ESP_LOGE(TAG, "write error");
        }
        vTaskDelay( 1000 / portTICK_RATE_MS );
    }
}

void pcf8574_test_task(pcf8574_handle_t pcf8574_h)
{
    ESP_LOGI(TAG, __func__);
    ESP_LOGI(TAG, "pcf8574_h is %p", pcf8574_h);
    xTaskCreate(pcf8574_test_task_cb, "pcf8574_test", 2048, pcf8574_h, 15, NULL);
    
}
#endif
