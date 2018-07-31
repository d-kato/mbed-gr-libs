
#include "mbed.h"
#include "rtos.h"
#include "EasyAttach_CameraAndLCD.h"

#if MBED_CONF_APP_LCD
#if ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x0000)   /* GR-PEACH LVDS */
static DigitalOut lcd_cntrst(P8_15);
#elif ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x1000) /* GR-LYCHEE */
static PwmOut lcd_cntrst(P3_12);
#endif
#define MAX_BACKLIGHT_VOL       (33.0f)
#ifndef TYPICAL_BACKLIGHT_VOL
  #define TYPICAL_BACKLIGHT_VOL (33.0f)
#endif
static float voltage_adjust = (TYPICAL_BACKLIGHT_VOL / MAX_BACKLIGHT_VOL);
#endif

static const DisplayBase::lcd_config_t * lcd_port_init(DisplayBase& Display) {
#if MBED_CONF_APP_LCD
  #if ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x0000)   /* GR-PEACH LVDS */
    PinName lvds_pin[8] = {
        /* data pin */
        P5_7, P5_6, P5_5, P5_4, P5_3, P5_2, P5_1, P5_0
    };
    Display.Graphics_Lvds_Port_Init(lvds_pin, 8);

    DigitalOut lcd_pwon(P7_15);
    DigitalOut lcd_blon(P8_1);
    lcd_pwon = 0;
    lcd_blon = 0;
    Thread::wait(100);
    lcd_pwon = 1;
    lcd_blon = 1;
  #elif ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x0100) /* GR-PEACH RGB */
    PinName lcd_pin[28] = {
        /* data pin */
        P11_15, P11_14, P11_13, P11_12, P5_7, P5_6, P5_5, P5_4, P5_3, P5_2, P5_1, P5_0,
        P4_7, P4_6, P4_5, P4_4, P10_12, P10_13, P10_14, P10_15, P3_15, P3_14, P3_13,
        P3_12, P3_11, P3_10, P3_9, P3_8
    };
    Display.Graphics_Lcd_Port_Init(lcd_pin, 28);
  #elif ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x1000) /* GR-LYCHEE */
    PinName lcd_pin[28] = {
        /* data pin */
        P6_15, P6_14, P6_13, P6_12, P6_11, P6_10, P6_9, P6_8, P6_7, P6_6, P6_5, P6_4,
        P6_3, P6_2, P6_1, P6_0, P3_7, P3_6, P3_5, P3_4, P3_3, P3_2, P3_1, P3_0, P5_2,
        P5_1, P5_0, P7_4, 
    };
    Display.Graphics_Lcd_Port_Init(lcd_pin, 28);

    DigitalOut lcd_pwon(P5_12);
    lcd_pwon = 0;
    lcd_cntrst.period_us(500);
    Thread::wait(100);
    lcd_pwon = 1;
  #endif
    return &LcdCfgTbl_LCD_shield;
#else
    return NULL;
#endif
}

