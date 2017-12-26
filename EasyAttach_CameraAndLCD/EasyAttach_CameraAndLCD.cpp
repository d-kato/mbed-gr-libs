
#include "mbed.h"
#include "rtos.h"
#include "EasyAttach_CameraAndLCD.h"

#if MBED_CONF_APP_LCD
#if (MBED_CONF_APP_LCD_TYPE == GR_PEACH_4_3INCH_SHIELD) || (MBED_CONF_APP_LCD_TYPE == GR_PEACH_7_1INCH_SHIELD) || (MBED_CONF_APP_LCD_TYPE == GR_PEACH_RSK_TFT)
static DigitalOut lcd_pwon(P7_15);
static DigitalOut lcd_blon(P8_1);
static DigitalOut lcd_cntrst(P8_15);
#elif (MBED_CONF_APP_LCD_TYPE == GR_LYCHEE_LCD)
static DigitalOut lcd_pwon(P5_12);
static DigitalOut lcd_sd(P7_5);
static PwmOut lcd_cntrst(P3_12);
#endif
#ifndef VOLTAGE_ADJUSTMENT
  #define VOLTAGE_ADJUSTMENT   (1.00f)
#endif
#endif

static const DisplayBase::lcd_config_t * lcd_port_init(DisplayBase& Display) {
#if MBED_CONF_APP_LCD
  #if (MBED_CONF_APP_LCD_TYPE == GR_PEACH_4_3INCH_SHIELD) || (MBED_CONF_APP_LCD_TYPE == GR_PEACH_7_1INCH_SHIELD) || (MBED_CONF_APP_LCD_TYPE == GR_PEACH_RSK_TFT)
    PinName lvds_pin[8] = {
        /* data pin */
        P5_7, P5_6, P5_5, P5_4, P5_3, P5_2, P5_1, P5_0
    };

    lcd_pwon = 0;
    lcd_blon = 0;
    Thread::wait(100);
    lcd_pwon = 1;
    lcd_blon = 1;

    Display.Graphics_Lvds_Port_Init(lvds_pin, 8);

    return &LcdCfgTbl_LCD_shield;
  #elif MBED_CONF_APP_LCD_TYPE == GR_PEACH_DISPLAY_SHIELD
    PinName lcd_pin[28] = {
        /* data pin */
        P11_15, P11_14, P11_13, P11_12, P5_7, P5_6, P5_5, P5_4, P5_3, P5_2, P5_1, P5_0,
        P4_7, P4_6, P4_5, P4_4, P10_12, P10_13, P10_14, P10_15, P3_15, P3_14, P3_13,
        P3_12, P3_11, P3_10, P3_9, P3_8
    };
    Display.Graphics_Lcd_Port_Init(lcd_pin, 28);

    return &LcdCfgTbl_LCD_shield;
  #elif (MBED_CONF_APP_LCD_TYPE == GR_LYCHEE_LCD)
    PinName lcd_pin[28] = {
        /* data pin */
        P6_15, P6_14, P6_13, P6_12, P6_11, P6_10, P6_9, P6_8, P6_7, P6_6, P6_5, P6_4,
        P6_3, P6_2, P6_1, P6_0, P3_7, P3_6, P3_5, P3_4, P3_3, P3_2, P3_1, P3_0, P5_2,
        P5_1, P5_0, P7_4, 
    };

    lcd_pwon = 0;
    #if MBED_CONF_APP_LCD_TYPE == GR_LYCHEE_LCD
    lcd_sd = 0;
    #else
    lcd_sd = 1;
    #endif
    lcd_cntrst.period_us(500);
    Thread::wait(100);
    lcd_pwon = 1;
    Thread::wait(1);
    #if MBED_CONF_APP_LCD_TYPE == GR_LYCHEE_LCD
    lcd_sd = 1;
    #else
    lcd_sd = 0;
    #endif

    Display.Graphics_Lcd_Port_Init(lcd_pin, 28);

    return &LcdCfgTbl_LCD_shield;
  #endif
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

  #if MBED_CONF_APP_CAMERA_TYPE == CAMERA_MT9V111
    /* MT9V111 camera input config */
    ext_in_config.inp_format     = DisplayBase::VIDEO_EXTIN_FORMAT_BT601; /* BT601 8bit YCbCr format */
    ext_in_config.inp_pxd_edge   = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing data          */
    ext_in_config.inp_vs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Vsync signals */
    ext_in_config.inp_hs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Hsync signals */
    ext_in_config.inp_endian_on  = DisplayBase::OFF;                      /* External input bit endian change on/off       */
    ext_in_config.inp_swap_on    = DisplayBase::OFF;                      /* External input B/R signal swap on/off         */
    ext_in_config.inp_vs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_VSYNC inversion control     */
    ext_in_config.inp_hs_inv     = DisplayBase::SIG_POL_INVERTED;         /* External input DV_HSYNC inversion control     */
    ext_in_config.inp_f525_625   = DisplayBase::EXTIN_LINE_525;           /* Number of lines for BT.656 external input */
    ext_in_config.inp_h_pos      = DisplayBase::EXTIN_H_POS_CRYCBY;       /* Y/Cb/Y/Cr data string start timing to Hsync reference */
    ext_in_config.cap_vs_pos     = 6;                                     /* Capture start position from Vsync */
    ext_in_config.cap_hs_pos     = 150;                                   /* Capture start position form Hsync */
    if (cap_width != 0) {
        ext_in_config.cap_width  = cap_width;                             /* Capture width */
    } else {
        ext_in_config.cap_width  = 640;                                   /* Capture width Max */
    }
    if (cap_height != 0) {
        ext_in_config.cap_height = cap_height;                            /* Capture heigh */
    } else {
        ext_in_config.cap_height = 468u;                                  /* Capture height Max 468[line]
                                                                             Due to CMOS(MT9V111) output signal timing and VDC5 specification */
    }
  #elif MBED_CONF_APP_CAMERA_TYPE == CAMERA_OV7725
    /* OV7725 camera input config */
    OV7725_config::Initialise();

    ext_in_config.inp_format     = DisplayBase::VIDEO_EXTIN_FORMAT_BT601; /* BT601 8bit YCbCr format */
    ext_in_config.inp_pxd_edge   = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing data          */
    ext_in_config.inp_vs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Vsync signals */
    ext_in_config.inp_hs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Hsync signals */
    ext_in_config.inp_endian_on  = DisplayBase::OFF;                      /* External input bit endian change on/off       */
    ext_in_config.inp_swap_on    = DisplayBase::OFF;                      /* External input B/R signal swap on/off         */
    ext_in_config.inp_vs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_VSYNC inversion control     */
    ext_in_config.inp_hs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_HSYNC inversion control     */
    ext_in_config.inp_f525_625   = DisplayBase::EXTIN_LINE_525;           /* Number of lines for BT.656 external input */
    ext_in_config.inp_h_pos      = DisplayBase::EXTIN_H_POS_YCBYCR;       /* Y/Cb/Y/Cr data string start timing to Hsync reference */
    ext_in_config.cap_vs_pos     = 4+21;                                  /* Capture start position from Vsync */
    ext_in_config.cap_hs_pos     = 68;                                    /* Capture start position form Hsync */
    if (cap_width != 0) {
        ext_in_config.cap_width  = cap_width;                             /* Capture width */
    } else {
        ext_in_config.cap_width  = 640;                                   /* Capture width Max */
    }
    if (cap_height != 0) {
        ext_in_config.cap_height = cap_height;                            /* Capture heigh */
    } else {
        ext_in_config.cap_height = 480;                                   /* Capture height Max */
    }
  #elif MBED_CONF_APP_CAMERA_TYPE == CAMERA_OV5642
    /* OV5642 camera input config */
    OV5642_config::Initialise();

    ext_in_config.inp_format     = DisplayBase::VIDEO_EXTIN_FORMAT_BT601; /* BT601 8bit YCbCr format */
    ext_in_config.inp_pxd_edge   = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing data          */
    ext_in_config.inp_vs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Vsync signals */
    ext_in_config.inp_hs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Hsync signals */
    ext_in_config.inp_endian_on  = DisplayBase::OFF;                      /* External input bit endian change on/off       */
    ext_in_config.inp_swap_on    = DisplayBase::OFF;                      /* External input B/R signal swap on/off         */
    ext_in_config.inp_vs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_VSYNC inversion control     */
    ext_in_config.inp_hs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_HSYNC inversion control     */
    ext_in_config.inp_f525_625   = DisplayBase::EXTIN_LINE_525;           /* Number of lines for BT.656 external input */
    ext_in_config.inp_h_pos      = DisplayBase::EXTIN_H_POS_YCBYCR;       /* Y/Cb/Y/Cr data string start timing to Hsync reference */
    ext_in_config.cap_vs_pos     = 8;                                     /* Capture start position from Vsync */
    ext_in_config.cap_hs_pos     = 8;                                     /* Capture start position form Hsync */
    if (cap_width != 0) {
        ext_in_config.cap_width  = cap_width;                             /* Capture width */
    } else {
        ext_in_config.cap_width  = 640;                                   /* Capture width  */
    }
    if (cap_height != 0) {
        ext_in_config.cap_height = cap_height;                            /* Capture heigh */
    } else {
        ext_in_config.cap_height = 480u;                                  /* Capture height Max 480[line]
                                                                            Due to CMOS(MT9D111) output signal timing and VDC5 specification */
    }
  #else
    #error "No camera chosen. Please add 'config.camera-type.value' to your mbed_app.json (see README.md for more information)."
  #endif
    error = Display.Graphics_Video_init( DisplayBase::INPUT_SEL_EXT, &ext_in_config);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    /* camera input port setting */
    error = Display.Graphics_Dvinput_Port_Init(cmos_camera_pin, 11);
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
#if (MBED_CONF_APP_LCD_TYPE == GR_PEACH_4_3INCH_SHIELD) || (MBED_CONF_APP_LCD_TYPE == GR_PEACH_7_1INCH_SHIELD) || (MBED_CONF_APP_LCD_TYPE == GR_PEACH_RSK_TFT) || (MBED_CONF_APP_LCD_TYPE == GR_LYCHEE_LCD)
    lcd_cntrst = (value * VOLTAGE_ADJUSTMENT);
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
