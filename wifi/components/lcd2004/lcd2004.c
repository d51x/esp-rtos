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
//#include "pcf8574.h"
#include "lcd2004.h"


#ifdef CONFIG_COMPONENT_LCD2004


static const char* TAG = "LCD";

static lcd2004_conf_t lcd2004;



static esp_err_t lcd2004_i2c_write_byte(uint8_t val);




static void lcd2004_send_half_byte(uint8_t nibble, uint8_t mode);
static void lcd2004_send_byte(uint8_t cmd, uint8_t mode);

void lcd2004_init(uint8_t addr, uint8_t cols, uint8_t rows);
void lcd2004_set_cursor();
void lcd2004_set_text();
void lcd2004_set_backlight();
void lcd2004_clear();

static const uint8_t line_addr[] = { LCD2004_LINE_1, LCD2004_LINE_2, LCD2004_LINE_3, LCD2004_LINE_4 };

static esp_err_t lcd2004_i2c_write_byte(uint8_t val)
{
    if ( xSemaphoreI2C == NULL ) return ESP_FAIL;
    if( xSemaphoreTake( xSemaphoreI2C, I2C_SEMAPHORE_WAIT ) == pdFALSE ) return ESP_FAIL;
    esp_err_t err = i2c_send_command(lcd2004.addr, val);
    xSemaphoreGive( xSemaphoreI2C );
    return err;
}

static void lcd2004_send_i2c(uint8_t nibble, uint8_t mode, uint8_t enable)
{
    uint8_t i2c_data = nibble << 4;
    if ( mode == RS_MODE_DATA ) 
        i2c_data |= RS;
    else 
        i2c_data &= ~RS;

    if ( enable ) 
        i2c_data |= EN;
    else
        i2c_data &= ~EN;

    if ( lcd2004.backlight == LCD2004_BACKLIGHT_ON ) 
        i2c_data |= BL;
    else
        i2c_data &= ~BL;    

    lcd2004_i2c_write_byte( i2c_data );   
    os_delay_us( 50 ); 
}

static void lcd2004_send_half_byte(uint8_t nibble, uint8_t mode)
{  
    lcd2004_send_i2c( nibble, mode, 1);
    i2c_master_wait(10);

    lcd2004_send_i2c( nibble, mode, 0);
    i2c_master_wait(40);  // >37us
}

static void lcd2004_send_byte(uint8_t cmd, uint8_t mode)
{
    lcd2004_send_half_byte( cmd >> 4, mode);
    lcd2004_send_half_byte( cmd, mode);
}

static void lcd2004_send_command(uint8_t cmd)
{
    lcd2004_send_byte(cmd, RS_MODE_CMD);
}


