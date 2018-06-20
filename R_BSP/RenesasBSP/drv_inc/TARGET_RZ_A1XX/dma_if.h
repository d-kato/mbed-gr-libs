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
* http://www.renesas.com/disclaimer*
* Copyright (C) 2013-2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/**************************************************************************//**
* @file         dma_if.h
* $Rev: $
* $Date::                           $
* @brief        DMA Driver interface headers
******************************************************************************/

/*****************************************************************************
* History : DD.MM.YYYY Version Description
* : 15.01.2013 1.00 First Release
******************************************************************************/

#ifndef DMA_IF_H
#define DMA_IF_H

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/

#include "cmsis_os.h"
#include "r_errno.h"
#include "r_typedefs.h"
#include "ioif_aio.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*************************************************************************
 User Includes
*************************************************************************/

/*************************************************************************
 Defines
*************************************************************************/

/* for searching free channel */
#define DMA_ALLOC_CH  (-1)

/*************************************************************************
 Enumerated Types
*************************************************************************/

/* Number od DMA channel */
typedef enum
{
    DMA_CH_0 = 0,
    DMA_CH_1 = 1,
    DMA_CH_2 = 2,
    DMA_CH_3 = 3,
    DMA_CH_4 = 4,
    DMA_CH_5 = 5,
    DMA_CH_6 = 6,
    DMA_CH_7 = 7,
    DMA_CH_8 = 8,
    DMA_CH_9 = 9,
    DMA_CH_10 = 10,
    DMA_CH_11 = 11,
    DMA_CH_12 = 12,
    DMA_CH_13 = 13,
    DMA_CH_14 = 14,
    DMA_CH_15 = 15,
    DMA_CH_NUM = 16  /* Number of DMA channel */
} dma_ch_num_t;

/* Unit Size of DMA transfer */
typedef enum
{
    DMA_UNIT_MIN =(-1),
    DMA_UNIT_1 = 0,   /* Unit Size of DMA transfer = 1byte */
    DMA_UNIT_2 = 1,   /* Unit Size of DMA transfer = 2byte */
    DMA_UNIT_4 = 2,   /* Unit Size of DMA transfer = 4byte */
    DMA_UNIT_8 = 3,   /* Unit Size of DMA transfer = 8byte */
    DMA_UNIT_16 = 4,  /* Unit Size of DMA transfer = 16byte */
    DMA_UNIT_32 = 5,  /* Unit Size of DMA transfer = 32byte */
    DMA_UNIT_64 = 6,  /* Unit Size of DMA transfer = 64byte */
    DMA_UNIT_128 = 7, /* Unit Size of DMA transfer = 128byte */
    DMA_UNIT_MAX = 8
} dma_unit_size_t;

