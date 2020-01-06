#ifndef __IR_BUTTONS_H__
#define __IR_BUTTONS_H__

//#include "core/core.h"
#include "irrcv.h"
#include "colors.h"
#include "effects.h"

#define IR_RECEIVER_BUTTONS_COUNT 40

    // row 1
#define IR_CODE_ONOFF                    0x08F70BF4       // ALL OFF
#define IR_CODE_MODE                     0x08F70CF3       // Auto Mode
#define IR_CODE_PLAY_PAUSE               0x08F754AB       // FAN Attic   ?????  
#define IR_CODE_MUTE                     0x08F748B7       // FAN Kitchen
    //row 2
#define IR_CODE_BND_SYS                  0x08F741BE         // JUMP 7 RAINBOW
#define IR_CODE_UP                       0x08F701FE         // JUMP 3 RGB
#define IR_CODE_TITLE                    0x08F702FD         // COLOR WHEEL
#define IR_CODE_SUB_T                    0x08F703FC         // mode sunset
    //row 3
#define IR_CODE_LEFT                     0x08F74AB5         // FADE 7 RAINBOW
#define IR_CODE_ENTER                    0x08F704FB         // FADE 3 RGB
#define IR_CODE_RIGHT                    0x08F705FA         // FADE 1 CURRENT
#define IR_CODE_SETUP                    0x08F706F9         // SPEED UP
    // row 4
#define IR_CODE_STOP_BACK                0x08F714EB         // COLOR OLIVE
#define IR_CODE_DOWN                     0x08F707F8         // COLOR TEAL
#define IR_CODE_ANGLE                    0x08F708F7         // COLOR_DARKPURPLE
#define IR_CODE_SLOW                     0x08F709F6         // SPEED DOWN
    // row 5
#define IR_CODE_AMS_RPT                  0x08F715EA         // COLOR_YELLOW
#define IR_CODE_ST_PROG                  0x08F751AE         // COLOR_CYAN
#define IR_CODE_VOL_PLUS                 0x08F752AD         // COLOR_PURPLE 
#define IR_CODE_ZOOM                     0x08F700FF         // BRIGHTNESS_UP
    // row 6
#define IR_CODE_LOC_RDM                  0x08F71FE0         // COLOR_PINK
#define IR_CODE_SEEK_MINUS               0x08F71CE3         // COLOR_LIGHTGREEN
#define IR_CODE_SEL                      0x08F71DE2         // COLOR_VIOLET
#define IR_CODE_SEEK_PLUS                0x08F753AC         // BRIGHTNESS DOWN
    // row 7
#define IR_CODE_PBC                      0x08F71AE5       // COLOR_ORANGE
#define IR_CODE_OSD                      0x08F759A6       // COLOR_LIMEGREEN
#define IR_CODE_VOL_MINUS                0x08F740BF       // COLOR_LIGHTBLUE
#define IR_CODE_AUDIO                    0x08F716E9       // RESET COLORS
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
void ir_btn_pir_ctrl(void *arg);
void ir_btn_fan_ctrl(void *arg);
void ir_btn_color_effect(void *arg);
void ir_btn_adc_mode(void *arg);    
void ir_btn_speed_ctrl(void *arg);
void ir_btn_brightness_ctrl(void *arg);
void ir_btn_set_color(void *arg);
void ir_btn_led_white_ctrl(void *arg);