void lcd2004_init(uint8_t addr, uint8_t cols, uint8_t rows)
{
    lcd2004.addr = addr;
    lcd2004.cols = cols;
    lcd2004.rows = rows;
    lcd2004.control_flag = 0;
    lcd2004.font_size = LCD2004_FONT_5X8;
    lcd2004.backlight = LCD2004_BACKLIGHT_ON;
    lcd2004.mode = 0;

    lcd2004.i2c_bus_handle = i2c_bus_init();

    lcd2004_i2c_write_byte( 0x00 );
    vTaskDelay( 500 / portTICK_RATE_MS);

    lcd2004_send_half_byte( 0x03, RS_MODE_CMD );
    i2c_master_wait( 4500 );

    lcd2004_send_half_byte( 0x03, RS_MODE_CMD ); 
    i2c_master_wait( 4500 );

    lcd2004_send_half_byte( 0x03, RS_MODE_CMD ); 
    i2c_master_wait( 150 );

    lcd2004_send_half_byte( 0x02, RS_MODE_CMD ); 

                // 0x28 = 0x20                   0x08        0x00     
    //lcd2004_send_command( LCD_CMD_FUNCTION_SET | LCD_2LINE | LCD2004_FONT_5X8);
    lcd2004_send_command( LCD_CMD_FUNCTION_SET | LCD_2LINE | lcd2004.font_size);
    //lcd2004_send_byte(0x28,0);//режим 4 бит, 2 линии (для нашего большого дисплея это 4 линии, шрифт 5х8
                                // LCD2004_CMD_FUNCTION_SET |                                   | LCD2004_FONT_5X8
                                    //0x20                                                                 0           
    // LCD_DISPLAYCONTROL  0x08
    
    // display()   display_on()
    //lcd2004_send_byte(0x08,0);//дисплей пока выключаем LCD2004_CMD_CONTROL
    //lcd2004_send_byte( LCD_CMD_CONTROL, RS_MODE_CMD);//дисплей пока выключаем LCD2004_CMD_CONTROL
    lcd2004_send_command( LCD_CMD_CONTROL);//дисплей пока выключаем LCD2004_CMD_CONTROL и никакие флаги контроля курсора не выставлены
    i2c_master_wait( 1000 );

    // clear()
    //lcd2004_send_byte(0x01,0);//уберем мусор LCD2004_CMD_CLEAR
    //lcd2004_send_byte( LCD_CMD_CLEAR, RS_MODE_CMD);//уберем мусор LCD2004_CMD_CLEAR
    lcd2004_send_command( LCD_CMD_CLEAR );//уберем мусор LCD2004_CMD_CLEAR
    i2c_master_wait( 2000 );

    lcd2004.mode = LCD_CMD_ENTRY_LEFT | LCD_CMD_ENTRY_SHIFT_OFF;
    //lcd2004_send_byte(0x06,0);// пишем вправо   LCD2004_CMD_ENTRY_MODE (0x04) | LCD2004_CMD_ENTRY_LEFT (0x02)
    //lcd2004_send_byte( LCD_CMD_ENTRY_MODE_SET | lcd2004.mode, RS_MODE_CMD);// пишем вправо   LCD2004_CMD_ENTRY_MODE (0x04) | LCD2004_CMD_ENTRY_LEFT (0x02)
    lcd2004_send_command( LCD_CMD_ENTRY_MODE_SET | lcd2004.mode);// пишем вправо   LCD2004_CMD_ENTRY_MODE (0x04) | LCD2004_CMD_ENTRY_LEFT (0x02)
    i2c_master_wait(1000);    

    //                     0x04                 0x00                           0x00
    lcd2004.control_flag = LCD_CMD_DISPLAY_ON | LCD_CMD_UNDERLINE_CURSOR_OFF | LCD_CMD_BLINK_CURSOR_OFF;
    //  0x0C 0x08             0x04
    // LCD_CMD_CONTROL | lcd2004.control_flag

    //lcd2004_send_byte(0x0C,0);//дисплей включаем (D=1), курсоры никакие не нужны
    //lcd2004_send_byte( LCD_CMD_CONTROL | lcd2004.control_flag, RS_MODE_CMD);//дисплей включаем (D=1), курсоры никакие не нужны
    lcd2004_send_command( LCD_CMD_CONTROL | lcd2004.control_flag);//дисплей включаем (D=1), курсоры никакие не нужны
    
    //lcd2004_send_byte(0x02,0);//курсор на место
    //lcd2004_send_byte( LCD_CMD_RETURN_HOME, RS_MODE_CMD);//курсор на место
    lcd2004_home();

    lcd2004_clear();

    //lcd2004_send_byte(0X80,0);//SET POS LINE 0
    //lcd2004_send_byte( LCD_CMD_DDRAM_ADDR_SET, RS_MODE_CMD);//SET POS LINE 0
    lcd2004_send_command( LCD_CMD_DDRAM_ADDR_SET);//SET POS LINE 0
    i2c_master_wait(2000);

    //lcd2004_bl_set();//подсветка
    //lcd2004.backlight = LCD2004_BACKLIGHT_ON;
    //lcd2004_send_byte( LCD_CMD_CONTROL | lcd2004.control_flag, RS_MODE_CMD);//дисплей включаем (D=1), курсоры никакие не нужны
    //lcd2004_send_command( LCD_CMD_CONTROL | lcd2004.control_flag);
       
}

void lcd2004_backlight(lcd2004_backlight_t state)
{
    lcd2004.backlight = state;
    //lcd2004_send_byte( LCD_CMD_CONTROL | lcd2004.control_flag, RS_MODE_CMD);
    lcd2004_send_command( LCD_CMD_CONTROL | lcd2004.control_flag);
}

lcd2004_backlight_t lcd2004_backlight_state()
{
    return lcd2004.backlight;
}

void lcd2004_cursor_show(uint8_t val)
{
    if ( val )
        lcd2004.control_flag |= LCD_CMD_UNDERLINE_CURSOR_ON;
    else
        lcd2004.control_flag &= ~ LCD_CMD_UNDERLINE_CURSOR_ON;
    
    lcd2004_send_command( LCD_CMD_CONTROL | lcd2004.control_flag);
}

void lcd2004_cursor_bink(uint8_t val)
{
    if ( val )
        lcd2004.control_flag |= LCD_CMD_BLINK_CURSOR_ON;
    else
        lcd2004.control_flag &= ~ LCD_CMD_BLINK_CURSOR_ON;

    lcd2004_send_command( LCD_CMD_CONTROL | lcd2004.control_flag);
}

