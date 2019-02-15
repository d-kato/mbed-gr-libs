
#include "mbed.h"
#include "rtos.h"
#include "EasyAttach_CameraAndLCD.h"

#if MBED_CONF_APP_LCD
#if ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x0000)   /* GR-PEACH LVDS */
static DigitalOut lcd_cntrst(P8_15);
#elif ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x1000) /* GR-LYCHEE */
static PwmOut lcd_cntrst(P3_12);
#elif ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x2000) /* RZ/A2M */
#if MBED_CONF_APP_LCD_TYPE != RZ_A2M_DVI_STICK
static PwmOut lcd_cntrst(P7_6);
#endif
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
    ThisThread::sleep_for(100);
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
    ThisThread::sleep_for(100);
    lcd_pwon = 1;
  #elif ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x2000) /* RZ/A2M */
    PinName lcd_pin[28] = {
        /* data pin */
        PB_5, PB_4, PB_3, PB_2, PB_1, PB_0, PA_7, PA_6, PA_5, PA_4, PA_3, PA_2, PA_1, PA_0,
        P8_0, PF_0, PF_1, PF_2, PF_3, PF_4, PF_5, PF_6, PH_2, PF_7, PC_3, PC_4, P7_7, PJ_6
    };
    Display.Graphics_Lcd_Port_Init(lcd_pin, 28);
    #if MBED_CONF_APP_LCD_TYPE != RZ_A2M_DVI_STICK
    lcd_cntrst.period_us(500);
    #else
    const char send_cmd[3] = {0x08u, 0xbfu, 0x70u};
    #if defined(TARGET_RZ_A2M_SBEV) || defined(TARGET_SEMB1402)
    I2C mI2c_(PD_5, PD_4);
    #else
    I2C mI2c_(I2C_SDA, I2C_SCL);
    #endif
    mI2c_.write(0x78, send_cmd, 3);
    #endif
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
    DisplayBase::video_input_sel_t video_input_sel;

  #if CAMERA_MODULE == MODULE_MIPI
    // do nothing
  #elif defined(TARGET_RZ_A1H)
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
    ThisThread::sleep_for(1 + 1);
   #elif MBED_CONF_APP_SHIELD_TYPE == SHIELD_WIRELESS_CAMERA
    DigitalOut pwdn(P3_15);
    DigitalOut rstb(P3_14);
    pwdn = 0;
    rstb = 0;
    ThisThread::sleep_for(10 + 1);
    rstb = 1;
    ThisThread::sleep_for(1 + 1);
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
    ThisThread::sleep_for(10 + 1);
    rstb = 1;
    ThisThread::sleep_for(1 + 1);
  #elif defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_SBEV) || defined(TARGET_SEMB1402)
    PinName cmos_camera_pin[11] = {
        /* data pin */
        PE_1, PE_2, PE_3, PE_4, PE_5, PE_6, PH_0, PH_1,
        /* control pin */
        P6_1,       /* VIO_CLK   */
        P6_2,       /* VIO_VD */
        P6_3        /* VIO_HD */
    };
    DigitalOut camera_stby(PE_0);
    camera_stby = 0;
    ThisThread::sleep_for(1 + 1);
  #endif

    /* camera input port setting */
  #if CAMERA_MODULE == MODULE_VDC
    video_input_sel = DisplayBase::INPUT_SEL_EXT;
    error = Display.Graphics_Dvinput_Port_Init(cmos_camera_pin, 11);
  #elif CAMERA_MODULE == MODULE_CEU
    video_input_sel = DisplayBase::INPUT_SEL_CEU;
    error = Display.Graphics_Ceu_Port_Init(cmos_camera_pin, 11);
  #else
    video_input_sel = DisplayBase::INPUT_SEL_MIPI;
    error = DisplayBase::GRAPHICS_OK;
  #endif
    if( error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

  #if MBED_CONF_APP_CAMERA_TYPE == CAMERA_MT9V111
    MT9V111_config camera_cfg;
  #elif MBED_CONF_APP_CAMERA_TYPE == CAMERA_OV7725
    OV7725_config camera_cfg;
  #elif MBED_CONF_APP_CAMERA_TYPE == CAMERA_OV5642
    OV5642_config camera_cfg;
  #elif MBED_CONF_APP_CAMERA_TYPE == CAMERA_RASPBERRY_PI
    RaspberryPi_config camera_cfg;
  #else
    #error "No camera chosen. Please add 'config.camera-type.value' to your mbed_app.json (see README.md for more information)."
  #endif

  #if CAMERA_MODULE == MODULE_MIPI
    DisplayBase::video_mipi_param_t mipi_config;
    DisplayBase::video_vin_setup_t  vin_setup;

    camera_cfg.Initialise();
    camera_cfg.SetMipiConfig(&mipi_config);
    camera_cfg.SetVinSetup(&vin_setup);
    error = Display.Graphics_Video_init(video_input_sel, &mipi_config, &vin_setup);
  #else
    DisplayBase::video_ext_in_config_t ext_in_config;

    camera_cfg.Initialise();
    camera_cfg.SetExtInConfig(&ext_in_config);
    if (cap_width != 0) {
        ext_in_config.cap_width  = cap_width;                             /* Capture width */
    }
    if (cap_height != 0) {
        ext_in_config.cap_height = cap_height;                            /* Capture heigh */
    }

    error = Display.Graphics_Video_init(video_input_sel, &ext_in_config);
  #endif
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
#if ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x0000) || ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x1000) || ((MBED_CONF_APP_LCD_TYPE & 0xFF00) == 0x2000)
    #if MBED_CONF_APP_LCD_TYPE != RZ_A2M_DVI_STICK
    lcd_cntrst = (value * voltage_adjust);
    #endif
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

  #if CAMERA_MODULE == MODULE_VDC
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
#endif

    return DisplayBase::GRAPHICS_OK;
}
