#ifndef __IR_BUTTONS_H__
#define __IR_BUTTONS_H__

//#include "core/core.h"
#include "irrcv.h"
#include "colors.h"
#include "effects.h"
#include "main.h"
#include "core/core.h"

#define IR_RECEIVER_BUTTONS_COUNT 40

    // row 1
#define IR_CODE_ONOFF                    0x08F70BF4       // ALL OFF
#define IR_CODE_MODE                     0x08F70CF3       // FAN Kitchen
#define IR_CODE_PLAY_PAUSE               0x08F754AB       // COLOR WHEEL  
#define IR_CODE_MUTE                     0x08F748B7       // COLOR RND RNDL
    //row 2
#define IR_CODE_BND_SYS                  0x08F741BE         // JUMP 3 RGB
#define IR_CODE_UP                       0x08F701FE         // JUMP 7 RAINBOW
#define IR_CODE_TITLE                    0x08F702FD         // JUMP 12 RAINBOW
#define IR_CODE_SUB_T                    0x08F703FC         // NEXT RND JUMP
    //row 3
#define IR_CODE_LEFT                     0x08F74AB5         // FADE 3 RGB
#define IR_CODE_ENTER                    0x08F704FB         // FADE 7 RAINBOW
#define IR_CODE_RIGHT                    0x08F705FA         // FADE 12 RAINBOW
#define IR_CODE_SETUP                    0x08F706F9         // NEXT RND FADE
    // row 4
#define IR_CODE_STOP_BACK                0x08F714EB         // COLOR OLIVE
#define IR_CODE_DOWN                     0x08F707F8         // COLOR TEAL
#define IR_CODE_ANGLE                    0x08F708F7         // COLOR_DARKPURPLE
#define IR_CODE_SLOW                     0x08F709F6         // SPEED UP
    // row 5
#define IR_CODE_AMS_RPT                  0x08F715EA         // COLOR_YELLOW
#define IR_CODE_ST_PROG                  0x08F751AE         // COLOR_CYAN
#define IR_CODE_VOL_PLUS                 0x08F752AD         // COLOR_PURPLE 
#define IR_CODE_ZOOM                     0x08F700FF         // SPEED DOWN
    // row 6
#define IR_CODE_LOC_RDM                  0x08F71FE0         // COLOR_PINK
#define IR_CODE_SEEK_MINUS               0x08F71CE3         // COLOR_LIGHTGREEN
#define IR_CODE_SEL                      0x08F71DE2         // COLOR_VIOLET
#define IR_CODE_SEEK_PLUS                0x08F753AC         // BRIGHTNESS UP
    // row 7
#define IR_CODE_PBC                      0x08F71AE5       // COLOR_ORANGE
#define IR_CODE_OSD                      0x08F759A6       // COLOR_LIMEGREEN
#define IR_CODE_VOL_MINUS                0x08F740BF       // COLOR_LIGHTBLUE
#define IR_CODE_AUDIO                    0x08F716E9       // BRIGHTNESS DOWN
    // row 8
#define IR_CODE_BTN_1                    0x08F746B9       // RED Led - Increase
#define IR_CODE_BTN_2                    0x08F719E6       // GREEN Led - Increase
#define IR_CODE_BTN_3                    0x08F70DF2       // BLUE Led - Increase
#define IR_CODE_BTN_4                    0x08F74BB4       // WHITE Led - Increase
    // row 9
#define IR_CODE_BTN_5                    0x08F75FA0       // RED Led - Decrease
#define IR_CODE_BTN_6                    0x08F75BA4       // GREEN Led - Decrease
#define IR_CODE_BTN_7                    0x08F74FB0       // BLUE Led - Decrease
#define IR_CODE_BTN_8                    0x08F757A8       // WHITE Led - Decrease
    // row 10
#define IR_CODE_BTN_9                    0x08F75EA1       // RED Led
#define IR_CODE_BTN_0                    0x08F75AA5       // GREEN Led  
#define IR_CODE_BTN_10_PLUS              0x08F74EB1       // BLUE Led
#define IR_CODE_GOTO                     0x08F756A9       // WHITE Led


/*
struct ir_btn {
	uint32_t code;
	void *user_ctx;
	button_cb cb;
};
*/
//==== ir receiver button callbacks
void ir_btn_all_off(void *arg);
void ir_btn_fan_ctrl(void *arg);
void ir_btn_color_effect(void *arg);
void ir_btn_adc_mode(void *arg);    
void ir_btn_speed_ctrl(void *arg);
void ir_btn_brightness_dec(void *arg);
void ir_btn_brightness_inc(void *arg);
void ir_btn_set_color(void *arg);
void ir_btn_led_ctrl(void *arg);

typedef enum {
    IR_CM_NONE,
    IR_CMD_ALL_OFF
} ir_cmt_type_t;

