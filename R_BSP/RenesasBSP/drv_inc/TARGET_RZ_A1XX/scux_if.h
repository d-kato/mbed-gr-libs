/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2013-2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file          scux_if.h
* $Rev: $
* $Date::                           $
* @brief         SCUX Driver IOIF API header
******************************************************************************/

#ifndef SCUX_IF_H
#define SCUX_IF_H

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/

#include "cmsis_os.h"
#include "r_errno.h"
#include "r_typedefs.h"
#if(1) /* mbed */
#include "ioif_aio.h"
#include "misratypes.h"
#include "R_BSP_mbed_fns.h"
#include "R_BSP_ScuxDef.h"
#else  /* not mbed */
#include "ioif_public.h"
#endif /* end mbed */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*************************************************************************
 Enumerated Types
*************************************************************************/

#if(1) /* mbed */
#else  /* not mbed */
/* Number of SCUX channel */
typedef enum
{
    SCUX_CH_0 = 0,
    SCUX_CH_1 = 1,
    SCUX_CH_2 = 2,
    SCUX_CH_3 = 3,
    SCUX_CH_NUM = 4  /* Number of SCUX channel */
} scux_ch_num_t;
#endif /* end mbed */

/* SCUX route setting */
typedef enum
{
#if defined(TARGET_RZA1H) /* mbed */
    /* mem to mem */
    SCUX_ROUTE_SRC_MEM_MIN = 0x1000,
    SCUX_ROUTE_SRC0_MEM    = 0x1001,
    SCUX_ROUTE_SRC1_MEM    = 0x1002,
    SCUX_ROUTE_SRC2_MEM    = 0x1003,
    SCUX_ROUTE_SRC3_MEM    = 0x1004,
    SCUX_ROUTE_SRC_MEM_MAX = 0x1005,
    /* mem to SSIF */
    SCUX_ROUTE_SRC_SSIF_MIN = 0x2000,
    SCUX_ROUTE_SRC0_SSIF0   = 0x2001,
    SCUX_ROUTE_SRC0_SSIF012 = 0x2002,
    SCUX_ROUTE_SRC0_SSIF3   = 0x2003,
    SCUX_ROUTE_SRC0_SSIF345 = 0x2004,
    SCUX_ROUTE_SRC1_SSIF0   = 0x2005,
    SCUX_ROUTE_SRC1_SSIF012 = 0x2006,
    SCUX_ROUTE_SRC1_SSIF3   = 0x2007,
    SCUX_ROUTE_SRC1_SSIF345 = 0x2008,    
    SCUX_ROUTE_SRC2_SSIF1   = 0x2009,
    SCUX_ROUTE_SRC2_SSIF4   = 0x200A,
    SCUX_ROUTE_SRC3_SSIF2   = 0x200B,
    SCUX_ROUTE_SRC3_SSIF5   = 0x200C,
    SCUX_ROUTE_SRC_SSIF_MAX = 0x200D,
    /* mem to NIX to SSIF */
    SCUX_ROUTE_SRC_MIX_SSIF_MIN  = 0x3000,
    SCUX_ROUTE_SRC0_MIX_SSIF0   = 0x3001,
    SCUX_ROUTE_SRC0_MIX_SSIF012 = 0x3002,
    SCUX_ROUTE_SRC0_MIX_SSIF3   = 0x3003,
    SCUX_ROUTE_SRC0_MIX_SSIF345 = 0x3004,
    SCUX_ROUTE_SRC1_MIX_SSIF0   = 0x3005,
    SCUX_ROUTE_SRC1_MIX_SSIF012 = 0x3006,
    SCUX_ROUTE_SRC1_MIX_SSIF3   = 0x3007,
    SCUX_ROUTE_SRC1_MIX_SSIF345 = 0x3008,   
    SCUX_ROUTE_SRC2_MIX_SSIF0   = 0x3009,
    SCUX_ROUTE_SRC2_MIX_SSIF012 = 0x300A,
    SCUX_ROUTE_SRC2_MIX_SSIF3   = 0x300B,
    SCUX_ROUTE_SRC2_MIX_SSIF345 = 0x300C,
    SCUX_ROUTE_SRC3_MIX_SSIF0   = 0x300D,
    SCUX_ROUTE_SRC3_MIX_SSIF012 = 0x300E,
    SCUX_ROUTE_SRC3_MIX_SSIF3   = 0x300F,
    SCUX_ROUTE_SRC3_MIX_SSIF345 = 0x3010,   
    SCUX_ROUTE_SRC_MIX_SSIF_MAX = 0x3011
#else
    /* mem to mem */
    SCUX_ROUTE_SRC_MEM_MIN = 0x1000,
    SCUX_ROUTE_SRC0_MEM    = 0x1001,
    SCUX_ROUTE_SRC1_MEM    = 0x1002,
    SCUX_ROUTE_SRC2_MEM    = 0x1003,
    SCUX_ROUTE_SRC3_MEM    = 0x1004,
    SCUX_ROUTE_SRC_MEM_MAX = 0x1005,
    /* mem to SSIF */
    SCUX_ROUTE_SRC_SSIF_MIN = 0x2000,
    SCUX_ROUTE_SRC0_SSIF0   = 0x2001,
    SCUX_ROUTE_SRC0_SSIF012 = 0x2002,
    SCUX_ROUTE_SRC0_SSIF3   = 0x2003,
    SCUX_ROUTE_SRC1_SSIF0   = 0x2004,
    SCUX_ROUTE_SRC1_SSIF012 = 0x2005,
    SCUX_ROUTE_SRC1_SSIF3   = 0x2006,
    SCUX_ROUTE_SRC2_SSIF1   = 0x2007,
    SCUX_ROUTE_SRC3_SSIF2   = 0x2008,
    SCUX_ROUTE_SRC_SSIF_MAX = 0x2009,
    /* mem to MIX to SSIF */
    SCUX_ROUTE_SRC_MIX_SSIF_MIN  = 0x3000,
    SCUX_ROUTE_SRC0_MIX_SSIF0   = 0x3001,
    SCUX_ROUTE_SRC0_MIX_SSIF012 = 0x3002,
    SCUX_ROUTE_SRC0_MIX_SSIF3   = 0x3003,
    SCUX_ROUTE_SRC1_MIX_SSIF0   = 0x3004,
    SCUX_ROUTE_SRC1_MIX_SSIF012 = 0x3005,
    SCUX_ROUTE_SRC1_MIX_SSIF3   = 0x3006,
    SCUX_ROUTE_SRC2_MIX_SSIF0   = 0x3007,
    SCUX_ROUTE_SRC2_MIX_SSIF012 = 0x3008,
    SCUX_ROUTE_SRC2_MIX_SSIF3   = 0x3009,
    SCUX_ROUTE_SRC3_MIX_SSIF0   = 0x300A,
    SCUX_ROUTE_SRC3_MIX_SSIF012 = 0x300B,
    SCUX_ROUTE_SRC3_MIX_SSIF3   = 0x300C,
    SCUX_ROUTE_SRC_MIX_SSIF_MAX = 0x300D
#endif /* mbed */
} scux_route_t;

