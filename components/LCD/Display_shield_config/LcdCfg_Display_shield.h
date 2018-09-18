#ifndef LCD_CFG_DISPLAY_SHIELD_H
#define LCD_CFG_DISPLAY_SHIELD_H

#include "DisplayBace.h"

#define SVGA                                (0u)  /*  800x600@60  37.9kHz/60Hz */
#define XGA                                 (1u)  /* 1024x768@60  48.4kHz/60Hz */
#define HD_720p                             (2u)  /* 1280x720@60  45.0kHz/60Hz */

#ifndef LCD_SIZE
#define LCD_SIZE                            HD_720p /* Select SVGA, XGA, or HD_720p */
#endif

/* LCD Parameter */
#define LCD_INPUT_CLOCK                     (66.67)  /* not use */
#if ( LCD_SIZE == SVGA )
#define LCD_OUTPUT_CLOCK                    (40.0003)
#define LCD_PIXEL_WIDTH                     (800u)
#define LCD_PIXEL_HEIGHT                    (600u)
#define LCD_H_BACK_PORCH                    (88u)
#define LCD_H_FRONT_PORCH                   (40u)
#define LCD_H_SYNC_WIDTH                    (128u)
#define LCD_V_BACK_PORCH                    (23u)
#define LCD_V_FRONT_PORCH                   (1u)
#define LCD_V_SYNC_WIDTH                    (4u)
#elif ( LCD_SIZE == XGA )
#define LCD_OUTPUT_CLOCK                    (65.0002)
#define LCD_PIXEL_WIDTH                     (1024u)
#define LCD_PIXEL_HEIGHT                    (768u)
#define LCD_H_BACK_PORCH                    (160u)
#define LCD_H_FRONT_PORCH                   (24u)
#define LCD_H_SYNC_WIDTH                    (136u)
#define LCD_V_BACK_PORCH                    (29u)
#define LCD_V_FRONT_PORCH                   (3u)
#define LCD_V_SYNC_WIDTH                    (6u)
#elif ( LCD_SIZE == HD_720p )
#define LCD_OUTPUT_CLOCK                    (74.1800)
#define LCD_PIXEL_WIDTH                     (1280u)
#define LCD_PIXEL_HEIGHT                    (720u)
#define LCD_H_BACK_PORCH                    (220u)
#define LCD_H_FRONT_PORCH                   (70u)
#define LCD_H_SYNC_WIDTH                    (80u)
#define LCD_V_BACK_PORCH                    (20u)
#define LCD_V_FRONT_PORCH                   (5u)
#define LCD_V_SYNC_WIDTH                    (5u)
#endif

extern const DisplayBase::lcd_config_t LcdCfgTbl_Display_shield;

#endif

