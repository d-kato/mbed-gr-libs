
#ifndef LCD_CFG_4_3INCH_H
#define LCD_CFG_4_3INCH_H

#include "DisplayBace.h"

/* LCD Parameter */
#define LCD_INPUT_CLOCK                     (66.67)  /* not use */
#define LCD_OUTPUT_CLOCK                    (13.40f)
#define LCD_PIXEL_WIDTH                     (480u)
#define LCD_PIXEL_HEIGHT                    (272u)
#define LCD_H_BACK_PORCH                    (43u)
#define LCD_H_FRONT_PORCH                   (52u)
#define LCD_H_SYNC_WIDTH                    (41u)
#define LCD_V_BACK_PORCH                    (12u)
#define LCD_V_FRONT_PORCH                   (2u)
#define LCD_V_SYNC_WIDTH                    (10u)

extern const DisplayBase::lcd_config_t LcdCfgTbl_4_3inch;

#endif


