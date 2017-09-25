
#ifndef EASY_ATTACH_CAMERA_AND_LCD_H
#define EASY_ATTACH_CAMERA_AND_LCD_H

#include "DisplayBace.h"

// camera
#if MBED_CONF_APP_CAMERA
  // camera-type
  #define CAMERA_CVBS                 1
  #define CAMERA_MT9V111              2
  #define CAMERA_OV7725               3

  #ifndef MBED_CONF_APP_CAMERA_TYPE
    #if defined(TARGET_RZ_A1H)
      #define MBED_CONF_APP_CAMERA_TYPE    CAMERA_MT9V111
    #elif defined(TARGET_GR_LYCHEE)
      #define MBED_CONF_APP_CAMERA_TYPE    CAMERA_OV7725
    #endif
  #endif

  #if (MBED_CONF_APP_CAMERA_TYPE == CAMERA_CVBS) && defined(TARGET_GR_LYCHEE)
    #error "MBED_CONF_APP_CAMERA_TYPE is not supported in this target."
  #endif

  #if MBED_CONF_APP_CAMERA_TYPE == CAMERA_OV7725
    #include "OV7725_config.h"
  #endif
#endif

// lcd
#if MBED_CONF_APP_LCD
  // lcd-type
  #define GR_PEACH_4_3INCH_SHIELD     1
  #define GR_PEACH_7_1INCH_SHIELD     2
  #define GR_PEACH_RSK_TFT            3
  #define GR_PEACH_DISPLAY_SHIELD     4
  #define GR_LYCHEE_LCD               5

  #ifndef MBED_CONF_APP_LCD_TYPE
    #if defined(TARGET_RZ_A1H)
      #define MBED_CONF_APP_LCD_TYPE    GR_PEACH_4_3INCH_SHIELD
    #elif defined(TARGET_GR_LYCHEE)
      #define MBED_CONF_APP_LCD_TYPE    GR_LYCHEE_LCD
    #endif
  #endif

  #if (MBED_CONF_APP_LCD_TYPE == GR_LYCHEE_LCD) && defined(TARGET_RZ_A1H)
    #error "MBED_CONF_APP_LCD_TYPE is not supported in this target."
  #endif
  #if (MBED_CONF_APP_LCD_TYPE >= GR_PEACH_4_3INCH_SHIELD) && (MBED_CONF_APP_LCD_TYPE <= GR_PEACH_DISPLAY_SHIELD) && defined(TARGET_GR_LYCHEE)
    #error "MBED_CONF_APP_LCD_TYPE is not supported in this target."
  #endif

  #if MBED_CONF_APP_LCD_TYPE == GR_PEACH_4_3INCH_SHIELD
    #include "LCD_shield_config_4_3inch.h"
  #elif MBED_CONF_APP_LCD_TYPE == GR_PEACH_7_1INCH_SHIELD
    #include "LCD_shield_config_7_1inch.h"
  #elif MBED_CONF_APP_LCD_TYPE == GR_PEACH_RSK_TFT
    #include "LCD_shield_config_RSK_TFT.h"
  #elif MBED_CONF_APP_LCD_TYPE == GR_PEACH_DISPLAY_SHIELD
    #include "Display_shield_config.h"
  #elif MBED_CONF_APP_LCD_TYPE == GR_LYCHEE_LCD
    #include "LCD_config_lychee.h"
  #else
    #error "No lcd chosen. Please add 'config.lcd-type.value' to your mbed_app.json (see README.md for more information)."
  #endif
#endif

extern DisplayBase::graphics_error_t EasyAttach_Init(
    DisplayBase& Display,
    uint16_t cap_width = 0,
    uint16_t cap_height = 0
);

extern DisplayBase::graphics_error_t EasyAttach_CameraStart(
    DisplayBase& Display,
    DisplayBase::video_input_channel_t channel = DisplayBase::VIDEO_INPUT_CHANNEL_0
);

extern void EasyAttach_LcdBacklight(bool type = true);

extern void EasyAttach_LcdBacklight(float value);

#endif
