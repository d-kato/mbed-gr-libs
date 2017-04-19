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

/******************************************************************************
* File Name    : ssif_if.h
* $Rev: $
* $Date::                           $
* Description  : SSIF Driver API header
******************************************************************************/

#ifndef SSIF_IF_H
#define SSIF_IF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include "cmsis_os.h"

#include "r_typedefs.h"
#include "r_errno.h"
#if(1) /* mbed */
#include "misratypes.h"
#include "aioif.h"
#include "R_BSP_mbed_fns.h"
#include "R_BSP_SsifDef.h"
#else  /* not mbed */
#include "ioif_public.h"
#endif /* end mbed */

#ifdef __cplusplus
extern "C" {
#endif

#if(1) /* mbed */
/******************************************************************************
 Function Prototypes
 *****************************************************************************/

extern RBSP_MBED_FNS* R_SSIF_MakeCbTbl_mbed(void);
#else  /* not mbed */
/******************************************************************************
 Defines
 *****************************************************************************/

/******************************************************************************
 Constant Macros
 *****************************************************************************/
#if defined(TARGET_RZA1H) /* mbed */
#define SSIF_NUM_CHANS       (6u)    /**< Number of SSIF channels */

#define SSIF_CFG_DISABLE_ROMDEC_DIRECT (0x0u)        /* Disable SSIRDR->STRMDIN0 route */
#define SSIF_CFG_ENABLE_ROMDEC_DIRECT  (0xDEC0DEC1u) /* Enable  SSIRDR->STRMDIN0 route */
#else /* mbed */
#define SSIF_NUM_CHANS       (4u)    /**< Number of SSIF channels */
#endif /* mbed */

/******************************************************************************
 Function Macros
 *****************************************************************************/


/******************************************************************************
 Enumerated Types
 *****************************************************************************/

/** SSICR:CKS(Clock source for oversampling) */
typedef enum
{
    SSIF_CFG_CKS_AUDIO_X1  = 0,  /**< select AUDIO_X1   */
    SSIF_CFG_CKS_AUDIO_CLK = 1   /**< select AUIDIO_CLK */
} ssif_chcfg_cks_t;

/** SSICR:CHNL(Audio channels per system word) */
typedef enum
{
    SSIF_CFG_MULTI_CH_1 = 0,     /**< 1ch within systemword (on tdm=0) */
    SSIF_CFG_MULTI_CH_2 = 1,     /**< 2ch within systemword (on tdm=0) */
    SSIF_CFG_MULTI_CH_3 = 2,     /**< 3ch within systemword (on tdm=0) */
    SSIF_CFG_MULTI_CH_4 = 3      /**< 4ch within systemword (on tdm=0) */
} ssif_chcfg_multi_ch_t;

/** SSICR:DWL(Data word length) */
typedef enum
{
    SSIF_CFG_DATA_WORD_8  = 0,   /**< Data word length 8  */
    SSIF_CFG_DATA_WORD_16 = 1,   /**< Data word length 16 */
    SSIF_CFG_DATA_WORD_18 = 2,   /**< Data word length 18 */
    SSIF_CFG_DATA_WORD_20 = 3,   /**< Data word length 20 */
    SSIF_CFG_DATA_WORD_22 = 4,   /**< Data word length 22 */
    SSIF_CFG_DATA_WORD_24 = 5,   /**< Data word length 24 */
    SSIF_CFG_DATA_WORD_32 = 6    /**< Data word length 32 */
} ssif_chcfg_data_word_t;

/** SSICR:SWL(System word length) */
typedef enum
{
    SSIF_CFG_SYSTEM_WORD_8   = 0,    /**< System word length 8   */
    SSIF_CFG_SYSTEM_WORD_16  = 1,    /**< System word length 16  */
    SSIF_CFG_SYSTEM_WORD_24  = 2,    /**< System word length 24  */
    SSIF_CFG_SYSTEM_WORD_32  = 3,    /**< System word length 32  */
    SSIF_CFG_SYSTEM_WORD_48  = 4,    /**< System word length 48  */
    SSIF_CFG_SYSTEM_WORD_64  = 5,    /**< System word length 64  */
    SSIF_CFG_SYSTEM_WORD_128 = 6,    /**< System word length 128 */
    SSIF_CFG_SYSTEM_WORD_256 = 7     /**< System word length 256 */
} ssif_chcfg_system_word_t;

/** SSICR:SCKP(Clock polarity) */
typedef enum
{
    SSIF_CFG_FALLING = 0,    /**< Falling edge */
    SSIF_CFG_RISING  = 1     /**< Rising edge  */
} ssif_chcfg_clock_pol_t;

/** SSICR:SWSP(Word select polarity) */
typedef enum
{
    SSIF_CFG_WS_LOW  = 0,    /**< Low for ther 1st channel(not TDM) */
    SSIF_CFG_WS_HIGH = 1     /**< High for the 1st channel(not TDM) */
} ssif_chcfg_ws_pol_t;

/** SSICR:SPDP(Serial padding polarity) */
typedef enum
{
    SSIF_CFG_PADDING_LOW  = 0,   /**< Padding bits are low  */
    SSIF_CFG_PADDING_HIGH = 1    /**< Padding bits are high */
} ssif_chcfg_padding_pol_t;

/** SSICR:SDTA(Serial data alignment) */
typedef enum
{
    SSIF_CFG_DATA_FIRST    = 0,  /**< Data first         */
    SSIF_CFG_PADDING_FIRST = 1   /**< Padding bits first */
} ssif_chcfg_serial_alignment_t;

/** SSICR:PDTA(Parallel data alignment) */
typedef enum
{
    SSIF_CFG_LEFT  = 0,  /**< Left aligned  */
    SSIF_CFG_RIGHT = 1   /**< Right aligned */
} ssif_chcfg_parallel_alignment_t;

/** SSICR:DEL(Serial data delay) */
typedef enum
{
    SSIF_CFG_DELAY    = 0,   /**< 1 clock delay */
    SSIF_CFG_NO_DELAY = 1    /**< No delay      */
} ssif_chcfg_ws_delay_t;

/** SSICR:CKDV(Serial oversampling clock division ratio) */
typedef enum
{
    SSIF_CFG_CKDV_BITS_1   = 0,
    SSIF_CFG_CKDV_BITS_2   = 1,
    SSIF_CFG_CKDV_BITS_4   = 2,
    SSIF_CFG_CKDV_BITS_8   = 3,
    SSIF_CFG_CKDV_BITS_16  = 4,
    SSIF_CFG_CKDV_BITS_32  = 5,
    SSIF_CFG_CKDV_BITS_64  = 6,
    SSIF_CFG_CKDV_BITS_128 = 7,
    SSIF_CFG_CKDV_BITS_6   = 8,
    SSIF_CFG_CKDV_BITS_12  = 9,
    SSIF_CFG_CKDV_BITS_24  = 10,
    SSIF_CFG_CKDV_BITS_48  = 11,
    SSIF_CFG_CKDV_BITS_96  = 12
} ssif_chcfg_ckdv_t;


/** SNCR:SSIxNL(Serial sound interface channel x noise canceler enable) */
typedef enum
{
    SSIF_CFG_DISABLE_NOISE_CANCEL = 0, /**< Not use noise cancel function */
    SSIF_CFG_ENABLE_NOISE_CANCEL  = 1  /**< Use noise cancel function     */
} ssif_chcfg_noise_cancel_t;


/** SSITDMR:TDM(TDM mode) */
typedef enum
{
    SSIF_CFG_DISABLE_TDM = 0,   /**< not TDM mode */
    SSIF_CFG_ENABLE_TDM  = 1    /**< set TDM mode */
} ssif_chcfg_tdm_t;

/******************************************************************************
 Structures
 *****************************************************************************/

#if defined(TARGET_RZA1H) /* mbed */
typedef struct
{
    uint32_t    mode;               /* Enable/Disable SSIRDR->STRMDIN0 route */
    void        (*p_cbfunc)(void);  /* SSIF error callback function          */
} ssif_chcfg_romdec_t;

#endif /* mbed */
/**< This structure contains the configuration settings */
typedef struct
{
    bool_t                          enabled;             /* The enable flag for the channel       */
    uint8_t                         int_level;           /* Interrupt priority for the channel    */
    bool_t                          slave_mode;          /* Mode of operation                     */
    uint32_t                        sample_freq;         /* Audio Sampling frequency(Hz)          */
    ssif_chcfg_cks_t                clk_select;          /* SSICR-CKS : Audio clock select        */
    ssif_chcfg_multi_ch_t           multi_ch;            /* SSICR-CHNL: Audio channels per system word */
    ssif_chcfg_data_word_t          data_word;           /* SSICR-DWL : Data word length          */
    ssif_chcfg_system_word_t        system_word;         /* SSICR-SWL : System word length        */
    ssif_chcfg_clock_pol_t          bclk_pol;            /* SSICR-SCKP: Bit Clock polarity        */
    ssif_chcfg_ws_pol_t             ws_pol;              /* SSICR-SWSP: Word Clock polarity       */
    ssif_chcfg_padding_pol_t        padding_pol;         /* SSICR-SPDP: Padding polarity          */
    ssif_chcfg_serial_alignment_t   serial_alignment;    /* SSICR-SDTA: Serial data alignment     */
    ssif_chcfg_parallel_alignment_t parallel_alignment;  /* SSICR-PDTA: Parallel data alignment   */
    ssif_chcfg_ws_delay_t           ws_delay;            /* SSICR-DEL : Serial clock delay        */
    ssif_chcfg_noise_cancel_t       noise_cancel;        /* GPIO-SNCR : Noise cancel              */
    ssif_chcfg_tdm_t                tdm_mode;            /* SSITDMR-TDM: TDM mode                 */
#if defined(TARGET_RZA1H) /* mbed */
    ssif_chcfg_romdec_t             romdec_direct;       /* DMA : SSIRDR->STRMDIN0 route settings */
#endif /* mbed */
} ssif_channel_cfg_t;

/******************************************************************************
 IOCTLS
 *****************************************************************************/

#define SSIF_CONFIG_CHANNEL        (7)
#define SSIF_GET_STATUS           (13)


/******************************************************************************
 External Data
 *****************************************************************************/


/******************************************************************************
 Function Prototypes
 *****************************************************************************/
/**
* @ingroup API
*
* This function returns a pointer to the function table of the SSIF driver.
* This is intended to be used as a parameter in the ioif_start_device function.
*
* @retval IOIF_DRV_API* - Pointer to the table of functions supported by the
* driver.
*
*/
extern IOIF_DRV_API *R_SSIF_MakeCbTbl(void);
#endif /* end mbed */

extern int_t R_SSIF_SWLtoLen(const ssif_chcfg_system_word_t ssicr_swl);

extern int_t R_SSIF_Userdef_InitPinMux(const uint32_t ssif_ch);
extern int_t R_SSIF_Userdef_SetClockDiv(const ssif_channel_cfg_t* const p_ch_cfg, ssif_chcfg_ckdv_t* const p_clk_div);

extern uint16_t R_SSIF_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* SSIF_IF_H */
/*EOF*/