/* DMA transfer resource */
typedef enum
{
    DMA_RS_OSTIM0     = 0x023,   /* OS Timer ch0 */
    DMA_RS_OSTIM1     = 0x027,   /* OS Timer ch1 */
    DMA_RS_TGI0A      = 0x043,   /* Multi Function Timer Pulse Unit2 ch0 */
    DMA_RS_TGI1A      = 0x047,   /* Multi Function Timer Pulse Unit2 ch1  */
    DMA_RS_TGI2A      = 0x04B,   /* Multi Function Timer Pulse Unit2 ch2  */
    DMA_RS_TGI3A      = 0x04F,   /* Multi Function Timer Pulse Unit2 ch3  */
    DMA_RS_TGI4A      = 0x053,   /* Multi Function Timer Pulse Unit2 ch4  */
    DMA_RS_TXI0       = 0x061,   /* FIFO Serial Communication Interface ch0 (TX) */
    DMA_RS_RXI0       = 0x062,   /* FIFO Serial Communication Interface ch0 (RX) */
    DMA_RS_TXI1       = 0x065,   /* FIFO Serial Communication Interface ch1 (TX) */
    DMA_RS_RXI1       = 0x066,   /* FIFO Serial Communication Interface ch1 (RX) */
    DMA_RS_TXI2       = 0x069,   /* FIFO Serial Communication Interface ch2 (TX) */
    DMA_RS_RXI2       = 0x06A,   /* FIFO Serial Communication Interface ch2 (RX) */
    DMA_RS_TXI3       = 0x06D,   /* FIFO Serial Communication Interface ch3 (TX) */
    DMA_RS_RXI3       = 0x06E,   /* FIFO Serial Communication Interface ch3 (RX) */
    DMA_RS_TXI4       = 0x071,   /* FIFO Serial Communication Interface ch4 (TX) */
    DMA_RS_RXI4       = 0x072,   /* FIFO Serial Communication Interface ch4 (RX) */
#if defined(TARGET_RZA1H) /* mbed */
    DMA_RS_TXI5       = 0x075,   /* FIFO Serial Communication Interface ch5 (TX) */
    DMA_RS_RXI5       = 0x076,   /* FIFO Serial Communication Interface ch5 (RX) */
    DMA_RS_TXI6       = 0x079,   /* FIFO Serial Communication Interface ch6 (TX) */
    DMA_RS_RXI6       = 0x07A,   /* FIFO Serial Communication Interface ch6 (RX) */
    DMA_RS_TXI7       = 0x07D,   /* FIFO Serial Communication Interface ch7 (TX) */
    DMA_RS_RXI7       = 0x07E,   /* FIFO Serial Communication Interface ch7 (RX) */
#endif /* mbed */
    DMA_RS_USB0_DMA0  = 0x083,   /* USB Module0 ch0 */
    DMA_RS_USB0_DMA1  = 0x087,   /* USB Module0 ch1 */
    DMA_RS_USB1_DMA0  = 0x08B,   /* USB Module1 ch0 */
    DMA_RS_USB1_DMA1  = 0x08F,   /* USB Module1 ch1 */
    DMA_RS_ADEND      = 0x093,   /* A/D Converter */
#if defined(TARGET_RZA1H) /* mbed */
    DMA_RS_IEBBTD     = 0x0A3,   /* IEBus Controller (Data interrupt) */
    DMA_RS_IEBBTV     = 0x0A7,   /* IEBus Controller (Vector interrupt) */
    DMA_RS_IREADY     = 0x0AB,   /* CD-Rom Decoder */
    DMA_RS_FLDT       = 0x0B3,   /* NAND Memory Controller (Data) */
#endif /* mbed */
    DMA_RS_SDHI_0T    = 0x0C1,   /* SD Host Interface0 (TX) */
    DMA_RS_SDHI_0R    = 0x0C2,   /* SD Host Interface0 (RX) */
    DMA_RS_SDHI_1T    = 0x0C5,   /* SD Host Interface1 (RX) */
    DMA_RS_SDHI_1R    = 0x0C6,   /* SD Host Interface1 (TX) */
    DMA_RS_MMCT       = 0x0C9,   /* MMC Host Interface (TX) */
    DMA_RS_MMCR       = 0x0CA,   /* MMC Host Interface (RX) */
    DMA_RS_SSITXI0    = 0x0E1,   /* SSIF0 (TX) */
    DMA_RS_SSIRXI0    = 0x0E2,   /* SSIF0 (RX) */
    DMA_RS_SSITXI1    = 0x0E5,   /* SSIF1 (TX) */
    DMA_RS_SSIRXI1    = 0x0E6,   /* SSIF1 (RX) */
    DMA_RS_SSIRTI2    = 0x0EB,   /* SSIF2 (TX) */
    DMA_RS_SSITXI3    = 0x0ED,   /* SSIF2 (RTX) */
    DMA_RS_SSIRXI3    = 0x0EE,   /* SSIF3 (TX) */
#if defined(TARGET_RZA1H) /* mbed */
    DMA_RS_SSIRTI4    = 0x0F3,   /* SSIF4 (RTX) */
    DMA_RS_SSITXI5    = 0x0F5,   /* SSIF5 (TX) */
    DMA_RS_SSIRXI5    = 0x0F6,   /* SSIF5 (RX) */
#endif /* mbed */
    DMA_RS_SCUTXI0    = 0x101,   /* SCUX (FFD0) */
    DMA_RS_SCURXI0    = 0x102,   /* SCUX (FFU0) */
    DMA_RS_SCUTXI1    = 0x105,   /* SCUX (FFD1) */
    DMA_RS_SCURXI1    = 0x106,   /* SCUX (FFU1) */
    DMA_RS_SCUTXI2    = 0x109,   /* SCUX (FFD2) */
    DMA_RS_SCURXI2    = 0x10A,   /* SCUX (FFU2) */
    DMA_RS_SCUTXI3    = 0x10D,   /* SCUX (FFD3) */
    DMA_RS_SCURXI3    = 0x10E,   /* SCUX (FFU3) */
    DMA_RS_SPTI0      = 0x121,   /* SPI0 (TX) */
    DMA_RS_SPRI0      = 0x122,   /* SPI0 (RX) */
    DMA_RS_SPTI1      = 0x125,   /* SPI1 (TX) */
    DMA_RS_SPRI1      = 0x126,   /* SPI1 (RX) */
    DMA_RS_SPTI2      = 0x129,   /* SPI2 (TX) */
    DMA_RS_SPRI2      = 0x12A,   /* SPI2 (RX) */
#if defined(TARGET_RZA1H) /* mbed */
    DMA_RS_SPTI3      = 0x12B,   /* SPI3 (TX) */
    DMA_RS_SPRI3      = 0x12E,   /* SPI3 (RX) */
    DMA_RS_SPTI4      = 0x131,   /* SPI4 (TX) */
    DMA_RS_SPRI4      = 0x132,   /* SPI4 (RX) */
#endif /* mbed */
    DMA_RS_SPDIFTXI   = 0x141,   /* SPDIF (TX) */
    DMA_RS_SPDIFRXI   = 0x142,   /* SPDIF (RX) */
#if defined(TARGET_RZA1H) /* mbed */
    DMA_RS_CMI1       = 0x147,   /* Motor Control PWM Timer ch1 */
    DMA_RS_CMI2       = 0x14B,   /* Motor Control PWM Timer ch2 */
    DMA_RS_MLBCI      = 0x14F,   /* Media Local Bus */
    DMA_RS_SGDEI0     = 0x153,   /* Sound Generator0 */
    DMA_RS_SGDEI1     = 0x157,   /* Sound Generator1 */
    DMA_RS_SGDEI2     = 0x15B,   /* Sound Generator2 */
    DMA_RS_SGDEI3     = 0x15F,   /* Sound Generator3 */
#endif /* mbed */
    DMA_RS_SCITXI0    = 0x169,   /* Serial Communication Interface ch0 (TX) */
    DMA_RS_SCIRXI0    = 0x16A,   /* Serial Communication Interface ch0 (RX) */
    DMA_RS_SCITXI1    = 0x16D,   /* Serial Communication Interface ch1 (TX) */
    DMA_RS_SCIRXI1    = 0x16E,   /* Serial Communication Interface ch1 (RX) */
    DMA_RS_TI0        = 0x181,   /* IIC ch0 (TX) */
    DMA_RS_RI0        = 0x182,   /* IIC ch0 (RX) */
    DMA_RS_TI1        = 0x185,   /* IIC ch1 (TX) */
    DMA_RS_RI1        = 0x186,   /* IIC ch1 (RX) */
    DMA_RS_TI2        = 0x189,   /* IIC ch2 (TX) */
    DMA_RS_RI2        = 0x18A,   /* IIC ch2 (RX) */
    DMA_RS_TI3        = 0x18D,   /* IIC ch3 (TX) */
    DMA_RS_RI3        = 0x18E,   /* IIC ch3 (RX) */
#if defined(TARGET_RZA1H) /* mbed */
    DMA_RS_LIN0_INT_T = 0x1A1,   /* LIN0 (TX) */
    DMA_RS_LIN0_INT_R = 0x1A2,   /* LIN0 (RX) */
    DMA_RS_LIN1_INT_T = 0x1A5,   /* LIN1 (TX) */
    DMA_RS_LIN1_INT_R = 0x1A6,   /* LIN1 (RX) */
    DMA_RS_IFEI0      = 0x1B1,   /* Pixel Format Converter ch0 (TX) */
    DMA_RS_OFFI0      = 0x1B2,   /* Pixel Format Converter ch0 (TX) */
    DMA_RS_IFEI1      = 0x1B5,   /* Pixel Format Converter ch1 (RX) */
    DMA_RS_OFFI1      = 0x1B6    /* Pixel Format Converter ch1 (TX) */
#endif /* mbed */
} dma_res_select_t;