#if(1) /* mbed */
#else  /* not mbed */
/* used audio channel number setting */
typedef enum
{
    SCUX_USE_CH_1 = 1,   /* used audio channel number is 1 */
    SCUX_USE_CH_2 = 2,   /* used audio channel number is 2 */
    SCUX_USE_CH_4 = 4,   /* used audio channel number is 4 */
    SCUX_USE_CH_6 = 6,   /* used audio channel number is 6 */
    SCUX_USE_CH_8 = 8    /* used audio channel number is 8 */
} scux_use_channel_t;
#endif /* end mbed */

/* select audio channel number setting */
typedef enum
{
    SCUX_AUDIO_CH_MIN = -1,
    SCUX_AUDIO_CH_0 = 0,   /* select audio channel number is 0 */
    SCUX_AUDIO_CH_1 = 1,   /* select audio channel number is 1 */
    SCUX_AUDIO_CH_2 = 2,   /* select audio channel number is 2 */
    SCUX_AUDIO_CH_3 = 3,   /* select audio channel number is 3 */
    SCUX_AUDIO_CH_4 = 4,   /* select audio channel number is 4 */
    SCUX_AUDIO_CH_5 = 5,   /* select audio channel number is 5 */
    SCUX_AUDIO_CH_6 = 6,   /* select audio channel number is 6 */
    SCUX_AUDIO_CH_7 = 7,   /* select audio channel number is 7 */
    SCUX_AUDIO_CH_MAX = 8
} scux_audio_channel_t;