static DisplayBase::graphics_error_t camera_init(DisplayBase& Display, uint16_t cap_width, uint16_t cap_height) {
#if MBED_CONF_APP_CAMERA
 #if MBED_CONF_APP_CAMERA_TYPE == CAMERA_CVBS
    DisplayBase::graphics_error_t error;
    error = Display.Graphics_Video_init(DisplayBase::INPUT_SEL_VDEC, NULL);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    return DisplayBase::GRAPHICS_OK;
 #else
    DisplayBase::graphics_error_t error;
    DisplayBase::video_ext_in_config_t ext_in_config;

  #if defined(TARGET_RZ_A1H)
    PinName cmos_camera_pin[11] = {
        /* data pin */
        P2_7, P2_6, P2_5, P2_4, P2_3, P2_2, P2_1, P2_0,
        /* control pin */
        P10_0,      /* DV0_CLK   */
        P1_0,       /* DV0_Vsync */
        P1_1        /* DV0_Hsync */
    };
   #if MBED_CONF_APP_SHIELD_TYPE == SHIELD_AUDIO_CAMERA
    DigitalOut pwdn(P3_12);
    pwdn = 0;
    Thread::wait(1 + 1);
   #elif MBED_CONF_APP_SHIELD_TYPE == SHIELD_WIRELESS_CAMERA
    DigitalOut pwdn(P3_15);
    DigitalOut rstb(P3_14);
    pwdn = 0;
    rstb = 0;
    Thread::wait(10 + 1);
    rstb = 1;
    Thread::wait(1 + 1);
   #endif
  #elif defined(TARGET_GR_LYCHEE)
    PinName cmos_camera_pin[11] = {
        /* data pin */
        P1_0, P1_1, P1_2, P1_3, P1_8, P1_9, P1_10, P1_11,
        /* control pin */
        P7_8,       /* DV0_CLK   */
        P7_9,       /* DV0_Vsync */
        P7_10       /* DV0_Hsync */
    };
    DigitalOut pwdn(P7_11);
    DigitalOut rstb(P2_3);

    pwdn = 0;
    rstb = 0;
    Thread::wait(10 + 1);
    rstb = 1;
    Thread::wait(1 + 1);
  #endif

    /* camera input port setting */
    error = Display.Graphics_Dvinput_Port_Init(cmos_camera_pin, 11);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

  #if MBED_CONF_APP_CAMERA_TYPE == CAMERA_MT9V111
    MT9V111_config camera_cfg;
  #elif MBED_CONF_APP_CAMERA_TYPE == CAMERA_OV7725
    OV7725_config camera_cfg;
  #elif MBED_CONF_APP_CAMERA_TYPE == CAMERA_OV5642
    OV5642_config camera_cfg;
  #else
    #error "No camera chosen. Please add 'config.camera-type.value' to your mbed_app.json (see README.md for more information)."
  #endif

    camera_cfg.Initialise();
    camera_cfg.SetExtInConfig(&ext_in_config);
    if (cap_width != 0) {
        ext_in_config.cap_width  = cap_width;                             /* Capture width */
    }
    if (cap_height != 0) {
        ext_in_config.cap_height = cap_height;                            /* Capture heigh */
    }

    error = Display.Graphics_Video_init(DisplayBase::INPUT_SEL_EXT, &ext_in_config);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    return DisplayBase::GRAPHICS_OK;
 #endif
#else
    return DisplayBase::GRAPHICS_OK;
#endif
}

DisplayBase::graphics_error_t EasyAttach_Init(DisplayBase& Display, uint16_t cap_width, uint16_t cap_height) {
    DisplayBase::graphics_error_t error;
    const DisplayBase::lcd_config_t * lcd_config = lcd_port_init(Display);

    error = Display.Graphics_init(lcd_config);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }
    return camera_init(Display, cap_width, cap_height);
}

void EasyAttach_SetTypicalBacklightVol(float typ_vol) {
#if MBED_CONF_APP_LCD
    if (typ_vol > MAX_BACKLIGHT_VOL) {
        voltage_adjust = 1.0f;
    } else {
        voltage_adjust = (typ_vol / MAX_BACKLIGHT_VOL);
    }
#endif
}

void EasyAttach_LcdBacklight(bool type) {
#if MBED_CONF_APP_LCD
    if (type) {
        EasyAttach_LcdBacklight(1.0f);
    } else {
        EasyAttach_LcdBacklight(0.0f);
    }
#endif
}

void EasyAttach_LcdBacklight(float value) {
#if MBED_CONF_APP_LCD
#if ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x0000) || ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x1000)
    lcd_cntrst = (value * voltage_adjust);
#endif
#endif
}

DisplayBase::graphics_error_t EasyAttach_CameraStart(DisplayBase& Display, DisplayBase::video_input_channel_t channel) {
#if MBED_CONF_APP_CAMERA
    DisplayBase::graphics_error_t error;

    /* Video write process start */
    error = Display.Video_Start(channel);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    /* Video write process stop */
    error = Display.Video_Stop(channel);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    /* Video write process start */
    error = Display.Video_Start(channel);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }
#endif

    return DisplayBase::GRAPHICS_OK;
}