/* DMA transfer direction */
typedef enum
{
    DMA_REQ_MIN =(-1),
    DMA_REQ_SRC = 0, /* Read DMA */
    DMA_REQ_DES = 1, /* Write DMA */
    DMA_REQ_MAX = 2
} dma_req_dir_t;

/* Address count direction */
typedef enum
{
    DMA_ADDR_MIN = (-1),
    DMA_ADDR_INCREMENT = 0, /* Address Count Increment */
    DMA_ADDR_FIX = 1,       /* Address Count Fix */
    DMA_ADDR_MAX = 2
} dma_addr_cnt_t;


/*************************************************************************
 Structures
*************************************************************************/

/* DMA Init Parameter */
typedef struct
{
    bool_t   channel[DMA_CH_NUM]; /* Set enable channel */
    AIOCB    *p_aio;              /* set callback function (DMA error interrupt) */
}dma_drv_init_t;

/* DMA Setup Parameter */
typedef struct
{
    dma_res_select_t   resource;     /* DMA Transfer Resource */
    dma_req_dir_t      direction;    /* DMA Transfer Direction */
    dma_unit_size_t    dst_width;    /* DMA Transfer Unit Size (Destination) */
    dma_unit_size_t    src_width;    /* DMA Transfer Unit Size (Source) */
    dma_addr_cnt_t     dst_cnt;      /* DMA Address Count (Destination) */
    dma_addr_cnt_t     src_cnt;      /* DMA Address Count (Source) */
    AIOCB              *p_aio;       /* set callback function (DMA end interrupt) */
#if(1) /* mbed */
    uint8_t            int_level;
#endif
} dma_ch_setup_t;

