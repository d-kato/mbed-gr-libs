#include "LcdCfg_DVI_Stick.h"

const DisplayBase::lcd_config_t LcdCfgTbl_DVI_Stick = {
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
    , DisplayBase::LCD_TCON_PIN_3                                                   /* h_sync_port          */
    , DisplayBase::SIG_POL_INVERTED                                                 /* h_sync_port_polarity */
    , LCD_H_SYNC_WIDTH                                                              /* h_sync_width         */
    , DisplayBase::LCD_TCON_PIN_0                                                   /* v_sync_port          */
    , DisplayBase::SIG_POL_INVERTED                                                 /* v_sync_port_polarity */
    , LCD_V_SYNC_WIDTH                                                              /* v_sync_width         */
    , DisplayBase::LCD_TCON_PIN_4                                                   /* de_port              */
    , DisplayBase::SIG_POL_NOT_INVERTED                                             /* de_port_polarity     */
};