void lcd2004_set_cursor_position(uint8_t col, uint8_t row)
{
   uint8_t pos = 0;
   if ( row == 1 )      pos = LCD2004_LINE_1;
   else if ( row == 2)  pos = LCD2004_LINE_2;
   else if ( row == 3)  pos = LCD2004_LINE_3;
   else if ( row >= 4)  pos = LCD2004_LINE_4;
   
   
    uint8_t val = LCD_CMD_DDRAM_ADDR_SET | (col-1) | pos;

    //ESP_LOGI(TAG, "%s(%02d, %d) \t --> \t 0x%02X (DDRAM: 0x%02X \t line: 0x%02X \t pos: 0x%02X \t total: 0x%02X)", __func__, col, row, 
    //                                                    val, LCD_CMD_DDRAM_ADDR_SET, pos, col-1, pos + col-1);
  lcd2004_send_command ( val );   
}

static void lcd2004_print_char(char ch)
{
    lcd2004_send_byte( ch, RS_MODE_DATA);
}

void lcd2004_print_string(char *str)
{
    //while (*str)
    //{
    //    lcd2004_print_char(*str);
    //    str++;
    //}
  uint8_t i=0;
  while(str[i]!=0)
  {
    lcd2004_print_char(str[i]);
    i++;
  }

}

void lcd2004_clear()
{
    lcd2004_send_command( LCD_CMD_CLEAR );//уберем мусор LCD2004_CMD_CLEAR
    i2c_master_wait( 2000 );
}

void lcd2004_home()
{
    lcd2004_send_command( LCD_CMD_RETURN_HOME);//курсор на место
    i2c_master_wait(2000); 
}

void lcd2004_print(uint8_t line, const char *str)
{
    ESP_LOGI(TAG, "%d", strlen(str));
    ESP_LOGI(TAG, "%s", str);

    uint8_t len = strlen(str);
    char *s = (char *) calloc( LCD_LINE_LENGTH + 1, sizeof(char*));
    memcpy(s, str, LCD_LINE_LENGTH);
    memset(s+len, 0x20, LCD_LINE_LENGTH-len);
    ESP_LOGI(TAG, "s = '%s'", s);
    //strncpy(s, str, 20);

    lcd2004_set_cursor_position( 1, line);
    lcd2004_print_string( s );
    //lcd2004_print_string( str );
    free(s);
    
}

void lcd2004_test_task_cb(void *arg)
{

    ESP_LOGI(TAG, "init LCD");
    lcd2004_init(LCD2004_ADDR_DEFAULT, 20, 4);
    ets_delay_us(100000);



    while (1)
    {

        lcd2004_backlight( 1 );
        vTaskDelay(  2000 / portTICK_RATE_MS );

        lcd2004_clear();

        lcd2004_home();

        lcd2004_set_cursor_position( 4, 2);

        lcd2004_print_string( "Hello !!!");
        vTaskDelay(  5000 / portTICK_RATE_MS );

        lcd2004_backlight( 0 );
        vTaskDelay(  5000 / portTICK_RATE_MS );

        lcd2004_backlight( 1 );

        

        lcd2004_clear();
        
        lcd2004_set_cursor_position( 1, 1);
        lcd2004_print_string( "01234567899876543210");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 1, 2);
        lcd2004_print_string( "--------------------");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 1, 3);
        lcd2004_print_string( "09876543211234567890");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 1, 4);
        lcd2004_print_string( "++==(())**&&==--**-=");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

       

        //
        lcd2004_print(2, "Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, " Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "  Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "   Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "    Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "   Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "  Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, " Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, " Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "10          Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "11           Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "12            Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "13             Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "14            Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "15             Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "16              Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "17               Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_print(2, "18                Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS );         

        lcd2004_print(2, "19                 Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS );         

        lcd2004_print(2, "20                  Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS );         

        lcd2004_print(2, "21                   Hello!");
        vTaskDelay(  2000 / portTICK_RATE_MS );         

        // сдвинуть
        lcd2004_clear();
        lcd2004_set_cursor_position( 5, 3);
        lcd2004_print_string( "****");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 13, 3);
        lcd2004_print_string( "****");                  // не пропечатно
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 17, 3);
        lcd2004_print_string( "****");                  // не пропечатно
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 5, 4);
        lcd2004_print_string( "****");
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 13, 4);
        lcd2004_print_string( "****");                  // не пропечатно
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        lcd2004_set_cursor_position( 17, 4);
        lcd2004_print_string( "****");                  // не пропечатно
        vTaskDelay(  2000 / portTICK_RATE_MS ); 

        for ( uint8_t row = 1; row <= 4; row++)
        {
            for ( uint8_t col = 1; col <= 20; col++)    
            {
                lcd2004_set_cursor_position(col, row);
                lcd2004_print_string( "X");
                vTaskDelay(  2000 / portTICK_RATE_MS ); 
            }
        }
                
    }

    vTaskDelete( NULL );
}
void lcd2004_test_task()
{
        xTaskCreate(lcd2004_test_task_cb, "lcd2004_test", 2048, NULL, 15, NULL);
    
}
#endif