/* DMA Transfer Paramter */
typedef struct
{
    void       *src_addr;    /* Sorce Address */
    void       *dst_addr;    /* Destination Address */
    uint32_t   count;        /* DMA Transfer Size */
} dma_trans_data_t;

/***********************************************************************************
 Function Prototypes
***********************************************************************************/

/***********************************************************************************
* ingroup API
* This function initializes the driver and must be called at system start
* up, prior to any required DMA functionality being available. This function
* also sets the enable or disable for each DMA channel and DMA error call back
* function.
*
* param [in] p_dma_init_param - parameter of ch enable and DMA error callback function.
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval ESUCCESS - successfully initialized.
* retval -1       - error occured.
***********************************************************************************/

extern int_t R_DMA_Init(const dma_drv_init_t * const p_dma_init_param, int32_t * const p_errno);

/***********************************************************************************
* ingroup API
* This function shutdown the driver, making DMA functionality is no longer available.
* It can be carried out only in calse of all channel free.
*
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval ESUCCESS - successfully uninitialized.
* retval -1       - error occured.
***********************************************************************************/

extern int_t R_DMA_UnInit(int32_t * const p_errno);

/***********************************************************************************
* ingroup API
* This function allocates a DMA channel.
* When channel is (-1), it looking for a free channel
* When set channel to DMA channel number, a set channel is allocated
*
* param [in] channel     - allocate channel. (when channel is (-1), it looking for a
*                           free channel.)
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval channel number - successfully allocated.
* retval -1             - error occured.
***********************************************************************************/

extern int_t R_DMA_Alloc(const int_t channel, int32_t * const p_errno);

/***********************************************************************************
* ingroup API
* This function close a DMA channel.
*
* param [in] channel     - close channel.
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval ESUCCESS - successfully allocate.
* retval -1       - error occured.
***********************************************************************************/

extern int_t R_DMA_Free(const int_t channel, int32_t * const p_errno);

/***********************************************************************************
* ingroup API
* This function set up a DMA transfer parameter.
* before calling R_DMA_Start(), please carry out this function.
*
* param [in] channel     - set up channel.
* param [in] p_ch_setup  - DMA transfer parameters.
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval ESUCCESS - successfully setup.
* retval -1       - error occured.
***********************************************************************************/

extern int_t R_DMA_Setup(const int_t channel, const dma_ch_setup_t * const p_ch_setup,
                         int32_t * const p_errno);

/***********************************************************************************
* ingroup API
* This function set up a DMA transfer address and start DMA.
*
* param [in] channel     - DMA start channel.
* param [in] p_ch_setup  - DMA address parameters.
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval ESUCCESS - successfully DMA start.
* retval -1       - error occured.
***********************************************************************************/

extern int_t R_DMA_Start(const int_t channel, const dma_trans_data_t * const p_dma_data,
                         int32_t * const p_errno);

/***********************************************************************************
* ingroup API
* This function set up a continous DMA transfer address and start continuous DMA.
*
* param [in] channel     - continuous DMA start channel.
* param [in] p_ch_setup  - continuous DMA address parameters.
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval ESUCCESS - successfully continuous DMA start.
* retval -1       - error occured.
***********************************************************************************/

extern int_t R_DMA_NextData(const int_t channel, const dma_trans_data_t * const p_dma_data,
                            int32_t * const p_errno);

/***********************************************************************************
* ingroup API
* This function cancel DMA transfer.
* Continous DMA also stops at the same time.
* Please call this function during DMA transfer.
*
* param [in] channel     - chancel DMA start channel.
* param [out] p_remain   - remain sizei of DMA transfer.
* param [in/out] p_errno - get error code. (when p_errno is NULL, erroc code isn't set.)
*
* retval ESUCCESS - successfully cancel.
* retval -1       - error occured.
***********************************************************************************/

extern int_t R_DMA_Cancel(const int_t channel, uint32_t * const p_remain, int32_t * const p_errno);


/***********************************************************************************
* ingroup API
* This function get DMA driver version.
*
* param none
*
* retval driver version
***********************************************************************************/

extern uint16_t R_DMA_GetVersion(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DMA_IF_H */