static ir_btn_t ir_buttons[IR_RECEIVER_BUTTONS_COUNT] = {
    // row 1
        { IR_CODE_ONOFF        , "alloff"           , ir_btn_all_off            },      
        { IR_CODE_MODE         , "fan"              , ir_btn_fan_ctrl           },      
        { IR_CODE_PLAY_PAUSE   , "wheel"            , ir_btn_color_effect           },   
        { IR_CODE_MUTE         , "rnd"              , ir_btn_color_effect           },  
    //row 2
        { IR_CODE_BND_SYS      , "jump3"            , ir_btn_color_effect       },      
        { IR_CODE_UP           , "jump7"            , ir_btn_color_effect       },      
        { IR_CODE_TITLE        , "jump12"           , ir_btn_color_effect       },      
        { IR_CODE_SUB_T        , "nextrndjump"      , ir_btn_color_effect           },  
    //row 3
        { IR_CODE_LEFT         , "fade3"            , ir_btn_color_effect       },      
        { IR_CODE_ENTER        , "fade7"            , ir_btn_color_effect       },      
        { IR_CODE_RIGHT        , "fade12"           , ir_btn_color_effect       },      
        { IR_CODE_SETUP        , "nextrndfade"      , ir_btn_color_effect         },    
    // row 4
        { IR_CODE_STOP_BACK    ,  HEX_COLOR_OLIVE       , ir_btn_set_color          },    
        { IR_CODE_DOWN         ,  HEX_COLOR_TEAL        , ir_btn_set_color          },    
        { IR_CODE_ANGLE        ,  HEX_COLOR_DARKPURPLE  , ir_btn_set_color          },    
        { IR_CODE_SLOW         ,  LED_SPEED_UP          , ir_btn_speed_ctrl         },    
    // row 5
        { IR_CODE_AMS_RPT      , HEX_COLOR_YELLOW       , ir_btn_set_color          },   
        { IR_CODE_ST_PROG      , HEX_COLOR_CYAN         , ir_btn_set_color          },   
        { IR_CODE_VOL_PLUS     , HEX_COLOR_PURPLE       , ir_btn_set_color          },   
        { IR_CODE_ZOOM         , LED_SPEED_DOWN         , ir_btn_speed_ctrl         },   
    // row 6
        { IR_CODE_LOC_RDM      , HEX_COLOR_PINK         , ir_btn_set_color          },   
        { IR_CODE_SEEK_MINUS   , HEX_COLOR_LIGHTGREEN   , ir_btn_set_color          },   
        { IR_CODE_SEL          , HEX_COLOR_VIOLET       , ir_btn_set_color          },   
        { IR_CODE_SEEK_PLUS    , "up"                   , ir_btn_brightness_inc    },   
    // row 7
        { IR_CODE_PBC          , HEX_COLOR_ORANGE       , ir_btn_set_color          },   
        { IR_CODE_OSD          , HEX_COLOR_LIMEGREEN    , ir_btn_set_color          },   
        { IR_CODE_VOL_MINUS    , HEX_COLOR_LIGHTBLUE    , ir_btn_set_color          },   
        { IR_CODE_AUDIO        , "down"                 , ir_btn_brightness_dec    },   
    // row 8
        { IR_CODE_BTN_1        , LED_CTRL_RED_CH          , ir_btn_brightness_inc    },     
        { IR_CODE_BTN_2        , LED_CTRL_GREEN_CH        , ir_btn_brightness_inc    },  
        { IR_CODE_BTN_3        , LED_CTRL_BLUE_CH         , ir_btn_brightness_inc    },  
        { IR_CODE_BTN_4        , LED_CTRL_WHITE_CH        , ir_btn_brightness_inc     },  
    // row 9
        { IR_CODE_BTN_5        , LED_CTRL_RED_CH          , ir_btn_brightness_dec    },  
        { IR_CODE_BTN_6        , LED_CTRL_GREEN_CH        , ir_btn_brightness_dec    },  
        { IR_CODE_BTN_7        , LED_CTRL_BLUE_CH         , ir_btn_brightness_dec    },  
        { IR_CODE_BTN_8        , LED_CTRL_WHITE_CH        , ir_btn_brightness_dec     },  
    // row 10
        { IR_CODE_BTN_9        , LED_CTRL_RED_CH          , ir_btn_led_ctrl          },
        { IR_CODE_BTN_0        , LED_CTRL_GREEN_CH        , ir_btn_led_ctrl          },
        { IR_CODE_BTN_10_PLUS  , LED_CTRL_BLUE_CH         , ir_btn_led_ctrl          },
        { IR_CODE_GOTO         , LED_CTRL_WHITE_CH           , ir_btn_led_ctrl     }   
};

#endif