#if(1) /* mbed */
#else  /* not mbed */
/* SCUX data word length */
typedef enum
{
    SCUX_DATA_LEN_MIN      =(-1),
    SCUX_DATA_LEN_24       = 0,   /* data word length is 24bit */
    SCUX_DATA_LEN_16       = 1,   /* data word length is 16bit */
    SCUX_DATA_LEN_16_TO_24 = 2,   /* data word length is 24bit */
    SCUX_DATA_LEN_MAX      = 3
} scux_data_word_len_t;
#endif /* end mbed */

/* SSIF PIN mode setting */
typedef enum
{
    SCUX_PIN_MODE_MIN          =(-1),
    SCUX_PIN_MODE_INDEPEND     = 0,   /* PIN mode is independent */
    SCUX_PIN_MODE_SLAVE_SLAVE  = 1,   /* PIN mode is slave-slave */
    SCUX_PIN_MODE_MASTER_SLAVE = 2,   /* PIN mode is master-slave */
    SCUX_PIN_MODE_MAX = 3
} scux_pin_mode_t;

/* SCUX sampling rate */
typedef enum
{
    SCUX_SYNC_RATE_8      = 8000,    /* 8KHz */
    SCUX_SYNC_RATE_11_025 = 11025,   /* 11.025KHz */
    SCUX_SYNC_RATE_12     = 12000,   /* 12KHz */
    SCUX_SYNC_RATE_16     = 16000,   /* 16KHz */
    SCUX_SYNC_RATE_22_05  = 22050,   /* 22.05KHz */
    SCUX_SYNC_RATE_24     = 24000,   /* 24KHz */
    SCUX_SYNC_RATE_32     = 32000,   /* 32KHz */
    SCUX_SYNC_RATE_44_1   = 44100,   /* 44.1KHz */
    SCUX_SYNC_RATE_48     = 48000,   /* 48KHz */
    SCUX_SYNC_RATE_64     = 64000,   /* 64KHz */
    SCUX_SYNC_RATE_88_2   = 88200,   /* 88.2KHz */
    SCUX_SYNC_RATE_96     = 96000    /* 96KHz */
} scux_src_sync_rate_t;

/* SCUX clock source setting */
typedef enum
{
    SCUX_CLK_MIN  =(-1),
    SCUX_CLK_AUDIO_CLK  = 0,   /* clock source is AUDIO_CLK */
    SCUX_CLK_AUDIO_X1   = 1,   /* clock source is AUDIO_X1 */
    SCUX_CLK_MLB_CLK    = 2,   /* clock source is MLB_CLK */
    SCUX_CLK_USB_X1     = 3,   /* clock source is USB_X1 */
    SCUX_CLK_CLKP1_2    = 4,   /* clock source is surrounding clock */
    SCUX_CLK_MTU_TIOC3A = 5,   /* clock source is TIOC3A */
    SCUX_CLK_MTU_TIOC4A = 6,   /* clock source is TIOC4A */
    SCUX_CLK_SSIF0_WS   = 8,   /* clock source is SSIF0 WS */
    SCUX_CLK_SSIF1_WS   = 9,   /* clock source is SSIF1 WS */
    SCUX_CLK_SSIF2_WS   = 10,   /* clock source is SSIF2 WS */
    SCUX_CLK_SSIF3_WS   = 11,  /* clock source is SSIF3 WS */
#if defined(TARGET_RZA1H) /* mbed */
    SCUX_CLK_SSIF4_WS   = 12,  /* clock source is SSIF4 WS */
    SCUX_CLK_SSIF5_WS   = 13,  /* clock source is SSIF5 WS */
#endif /* mbed */
    SCUX_CLK_MAX
} scux_src_clk_source_t;

