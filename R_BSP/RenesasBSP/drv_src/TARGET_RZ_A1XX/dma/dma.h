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
* @file         dma.h
* $Rev: $
* $Date::                           $
* @brief        DMA Driver internal headers
******************************************************************************/

/*****************************************************************************
* History : DD.MM.YYYY Version Description
* : 15.01.2013 1.00 First Release
******************************************************************************/

#ifndef DMA_H
#define DMA_H

/******************************************************************************
Includes <System Includes>, "Project Includes"
******************************************************************************/

#include "dma_if.h"
#include "cmsis.h"
#include "bsp_drv_cmn.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/* Number of channel configure table */
#if defined(TARGET_RZA1H) /* mbed */
#define DMA_CH_CONFIG_TABLE_NUM  (95U)
#else /* mbed */
#define DMA_CH_CONFIG_TABLE_NUM  (63U)
#endif /* mbed */

/* Magic Number */
#define SHIFT_DMARS_EVEN_CH      (0U)  /* Shift Value for DMARS Register access in Even ch */
#define SHIFT_DMARS_ODD_CH       (16U) /* Shift Value for DMARS Register access in Odd ch */
#define MASK_DMARS_EVEN_CH       (0xFFFF0000U) /* Mask value for DMARS Register in Even ch */
#define MASK_DMARS_ODD_CH        (0x0000FFFFU) /* Mask value for DMARS Register in Odd ch */
#define HIGH_COMMON_REG_OFFSET   (8)  /* for Common Register Access in ch 0-8 */
#define CHECK_ODD_EVEN_MASK      (0x00000001U)  /* for check value of odd or even */
#define DMA_STOP_WAIT_MAX_CNT    (10U) /* Loop count for DMA stop (usually, a count is set to 0 or 1) */

/* Register Set Value */
/* Init Value */
#define N0SA_INIT_VALUE      (0U)          /* HW init value */
#define N1SA_INIT_VALUE      (0U)          /* HW init value */
#define N0DA_INIT_VALUE      (0U)          /* HW init value */
#define N1DA_INIT_VALUE      (0U)          /* HW init value */
#define N0TB_INIT_VALUE      (0U)          /* HW init value */
#define N1TB_INIT_VALUE      (0U)          /* HW init value */
#define CHCTRL_INIT_VALUE    (0U)          /* HW init value */
#define CHCFG_INIT_VALUE     (0x01000000U) /* interrupt disable */
#define CHITVL_INIT_VALUE    (0U)          /* DMA interval = 0 */
#define CHEXT_INIT_VALUE     (0U)          /* HW init value */
#define NXLA_INIT_VALUE      (0U)          /* HW init value */
#define DCTRL_INIT_VALUE     (0x00000001U) /* interrupt output : pulse, round robin mode */
#define DMARS_INIT_VALUE     (0U)          /* HW init value */
/* Fixed Setting for CHCFG */
#define CHCFG_FIXED_VALUE    (0x00000020U) /* register mode, not buffer sweep, interrupt detect when high pulse */

