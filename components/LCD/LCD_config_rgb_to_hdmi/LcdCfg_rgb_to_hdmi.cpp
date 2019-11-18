#include "LcdCfg_rgb_to_hdmi.h"

#if defined(TARGET_GR_MANGO)
  #define LCD_H_SYNC_PORT   DisplayBase::LCD_TCON_PIN_0
  #define LCD_V_SYNC_PORT   DisplayBase::LCD_TCON_PIN_1
  #define LCD_DE_PORT       DisplayBase::LCD_TCON_PIN_2
#elif defined(TARGET_RZ_A2XX)
  #define LCD_H_SYNC_PORT   DisplayBase::LCD_TCON_PIN_3
  #define LCD_V_SYNC_PORT   DisplayBase::LCD_TCON_PIN_0
  #define LCD_DE_PORT       DisplayBase::LCD_TCON_PIN_4
#else
  #define LCD_H_SYNC_PORT   DisplayBase::LCD_TCON_PIN_NON
  #define LCD_V_SYNC_PORT   DisplayBase::LCD_TCON_PIN_NON
  #define LCD_DE_PORT       DisplayBase::LCD_TCON_PIN_NON
#endif

const DisplayBase::lcd_config_t LcdCfgTbl_rgb_to_hdmi = {
      DisplayBase::LCD_TYPE_PARALLEL_RGB                                            /* lcd_type             */
    , LCD_INPUT_CLOCK                                                               /* intputClock          */
    , LCD_OUTPUT_CLOCK                                                              /* outputClock          */
    , DisplayBase::LCD_OUTFORMAT_RGB888                                             /* lcd_outformat        */
    , DisplayBase::EDGE_FALLING                                                     /* lcd_edge             */
    , (LCD_PIXEL_WIDTH  + LCD_H_FRONT_PORCH + LCD_H_BACK_PORCH + LCD_H_SYNC_WIDTH)  /* h_toatal_period      */
    , (LCD_PIXEL_HEIGHT + LCD_V_FRONT_PORCH + LCD_V_BACK_PORCH + LCD_V_SYNC_WIDTH)  /* v_toatal_period      */
    , LCD_PIXEL_WIDTH                                                               /* h_disp_widht         */
    , LCD_PIXEL_HEIGHT                                                              /* v_disp_widht         */
    , (LCD_H_BACK_PORCH + LCD_H_SYNC_WIDTH)                                         /* h_back_porch         */
    , (LCD_V_BACK_PORCH + LCD_V_SYNC_WIDTH)                                         /* v_back_porch         */
    , LCD_H_SYNC_PORT                                                               /* h_sync_port          */
    , DisplayBase::SIG_POL_INVERTED                                                 /* h_sync_port_polarity */
    , LCD_H_SYNC_WIDTH                                                              /* h_sync_width         */
    , LCD_V_SYNC_PORT                                                               /* v_sync_port          */
    , DisplayBase::SIG_POL_INVERTED                                                 /* v_sync_port_polarity */
    , LCD_V_SYNC_WIDTH                                                              /* v_sync_width         */
    , LCD_DE_PORT                                                                   /* de_port              */
    , DisplayBase::SIG_POL_NOT_INVERTED                                             /* de_port_polarity     */
};

