#ifndef _SHT21_H_
#define _SHT21_H_
#include "driver/i2c.h"
#include "nvsparam.h"
#include "i2c_bus.h"
#include "freertos/task.h"

#endif

#define SHT21_ADDR 0x40



esp_err_t sht21_init();
esp_err_t sht21_available();

float sht21_get_temp();
float sht21_get_hum();