/* Bit Value & Mask */
/* CHSTAT */
#define CHSTAT_MASK_SR         (0x00000080U)
#define CHSTAT_MASK_END        (0x00000020U)
#define CHSTAT_MASK_ER         (0x00000010U)
#define CHSTAT_MASK_TACT       (0x00000004U)
#define CHSTAT_MASK_EN         (0x00000001U)
/* CHCTRL */
#define CHCTRL_SET_CLRTC       (0x00000040U)
#define CHCTRL_SET_CLREND      (0x00000020U)
#define CHCTRL_SET_SWRST       (0x00000008U)
#define CHCTRL_SET_CLREN       (0x00000002U)
#define CHCTRL_SET_SETEN       (0x00000001U)
/* CHCFG */
#define CHCFG_SET_REN          (0x40000000U)
#define CHCFG_MASK_REN         (0x40000000U)
#define CHCFG_SET_RSW          (0x20000000U)
#define CHCFG_MASK_RSW         (0x20000000U)
#define CHCFG_SET_RSEL         (0x10000000U)
#define CHCFG_MASK_RSEL        (0x10000000U)
#define CHCFG_SET_DEM          (0x01000000U)
#define CHCFG_MASK_DEM         (0x01000000U)
#define CHCFG_MASK_DAD         (0x00200000U)
#define CHCFG_MASK_SAD         (0x00100000U)
#define CHCFG_MASK_DDS         (0x000f0000U)
#define CHCFG_MASK_SDS         (0x0000f000U)
#define CHCFG_SET_AM_LEVEL     (0x00000100U)
#define CHCFG_SET_AM_BUS_CYCLE (0x00000200U)
#define CHCFG_MASK_AM          (0x00000700U)
#define CHCFG_SET_LVL_EDGE     (0x00000000U)
#define CHCFG_SET_LVL_LEVEL    (0x00000040U)
#define CHCFG_MASK_LVL         (0x00000040U)
#define CHCFG_SET_REQD_SRC     (0x00000000U)
#define CHCFG_SET_REQD_DST     (0x00000008U)
#define CHCFG_MASK_REQD        (0x00000008U)
#define CHCFG_SHIFT_DAD        (21U)
#define CHCFG_SHIFT_SAD        (20U)
#define CHCFG_SHIFT_DDS        (16U)
#define CHCFG_SHIFT_SDS        (12U)
/* CHEXT */
#define CHEXT_SET_DCA_NORMAL      (0x00003000U)
#define CHEXT_SET_DCA_STRONG      (0x00000000U)
#define CHEXT_SET_DPR_NON_SECURE  (0x00000200U)
#define CHEXT_SET_SCA_NORMAL      (0x00000030U)
#define CHEXT_SET_SCA_STRONG      (0x00000000U)
#define CHEXT_SET_SPR_NON_SECURE  (0x00000002U)


/* REQD value in CHCFG is undecided on config table */
/* used case of a resource is the same and two or more direction value exists.  */
#define CHCFG_REQD_UNDEFINED   DMA_REQ_MAX

/* Address of area which is the target of setting change */
#define DMA_EXTERNAL_BUS_START         (0x00000000U)
#define DMA_EXTERNAL_BUS_END           (0x1FFFFFFFU)
#define DMA_EXTERNAL_BUS_MIRROR_START  (0x40000000U)
#define DMA_EXTERNAL_BUS_MIRROR_END    (0x5FFFFFFFU)

/*************************************************************************
 Enumerated Types
*************************************************************************/

/* DRV Status */
typedef enum
{
    DMA_DRV_UNINIT = 0,   /* Uninit */
    DMA_DRV_INIT   = 1    /* Init */
} dma_stat_drv_t;

/* Channel Status */
typedef enum
{
    DMA_CH_UNINIT   = 0,   /* Uninit */
    DMA_CH_INIT     = 1,   /* Init */
    DMA_CH_OPEN     = 2,   /* Open */
    DMA_CH_TRANSFER = 4    /* Transfer */
} dma_stat_ch_t;

/*************************************************************************
 Structures
*************************************************************************/

/* DMA Register (Common) */
typedef struct
{
    volatile uint32_t  dctrl;
    volatile uint32_t  dstat_en;
    volatile uint32_t  dstat_er;
    volatile uint32_t  dstat_end;
    volatile uint32_t  dstat_tc;
    volatile uint32_t  dstat_sus;
} dma_reg_common_t;

/* DMA Register (every Channel) */
typedef struct
{
    volatile uint32_t  n0sa;
    volatile uint32_t  n0da;
    volatile uint32_t  n0tb;
    volatile uint32_t  n1sa;
    volatile uint32_t  n1da;
    volatile uint32_t  n1tb;
    volatile uint32_t  crsa;
    volatile uint32_t  crda;
    volatile uint32_t  crtb;
    volatile uint32_t  chstat;
    volatile uint32_t  chctrl;
    volatile uint32_t  chcfg;
    volatile uint32_t  chitvl;
    volatile uint32_t  chext;
    volatile uint32_t  nxla;
    volatile uint32_t  crla;
} dma_reg_ch_t;

