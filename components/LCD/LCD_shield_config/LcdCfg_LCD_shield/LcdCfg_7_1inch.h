
#ifndef LCD_CFG_7_1INCH_H
#define LCD_CFG_7_1INCH_H

#include "DisplayBace.h"

/* LCD Parameter */
#define LCD_INPUT_CLOCK                     (66.67)  /* not use */
#define LCD_OUTPUT_CLOCK                    (33.26)
#define LCD_PIXEL_WIDTH                     (800u)
#define LCD_PIXEL_HEIGHT                    (480u)
#define LCD_H_BACK_PORCH                    (128u)
#define LCD_H_FRONT_PORCH                   (92u)
#define LCD_H_SYNC_WIDTH                    (36u)
#define LCD_V_BACK_PORCH                    (35u)
#define LCD_V_FRONT_PORCH                   (5u)
#define LCD_V_SYNC_WIDTH                    (5u)

extern const DisplayBase::lcd_config_t LcdCfgTbl_7_1inch;

#endif