/* SCUX delay mode setting */
typedef enum
{
    SCUX_DELAY_MIN         =(-1),
    SCUX_DELAY_NORMAL      = 0,    /* Delay mode is normal */
    SCUX_DELAY_LOW_DELAY1  = 1,    /* Delay mode is Low Delay1 */
    SCUX_DELAY_LOW_DELAY2  = 2,    /* Delay mode is Low Delay2 */
    SCUX_DELAY_MAX         = 3
} scux_src_delay_mode_t;

/* DVU ramp time setting */
typedef enum
{
    SCUX_DVU_TIME_MIN  =(-1),
    SCUX_DVU_TIME_128DB_1STEP      = 0,   /* volume change 128DB among 1 step */
    SCUX_DVU_TIME_64DB_1STEP       = 1,   /* volume change 64DB among 1 step */
    SCUX_DVU_TIME_32DB_1STEP       = 2,   /* volume change 32DB among 1 step */
    SCUX_DVU_TIME_16DB_1STEP       = 3,   /* volume change 16DB among 1 step */
    SCUX_DVU_TIME_8DB_1STEP        = 4,   /* volume change 8DB among 1 step */
    SCUX_DVU_TIME_4DB_1STEP        = 5,   /* volume change 4DB among 1 step */
    SCUX_DVU_TIME_2DB_1STEP        = 6,   /* volume change 2DB among 1 step */
    SCUX_DVU_TIME_1DB_1STEP        = 7,   /* volume change 1DB among 1 step */
    SCUX_DVU_TIME_0_5DB_1STEP      = 8,   /* volume change 0.5DB among 1 step */
    SCUX_DVU_TIME_0_25DB_1STEP     = 9,   /* volume change 0.25DB among 1 step */
    SCUX_DVU_TIME_0_125DB_1STEP    = 10,  /* volume change 0.125DB among 1 step */
    SCUX_DVU_TIME_0_125DB_2STEP    = 11,  /* volume change 0.125DB among 2 step */
    SCUX_DVU_TIME_0_125DB_4STEP    = 12,  /* volume change 0.125DB among 4 step */
    SCUX_DVU_TIME_0_125DB_8STEP    = 13,  /* volume change 0.125DB among 8 step */
    SCUX_DVU_TIME_0_125DB_16STEP   = 14,  /* volume change 0.125DB among 16 step */
    SCUX_DVU_TIME_0_125DB_32STEP   = 15,  /* volume change 0.125DB among 32 step */
    SCUX_DVU_TIME_0_125DB_64STEP   = 16,  /* volume change 0.125DB among 64 step */
    SCUX_DVU_TIME_0_125DB_128STEP  = 17,  /* volume change 0.125DB among 128 step */
    SCUX_DVU_TIME_0_125DB_256STEP  = 18,  /* volume change 0.125DB among 256 step */
    SCUX_DVU_TIME_0_125DB_512STEP  = 19,  /* volume change 0.125DB among 512 step */
    SCUX_DVU_TIME_0_125DB_1024STEP = 20,  /* volume change 0.125DB among 1024 step */
    SCUX_DVU_TIME_0_125DB_2048STEP = 21,  /* volume change 0.125DB among 2048 step */
    SCUX_DVU_TIME_0_125DB_4096STEP = 22,  /* volume change 0.125DB among 4096 step */
    SCUX_DVU_TIME_0_125DB_8192STEP = 23,  /* volume change 0.125DB among 8192 step */
    SCUX_DVU_TIME_MAX              = 24
} scux_dvu_ramp_time_t;

