
#ifndef LCD_CFG_LYCHEE_H
#define LCD_CFG_LYCHEE_H

#include "DisplayBace.h"

/* LCD Parameter */
#define LCD_INPUT_CLOCK                     (64.0)
#define LCD_OUTPUT_CLOCK                    (6.50)
#define LCD_PIXEL_WIDTH                     (320u)
#define LCD_PIXEL_HEIGHT                    (240u)
#define LCD_H_BACK_PORCH                    (68u)
#define LCD_H_FRONT_PORCH                   (20u)
#define LCD_H_SYNC_WIDTH                    (2u)
#define LCD_V_BACK_PORCH                    (18u)
#define LCD_V_FRONT_PORCH                   (4u)
#define LCD_V_SYNC_WIDTH                    (2u)

extern const DisplayBase::lcd_config_t LcdCfgTbl_lychee;

#endif