static ir_btn_t ir_buttons[IR_RECEIVER_BUTTONS_COUNT] = {
    // row 1
        { IR_CODE_ONOFF        , "alloff"           , ir_btn_all_off            },        // ALL OFF
        { IR_CODE_MODE         , "automode"         , ir_btn_pir_ctrl           },        // Auto Mode
        { IR_CODE_PLAY_PAUSE   , "fan_attic"        , ir_btn_fan_ctrl           },        // FAN Attic   ?????  
        { IR_CODE_MUTE         , "fan_kitchen"      , ir_btn_fan_ctrl           },        // FAN Kitchen
    //row 2
        { IR_CODE_BND_SYS      , "jump7"            , ir_btn_color_effect       },       // JUMP 7 RAINBOW
        { IR_CODE_UP           , "jump3"            , ir_btn_color_effect       },       // JUMP 3 RGB
        { IR_CODE_TITLE        , "wheel"            , ir_btn_color_effect       },       // COLOR WHEEL
        { IR_CODE_SUB_T        , "sunset"           , ir_btn_adc_mode           },       // mode sunset
    //row 3
        { IR_CODE_LEFT         , "fade7"            , ir_btn_color_effect       },       // FADE 7 RAINBOW
        { IR_CODE_ENTER        , "fade3"            , ir_btn_color_effect       },       // FADE 3 RGB
        { IR_CODE_RIGHT        , "fade12"           , ir_btn_color_effect       },       // FADE 12 
        { IR_CODE_SETUP        ,  "up"              , ir_btn_speed_ctrl         },       // SPEED UP
    // row 4
        { IR_CODE_STOP_BACK    ,  "COLOR_OLIVE"       , ir_btn_set_color          },      // COLOR OLIVE
        { IR_CODE_DOWN         ,  "COLOR_TEAL"        , ir_btn_set_color          },      // COLOR TEAL
        { IR_CODE_ANGLE        ,  "COLOR_DARKPURPLE"  , ir_btn_set_color          },      // COLOR_DARKPURPLE
        { IR_CODE_SLOW         ,  "down"            , ir_btn_speed_ctrl         },      // SPEED DOWN
    // row 5
        { IR_CODE_AMS_RPT      , "COLOR_YELLOW"       , ir_btn_set_color          },      // COLOR_YELLOW
        { IR_CODE_ST_PROG      , "COLOR_CYAN"         , ir_btn_set_color          },      // COLOR_CYAN
        { IR_CODE_VOL_PLUS     , "COLOR_PURPLE"       , ir_btn_set_color          },      // COLOR_PURPLE 
        { IR_CODE_ZOOM         , "up"               , ir_btn_brightness_ctrl    },      // BRIGHTNESS_UP
    // row 6
        { IR_CODE_LOC_RDM      , "COLOR_PINK"         , ir_btn_set_color          },      // COLOR_PINK
        { IR_CODE_SEEK_MINUS   , "COLOR_LIGHTGREEN"   , ir_btn_set_color          },      // COLOR_LIGHTGREEN
        { IR_CODE_SEL          , "COLOR_VIOLET"       , ir_btn_set_color          },      // COLOR_VIOLET
        { IR_CODE_SEEK_PLUS    , "down"             , ir_btn_brightness_ctrl    },      // BRIGHTNESS DOWN
    // row 7
        { IR_CODE_PBC          , "COLOR_ORANGE"       , ir_btn_set_color          },      // COLOR_ORANGE
        { IR_CODE_OSD          , "COLOR_LIMEGREEN"    , ir_btn_set_color          },      // COLOR_LIMEGREEN
        { IR_CODE_VOL_MINUS    , "COLOR_LIGHTBLUE"    , ir_btn_set_color          },      // COLOR_LIGHTBLUE
        { IR_CODE_AUDIO        , "COLOR_BLACK"        , ir_btn_set_color          },      // RESET COLORS
    // row 8
        { IR_CODE_BTN_1        , "red_inc"          , ir_btn_brightness_ctrl    },     // RED Led - Increase
        { IR_CODE_BTN_2        , "green_inc"        , ir_btn_brightness_ctrl    },     // GREEN Led - Increase
        { IR_CODE_BTN_3        , "blue_inc"         , ir_btn_brightness_ctrl    },     // BLUE Led - Increase
        { IR_CODE_BTN_4        , "white_inc"        , ir_btn_led_white_ctrl     },     // WHITE Led - Increase
    // row 9
        { IR_CODE_BTN_5        , "red_dec"          , ir_btn_brightness_ctrl    },     // RED Led - Decrease
        { IR_CODE_BTN_6        , "green_dec"        , ir_btn_brightness_ctrl    },     // GREEN Led - Decrease
        { IR_CODE_BTN_7        , "blue_dec"         , ir_btn_brightness_ctrl    },     // BLUE Led - Decrease
        { IR_CODE_BTN_8        , "white_dec"        , ir_btn_led_white_ctrl     },     // WHITE Led - Decrease
    // row 10
        { IR_CODE_BTN_9        , "COLOR_RED"          , ir_btn_set_color          },    // RED Led
        { IR_CODE_BTN_0        , "COLOR_GREEN"        , ir_btn_set_color          },    // GREEN Led  
        { IR_CODE_BTN_10_PLUS  , "COLOR_BLUE"         , ir_btn_set_color          },    // BLUE Led
        { IR_CODE_GOTO         , "switch"           , ir_btn_led_white_ctrl     }    // WHITE Led
};

#endif