/* Information of Channel */
typedef struct
{
    int_t              ch;                /* Channel Number */
    dma_stat_ch_t      ch_stat;           /* Channel Status */
    dma_res_select_t   resource;          /* DMA Transfer Resource */
    dma_req_dir_t      direction;         /* DMA Transfer Direction */
    dma_unit_size_t    src_width;         /* DMA Transfer Unit Size (Source) */
    dma_addr_cnt_t     src_cnt;           /* DMA Address Count (Source) */
    dma_unit_size_t    dst_width;         /* DMA Transfer Unit Size (Destination) */
    dma_addr_cnt_t     dst_cnt;           /* DMA Address Count (Destination) */
    void               *src_addr0;        /* Sorce Address (Next Register Set 0)*/
    void               *dst_addr0;        /* Destination Address (Next Register Set 0)*/
    uint32_t           count0;            /* DMA Transfer Size (Next Register Set 0)*/
    void               *src_addr1;        /* Sorce Address (Next Register Set 1)*/
    void               *dst_addr1;        /* Destination Address (Next Register Set 1)*/
    uint32_t           count1;            /* DMA Transfer Size (Next Register Set 1)*/
    IRQn_Type          end_irq_num;       /* DMA end interrupt number */
    AIOCB              *p_end_aio;        /* set callback function (DMA end interrupt) */
    bool_t             next_dma_flag;     /* Setting Flag of Continous DMA */
    uint32_t           shift_dmars;       /* set SHIFT_DMARS_ODD_CH or SHIFT_DMARS_EVEN_CH */
    uint32_t           mask_dmars;        /* set MASK_DMA_ODD_CH or MASK_DMARS_EVEN_CH */
    bool_t             setup_flag;        /* incdicate called DMA_Setup() flag */
    volatile struct st_dmac_n       *p_dma_ch_reg;     /* DMA Register for every channel */
    volatile struct st_dmaccommon_n *p_dma_common_reg; /* DMA Register for common */
    volatile uint32_t           *p_dma_dmars_reg;  /* DMARS Regsiter */
} dma_info_ch_t;

/* Information of Driver */
typedef struct
{
    dma_stat_drv_t  drv_stat;            /* DRV Status */
    AIOCB           *p_err_aio;          /* set callback function (DMA error interrupt) */
    IRQn_Type       err_irq_num;         /* DMA error interrupt number */
    dma_info_ch_t   info_ch[DMA_CH_NUM]; /* Enable Channel */
} dma_info_drv_t;

/* DMA Channel Configure Table */
typedef struct
{
    dma_res_select_t   dmars;             /* Set Value for DMARS Register */
    uint32_t           tm;                /* Set Value for TM Bit (CHCFG Register) */
    uint32_t           lvl;               /* Set Value for LVL Bit (CHCFG Register) */
    uint32_t           reqd;              /* Set Value for REQD (CHCFG Register) */
} dma_ch_cfg_t;

/***********************************************************************************
 Function Prototypes
***********************************************************************************/

dma_info_drv_t *DMA_GetDrvInstance(void);
dma_info_ch_t  *DMA_GetDrvChInfo(const int_t channel);
int_t DMA_Initialize(const dma_drv_init_t * const p_dma_init_param);
int_t DMA_UnInitialize(void);
int_t DMA_GetFreeChannel(void);
int_t DMA_GetFixedChannel(const int_t channel);
void  DMA_CloseChannel(const int_t channel);
void  DMA_SetParam(const int_t channel, const dma_ch_setup_t * const p_ch_setup,
                   const dma_ch_cfg_t * const p_ch_cfg, const uint32_t reqd);
void  DMA_BusParam(const int_t channel, const dma_trans_data_t * const p_dma_data);
void  DMA_SetData(const int_t channel, const dma_trans_data_t * const p_dma_data,
                 const uint32_t next_register_set);
void  DMA_SetNextData(const int_t channel, const dma_trans_data_t * const p_dma_data);
void  DMA_Start(const int_t channel, const bool_t restart_flag);
void  DMA_Stop(const int_t channel, uint32_t * const p_remain);
void  DMA_SetErrCode(const int_t error_code, int32_t * const p_errno);

#endif /* DMA_H */