/* MIX ramp time setting */
typedef enum
{
    SCUX_MIX_TIME_MIN            =(-1),
    SCUX_MIX_TIME_128DB_1STEP    = 0,    /* volume change 128DB among 1 step */
    SCUX_MIX_TIME_64DB_1STEP     = 1,    /* volume change 64DB among 1 step */
    SCUX_MIX_TIME_32DB_1STEP     = 2,    /* volume change 32DB among 1 step */
    SCUX_MIX_TIME_16DB_1STEP     = 3,    /* volume change 16DB among 1 step */
    SCUX_MIX_TIME_8DB_1STEP      = 4,    /* volume change 8DB among 1 step */
    SCUX_MIX_TIME_4DB_1STEP      = 5,    /* volume change 4DB among 1 step */
    SCUX_MIX_TIME_2DB_1STEP      = 6,    /* volume change 2DB among 1 step */
    SCUX_MIX_TIME_1DB_1STEP      = 7,    /* volume change 1DB among 1 step */
    SCUX_MIX_TIME_0_5DB_1STEP    = 8,    /* volume change 0.5DB among 1 step */
    SCUX_MIX_TIME_0_25DB_1STEP   = 9,    /* volume change 0.25DB among 1 step */
    SCUX_MIX_TIME_0_125DB_1STEP  = 10,   /* volume change 0.125DB among 1 step */
    SCUX_MIX_TIME_MAX            = 11
} scux_mix_ramp_time_t;

/* SSIF channels */
typedef enum
{
    SCUX_SSIF_CH_0    = 0,    /* volume change 128DB among 1 step */
    SCUX_SSIF_CH_1    = 1,    /* volume change 64DB among 1 step */
    SCUX_SSIF_CH_2    = 2,    /* volume change 32DB among 1 step */
    SCUX_SSIF_CH_3    = 3,    /* volume change 16DB among 1 step */
#if defined(TARGET_RZA1H) /* mbed */
    SCUX_SSIF_CH_4    = 4,    /* volume change 8DB among 1 step */
    SCUX_SSIF_CH_5    = 5,    /* volume change 4DB among 1 step */
#endif /* mbed */
    SCUX_SSIF_CH_NUM
} scux_ssif_ch_num_t;

/* SSIF channels */
typedef enum
{
    SCUX_SSIF_SYSTEM_LEN_MIN   = 0,
    SCUX_SSIF_SYSTEM_LEN_16    = 1,    /* SSIF system word length is 16bit */
    SCUX_SSIF_SYSTEM_LEN_24    = 2,    /* SSIF system word length is 24bit */
    SCUX_SSIF_SYSTEM_LEN_32    = 3,    /* SSIF system word length is 32bit */
    SCUX_SSIF_SYSTEM_LEN_48    = 4,    /* SSIF system word length is 48bit */
    SCUX_SSIF_SYSTEM_LEN_64    = 5,    /* SSIF system word length is 64bit */
    SCUX_SSIF_SYSTEM_LEN_128   = 6,    /* SSIF system word length is 128bit */
    SCUX_SSIF_SYSTEM_LEN_256   = 7,    /* SSIF system word length is 256bit */
    SCUX_SSIF_SYSTEM_LEN_MAX   = 8
} scux_ssif_system_len_t;

/******************************************************************************
Macro definitions
******************************************************************************/

