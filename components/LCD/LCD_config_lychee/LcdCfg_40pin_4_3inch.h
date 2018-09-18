
#ifndef LCD_CFG_40PIN_4_3INCH_H
#define LCD_CFG_40PIN_4_3INCH_H

#include "DisplayBace.h"

/* LCD Parameter */
#define LCD_INPUT_CLOCK                     (64.0)  /* not use */
#define LCD_OUTPUT_CLOCK                    (9)
#define LCD_PIXEL_WIDTH                     (480u)
#define LCD_PIXEL_HEIGHT                    (272u)
#define LCD_H_BACK_PORCH                    (43u)
#define LCD_H_FRONT_PORCH                   (8u)
#define LCD_H_SYNC_WIDTH                    (1u)
#define LCD_V_BACK_PORCH                    (12u)
#define LCD_V_FRONT_PORCH                   (4u)
#define LCD_V_SYNC_WIDTH                    (10u)

extern const DisplayBase::lcd_config_t LcdCfgTbl_40pin_4_3inch;

#endif

