
#ifndef LCD_CFG_RGB_TO_HDMI_H
#define LCD_CFG_RGB_TO_HDMI_H

#include "DisplayBace.h"

#define SD_7INCH                            (0u)  /*  800x480@60 */
#define SVGA                                (1u)  /*  800x600@60 */
#define XGA                                 (2u)  /* 1024x768@60 */
#define HD_720p                             (3u)  /* 1280x720@60 */

#ifndef LCD_SIZE
#define LCD_SIZE                            HD_720p /* Select SD_7INCH, SVGA, XGA, or HD_720p */
#endif

/* LCD Parameter */
#define LCD_INPUT_CLOCK                     (66.67)  /* not use */
#if ( LCD_SIZE == SD_7INCH )
#define LCD_OUTPUT_CLOCK                    (33.26)
#define LCD_PIXEL_WIDTH                     (800u)
#define LCD_PIXEL_HEIGHT                    (480u)
#define LCD_H_BACK_PORCH                    (128u)
#define LCD_H_FRONT_PORCH                   (92u)
#define LCD_H_SYNC_WIDTH                    (20u)
#define LCD_V_BACK_PORCH                    (35u)
#define LCD_V_FRONT_PORCH                   (5u)
#define LCD_V_SYNC_WIDTH                    (10u)
#elif ( LCD_SIZE == SVGA )
#define LCD_OUTPUT_CLOCK                    (40.0003)
#define LCD_PIXEL_WIDTH                     (800u)
#define LCD_PIXEL_HEIGHT                    (600u)
#define LCD_H_BACK_PORCH                    (88u)
#define LCD_H_FRONT_PORCH                   (40u)
#define LCD_H_SYNC_WIDTH                    (128u)
#define LCD_V_BACK_PORCH                    (20u)
#define LCD_V_FRONT_PORCH                   (4u)
#define LCD_V_SYNC_WIDTH                    (4u)
#elif ( LCD_SIZE == XGA )
#define LCD_OUTPUT_CLOCK                    (65.0002)
#define LCD_PIXEL_WIDTH                     (1024u)
#define LCD_PIXEL_HEIGHT                    (768u)
#define LCD_H_BACK_PORCH                    (160u)
#define LCD_H_FRONT_PORCH                   (24u)
#define LCD_H_SYNC_WIDTH                    (136u)
#define LCD_V_BACK_PORCH                    (28u)
#define LCD_V_FRONT_PORCH                   (4u)
#define LCD_V_SYNC_WIDTH                    (6u)
#elif ( LCD_SIZE == HD_720p )
#define LCD_OUTPUT_CLOCK                    (74.2500)
#define LCD_PIXEL_WIDTH                     (1280u)
#define LCD_PIXEL_HEIGHT                    (720u)
#define LCD_H_BACK_PORCH                    (216u)
#define LCD_H_FRONT_PORCH                   (72u)
#define LCD_H_SYNC_WIDTH                    (80u)
#define LCD_V_BACK_PORCH                    (22u)
#define LCD_V_FRONT_PORCH                   (3u)
#define LCD_V_SYNC_WIDTH                    (5u)
#endif

extern const DisplayBase::lcd_config_t LcdCfgTbl_rgb_to_hdmi;

#endif