/* SCUX IOCTL function code */
#define SCUX_IOCTL_MIN                 (-1)
#define SCUX_IOCTL_SET_START           0   /* start transfer */
#define SCUX_IOCTL_SET_FLUSH_STOP      1   /* set flush stop function */
#define SCUX_IOCTL_SET_CLEAR_STOP      2   /* set clear stop function */
#define SCUX_IOCTL_SET_ROUTE           3   /* set route parameter */
#define SCUX_IOCTL_SET_PIN_CLK         4   /* set pin clock parameter */
#define SCUX_IOCTL_SET_PIN_MODE        5   /* set pin mode parameter */
#define SCUX_IOCTL_SET_SRC_CFG         6   /* set SRC parameter */
#define SCUX_IOCTL_SET_DVU_CFG         7   /* set DVU parameter */
#define SCUX_IOCTL_SET_DVU_DIGI_VOL    8   /* set dgital volume parameter */
#define SCUX_IOCTL_SET_DVU_RAMP_VOL    9   /* set ramp volume parameter */
#define SCUX_IOCTL_SET_ZEROCROSS_MUTE  10  /* set zerocross mute paramter */
#define SCUX_IOCTL_SET_STOP_MUTE       11  /* set mute stop */
#define SCUX_IOCTL_SET_MIX_CFG         12  /* set MIX parameter */
#define SCUX_IOCTL_SET_MIX_VOL         13  /* set MIX volume parameter */
#define SCUX_IOCTL_SET_SSIF_CFG        14  /* set SSIF parameter */
#define SCUX_IOCTL_GET_WRITE_STAT      15  /* get write status */
#define SCUX_IOCTL_GET_READ_STAT       16  /* get read status */
#define SCUX_IOCTL_GET_DVU_STAT        17  /* get DVU status */
#define SCUX_IOCTL_GET_MUTE_STAT       18  /* get MUTE status */
#define SCUX_IOCTL_GET_MIX_STAT        19  /* get MIX status */
#define SCUX_IOCTL_MAX                 20

#if(1) /* mbed */
#else  /* not mbed */
/* SCUX status */
#define SCUX_STAT_STOP    0  /* request regstration isn't start */
#define SCUX_STAT_IDLE    1  /* request isn't receied */
#define SCUX_STAT_TRANS   2  /* under data transfer execution */
#endif /* end mbed */

/* DVU status */
#define SCUX_DVU_STAT_MUTE          0  /* DVU volume is mute */
#define SCUX_DVU_STAT_RAMP_DOWN     1  /* DVU volume is ramp down */
#define SCUX_DVU_STAT_RAMP_UP       2  /* DVU volume is ramp up */
#define SCUX_DVU_STAT_RAMP_FIXED    3  /* DVU volume change is stop */
#define SCUX_DVU_STAT_ORIGINAL_SIZE 4  /* DVU volume is original size */

/* MIX status */
#define SCUX_MIX_STAT_RAMP_FIXED  0  /* MIX volume change is stop */
#define SCUX_MIX_STAT_RAMP_DOWN   1  /* MIX volume is ramp down */
#define SCUX_MIX_STAT_RAMP_UP     2  /* MIX volume is ramp up */

/*************************************************************************
 Structures
*************************************************************************/

/* SSIF channel synchronous setting */
typedef struct
{
    scux_ssif_ch_num_t     ssif_ch_num;       /* ssif channel number */
    scux_pin_mode_t        pin_mode;          /* SSIF sync setting */
} scux_ssif_pin_mode_t;

/* SSIF AUDIO_CLK source parameters */
typedef struct
{
    scux_ssif_ch_num_t     ssif_ch_num;       /* ssif channel number */
    scux_src_clk_source_t  pin_clk;           /* AUDIO_CLK source */
} scux_ssif_pin_clk_t;

/* SSIF parameters */
typedef struct
{
    scux_ssif_ch_num_t     ssif_ch_num;       /* ssif channel number */
    bool_t                 mode_master;       /* master mode / slave mode select */
    bool_t                 select_audio_clk;  /* AUDIO_X1 / AUDIO_CLK select */
    scux_ssif_system_len_t system_word;       /* system word length */
    bool_t                 sck_polarity_rise; /* SCK polarity type select */
    bool_t                 ws_polarity_high;  /* WS polarity type select */
    bool_t                 padding_high;      /* padding type select */
    bool_t                 serial_data_align; /* serial data alingment type select */
    bool_t                 ws_delay;          /* WS delay type select */
    bool_t                 use_noise_cancel;  /* noise cancel ON / OFF select */
    bool_t                 use_tdm;           /* TDM mode ON / OFF select */
} scux_ssif_cfg_t;

/* MIX parameters */
typedef struct
{
    bool_t               mixmode_ramp;                          /* ramp mode / step mpde select */
    scux_mix_ramp_time_t up_period;                             /* ramp up period */
    scux_mix_ramp_time_t down_period;                           /* ramp down period */
    uint32_t             mix_vol[SCUX_CH_NUM];                  /* MIX volume value */
    scux_audio_channel_t select_out_data_ch[SCUX_AUDIO_CH_MAX]; /* audio data position setting */
} scux_mix_cfg_t;

/* zerocross mute parameters */
typedef struct
{
    bool_t               zc_mute_enable[SCUX_AUDIO_CH_MAX];     /* zerocross mute enable setting */
    void                 (*pcallback[SCUX_AUDIO_CH_MAX])(void); /* callback pointer for zerocross */
} scux_zc_mute_t;

/* ramp volume parameters */
typedef struct
{
    bool_t               ramp_vol_enable[SCUX_AUDIO_CH_MAX];  /* ramp volume enable setting */
    scux_dvu_ramp_time_t up_period;                           /* ramp up period */
    scux_dvu_ramp_time_t down_period;                         /* ramp down period */
    uint32_t             ramp_vol;                            /* ramp volume value */
    uint32_t             ramp_wait_time;                      /* wait time for volume change */
} scux_dvu_ramp_vol_t;

/* digital volume parameters */
typedef struct
{
    bool_t               digi_vol_enable;             /* digital volume enable setting */
    uint32_t             digi_vol[SCUX_AUDIO_CH_MAX]; /* digital volume value */
} scux_dvu_digi_vol_t;

/* DVU parameters */
typedef struct
{
    bool_t               dvu_enable;          /* DVU enable setting */
    scux_dvu_digi_vol_t  dvu_digi_vol;        /* digital volume setting */
    scux_dvu_ramp_vol_t  dvu_ramp_vol;        /* ramp volume setting */
    scux_zc_mute_t       dvu_zc_mute;         /* zerocross mute setting */
} scux_dvu_cfg_t;

/* SRC parameters */
typedef struct
{
    bool_t                src_enable;          /* SRC enable setting */
    scux_use_channel_t    use_ch;              /* used data channel setting */
    scux_data_word_len_t  word_len;            /* used word length setting */
    bool_t                mode_sync;           /* sync mode/async mode select */
    scux_src_sync_rate_t  input_rate_sync;     /* input rate on sync mode */
    scux_src_clk_source_t input_clk_async;      /* input clock source on async mode */
    uint32_t              input_div_async;     /* input divide rate on async mode */
    scux_src_sync_rate_t  output_rate_sync;    /* output rate on sync mode */
    scux_src_clk_source_t output_clk_async;    /* output clock source on async mode */
    uint32_t              output_div_async;    /* output divide rate on async mode */
    uint32_t              input_ws;            /* input WS frequency */
    uint32_t              output_ws;           /* output WS frequency */
    uint32_t              freq_tioc3a;         /* frequency of TIOC3A */
    uint32_t              freq_tioc4a;         /* frequency of TIOC4A */
    scux_src_delay_mode_t delay_mode;          /* delay mode setting */
    uint32_t              wait_sample;         /* wait time setting */
    uint8_t               min_rate_percentage; /* minimum rate setting */
    scux_audio_channel_t  select_in_data_ch[SCUX_AUDIO_CH_MAX]; /* audio data position setting */
} scux_src_cfg_t;

/* SCUX setup parameter */
typedef struct
{
    bool_t           enabled;               /* channel used flag */
    uint8_t          int_level;             /* interrupt priority */
    scux_route_t     route;                 /* reoute setting */
    scux_src_cfg_t   src_cfg;               /* SRC parameters */
} scux_channel_cfg_t;

/******************************************************************************
Functions Prototypes
******************************************************************************/

#if(1) /* mbed */
RBSP_MBED_FNS* R_SCUX_MakeCbTbl_mbed(void);
#else  /* not mbed */
IOIF_DRV_API* R_SCUX_MakeCbTbl(void);
#endif /* end mbed */

extern uint16_t R_SCUX_GetVersion(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SCUX_IF_H */
