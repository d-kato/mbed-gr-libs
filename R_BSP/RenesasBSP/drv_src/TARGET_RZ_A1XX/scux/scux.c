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
* @file         scux.c
* $Rev: $
* $Date::                           $
* @brief        SCUX Driver functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/

#include "scux.h"
#include "mbed_critical.h"

#if(1) /* mbed */
/******************************************************************************
Macro definitions
******************************************************************************/
#define INIT_WAIT_TIME_MSEC (1)
#define INIT_WAIT_NUM       (1000U)

#endif /* end mbed */
/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/* ->MISRA 8.8 This description is based on the way to fill out OS defined. */
/* ->IPA M2.2.2, MISRA 8.10 This description is based on the way to fill out OS defined. */
osSemaphoreDef(scux_ch0_access);
osSemaphoreDef(scux_ch1_access);
osSemaphoreDef(scux_ch2_access);
osSemaphoreDef(scux_ch3_access);

osSemaphoreDef(scux_ssif_ch0_access);
osSemaphoreDef(scux_ssif_ch1_access);
osSemaphoreDef(scux_ssif_ch2_access);
osSemaphoreDef(scux_ssif_ch3_access);
#if defined(TARGET_RZA1H) /* mbed */
osSemaphoreDef(scux_ssif_ch4_access);
osSemaphoreDef(scux_ssif_ch5_access);
#endif /* mbed */

osSemaphoreDef(scux_shared_access);
/* <-MISRA 8.10, IPA M2.2.2 */
/* <-MISRA 8.8 */

/******************************************************************************
Private global driver management information
******************************************************************************/

/* driver management infrmation */
static scux_info_drv_t gb_scux_info_drv = {
    SCUX_DRV_UNINIT,
    {
        {SCUX_CH_0, false, 0, SCUX_CH_UNINIT}, /* ch0 */
        {SCUX_CH_1, false, 0, SCUX_CH_UNINIT}, /* ch1 */
        {SCUX_CH_2, false, 0, SCUX_CH_UNINIT}, /* ch2 */
        {SCUX_CH_3, false, 0, SCUX_CH_UNINIT}  /* ch3 */
    }
};

/* SSIF management information */
static scux_ssif_info_t gb_scux_ssif_info[SCUX_SSIF_CH_NUM];

/* ->MISRA 11.3, 11.4 11.5 This cast is needed for register access. */
/* address table of register set for each SCUX channel */
static scux_reg_info_t p_scux_ch_reg_addr_table[SCUX_CH_NUM] = {
    {
        &SCUX_FROM_DVUIR_DVU0_0,
        &SCUX_FROM_SRCIR0_2SRC0_0,
        &SCUX_FROM_FFUIR_FFU0_0,
        &SCUX_FROM_FFDIR_FFD0_0,
        &SCUX_FROM_OPCIR_OPC0_0,
        &SCUX_FROM_IPCIR_IPC0_0,
        &SCUXMIXIR_MIX0_0,
        &SCUXMADIR_MIX0_0,
        &SCUXMIXBR_MIX0_0,
        &SCUXMIXMR_MIX0_0,
        &SCUXMVPDR_MIX0_0,
        &SCUXMDBAR_MIX0_0,
        &SCUXMDBER_MIX0_0,
        &SCUXMIXSR_MIX0_0,
        &SCUXSWRSR_CIM,
        &SCUXDMACR_CIM,
        &SCUXDMATD0_CIM,
        &SCUXDMATU0_CIM,
        &SCUXSSIRSEL_CIM,
        &SCUXFDTSEL0_CIM,
        &SCUXFUTSEL0_CIM,
        &SCUXSSIPMD_CIM,
        &SCUXSSICTRL_CIM,
        &SCUXSRCRSEL0_CIM,
        &SCUXMIXRSEL_CIM
    },
    {
        &SCUX_FROM_DVUIR_DVU0_1,
        &SCUX_FROM_SRCIR0_2SRC0_0,
        &SCUX_FROM_FFUIR_FFU0_1,
        &SCUX_FROM_FFDIR_FFD0_1,
        &SCUX_FROM_OPCIR_OPC0_1,
        &SCUX_FROM_IPCIR_IPC0_1,
        &SCUXMIXIR_MIX0_0,
        &SCUXMADIR_MIX0_0,
        &SCUXMIXBR_MIX0_0,
        &SCUXMIXMR_MIX0_0,
        &SCUXMVPDR_MIX0_0,
        &SCUXMDBBR_MIX0_0,
        &SCUXMDBER_MIX0_0,
        &SCUXMIXSR_MIX0_0,
        &SCUXSWRSR_CIM,
        &SCUXDMACR_CIM,
        &SCUXDMATD1_CIM,
        &SCUXDMATU1_CIM,
        &SCUXSSIRSEL_CIM,
        &SCUXFDTSEL1_CIM,
        &SCUXFUTSEL1_CIM,
        &SCUXSSIPMD_CIM,
        &SCUXSSICTRL_CIM,
        &SCUXSRCRSEL1_CIM,
        &SCUXMIXRSEL_CIM
    },
    {
        &SCUX_FROM_DVUIR_DVU0_2,
        &SCUX_FROM_SRCIR0_2SRC0_1,
        &SCUX_FROM_FFUIR_FFU0_2,
        &SCUX_FROM_FFDIR_FFD0_2,
        &SCUX_FROM_OPCIR_OPC0_2,
        &SCUX_FROM_IPCIR_IPC0_2,
        &SCUXMIXIR_MIX0_0,
        &SCUXMADIR_MIX0_0,
        &SCUXMIXBR_MIX0_0,
        &SCUXMIXMR_MIX0_0,
        &SCUXMVPDR_MIX0_0,
        &SCUXMDBCR_MIX0_0,
        &SCUXMDBER_MIX0_0,
        &SCUXMIXSR_MIX0_0,
        &SCUXSWRSR_CIM,
        &SCUXDMACR_CIM,
        &SCUXDMATD2_CIM,
        &SCUXDMATU2_CIM,
        &SCUXSSIRSEL_CIM,
        &SCUXFDTSEL2_CIM,
        &SCUXFUTSEL2_CIM,
        &SCUXSSIPMD_CIM,
        &SCUXSSICTRL_CIM,
        &SCUXSRCRSEL2_CIM,
        &SCUXMIXRSEL_CIM
    },
    {
        &SCUX_FROM_DVUIR_DVU0_3,
        &SCUX_FROM_SRCIR0_2SRC0_1,
        &SCUX_FROM_FFUIR_FFU0_3,
        &SCUX_FROM_FFDIR_FFD0_3,
        &SCUX_FROM_OPCIR_OPC0_3,
        &SCUX_FROM_IPCIR_IPC0_3,
        &SCUXMIXIR_MIX0_0,
        &SCUXMADIR_MIX0_0,
        &SCUXMIXBR_MIX0_0,
        &SCUXMIXMR_MIX0_0,
        &SCUXMVPDR_MIX0_0,
        &SCUXMDBDR_MIX0_0,
        &SCUXMDBER_MIX0_0,
        &SCUXMIXSR_MIX0_0,
        &SCUXSWRSR_CIM,
        &SCUXDMACR_CIM,
        &SCUXDMATD3_CIM,
        &SCUXDMATU3_CIM,
        &SCUXSSIRSEL_CIM,
        &SCUXFDTSEL3_CIM,
        &SCUXFUTSEL3_CIM,
        &SCUXSSIPMD_CIM,
        &SCUXSSICTRL_CIM,
        &SCUXSRCRSEL3_CIM,
        &SCUXMIXRSEL_CIM
    }
};
/* <-MISRA 11.3, 11.4 11.5 */

/* ->MISRA 11.3 This cast is needed for register access. */
/* address table of register set for each SSIF channel */
static volatile struct st_ssif * const p_scux_ssif_ch_reg_addr[SCUX_SSIF_CH_NUM] =
{
    &SSIF0,
    &SSIF1,
    &SSIF2,
    &SSIF3,
#if defined(TARGET_RZA1H) /* mbed */
    &SSIF4,
    &SSIF5
#endif /* mbed */
};
/* <-MISRA 11.3 */

/* SCUX semaphore table define */
static const osSemaphoreAttr_t * const p_semdef_ch_scux_access[SCUX_CH_NUM] =
{
    osSemaphore(scux_ch0_access),
    osSemaphore(scux_ch1_access),
    osSemaphore(scux_ch2_access),
    osSemaphore(scux_ch3_access)
};

/* SSIF semaphore table define */
static const osSemaphoreAttr_t * const p_semdef_ch_scux_ssif_access[SCUX_SSIF_CH_NUM] =
{
    osSemaphore(scux_ssif_ch0_access),
    osSemaphore(scux_ssif_ch1_access),
    osSemaphore(scux_ssif_ch2_access),
    osSemaphore(scux_ssif_ch3_access),
#if defined(TARGET_RZA1H) /* mbed */
    osSemaphore(scux_ssif_ch4_access),
    osSemaphore(scux_ssif_ch5_access)
#endif /* mbed */
};

/* write DMA resource define */
static const dma_res_select_t gb_dma_res_select_tx[SCUX_CH_NUM] =
{
    DMA_RS_SCUTXI0,
    DMA_RS_SCUTXI1,
    DMA_RS_SCUTXI2,
    DMA_RS_SCUTXI3
};

/* read DMA resource define */
static const dma_res_select_t gb_dma_res_select_rx[SCUX_CH_NUM] =
{
    DMA_RS_SCURXI0,
    DMA_RS_SCURXI1,
    DMA_RS_SCURXI2,
    DMA_RS_SCURXI3
};

/* write dummy data buffer */
static uint8_t gb_scux_write_dummy_buf[SCUX_DUMMY_BUF_SIZE];

/* read dummy data buffer */
static uint8_t gb_scux_read_dummy_buf[SCUX_DUMMY_BUF_SIZE];

/******************************************************************************
 Function prototypes
******************************************************************************/

static int_t SCUX_CheckSrcParam(scux_info_ch_t * const p_scux_info_ch, const uint32_t ssif_ch[SCUX_SSIF_NUM_CH_ARRANGEMENT]);
static int_t SCUX_CheckDvuParam(const scux_info_ch_t * const p_scux_info_ch);
static int_t SCUX_CheckSsifParam(scux_info_ch_t * const p_scux_info_ch, const uint32_t ssif_ch[SCUX_SSIF_NUM_CH_ARRANGEMENT], const bool_t use_mix_flag);
static int_t SCUX_CheckMixParam(const scux_info_ch_t * const p_scux_info_ch);
#if(1) /* mbed */
static int_t SCUX_CmnUnInitialize(void);
#endif /* end mbed */

/**************************************************************************//**
* Function Name: SCUX_GetDrvInstance
* @brief         Get pointer of gb_scux_info_drv.
*
*                Description:<br>
*
* @param         None.
* @retval        pointer of gb_scux_info_drv -
*                  driver instance.
******************************************************************************/

scux_info_drv_t *SCUX_GetDrvInstance(void)
{

    return &gb_scux_info_drv;
}

/******************************************************************************
End of function SCUX_GetDrv_Instance
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_GetDrvChInfo
* @brief         gb_scux_info_drv.info_ch[channel].
*
*                Description:<br>
*
* @param[in]     channel information number.
* @retval        pointer of gb_scux_info_drv -
*                  pointer of channel information.
******************************************************************************/

scux_info_ch_t *SCUX_GetDrvChInfo(const int_t channel)
{

    return &gb_scux_info_drv.info_ch[channel];
}

/******************************************************************************
End of function SCUX_GetDrvChInfo
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_GetSsifChInfo
* @brief         gb_scux_ssif_info.channel.
*
*                Description:<br>
*
* @param[in]     SSIF channel number.
* @retval        pointer of gb_scux_ssif_info -
*                  pointer of SSIF information.
******************************************************************************/

scux_ssif_info_t *SCUX_GetSsifChInfo(const int_t channel)
{

    return &gb_scux_ssif_info[channel];
}

/******************************************************************************
End of function SCUX_GetSsifChInfo
******************************************************************************/

#if(1) /* mbed */
/**************************************************************************//**
* Function Name: SCUX_InitializeOne
* @brief         Init SCUX driver.
*
*                Description:<br>
*                
* @param[in]     channel    :initialize channel number.
* @param[in]     p_scux_init_param :Initialize parameter for SCUX. 
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
*                    error code -
*                       ENOMEM_RBSP : Making semaphore is failed.
*                       EFAULT_RBSP : Internal error is occured.
******************************************************************************/
int_t SCUX_InitializeOne(const int_t channel, const scux_channel_cfg_t * const p_scux_init_param)
{
    int_t    retval = ESUCCESS;
    osStatus sem_ercd;
    int_t    scux_ch_count;
    int_t    audio_ch_count;
    scux_ssif_ch_num_t    ssif_ch_count;
    uint32_t cpg_value;
    bool_t   init_shared_flag = false;
    volatile uint8_t dummy_buf;
    uint32_t scux_init_count;
    int_t    uninit_ercd;
    bool_t   init_start_flag = false;
    bool_t   uninit_all_flag = false;
    uint32_t i;

    if (NULL == p_scux_init_param)
    {
        retval = EFAULT_RBSP;
    }
    else if (false == p_scux_init_param->enabled)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* init channel management information */
        scux_ch_count = channel;

        for (i = 0; ((i < INIT_WAIT_NUM) && (false == init_start_flag)); i++)
        {
            core_util_critical_section_enter();

            if (SCUX_DRV_INIT == gb_scux_info_drv.drv_stat)
            {
                /* already scux driver shared information is initialized */
                init_shared_flag = true;

                /* enable the channel */
                gb_scux_info_drv.info_ch[scux_ch_count].enabled = true;

                init_start_flag = true;
            }
            else if (SCUX_DRV_UNINIT == gb_scux_info_drv.drv_stat)
            {
                /* change the status to scux initialization running */
                gb_scux_info_drv.drv_stat = SCUX_DRV_INIT_RUNNING;

                for (scux_init_count = SCUX_CH_0; scux_init_count < SCUX_CH_NUM; scux_init_count++)
                {
                    gb_scux_info_drv.info_ch[scux_init_count].enabled = false;
                    gb_scux_info_drv.info_ch[scux_init_count].ch_stat = SCUX_CH_UNINIT;

                    gb_scux_info_drv.info_ch[scux_init_count].sem_ch_scux_access = NULL;
                }

                for (ssif_ch_count = SCUX_SSIF_CH_0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++)
                {
                    gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access = NULL;
                }

                gb_scux_info_drv.shared_info.sem_shared_access = NULL;

                /* enable the channel */
                gb_scux_info_drv.info_ch[scux_ch_count].enabled = true;

                init_start_flag = true;
            }
            else
            {
                /* do nothing : SCUX_DRV_INIT_RUNNING */
            }

            core_util_critical_section_exit();

            if (false == init_start_flag)
            {
                /* wait for the change of drv_stat to SCUX_DRV_INIT */
                (void)osDelay(INIT_WAIT_TIME_MSEC);
            }
        }

        if (false == init_start_flag)
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            {
    
                /* copy parameter */
                /* set interrupt parameter */
                gb_scux_info_drv.info_ch[scux_ch_count].int_level = p_scux_init_param->int_level;
                        
                /* set route parameter */
                gb_scux_info_drv.info_ch[scux_ch_count].route_set = p_scux_init_param->route;
                
                /* set SRC paramter */
                SCUX_IoctlSetSrcCfg(scux_ch_count, &p_scux_init_param->src_cfg);
                
                /* init SCUX parameter */
                if ((SCUX_CH_0 == scux_ch_count) || (SCUX_CH_1 == scux_ch_count))
                {
                    gb_scux_info_drv.info_ch[scux_ch_count].fifo_size = SCUX_FIFO_SIZE_CH0_1;
                }
                else
                {
                    gb_scux_info_drv.info_ch[scux_ch_count].fifo_size = SCUX_FIFO_SIZE_CH2_3;
                }
                gb_scux_info_drv.info_ch[scux_ch_count].dma_resource_tx = gb_dma_res_select_tx[scux_ch_count];
                gb_scux_info_drv.info_ch[scux_ch_count].dma_resource_rx = gb_dma_res_select_rx[scux_ch_count];
                gb_scux_info_drv.info_ch[scux_ch_count].futsel_cim_value = FUTSEL_CIM_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].err_stat_backup = ESUCCESS;
                
                /* init DVU parameter */
                for (audio_ch_count = SCUX_AUDIO_CH_0; audio_ch_count < SCUX_AUDIO_CH_MAX; audio_ch_count++)
                {
                    gb_scux_info_drv.info_ch[scux_ch_count].dvu_cfg.dvu_zc_mute.zc_mute_enable[audio_ch_count] = false; 
                }
                gb_scux_info_drv.info_ch[scux_ch_count].dvu_setup = false;
             
                if (false == init_shared_flag)
                {
                    /* init MIX parameter */
                    gb_scux_info_drv.shared_info.mix_setup = false;
                    gb_scux_info_drv.shared_info.mix_run_ch = 0U;
                    gb_scux_info_drv.shared_info.mix_ssif_ch = 0U;
                    
                    /* init SSIF parameter */
                    for (ssif_ch_count = SCUX_SSIF_CH_0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++)
                    {
                        gb_scux_ssif_info[ssif_ch_count].ssif_setup = false;
                        gb_scux_ssif_info[ssif_ch_count].ssif_cfg.ssif_ch_num = ssif_ch_count;
                        gb_scux_ssif_info[ssif_ch_count].scux_channel = 0;
                        gb_scux_ssif_info[ssif_ch_count].pin_mode = SCUX_PIN_MODE_INDEPEND;
                    }
                    
                    /* init regsiter store value */
                    gb_scux_info_drv.shared_info.ssictrl_cim_value = SSICTRL_CIM_INIT_VALUE;
                }
            
                /* set register address */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg = &p_scux_ch_reg_addr_table[scux_ch_count];
                                       
                if (false == init_shared_flag) {
                    core_util_critical_section_enter();

                    /* supply clock for SCUX */
                    cpg_value = (uint32_t)CPG.STBCR8 & ~(CPG_STBCR8_BIT_MSTP81);
                    CPG.STBCR8 = (uint8_t)cpg_value;
                    dummy_buf = CPG.STBCR8;
#if(1) /* mbed */
                    (void)dummy_buf;
#endif
                    
                    core_util_critical_section_exit();

                    /* software reset */
                    SCUX.SWRSR_CIM &= ~SWRSR_CIM_SWRST_SET;
                    SCUX.SWRSR_CIM |= SWRSR_CIM_SWRST_SET;
                }
                
                /* init DVU register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUIR_DVU0_0 = DVUIR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VADIR_DVU0_0 = VADIR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUBR_DVU0_0 = DVUBR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 = DVUCR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->ZCMCR_DVU0_0 = ZCMCR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRCTR_DVU0_0 = VRCTR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRPDR_DVU0_0 = VRPDR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRDBR_DVU0_0 = VRDBR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRWTR_DVU0_0 = VRWTR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL0R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL1R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL2R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL3R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL4R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL5R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL6R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL7R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUER_DVU0_0 = DVUER_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VEVMR_DVU0_0 = VEVMR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VEVCR_DVU0_0 = VEVCR_DVU0_INIT_VALUE;
                
                /* init SRC register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCIR0_2SRC0_0 = SRCIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SADIR0_2SRC0_0 = SADIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCBR0_2SRC0_0 = SRCBR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSCR0_2SRC0_0 = IFSCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSVR0_2SRC0_0 = IFSVR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCCR0_2SRC0_0 = (SRCCR_2SRC0_INIT_VALUE | SRCCR_2SRC0_BASE_VALUE);
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->MNFSR0_2SRC0_0 = MNFSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->BFSSR0_2SRC0_0 = BFSSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->WATSR0_2SRC0_0 = WATSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVMR0_2SRC0_0 = SEVMR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVCR0_2SRC0_0 = SEVCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCIR1_2SRC0_0 = SRCIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SADIR1_2SRC0_0 = SADIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCBR1_2SRC0_0 = SRCBR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSCR1_2SRC0_0 = IFSCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSVR1_2SRC0_0 = IFSVR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCCR1_2SRC0_0 = (SRCCR_2SRC0_INIT_VALUE | SRCCR_2SRC0_BASE_VALUE);
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->MNFSR1_2SRC0_0 = MNFSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->BFSSR1_2SRC0_0 = BFSSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->WATSR1_2SRC0_0 = WATSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVMR1_2SRC0_0 = SEVMR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVCR1_2SRC0_0 = SEVCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVCR1_2SRC0_0 = SRCIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCIRR_2SRC0_0 = SRCIRR_2SRC0_INIT_VALUE;
                
                /* init FFU register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->FFUIR_FFU0_0 = FFUIR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->FUAIR_FFU0_0 = FUAIR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->URQSR_FFU0_0 = URQSR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->FFUPR_FFU0_0 = FFUPR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->UEVMR_FFU0_0 = UEVMR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->UEVCR_FFU0_0 = UEVCR_FFU0_INIT_VALUE;
                
                /* init FFD register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FFDIR_FFD0_0 = FFDIR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FDAIR_FFD0_0 = FDAIR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->DRQSR_FFD0_0 = DRQSR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FFDPR_FFD0_0 = FFDPR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FFDBR_FFD0_0 = FFDBR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 = DEVMR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 = DEVCR_FFD0_INIT_VALUE;
                
                /* init OPC register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_opc_reg->OPCIR_OPC0_0 = OPCIR_OPC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_opc_reg->OPSLR_OPC0_0 = OPSLR_OPC0_INIT_VALUE;
    
                /* init IPC register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ipc_reg->IPCIR_IPC0_0 = IPCIR_IPC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ipc_reg->IPSLR_IPC0_0 = IPSLR_IPC0_INIT_VALUE;
    
                /* init MIX register for each channel */
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mdb_n_r_mix0_0) = MDB_N_R_MIX0_0_INIT_VALUE;
                
                /* init CIM register for each channel */
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->fdtsel_n_cim) = FDTSEL_CIM_INIT_VALUE;
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->futsel_n_cim) = FUTSEL_CIM_INIT_VALUE;
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->srcrsel_n_cim) = SRCRSEL_CIM_INIT_VALUE;
                
                /* init shared register */
                if (false == init_shared_flag)
                {
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixir_mix0_0) = MIXIR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->madir_mix0_0) = MADIR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixbr_mix0_0) = MIXBR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixmr_mix0_0) = MIXMR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mvpdr_mix0_0) = MVPDR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mdber_mix0_0) = MDBER_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->dmacr_cim) = DMACR_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->ssirsel_cim) = SSIRSEL_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->ssipmd_cim) = SSIPMD_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->ssictrl_cim) = SSICTRL_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixrsel_cim) = MIXRSEL_CIM_INIT_VALUE;
                }
                
                if (false == init_shared_flag)
                {
                    /* set SSIF register */
                    for (ssif_ch_count = SCUX_SSIF_CH_0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++)
                    {
                        gb_scux_ssif_info[ssif_ch_count].p_scux_ssif_reg = p_scux_ssif_ch_reg_addr[ssif_ch_count];
                    }
                }
            
                /* set  semaphore parameter */
                gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access = osSemaphoreNew(0xffff, 1, p_semdef_ch_scux_access[scux_ch_count]);
                if (NULL == gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access)
                {
                     retval = ENOMEM_RBSP;
                }
                if ((ESUCCESS == retval) && (false == init_shared_flag))
                {
                    for (ssif_ch_count = SCUX_SSIF_CH_0; ((ssif_ch_count < SCUX_SSIF_CH_NUM) && (ESUCCESS == retval)); ssif_ch_count++) 
                    {
                        gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access = osSemaphoreNew(0xffff, 1, p_semdef_ch_scux_ssif_access[ssif_ch_count]);
                        if (NULL == gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access)
                        {
                            retval = ENOMEM_RBSP;
                        }
                    }
                }
                if ((ESUCCESS == retval) && (false == init_shared_flag))
                {
                    gb_scux_info_drv.shared_info.sem_shared_access = osSemaphoreNew(0xffff, 1, osSemaphore(scux_shared_access));
                    if (NULL == gb_scux_info_drv.shared_info.sem_shared_access)
                    {
                        retval = ENOMEM_RBSP;
                    }
                }
                
                
                gb_scux_info_drv.info_ch[scux_ch_count].ch_stat = SCUX_CH_INIT;
            }

            if (ESUCCESS != retval)
            {
                /* uninit each resouces */
                gb_scux_info_drv.info_ch[scux_ch_count].ch_stat = SCUX_CH_UNINIT;

                if (NULL != gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access)
                {
                    /* semaphore delete */
                    sem_ercd = osSemaphoreDelete(gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access);
                    /* semaphore error check */
                    if (osOK != sem_ercd)
                    {
                        /* set error return value */
                        retval = EFAULT_RBSP;
                    }

                    gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access = NULL;
                }

                core_util_critical_section_enter();

                gb_scux_info_drv.info_ch[scux_ch_count].enabled = false;

                for (i = SCUX_CH_0; i < SCUX_CH_NUM; i++)
                {
                    if (false != gb_scux_info_drv.info_ch[i].enabled)
                    {
                        break;
                    }
                }

                if (SCUX_CH_NUM == i)
                {
                    gb_scux_info_drv.drv_stat = SCUX_DRV_INIT_RUNNING;
                    uninit_all_flag = true;
                }

                core_util_critical_section_exit();

                if (false != uninit_all_flag)
                {
                    /* uninitialize driver infomation */
                    uninit_ercd = SCUX_CmnUnInitialize();
                    if (ESUCCESS != uninit_ercd)
                    {
                        retval = uninit_ercd;
                    }

                    gb_scux_info_drv.drv_stat = SCUX_DRV_UNINIT;
                }
            }
            else
            {
                gb_scux_info_drv.drv_stat = SCUX_DRV_INIT;
            }
        }
    }
    
    return retval;
}

/******************************************************************************
End of function SCUX_InitializeOne
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_UnInitializeOne
* @brief         Uninit SCUX driver.
*
*                Description:<br>
*                
* @param[in]     channel    :unInitialize channel number.
* @retval        None.
******************************************************************************/
void SCUX_UnInitializeOne(const int_t channel)
{
    osStatus sem_ercd;
    int_t    ercd;
    int_t    scux_ch_count;
    int_t    ssif_ch_count;
    bool_t   uninit_all_flag = false;
    uint32_t i;

    scux_ch_count = channel;
    if (false != gb_scux_info_drv.info_ch[scux_ch_count].enabled)
    {
        /* check ch_stat whether going transfer */
        if ((SCUX_CH_UNINIT != gb_scux_info_drv.info_ch[scux_ch_count].ch_stat) && 
            (SCUX_CH_INIT != gb_scux_info_drv.info_ch[scux_ch_count].ch_stat) &&
            (SCUX_CH_STOP != gb_scux_info_drv.info_ch[scux_ch_count].ch_stat))
        {
            /* The exclusive access control (interrupt disabled) starts */
            core_util_critical_section_enter();

            /* This exclusive access control ends in the SCUX_IoctlClearStop */
            /* call the __enable_irq in the SCUX_IoctlClearStop */
            ercd = SCUX_IoctlClearStop(scux_ch_count);
            if (ESUCCESS != ercd)
            {
                /* NON_NOTICE_ASSERT: SCUX stop failed */
            }
            
            ercd = R_DMA_Free(gb_scux_info_drv.info_ch[scux_ch_count].dma_tx_ch, NULL);
            if (ESUCCESS != ercd)
            {
                 /* NON_NOTICE_ASSERT: DMA release failed */
            }
        }
    }
    
    if (false != gb_scux_info_drv.info_ch[scux_ch_count].enabled)
    {
        if (gb_scux_info_drv.info_ch[scux_ch_count].ch_stat == SCUX_CH_UNINIT)
        {
            /* NON_NOTICE_ASSERT: abnormal status */
        }
        
        /* uninit each resouces */
        gb_scux_info_drv.info_ch[scux_ch_count].ch_stat = SCUX_CH_UNINIT;
        
        if (NULL != gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access)
        {
            /* delete each semaphore  */
            sem_ercd = osSemaphoreRelease(gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* NON_NOTICE_ASSERT: semaphore error */
            }

            sem_ercd = osSemaphoreDelete(gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* NON_NOTICE_ASSERT: semaphore error */
            }

            gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access = NULL;
        }
        
        /* delete queue */
        ahf_cancelall(&gb_scux_info_drv.info_ch[scux_ch_count].tx_que);
        ahf_destroy(&gb_scux_info_drv.info_ch[scux_ch_count].tx_que);
        ahf_cancelall(&gb_scux_info_drv.info_ch[scux_ch_count].rx_que);
        ahf_destroy(&gb_scux_info_drv.info_ch[scux_ch_count].rx_que);
    }

    core_util_critical_section_enter();

    gb_scux_info_drv.info_ch[scux_ch_count].enabled = false;

    for (i = SCUX_CH_0; i < SCUX_CH_NUM; i++)
    {
        if (false != gb_scux_info_drv.info_ch[i].enabled)
        {
            break;
        }
    }

    if (SCUX_CH_NUM == i)
    {
        if (SCUX_DRV_INIT == gb_scux_info_drv.drv_stat)
        {
            gb_scux_info_drv.drv_stat = SCUX_DRV_INIT_RUNNING;
            uninit_all_flag = true;
        }
    }

    core_util_critical_section_exit();

    if (false != uninit_all_flag)
    {
        for (ssif_ch_count = SCUX_SSIF_CH_0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++) 
        {
            if (NULL != gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access)
            {
                sem_ercd = osSemaphoreRelease(gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access);
                /* semaphore error check */
                if (osOK != sem_ercd)
                {
                    /* NON_NOTICE_ASSERT: semaphore error */
                }
            }
        }

        if (NULL != gb_scux_info_drv.shared_info.sem_shared_access)
        {
            sem_ercd = osSemaphoreRelease(gb_scux_info_drv.shared_info.sem_shared_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* NON_NOTICE_ASSERT: semaphore error */
            }
        }

        /* uninitialize driver infomation */
        (void)SCUX_CmnUnInitialize();

        gb_scux_info_drv.drv_stat = SCUX_DRV_UNINIT;
    }
}

/******************************************************************************
End of function SCUX_UnInitializeOne
******************************************************************************/
#endif /* end mbed */

/**************************************************************************//**
* Function Name: SCUX_Initialize
* @brief         Init SCUX driver.
*
*                Description:<br>
*
* @param[in]     p_scux_init_param :Initialize parameter for SCUX.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
*                    error code -
*                       ENOMEM_RBSP : Making semaphore is failed.
*                       EFAULT_RBSP : Internal error is occured.
******************************************************************************/
int_t SCUX_Initialize(const scux_channel_cfg_t * const p_scux_init_param)
{
    int_t    retval = ESUCCESS;
    osStatus sem_ercd;
    int_t    scux_ch_count;
    int_t    audio_ch_count;
    scux_ssif_ch_num_t    ssif_ch_count;
    uint32_t cpg_value;
    bool_t   init_shared_flag = false;
    volatile uint8_t dummy_buf;

    if (NULL == p_scux_init_param)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* init channel management information */
        for (scux_ch_count = 0; scux_ch_count < SCUX_CH_NUM; scux_ch_count++)
        {
            if (false == p_scux_init_param[scux_ch_count].enabled)
            {
                /* set disable parameter */
                gb_scux_info_drv.info_ch[scux_ch_count].enabled = false;
                gb_scux_info_drv.info_ch[scux_ch_count].ch_stat = SCUX_CH_UNINIT;
            }
            else
            {
                gb_scux_info_drv.info_ch[scux_ch_count].enabled = true;

                /* copy parameter */
                /* set interrupt parameter */
                gb_scux_info_drv.info_ch[scux_ch_count].int_level = p_scux_init_param[scux_ch_count].int_level;

                /* set route parameter */
                gb_scux_info_drv.info_ch[scux_ch_count].route_set = p_scux_init_param[scux_ch_count].route;

                /* set SRC paramter */
                SCUX_IoctlSetSrcCfg(scux_ch_count, &p_scux_init_param[scux_ch_count].src_cfg);

                /* init SCUX parameter */
                if ((SCUX_CH_0 == scux_ch_count) || (SCUX_CH_1 == scux_ch_count))
                {
                    gb_scux_info_drv.info_ch[scux_ch_count].fifo_size = SCUX_FIFO_SIZE_CH0_1;
                }
                else
                {
                    gb_scux_info_drv.info_ch[scux_ch_count].fifo_size = SCUX_FIFO_SIZE_CH2_3;
                }
                gb_scux_info_drv.info_ch[scux_ch_count].dma_resource_tx = gb_dma_res_select_tx[scux_ch_count];
                gb_scux_info_drv.info_ch[scux_ch_count].dma_resource_rx = gb_dma_res_select_rx[scux_ch_count];
                gb_scux_info_drv.info_ch[scux_ch_count].futsel_cim_value = FUTSEL_CIM_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].err_stat_backup = ESUCCESS;

                /* init DVU parameter */
                for (audio_ch_count = SCUX_AUDIO_CH_0; audio_ch_count < SCUX_AUDIO_CH_MAX; audio_ch_count++)
                {
                    gb_scux_info_drv.info_ch[scux_ch_count].dvu_cfg.dvu_zc_mute.zc_mute_enable[audio_ch_count] = false;
                }
                gb_scux_info_drv.info_ch[scux_ch_count].dvu_setup = false;

                if (false == init_shared_flag)
                {
                    /* init MIX parameter */
                    gb_scux_info_drv.shared_info.mix_setup = false;
                    gb_scux_info_drv.shared_info.mix_run_ch = 0U;
                    gb_scux_info_drv.shared_info.mix_ssif_ch = 0U;

                    /* init SSIF parameter */
                    for (ssif_ch_count = SCUX_SSIF_CH_0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++)
                    {
                        gb_scux_ssif_info[ssif_ch_count].ssif_setup = false;
                        gb_scux_ssif_info[ssif_ch_count].ssif_cfg.ssif_ch_num = ssif_ch_count;
                        gb_scux_ssif_info[ssif_ch_count].scux_channel = 0;
                        gb_scux_ssif_info[ssif_ch_count].pin_mode = SCUX_PIN_MODE_INDEPEND;
                    }

                    /* init regsiter store value */
                    gb_scux_info_drv.shared_info.ssictrl_cim_value = SSICTRL_CIM_INIT_VALUE;
                }

                /* set register address */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg = &p_scux_ch_reg_addr_table[scux_ch_count];

                if (false == init_shared_flag) {
                    core_util_critical_section_enter();

                    /* supply clock for SCUX */
                    cpg_value = (uint32_t)CPG.STBCR8 & ~(CPG_STBCR8_BIT_MSTP81);
                    CPG.STBCR8 = (uint8_t)cpg_value;
                    dummy_buf = CPG.STBCR8;
#if(1) /* mbed */
                    (void)dummy_buf;
#endif

                    core_util_critical_section_exit();

                    /* software reset */
                    SCUX.SWRSR_CIM &= ~SWRSR_CIM_SWRST_SET;
                    SCUX.SWRSR_CIM |= SWRSR_CIM_SWRST_SET;
                }

                /* init DVU register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUIR_DVU0_0 = DVUIR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VADIR_DVU0_0 = VADIR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUBR_DVU0_0 = DVUBR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 = DVUCR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->ZCMCR_DVU0_0 = ZCMCR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRCTR_DVU0_0 = VRCTR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRPDR_DVU0_0 = VRPDR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRDBR_DVU0_0 = VRDBR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VRWTR_DVU0_0 = VRWTR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL0R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL1R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL2R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL3R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL4R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL5R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL6R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VOL7R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->DVUER_DVU0_0 = DVUER_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VEVMR_DVU0_0 = VEVMR_DVU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_dvu_reg->VEVCR_DVU0_0 = VEVCR_DVU0_INIT_VALUE;

                /* init SRC register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCIR0_2SRC0_0 = SRCIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SADIR0_2SRC0_0 = SADIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCBR0_2SRC0_0 = SRCBR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSCR0_2SRC0_0 = IFSCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSVR0_2SRC0_0 = IFSVR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCCR0_2SRC0_0 = (SRCCR_2SRC0_INIT_VALUE | SRCCR_2SRC0_BASE_VALUE);
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->MNFSR0_2SRC0_0 = MNFSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->BFSSR0_2SRC0_0 = BFSSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->WATSR0_2SRC0_0 = WATSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVMR0_2SRC0_0 = SEVMR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVCR0_2SRC0_0 = SEVCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCIR1_2SRC0_0 = SRCIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SADIR1_2SRC0_0 = SADIR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCBR1_2SRC0_0 = SRCBR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSCR1_2SRC0_0 = IFSCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->IFSVR1_2SRC0_0 = IFSVR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCCR1_2SRC0_0 = (SRCCR_2SRC0_INIT_VALUE | SRCCR_2SRC0_BASE_VALUE);
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->MNFSR1_2SRC0_0 = MNFSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->BFSSR1_2SRC0_0 = BFSSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->WATSR1_2SRC0_0 = WATSR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVMR1_2SRC0_0 = SEVMR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SEVCR1_2SRC0_0 = SEVCR_2SRC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_src_reg->SRCIRR_2SRC0_0 = SRCIRR_2SRC0_INIT_VALUE;

                /* init FFU register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->FFUIR_FFU0_0 = FFUIR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->FUAIR_FFU0_0 = FUAIR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->URQSR_FFU0_0 = URQSR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->FFUPR_FFU0_0 = FFUPR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->UEVMR_FFU0_0 = UEVMR_FFU0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffu_reg->UEVCR_FFU0_0 = UEVCR_FFU0_INIT_VALUE;

                /* init FFD register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FFDIR_FFD0_0 = FFDIR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FDAIR_FFD0_0 = FDAIR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->DRQSR_FFD0_0 = DRQSR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FFDPR_FFD0_0 = FFDPR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->FFDBR_FFD0_0 = FFDBR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 = DEVMR_FFD0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 = DEVCR_FFD0_INIT_VALUE;

                /* init OPC register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_opc_reg->OPCIR_OPC0_0 = OPCIR_OPC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_opc_reg->OPSLR_OPC0_0 = OPSLR_OPC0_INIT_VALUE;

                /* init IPC register */
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ipc_reg->IPCIR_IPC0_0 = IPCIR_IPC0_INIT_VALUE;
                gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->p_ipc_reg->IPSLR_IPC0_0 = IPSLR_IPC0_INIT_VALUE;

                /* init MIX register for each channel */
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mdb_n_r_mix0_0) = MDB_N_R_MIX0_0_INIT_VALUE;

                /* init CIM register for each channel */
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->fdtsel_n_cim) = FDTSEL_CIM_INIT_VALUE;
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->futsel_n_cim) = FUTSEL_CIM_INIT_VALUE;
                *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->srcrsel_n_cim) = SRCRSEL_CIM_INIT_VALUE;

                /* init shared register */
                if (false == init_shared_flag)
                {
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixir_mix0_0) = MIXIR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->madir_mix0_0) = MADIR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixbr_mix0_0) = MIXBR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixmr_mix0_0) = MIXMR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mvpdr_mix0_0) = MVPDR_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mdber_mix0_0) = MDBER_MIX0_0_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->dmacr_cim) = DMACR_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->ssirsel_cim) = SSIRSEL_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->ssipmd_cim) = SSIPMD_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->ssictrl_cim) = SSICTRL_CIM_INIT_VALUE;
                    *(gb_scux_info_drv.info_ch[scux_ch_count].p_scux_reg->mixrsel_cim) = MIXRSEL_CIM_INIT_VALUE;
                }

                if (false == init_shared_flag)
                {
                    /* set SSIF register */
                    for (ssif_ch_count = SCUX_SSIF_CH_0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++)
                    {
                        gb_scux_ssif_info[ssif_ch_count].p_scux_ssif_reg = p_scux_ssif_ch_reg_addr[ssif_ch_count];
                    }
                }

                /* set  semaphore parameter */
                gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access = osSemaphoreNew(0xffff, 1, p_semdef_ch_scux_access[scux_ch_count]);
                if (NULL == gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access)
                {
                     retval = ENOMEM_RBSP;
                }
                if ((ESUCCESS == retval) && (false == init_shared_flag))
                {
                    for (ssif_ch_count = SCUX_SSIF_CH_0; ((ssif_ch_count < SCUX_SSIF_CH_NUM) && (ESUCCESS == retval)); ssif_ch_count++)
                    {
                        gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access = osSemaphoreNew(0xffff, 1, p_semdef_ch_scux_ssif_access[ssif_ch_count]);
                        if (NULL == gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access)
                        {
                            retval = ENOMEM_RBSP;
                        }
                    }
                }
                if ((ESUCCESS == retval) && (false == init_shared_flag))
                {
                    gb_scux_info_drv.shared_info.sem_shared_access = osSemaphoreNew(0xffff, 1, osSemaphore(scux_shared_access));
                    if (NULL == gb_scux_info_drv.shared_info.sem_shared_access)
                    {
                        retval = ENOMEM_RBSP;
                    }
                }

                if (false == init_shared_flag)
                {
                    init_shared_flag = true;
                }

                gb_scux_info_drv.info_ch[scux_ch_count].ch_stat = SCUX_CH_INIT;
            }
        }
    }

    if (ESUCCESS != retval)
    {
        for (scux_ch_count = SCUX_SSIF_CH_0; scux_ch_count < SCUX_CH_NUM; scux_ch_count++)
        {
            if (false != gb_scux_info_drv.info_ch[scux_ch_count].enabled)
            {
                /* semaphore delete */
                sem_ercd = osSemaphoreDelete(gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access);
                /* semaphore error check */
                if (osOK != sem_ercd)
                {
                    /* set error return value */
                    retval = EFAULT_RBSP;
                }

                gb_scux_info_drv.info_ch[scux_ch_count].enabled = false;
                gb_scux_info_drv.info_ch[scux_ch_count].ch_stat = SCUX_CH_UNINIT;
            }
        }

        for (ssif_ch_count = SCUX_SSIF_CH_0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++)
        {
            sem_ercd = osSemaphoreDelete(gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set error return value */
                retval = EFAULT_RBSP;
            }
        }

        sem_ercd = osSemaphoreDelete(gb_scux_info_drv.shared_info.sem_shared_access);
        /* semaphore error check */
        if (osOK != sem_ercd)
        {
            /* set error return value */
            retval = EFAULT_RBSP;
        }

        gb_scux_info_drv.drv_stat = SCUX_DRV_UNINIT;
    }
    else
    {
        gb_scux_info_drv.drv_stat = SCUX_DRV_INIT;
    }

    return retval;
}

/******************************************************************************
End of function SCUX_Initialize
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_UnInitialize
* @brief         Uninit SCUX driver.
*
*                Description:<br>
*
* @param[in]     None.
* @retval        None.
******************************************************************************/
void SCUX_UnInitialize(void)
{
    osStatus sem_ercd;
    int_t    ercd;
    int_t    scux_ch_count;
    int_t    ssif_ch_count;
    uint32_t cpg_value;

    for (scux_ch_count = 0; scux_ch_count < SCUX_CH_NUM; scux_ch_count++)
    {
        if (false != gb_scux_info_drv.info_ch[scux_ch_count].enabled)
        {
            /* check ch_stat whether going transfer */
            if ((SCUX_CH_UNINIT != gb_scux_info_drv.info_ch[scux_ch_count].ch_stat) &&
                (SCUX_CH_INIT != gb_scux_info_drv.info_ch[scux_ch_count].ch_stat) &&
                (SCUX_CH_STOP != gb_scux_info_drv.info_ch[scux_ch_count].ch_stat))
            {
                core_util_critical_section_enter();

                /* This exclusive access control ends in the SCUX_IoctlClearStop */
                ercd = SCUX_IoctlClearStop(scux_ch_count);
                if (ESUCCESS != ercd)
                {
                    /* NON_NOTICE_ASSERT: SCUX stop failed */
                }

                ercd = R_DMA_Free(gb_scux_info_drv.info_ch[scux_ch_count].dma_tx_ch, NULL);
                if (ESUCCESS != ercd)
                {
                     /* NON_NOTICE_ASSERT: DMA release failed */
                }
            }
        }
    }

    /* software reset */
    SCUX.SWRSR_CIM &= ~SWRSR_CIM_SWRST_SET;

    for (scux_ch_count = 0; scux_ch_count < SCUX_CH_NUM; scux_ch_count++)
    {
        if (false != gb_scux_info_drv.info_ch[scux_ch_count].enabled)
        {
            if (gb_scux_info_drv.info_ch[scux_ch_count].ch_stat == SCUX_CH_UNINIT)
            {
                /* NON_NOTICE_ASSERT: abnormal status */
            }

            /* uninit each resouces */
            gb_scux_info_drv.info_ch[scux_ch_count].ch_stat = SCUX_CH_UNINIT;

            /* delete each semaphore  */
            sem_ercd = osSemaphoreRelease(gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* NON_NOTICE_ASSERT: semaphore error */
            }

            sem_ercd = osSemaphoreDelete(gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* NON_NOTICE_ASSERT: semaphore error */
            }

            gb_scux_info_drv.info_ch[scux_ch_count].sem_ch_scux_access = NULL;

            /* delete queue */
            ahf_cancelall(&gb_scux_info_drv.info_ch[scux_ch_count].tx_que);
            ahf_destroy(&gb_scux_info_drv.info_ch[scux_ch_count].tx_que);
            ahf_cancelall(&gb_scux_info_drv.info_ch[scux_ch_count].rx_que);
            ahf_destroy(&gb_scux_info_drv.info_ch[scux_ch_count].rx_que);
        }
    }

    for (ssif_ch_count = 0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++)
    {
        sem_ercd = osSemaphoreRelease(gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access);
        /* semaphore error check */
        if (osOK != sem_ercd)
        {
            /* NON_NOTICE_ASSERT: semaphore error */
        }

        sem_ercd = osSemaphoreDelete(gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access);
        /* semaphore error check */
        if (osOK != sem_ercd)
        {
            /* NON_NOTICE_ASSERT: semaphore error */
        }

        gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access = NULL;
    }

    sem_ercd = osSemaphoreRelease(gb_scux_info_drv.shared_info.sem_shared_access);
    /* semaphore error check */
    if (osOK != sem_ercd)
    {
        /* NON_NOTICE_ASSERT: semaphore error */
    }

    sem_ercd = osSemaphoreDelete(gb_scux_info_drv.shared_info.sem_shared_access);
    /* semaphore error check */
    if (osOK != sem_ercd)
    {
        /* NON_NOTICE_ASSERT: semaphore error */
    }

    gb_scux_info_drv.shared_info.sem_shared_access = NULL;

    core_util_critical_section_enter();

    /* stop clock for SCUX */
    cpg_value = ((uint32_t)CPG.STBCR8 | CPG_STBCR8_BIT_MSTP81);
    CPG.STBCR8 = (uint8_t)cpg_value;

    core_util_critical_section_exit();

    gb_scux_info_drv.drv_stat = SCUX_DRV_UNINIT;
}

/******************************************************************************
End of function SCUX_UnInitialize
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_OpenChannel
* @brief         Open SCUX driver channel.
*
*                Description:<br>
*
* @param[in]     channel:open channel number.
* @param[in]     flags:specifies the access mode whether the channel is
*                      opened for a read or a write
* @retval        ESUCCESS: Operation successful.
*                ENOMEM_RBSP: Create queue is failed.
*                EMFILE_RBSP: Allocate DMA ch for write is failed.
******************************************************************************/

int_t SCUX_OpenChannel(const int_t channel, const int_t flags)
{
    int_t    retval;

    /* create write request queue */
    retval = ahf_create(&gb_scux_info_drv.info_ch[channel].tx_que, AHF_LOCKINT);
    if (ESUCCESS != retval)
    {
        retval = ENOMEM_RBSP;
    }
    else
    {
        /* create read request queue */
        retval = ahf_create(&gb_scux_info_drv.info_ch[channel].rx_que, AHF_LOCKINT);
        if (ESUCCESS != retval)
        {
            retval = ENOMEM_RBSP;
        }
    }

    if (ESUCCESS == retval)
    {
        SCUX_InterruptInit(channel);

        /* init channel information parameter */
        gb_scux_info_drv.info_ch[channel].open_flags = flags;
        gb_scux_info_drv.info_ch[channel].p_tx_aio = NULL;
        gb_scux_info_drv.info_ch[channel].p_tx_next_aio = NULL;
        gb_scux_info_drv.info_ch[channel].p_rx_aio = NULL;
        gb_scux_info_drv.info_ch[channel].p_rx_next_aio = NULL;
        gb_scux_info_drv.info_ch[channel].p_ssif_info1 = NULL;
        gb_scux_info_drv.info_ch[channel].p_ssif_info2 = NULL;
        gb_scux_info_drv.info_ch[channel].p_ssif_info3 = NULL;
        gb_scux_info_drv.info_ch[channel].p_flush_callback = NULL;
        gb_scux_info_drv.info_ch[channel].p_tx_dummy_data = &gb_scux_write_dummy_buf[0];
        gb_scux_info_drv.info_ch[channel].p_rx_dummy_data = &gb_scux_read_dummy_buf[0];

        /* get DMA channel for write */
        gb_scux_info_drv.info_ch[channel].dma_tx_ch = R_DMA_Alloc(DMA_ALLOC_CH, NULL);
        if (EERROR == gb_scux_info_drv.info_ch[channel].dma_tx_ch)
        {
            retval = EMFILE_RBSP;
        }
        else
        {
            gb_scux_info_drv.info_ch[channel].ch_stat = SCUX_CH_STOP;
        }

    }

    return retval;
}

/******************************************************************************
End of function SCUX_OpenChannel
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CloseChannel
* @brief         Close SCUX driver channel.
*
*                Description:<br>
*
* @param[in]     channel: SCUX channel number.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t  SCUX_CloseChannel(const int_t channel)
{
    int_t    retval = ESUCCESS;
    int_t    ercd;

    /* check ch_stat whether going transfer */
    if ((SCUX_CH_UNINIT != gb_scux_info_drv.info_ch[channel].ch_stat) &&
        (SCUX_CH_INIT != gb_scux_info_drv.info_ch[channel].ch_stat) &&
        (SCUX_CH_STOP != gb_scux_info_drv.info_ch[channel].ch_stat))
    {
        core_util_critical_section_enter();

        /* This exclusive access control ends in the SCUX_IoctlClearStop */
        ercd = SCUX_IoctlClearStop(channel);
        if (ESUCCESS != ercd)
        {
            retval = EFAULT_RBSP;
        }
    }

    if (ESUCCESS == retval)
    {
        /* delete queue */
        ahf_cancelall(&gb_scux_info_drv.info_ch[channel].tx_que);
        ahf_destroy(&gb_scux_info_drv.info_ch[channel].tx_que);
        ahf_cancelall(&gb_scux_info_drv.info_ch[channel].rx_que);
        ahf_destroy(&gb_scux_info_drv.info_ch[channel].rx_que);

        SCUX_InterruptUninit(channel);

        ercd = R_DMA_Free(gb_scux_info_drv.info_ch[channel].dma_tx_ch, NULL);
        if (ESUCCESS != ercd)
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            /* reset error status */
            gb_scux_info_drv.info_ch[channel].err_stat_backup = ESUCCESS;
            /* set channel status to open */
            gb_scux_info_drv.info_ch[channel].ch_stat = SCUX_CH_INIT;
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CloseChannel
******************************************************************************/


/**************************************************************************//**
* Function Name: SCUX_CheckParam
* @brief         Check SCUX parameter.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch:SCUX channel information.
* @retval        ESUCCESS : Parameter is no problems.
*                EACCES_RBSP : DVU setting isn't performed when DVU is used.
*                EACCES_RBSP : MIX setting isn't performed when MIX is used.
*                EACCES_RBSP : SSIF setting isn't performed when SSIF is used.
*                EPERM_RBSP : Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_CheckParam(scux_info_ch_t * const p_scux_info_ch)
{
    int_t retval = ESUCCESS;
    uint32_t ssif_ch[SCUX_SSIF_NUM_CH_ARRANGEMENT] = {SCUX_SSIF_NO_USE_CH, SCUX_SSIF_NO_USE_CH, SCUX_SSIF_NO_USE_CH};
    bool_t use_mix_flag = false;

    if (NULL == p_scux_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* check route parameter */
        if (((SCUX_ROUTE_SRC_MEM_MIN >= p_scux_info_ch->route_set) || (SCUX_ROUTE_SRC_MEM_MAX <= p_scux_info_ch->route_set)) &&
            ((SCUX_ROUTE_SRC_SSIF_MIN >= p_scux_info_ch->route_set) || (SCUX_ROUTE_SRC_SSIF_MAX <= p_scux_info_ch->route_set)) &&
            ((SCUX_ROUTE_SRC_MIX_SSIF_MIN >= p_scux_info_ch->route_set) || (SCUX_ROUTE_SRC_MIX_SSIF_MAX <= p_scux_info_ch->route_set)))
        {
            retval = EPERM_RBSP;
        }
        else
        {
            /* check route whether include SCUX channel */
            switch (p_scux_info_ch->channel)
            {
                case SCUX_CH_0:
                    if ((SCUX_ROUTE_SRC0_MEM != p_scux_info_ch->route_set)         &&
                        (SCUX_ROUTE_SRC0_SSIF0 != p_scux_info_ch->route_set)       &&
                        (SCUX_ROUTE_SRC0_MIX_SSIF0 != p_scux_info_ch->route_set)   &&
                        (SCUX_ROUTE_SRC0_SSIF3 != p_scux_info_ch->route_set)       &&
                        (SCUX_ROUTE_SRC0_MIX_SSIF3 != p_scux_info_ch->route_set)   &&
                        (SCUX_ROUTE_SRC0_SSIF012 != p_scux_info_ch->route_set)     &&
#if defined(TARGET_RZA1H) /* mbed */
                        (SCUX_ROUTE_SRC0_MIX_SSIF012 != p_scux_info_ch->route_set) &&
                        (SCUX_ROUTE_SRC0_SSIF345 != p_scux_info_ch->route_set)     &&
                        (SCUX_ROUTE_SRC0_MIX_SSIF345 != p_scux_info_ch->route_set))
#else /* mbed */
                        (SCUX_ROUTE_SRC0_MIX_SSIF012 != p_scux_info_ch->route_set))
#endif /* mbed */
                    {
                        retval = EPERM_RBSP;
                    }
                break;

                case SCUX_CH_1:
                    if ((SCUX_ROUTE_SRC1_MEM != p_scux_info_ch->route_set)         &&
                        (SCUX_ROUTE_SRC1_SSIF0 != p_scux_info_ch->route_set)       &&
                        (SCUX_ROUTE_SRC1_MIX_SSIF0 != p_scux_info_ch->route_set)   &&
                        (SCUX_ROUTE_SRC1_SSIF3 != p_scux_info_ch->route_set)       &&
                        (SCUX_ROUTE_SRC1_MIX_SSIF3 != p_scux_info_ch->route_set)   &&
                        (SCUX_ROUTE_SRC1_SSIF012 != p_scux_info_ch->route_set)     &&
#if defined(TARGET_RZA1H) /* mbed */
                        (SCUX_ROUTE_SRC1_MIX_SSIF012 != p_scux_info_ch->route_set) &&
                        (SCUX_ROUTE_SRC1_SSIF345 != p_scux_info_ch->route_set)     &&
                        (SCUX_ROUTE_SRC1_MIX_SSIF345 != p_scux_info_ch->route_set))
#else /* mbed */
                        (SCUX_ROUTE_SRC1_MIX_SSIF012 != p_scux_info_ch->route_set))
#endif /* mbed */
                    {
                        retval = EPERM_RBSP;
                    }
                break;

                case SCUX_CH_2:
                    if ((SCUX_ROUTE_SRC2_MEM != p_scux_info_ch->route_set)         &&
                        (SCUX_ROUTE_SRC2_MIX_SSIF0 != p_scux_info_ch->route_set)   &&
                        (SCUX_ROUTE_SRC2_SSIF1 != p_scux_info_ch->route_set)       &&
                        (SCUX_ROUTE_SRC2_MIX_SSIF3 != p_scux_info_ch->route_set)   &&
#if defined(TARGET_RZA1H) /* mbed */
                        (SCUX_ROUTE_SRC2_SSIF4 != p_scux_info_ch->route_set)       &&
                        (SCUX_ROUTE_SRC2_MIX_SSIF012 != p_scux_info_ch->route_set) &&
                        (SCUX_ROUTE_SRC2_MIX_SSIF345 != p_scux_info_ch->route_set))
#else /* mbed */
                        (SCUX_ROUTE_SRC2_MIX_SSIF012 != p_scux_info_ch->route_set))
#endif /* mbed */
                    {
                        retval = EPERM_RBSP;
                    }
                break;

                case SCUX_CH_3:
                    if ((SCUX_ROUTE_SRC3_MEM != p_scux_info_ch->route_set)          &&
                        (SCUX_ROUTE_SRC3_MIX_SSIF0 != p_scux_info_ch->route_set)    &&
                        (SCUX_ROUTE_SRC3_SSIF2 != p_scux_info_ch->route_set)        &&
                        (SCUX_ROUTE_SRC3_MIX_SSIF3 != p_scux_info_ch->route_set)    &&
#if defined(TARGET_RZA1H) /* mbed */
                        (SCUX_ROUTE_SRC3_SSIF5 != p_scux_info_ch->route_set)        &&
                        (SCUX_ROUTE_SRC3_MIX_SSIF012 != p_scux_info_ch->route_set)  &&
                        (SCUX_ROUTE_SRC3_MIX_SSIF345 != p_scux_info_ch->route_set))
#else /* mbed */
                        (SCUX_ROUTE_SRC3_MIX_SSIF012 != p_scux_info_ch->route_set))
#endif /* mbed */
                    {
                        retval = EPERM_RBSP;
                    }
                break;

                default :
                    /* NOTREACHED on At the time of a normal performance */
                    retval = EFAULT_RBSP;
                break;

            }
        }

        if (ESUCCESS == retval)
        {
            /* set using SSIF channel and MIX information */
            switch (p_scux_info_ch->route_set)
            {
                case SCUX_ROUTE_SRC0_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC1_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC2_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC3_MEM :
                    /* do nothing, when mem to mem route is setting */
                break;

                case SCUX_ROUTE_SRC0_SSIF0 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_0;
                break;

                case SCUX_ROUTE_SRC0_SSIF012 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_0;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = SCUX_SSIF_CH_1;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = SCUX_SSIF_CH_2;
                break;

                case SCUX_ROUTE_SRC0_SSIF3 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_3;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC0_SSIF345 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_3;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = SCUX_SSIF_CH_4;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = SCUX_SSIF_CH_5;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC1_SSIF0 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_0;
                break;

                case SCUX_ROUTE_SRC1_SSIF012 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_0;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = SCUX_SSIF_CH_1;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = SCUX_SSIF_CH_2;
                break;

                case SCUX_ROUTE_SRC1_SSIF3 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_3;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC1_SSIF345 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_3;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = SCUX_SSIF_CH_4;
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = SCUX_SSIF_CH_5;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC2_SSIF1 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_1;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC2_SSIF4 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_4;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC3_SSIF2 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_2;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC3_SSIF5 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = SCUX_SSIF_CH_5;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC0_MIX_SSIF0 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC0_MIX_SSIF012 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_1);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_2);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC0_MIX_SSIF3 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    use_mix_flag = true;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC0_MIX_SSIF345 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_4);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_5);
                    use_mix_flag = true;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC1_MIX_SSIF0 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC1_MIX_SSIF012 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_1);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_2);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC1_MIX_SSIF3 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    use_mix_flag = true;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC1_MIX_SSIF345 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_4);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_5);
                    use_mix_flag = true;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC2_MIX_SSIF0 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC2_MIX_SSIF012 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_1);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_2);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC2_MIX_SSIF3 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    use_mix_flag = true;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC2_MIX_SSIF345 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_4);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_5);
                    use_mix_flag = true;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC3_MIX_SSIF0 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC3_MIX_SSIF012 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_0);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_1);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_2);
                    use_mix_flag = true;
                break;

                case SCUX_ROUTE_SRC3_MIX_SSIF3 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    use_mix_flag = true;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC3_MIX_SSIF345 :
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1] = (SCUX_SSIF_CH_3);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2] = (SCUX_SSIF_CH_4);
                    ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3] = (SCUX_SSIF_CH_5);
                    use_mix_flag = true;
                break;

#endif /* mbed */
                default :
                    /* NOTREACHED on At the time of a normal performance */
                    retval = EPERM_RBSP;
                break;
            }
        }

        if (ESUCCESS == retval)
        {
            if (false != p_scux_info_ch->src_cfg.src_enable)
            {
                retval = SCUX_CheckSrcParam(p_scux_info_ch, ssif_ch);
            }
            else
            {
                if (false != p_scux_info_ch->src_cfg.mode_sync)
                {
                    /* src disable is async mode only */
                    retval = EPERM_RBSP;
                }
            }
        }

        if (SCUX_SSIF_NO_USE_CH != ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1])
        {
            /* check parameter for SSIF direct route */
            if ((ESUCCESS == retval) && (false != p_scux_info_ch->dvu_cfg.dvu_enable))
            {
                retval = SCUX_CheckDvuParam(p_scux_info_ch);
            }

            if (ESUCCESS == retval)
            {
                retval = SCUX_CheckSsifParam(p_scux_info_ch, ssif_ch, use_mix_flag);
            }

            if ((ESUCCESS == retval) && (false != use_mix_flag))
            {
                retval = SCUX_CheckMixParam(p_scux_info_ch);
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CheckParam
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CheckSrcParam
* @brief         Check SRC parameter and set rate parameter to SCUX information.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @param[in]     ssif_ch : Used ssif channel number.
* @param[in]     use_mix_flag : Flag of Using MIX .
* @retval        ESUCCESS : Parameter is no problems.
*                EPERM_RBSP : Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

static int_t SCUX_CheckSrcParam(scux_info_ch_t * const p_scux_info_ch, const uint32_t ssif_ch[SCUX_SSIF_NUM_CH_ARRANGEMENT])
{
    int_t retval = ESUCCESS;
    uint32_t ssif_arrange_num;
    uint32_t rate_sample_ratio;
    uint32_t max_conv_rate;
    uint32_t min_conv_rate = 0;
    uint32_t freq_value = 0;
    int_t audio_ch;
    uint32_t max_rate;

    if ((NULL == p_scux_info_ch) || (NULL == ssif_ch))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* check use ch */
        if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_1 == p_scux_info_ch->channel))
        {
            if ((SCUX_USE_CH_1 != p_scux_info_ch->src_cfg.use_ch) &&
                (SCUX_USE_CH_2 != p_scux_info_ch->src_cfg.use_ch) &&
                (SCUX_USE_CH_4 != p_scux_info_ch->src_cfg.use_ch) &&
                (SCUX_USE_CH_6 != p_scux_info_ch->src_cfg.use_ch) &&
                (SCUX_USE_CH_8 != p_scux_info_ch->src_cfg.use_ch))
            {
                retval = EPERM_RBSP;
            }
        }
        else
        {
            /* on SCUX2, SCUX3, enable audio channel is only 1ch and 2ch */
            if ((SCUX_USE_CH_1 != p_scux_info_ch->src_cfg.use_ch) && (SCUX_USE_CH_2 != p_scux_info_ch->src_cfg.use_ch))
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            /* if using SSIF, 1ch audio channel is disabled */
            if ((SCUX_SSIF_NO_USE_CH != ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1]) && (SCUX_USE_CH_1 == p_scux_info_ch->src_cfg.use_ch))
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            /* if  mutiple SSIF channel and enable TDM mode, only 2ch audio channel is enabled */
            for (ssif_arrange_num = 0; ((ESUCCESS == retval) && (ssif_arrange_num < SCUX_SSIF_NUM_CH_ARRANGEMENT)); ssif_arrange_num++)
            {
                if (SCUX_SSIF_NO_USE_CH != ssif_ch[ssif_arrange_num])
                {
                    if (false != gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].ssif_cfg.use_tdm)
                    {
                        if (SCUX_USE_CH_2 == p_scux_info_ch->src_cfg.use_ch)
                        {
                            retval = EPERM_RBSP;
                        }
                    }
                }
            }
        }

        if (ESUCCESS == retval)
        {
            /* multiple SSIF ch check (multiple SSIF is used SSIF2) */
            if ((SCUX_SSIF_NO_USE_CH != ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2]) && (SCUX_USE_CH_6 != p_scux_info_ch->src_cfg.use_ch))
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            /* check word length */
            if ((SCUX_DATA_LEN_MIN >= p_scux_info_ch->src_cfg.word_len) || (SCUX_DATA_LEN_MAX <= p_scux_info_ch->src_cfg.word_len))
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            /* check delay mode */
            if ((SCUX_DELAY_MIN >= p_scux_info_ch->src_cfg.delay_mode) || (SCUX_DELAY_MAX <= p_scux_info_ch->src_cfg.delay_mode))
            {
                retval = EPERM_RBSP;
            }
            else
            {
                /* enable audio channel is less than 2ch when delay mode is enabled */
                if ((SCUX_DELAY_NORMAL != p_scux_info_ch->src_cfg.delay_mode) && (SCUX_USE_CH_2 < p_scux_info_ch->src_cfg.use_ch))
                {
                    retval = EPERM_RBSP;
                }
            }
        }

        if (ESUCCESS == retval)
        {
            /* check rate setting */
            if (false != p_scux_info_ch->src_cfg.mode_sync)
            {
                /* check input rate */
                if ((SCUX_SYNC_RATE_8      == p_scux_info_ch->src_cfg.input_rate_sync)  ||
                    (SCUX_SYNC_RATE_11_025  == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_12      == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_16      == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_22_05   == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_24      == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_32      == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_44_1    == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_48      == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_64      == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_88_2    == p_scux_info_ch->src_cfg.input_rate_sync) ||
                    (SCUX_SYNC_RATE_96      == p_scux_info_ch->src_cfg.input_rate_sync))
                {
                    /* enable rate is less than 66KHz on 6ch */
                    if ((SCUX_USE_CH_6 == p_scux_info_ch->src_cfg.use_ch) &&
                        ((SCUX_SYNC_RATE_88_2 == p_scux_info_ch->src_cfg.input_rate_sync) ||
                         (SCUX_SYNC_RATE_96 == p_scux_info_ch->src_cfg.input_rate_sync)))
                    {
                        retval = EPERM_RBSP;
                    }

                    /* enable rate is less than 49KHz on 8ch */
                    if ((SCUX_USE_CH_8 == p_scux_info_ch->src_cfg.use_ch) &&
                        ((SCUX_SYNC_RATE_64 == p_scux_info_ch->src_cfg.input_rate_sync)   ||
                         (SCUX_SYNC_RATE_88_2 == p_scux_info_ch->src_cfg.input_rate_sync) ||
                         (SCUX_SYNC_RATE_96 == p_scux_info_ch->src_cfg.input_rate_sync)))
                    {
                        retval = EPERM_RBSP;
                    }
                }
                else
                {
                    retval = EPERM_RBSP;
                }

                if (ESUCCESS == retval) {
                    /* check output rate */
                    if ((SCUX_SYNC_RATE_8   == p_scux_info_ch->src_cfg.output_rate_sync)  ||
                        (SCUX_SYNC_RATE_16   == p_scux_info_ch->src_cfg.output_rate_sync) ||
                        (SCUX_SYNC_RATE_24   == p_scux_info_ch->src_cfg.output_rate_sync) ||
                        (SCUX_SYNC_RATE_32   == p_scux_info_ch->src_cfg.output_rate_sync) ||
                        (SCUX_SYNC_RATE_44_1 == p_scux_info_ch->src_cfg.output_rate_sync) ||
                        (SCUX_SYNC_RATE_48   == p_scux_info_ch->src_cfg.output_rate_sync) ||
                        (SCUX_SYNC_RATE_96   == p_scux_info_ch->src_cfg.output_rate_sync))
                    {
                        /* enable rate is less than 66KHz on 6ch, enable rate is less than 49KHz on 8ch  */
                        if ((SCUX_USE_CH_6 <= p_scux_info_ch->src_cfg.use_ch) &&
                            (SCUX_SYNC_RATE_96 == p_scux_info_ch->src_cfg.output_rate_sync))
                        {
                            retval = EPERM_RBSP;
                        }
                    }
                    else
                    {
                        retval = EPERM_RBSP;
                    }
                }

                if (ESUCCESS == retval)
                {
                    p_scux_info_ch->input_rate = p_scux_info_ch->src_cfg.input_rate_sync;
                    p_scux_info_ch->output_rate = p_scux_info_ch->src_cfg.output_rate_sync;
                }
            }
            else
            {
                /* async mode */
                /* check input rate */
                /* get input source clock */
                switch (p_scux_info_ch->src_cfg.input_clk_async)
                {
                    case SCUX_CLK_AUDIO_CLK :
                        freq_value = SCUX_AUDIO_CLK;
                    break;

                    case SCUX_CLK_AUDIO_X1 :
                        freq_value = SCUX_AUDIO_X1;
                    break;

                    case SCUX_CLK_MLB_CLK :
                        freq_value = SCUX_MLB_CLK;
                    break;

                    case SCUX_CLK_USB_X1 :
                        freq_value = SCUX_USB_X1;
                    break;

                    case SCUX_CLK_CLKP1_2 :
                        freq_value = SCUX_CLKLP1_DIV2;
                    break;

                    case SCUX_CLK_MTU_TIOC3A :
                        freq_value = p_scux_info_ch->src_cfg.freq_tioc3a;
                        if (0U == freq_value)
                        {
                            retval = EPERM_RBSP;
                        }
                    break;

                    case SCUX_CLK_MTU_TIOC4A :
                        freq_value = p_scux_info_ch->src_cfg.freq_tioc4a;
                        if (0U == freq_value)
                        {
                            retval = EPERM_RBSP;
                        }
                    break;

                    case SCUX_CLK_SSIF0_WS :
                        /* fall through */
                    case SCUX_CLK_SSIF1_WS :
                        /* fall through */
                    case SCUX_CLK_SSIF2_WS :
                        /* fall through */
                    case SCUX_CLK_SSIF3_WS :
#if defined(TARGET_RZA1H) /* mbed */
                        /* fall through */
                    case SCUX_CLK_SSIF4_WS :
                        /* fall through */
                    case SCUX_CLK_SSIF5_WS :
#endif /* mbed */
                        freq_value = p_scux_info_ch->src_cfg.input_ws;
                        if (0U == freq_value)
                        {
                            retval = EPERM_RBSP;
                        }
                    break;

                    default :
                        /* NOTREACHED on At the time of a normal performance */
                        retval = EPERM_RBSP;
                    break;
                }

                if (ESUCCESS == retval)
                {
                    /* check devide rate and devide source clock */
                    /* check source clock isn't SSIF WS signal */
                    if ((SCUX_CLK_SSIF0_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF1_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF2_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
#if defined(TARGET_RZA1H) /* mbed */
                        (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async) && 
                        (SCUX_CLK_SSIF4_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF5_WS != p_scux_info_ch->src_cfg.input_clk_async))
#else /* mbed */
                        (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async))
#endif /* mbed */
                    {
                        if ((0U != (p_scux_info_ch->src_cfg.input_div_async % SCUX_EVEN_VALUE_DIV)) ||
                            (SCUX_MAX_DIV_CLK < p_scux_info_ch->src_cfg.input_div_async))
                        {
                            retval = EPERM_RBSP;
                        }
                        else
                        {
                            if (0U == p_scux_info_ch->src_cfg.input_div_async)
                            {
                                /* 0 is a same size */
                                p_scux_info_ch->input_rate = freq_value;
                            }
                            else{
                                p_scux_info_ch->input_rate = freq_value / p_scux_info_ch->src_cfg.input_div_async;
                            }
                        }
                    }
                    else
                    {
                        p_scux_info_ch->input_rate = freq_value;
                    }

                    if (ESUCCESS == retval)
                    {
                        /* check input frequency is enable range */
                        if (SCUX_USE_CH_6 == p_scux_info_ch->src_cfg.use_ch)
                        {
                            max_rate = SCUX_MAX_FREQ_CH6;
                        }
                        else if (SCUX_USE_CH_8 == p_scux_info_ch->src_cfg.use_ch)
                        {
                            max_rate = SCUX_MAX_FREQ_CH8;
                        }
                        else
                        {
                            max_rate = SCUX_MAX_FREQ_CH1_4;
                        }

                        if ((SCUX_MIN_FREQ > p_scux_info_ch->input_rate) ||
                           (max_rate < p_scux_info_ch->input_rate))
                        {
                            retval = EPERM_RBSP;
                        }
                    }
                }

                if (ESUCCESS == retval)
                {
                    /* if output route is SSIF direct route ,WS signal frequency is used */
                    if (SCUX_SSIF_NO_USE_CH != ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1])
                    {
                        if ((SCUX_SYNC_RATE_8      == p_scux_info_ch->src_cfg.output_ws)  ||
                            (SCUX_SYNC_RATE_11_025  == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_12      == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_16      == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_22_05   == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_24      == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_32      == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_44_1    == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_48      == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_64      == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_88_2    == p_scux_info_ch->src_cfg.output_ws) ||
                            (SCUX_SYNC_RATE_96      == p_scux_info_ch->src_cfg.output_ws))
                        {
                            p_scux_info_ch->output_rate = p_scux_info_ch->src_cfg.output_ws;
                        }
                        else
                        {
                            retval = EPERM_RBSP;
                        }
                    }
                    else
                    {
                        /* check output rate (mem to mem)*/
                        /* get output source clock */
                        switch (p_scux_info_ch->src_cfg.output_clk_async)
                        {
                            case SCUX_CLK_AUDIO_CLK :
                                freq_value = SCUX_AUDIO_CLK;
                            break;

                            case SCUX_CLK_AUDIO_X1 :
                                freq_value = SCUX_AUDIO_X1;
                            break;

                            case SCUX_CLK_MLB_CLK :
                                freq_value = SCUX_MLB_CLK;
                            break;

                            case SCUX_CLK_USB_X1 :
                                freq_value = SCUX_USB_X1;
                            break;

                            case SCUX_CLK_CLKP1_2 :
                                freq_value = SCUX_CLKLP1_DIV2;
                            break;

                            case SCUX_CLK_MTU_TIOC3A :
                                freq_value = p_scux_info_ch->src_cfg.freq_tioc3a;
                                if (0U == freq_value)
                                {
                                    retval = EPERM_RBSP;
                                }
                            break;

                            case SCUX_CLK_MTU_TIOC4A :
                                freq_value = p_scux_info_ch->src_cfg.freq_tioc4a;
                                if (0U == freq_value)
                                {
                                    retval = EPERM_RBSP;
                                }
                            break;

                            case SCUX_CLK_SSIF0_WS :
                                /* fall through */
                            case SCUX_CLK_SSIF1_WS :
                                /* fall through */
                            case SCUX_CLK_SSIF2_WS :
                                /* fall through */
                            case SCUX_CLK_SSIF3_WS :
#if defined(TARGET_RZA1H) /* mbed */
                                /* fall through */
                            case SCUX_CLK_SSIF4_WS :
                                /* fall through */
                            case SCUX_CLK_SSIF5_WS :
#endif /* mbed */
                                freq_value = p_scux_info_ch->src_cfg.output_ws;
                                if (0U == freq_value)
                                {
                                    retval = EPERM_RBSP;
                                }
                            break;

                            default :
                                /* error check is gone when route is other than SSIF */
                                if (SCUX_ROUTE_SSIF != (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                                {
                                    retval = EPERM_RBSP;
                                }
                            break;
                        }
                    }

                    if (ESUCCESS == retval)
                    {
                        /* check devide rate on only except for SSIF route, MISRA R1.1 mesure */
                        if ((SCUX_CLK_SSIF0_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
                            (SCUX_CLK_SSIF1_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
                            (SCUX_CLK_SSIF2_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
#if defined(TARGET_RZA1H) /* mbed */
                            (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.output_clk_async) && 
                            (SCUX_CLK_SSIF4_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
                            (SCUX_CLK_SSIF5_WS != p_scux_info_ch->src_cfg.output_clk_async))
#else /* mbed */
                            (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.output_clk_async))
#endif /* mbed */
                        {
                            if ((0U != (p_scux_info_ch->src_cfg.output_div_async % SCUX_EVEN_VALUE_DIV)) ||
                                (SCUX_MAX_DIV_CLK < p_scux_info_ch->src_cfg.output_div_async))
                            {
                                retval = EPERM_RBSP;
                            }
                        }
                    }

                    if (ESUCCESS == retval)
                    {
                        /* check source clock isn't SSIF WS signal */
                        if ((SCUX_CLK_SSIF0_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
                            (SCUX_CLK_SSIF1_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
                            (SCUX_CLK_SSIF2_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
#if defined(TARGET_RZA1H) /* mbed */
                            (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.output_clk_async) && 
                            (SCUX_CLK_SSIF4_WS != p_scux_info_ch->src_cfg.output_clk_async) &&
                            (SCUX_CLK_SSIF5_WS != p_scux_info_ch->src_cfg.output_clk_async))
#else /* mbed */
                            (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.output_clk_async))
#endif /* mbed */
                        {
                            if (0U == p_scux_info_ch->src_cfg.output_div_async)
                            {
                                /* 0 is a same size */
                                p_scux_info_ch->output_rate = freq_value;
                            }
                            else
                            {
                                p_scux_info_ch->output_rate = freq_value / p_scux_info_ch->src_cfg.output_div_async;
                            }
                        }
                        else
                        {
                            p_scux_info_ch->output_rate = freq_value;
                        }
                        /* check input frequency is enable range */
                        if (SCUX_USE_CH_6 == p_scux_info_ch->src_cfg.use_ch)
                        {
                            max_rate = SCUX_MAX_FREQ_CH6;
                        }
                        else if (SCUX_USE_CH_8 == p_scux_info_ch->src_cfg.use_ch)
                        {
                            max_rate = SCUX_MAX_FREQ_CH8;
                        }
                        else
                        {
                            max_rate = SCUX_MAX_FREQ_CH1_4;
                        }

                        if ((SCUX_MIN_FREQ > p_scux_info_ch->output_rate) ||
                            (max_rate < p_scux_info_ch->output_rate))
                        {
                            retval = EPERM_RBSP;
                        }

                    }
                }
            }

            if (ESUCCESS == retval)
            {
                /* check convert rate range */
                max_conv_rate = SCUX_MAX_CONV_RATE;
                switch (p_scux_info_ch->src_cfg.delay_mode)
                {
                    case SCUX_DELAY_NORMAL :
                        switch (p_scux_info_ch->src_cfg.use_ch)
                        {
                            case SCUX_USE_CH_1 :
                                /* fall through */
                            case SCUX_USE_CH_2 :
                                min_conv_rate = SCUX_MIN_CONV_RATE_NORMAL_CH1_2;
                            break;

                            case SCUX_USE_CH_4 :
                                min_conv_rate = SCUX_MIN_CONV_RATE_NORMAL_CH4;
                            break;

                            case SCUX_USE_CH_6 :
                                min_conv_rate = SCUX_MIN_CONV_RATE_NORMAL_CH6;
                            break;

                            case SCUX_USE_CH_8 :
                                min_conv_rate = SCUX_MIN_CONV_RATE_NORMAL_CH8;
                            break;

                            default :
                                /* NOTREACHED on At the time of a normal performance */
                                retval = EPERM_RBSP;
                            break;
                        }
                    break;

                    case SCUX_DELAY_LOW_DELAY1 :
                        min_conv_rate = SCUX_MIN_CONV_RATE_DELAY1;
                    break;

                    case SCUX_DELAY_LOW_DELAY2 :
                        min_conv_rate = SCUX_MIN_CONV_RATE_DELAY2;
                    break;

                    default :
                        /* NOTREACHED on At the time of a normal performance */
                        retval = EPERM_RBSP;
                    break;
                }

                rate_sample_ratio = ((p_scux_info_ch->output_rate * SCUX_RATE_INT_CONV_VALUE) / p_scux_info_ch->input_rate);
                if ((min_conv_rate > rate_sample_ratio) || (max_conv_rate < rate_sample_ratio))
                {
                    retval = EPERM_RBSP;
                }
            }
        }

        if (ESUCCESS == retval)
        {
            /* check wait time */
            if (SCUX_MAX_WAIT_TIME < p_scux_info_ch->src_cfg.wait_sample)
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            /* check min rate ratio */
            if ((SCUX_MIN_RATE_MIN_PAERCENTAGE > (uint32_t)p_scux_info_ch->src_cfg.min_rate_percentage) ||
                (SCUX_MIN_RATE_MAX_PAERCENTAGE < (uint32_t)p_scux_info_ch->src_cfg.min_rate_percentage))
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            /* check input data position */
            for (audio_ch = 0; ((ESUCCESS == retval) && (audio_ch < p_scux_info_ch->src_cfg.use_ch)); audio_ch++)
            {
                if ((SCUX_AUDIO_CH_MIN >= p_scux_info_ch->src_cfg.select_in_data_ch[audio_ch]) ||
                    (SCUX_AUDIO_CH_MAX <= p_scux_info_ch->src_cfg.select_in_data_ch[audio_ch]))
                {
                    retval = EPERM_RBSP;
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CheckSrcParam
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CheckDvuParam
* @brief         Check DVU parameter.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch:SCUX channel information.
* @retval        ESUCCESS : Parameter is no problems.
*                EACCES_RBSP : DVU setting isn't performed when DVU is used.
*                EPERM_RBSP : Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

static int_t SCUX_CheckDvuParam(const scux_info_ch_t * const p_scux_info_ch)
{
    int_t retval = ESUCCESS;
    int_t audio_ch;
    uint32_t enable_ch_flag = false;

    if (NULL == p_scux_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (false == p_scux_info_ch->dvu_setup)
        {
            retval = EACCES_RBSP;
        }
        else
        {
            if (false != p_scux_info_ch->dvu_cfg.dvu_enable)
            {
                /* check digital volume value */
                if (false !=  p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol_enable)
                {
                    /* check digital volume value */
                    for (audio_ch = 0; ((ESUCCESS == retval) && (audio_ch < p_scux_info_ch->src_cfg.use_ch)); audio_ch++)
                    {
                        if (SCUX_MAX_DIGITAL_VOLUME < p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch])
                        {
                            retval = EPERM_RBSP;
                        }
                    }
                }

                /* check ramp volume value */
                for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < p_scux_info_ch->src_cfg.use_ch; audio_ch++)
                {
                    enable_ch_flag |= (uint32_t)p_scux_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol_enable[audio_ch];
                }

                if ((ESUCCESS == retval) && (false != (bool_t)enable_ch_flag))
                {
                    /* check ramp up period */
                    if ((p_scux_info_ch->dvu_cfg.dvu_ramp_vol.up_period <= SCUX_DVU_TIME_MIN) ||
                        (p_scux_info_ch->dvu_cfg.dvu_ramp_vol.up_period >= SCUX_DVU_TIME_MAX))
                    {
                        retval = EPERM_RBSP;
                    }

                    if (ESUCCESS == retval)
                    {
                        /* check ramp down period */
                        if ((p_scux_info_ch->dvu_cfg.dvu_ramp_vol.down_period <= SCUX_DVU_TIME_MIN) ||
                            (p_scux_info_ch->dvu_cfg.dvu_ramp_vol.down_period >= SCUX_DVU_TIME_MAX))
                        {
                            retval = EPERM_RBSP;
                        }
                    }

                    if (ESUCCESS == retval)
                    {
                        /* check ramp volume */
                        if (SCUX_MAX_RAMP_VOLUME < p_scux_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol)
                        {
                            retval = EPERM_RBSP;
                        }
                    }

                    if (ESUCCESS != retval)
                    {
                        /* check wait time */
                        if (SCUX_MAX_WAIT_TIME < p_scux_info_ch->dvu_cfg.dvu_ramp_vol.ramp_wait_time)
                        {
                            retval = EPERM_RBSP;
                        }
                    }
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CheckDvuParam
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CheckSsifParam
* @brief         Check SSIF parameter.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @param[in]     ssif_ch : Used ssif channel number.
* @param[in]     use_mix_flag : Flag of Using MIX .
* @retval        ESUCCESS : Parameter is no problems.
*                EACCES_RBSP : SSIF setting isn't performed when SSIF is used.
*                EACCES_RBSP : SSIF channel is already used.
*                EACCES_RBSP : When use MIX, it is a setup which does not agree in a route setup.
*                EPERM_RBSP : Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

static int_t SCUX_CheckSsifParam(scux_info_ch_t * const p_scux_info_ch, const uint32_t ssif_ch[SCUX_SSIF_NUM_CH_ARRANGEMENT], const bool_t use_mix_flag)
{
    int_t retval = ESUCCESS;
    uint32_t ssif_arrange_num;
    uint32_t mix_ssif_ch_bit = 0;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch) || (NULL == ssif_ch))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (false == use_mix_flag)
        {
            /* used SSIF channel check no used MIX route */
            for (ssif_arrange_num = 0; ((ESUCCESS == retval) && (ssif_arrange_num < SCUX_SSIF_NUM_CH_ARRANGEMENT)); ssif_arrange_num++)
            {
                if (SCUX_SSIF_NO_USE_CH != ssif_ch[ssif_arrange_num])
                {
                    /* check SSIF is already setup */
                    if (false == gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].ssif_setup)
                    {
                        retval = EACCES_RBSP;
                    }
                    else
                    {
                        /* used SSIF channel is checked by other SCUX channel */
                        if (0 != gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].scux_channel)
                        {
                            retval = EACCES_RBSP;
                        }
                    }
                }
            }
        }
        else
        {
            /* used SSIF channel check on MIX route */
            for (ssif_arrange_num = 0; ((ESUCCESS == retval) && (ssif_arrange_num < SCUX_SSIF_NUM_CH_ARRANGEMENT)); ssif_arrange_num++)
            {
                if (SCUX_SSIF_NO_USE_CH != ssif_ch[ssif_arrange_num])
                {
                    /* clear SCUX_SSIF_use_mix_flag and get SSIF channel number */
                    mix_ssif_ch_bit |= (1U << ssif_ch[ssif_arrange_num]);
                    /* check SSIF is already setup */
                    if (false == gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].ssif_setup)
                    {
                        retval = EACCES_RBSP;
                    }
                }
            }
            if (ESUCCESS == retval)
            {
                /* used SSIF channel check, on MIX route check */
                if (0U != p_info_drv->shared_info.mix_ssif_ch)
                {
                    /* In the MIX setup for the and after 2times, it is checked that same SSIF ch on 1st setting is set up */
                    if (p_info_drv->shared_info.mix_ssif_ch != mix_ssif_ch_bit)
                    {
                        retval = EACCES_RBSP;
                    }
                }
                else
                {
                    /*It checks that SSIF channel is not used once on first MIX setup */
                    if (SCUX_SSIF_NO_USE_CH != ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2])
                    {
                        if ((0 != gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1]].scux_channel) ||
                            (0 != gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2]].scux_channel) ||
                            (0 != gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3]].scux_channel))
                        {
                            retval = EACCES_RBSP;
                        }
                    }
                    else
                    {
                        if (0 != gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1]].scux_channel)
                        {
                            retval = EACCES_RBSP;
                        }
                    }
                }
            }
        }

        /* each SSIF parameter check */
        for (ssif_arrange_num = 0; ((ESUCCESS == retval) && (ssif_arrange_num < SCUX_SSIF_NUM_CH_ARRANGEMENT)); ssif_arrange_num++)
        {
            /* used SSIF channel check, on SSIF direct route check */
            if (SCUX_SSIF_NO_USE_CH != ssif_ch[ssif_arrange_num])
            {

                /* check system word */
                if ((gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].ssif_cfg.system_word <= SCUX_SSIF_SYSTEM_LEN_MIN) ||
                    (gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].ssif_cfg.system_word >= SCUX_SSIF_SYSTEM_LEN_MAX))
                {
                    retval = EPERM_RBSP;
                }
                else
                {
                    /* check system word >= data word */
                    /* The combination applicable to error only of data word of 24bit and system word of 16bit */
                    if ((SCUX_DATA_LEN_16 != p_scux_info_ch->src_cfg.word_len) &&
                        (SCUX_SSIF_SYSTEM_LEN_16 == gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].ssif_cfg.system_word))
                    {
                        retval = EPERM_RBSP;
                    }
                }

                if (ESUCCESS == retval)
                {
                    /* if multiple channel is used, TDM is disable */
                    if ((SCUX_SSIF_NO_USE_CH != ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2]) &&
                        (false != gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].ssif_cfg.use_tdm))
                    {
                        retval = EPERM_RBSP;
                    }
                }
                if (ESUCCESS == retval)
                {
                    retval = SCUX_CheckSsifClockDiv(p_scux_info_ch, ssif_ch[ssif_arrange_num]);
                }

                if (ESUCCESS == retval)
                {
                    gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].scux_channel
                    = (int_t)((uint32_t)gb_scux_ssif_info[ssif_ch[ssif_arrange_num]].scux_channel | (1U << p_scux_info_ch->channel));
                }
            }
        }

        if (ESUCCESS == retval)
        {
            /* set SSIF information structure pointer */
            if (SCUX_SSIF_NO_USE_CH != ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2])
            {
                p_scux_info_ch->p_ssif_info1 = &gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1]];
                p_scux_info_ch->p_ssif_info2 = &gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT2]];
                p_scux_info_ch->p_ssif_info3 = &gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT3]];
            }
            else
            {
                p_scux_info_ch->p_ssif_info1 = &gb_scux_ssif_info[ssif_ch[SCUX_SSIF_CH_ARRANGEMENT1]];
                p_scux_info_ch->p_ssif_info2 = NULL;
                p_scux_info_ch->p_ssif_info3 = NULL;
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CheckSsifParam
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CheckMixParam
* @brief         Check MIX parameter.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch:SCUX channel information.
* @retval        ESUCCESS : Parameter is no problems.
*                EACCES_RBSP : MIX setting isn't performed when MIX is used.
*                EPERM_RBSP : Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

static int_t SCUX_CheckMixParam(const scux_info_ch_t * const p_scux_info_ch)
{
    int_t retval = ESUCCESS;
    int_t scux_ch;
    int_t audio_ch;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (false == p_info_drv->shared_info.mix_setup)
        {
            retval = EACCES_RBSP;
        }
        else
        {
            if (false != p_info_drv->shared_info.mixmode_ramp)
            {
                /* check ramp up time */
                if ((SCUX_MIX_TIME_MIN >= p_info_drv->shared_info.up_period) ||
                    (SCUX_MIX_TIME_MAX <= p_info_drv->shared_info.up_period))
                {
                    retval = EPERM_RBSP;
                }

                /* check ramp down time */
                if ((SCUX_MIX_TIME_MIN >= p_info_drv->shared_info.down_period) ||
                    (SCUX_MIX_TIME_MAX <= p_info_drv->shared_info.down_period))
                {
                    retval = EPERM_RBSP;
                }
            }

            /* check MIX volume */
            for (scux_ch = SCUX_CH_0; ((ESUCCESS == retval) && (SCUX_CH_NUM > scux_ch)); scux_ch++)
            {
                /* register set on the channel itself which sets up or channel which has already MIX operated */
                if ((scux_ch == p_scux_info_ch->channel) ||
                    (0U != (p_info_drv->shared_info.mix_run_ch & (1U << scux_ch))))
                {
                    if (SCUX_MAX_RAMP_VOLUME < p_info_drv->shared_info.mix_vol[scux_ch])
                    {
                        retval = EPERM_RBSP;
                    }
                }
            }

            /* check output data position */
            for (audio_ch = 0; ((ESUCCESS == retval) && (audio_ch < p_scux_info_ch->src_cfg.use_ch)); audio_ch++)
            {
                /* check min rate ratio */
                if ((SCUX_AUDIO_CH_MIN >= p_info_drv->shared_info.select_out_data_ch[audio_ch]) ||
                    (SCUX_AUDIO_CH_MAX <= p_info_drv->shared_info.select_out_data_ch[audio_ch]))
                {
                    retval = EPERM_RBSP;
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CheckMixParam
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_StrNLen
* @brief         computes the length of the string
*
*                Description:<br>
*                if string is longer than maxlen, this function return maxlen
* @param[in]     p_str      :pointer of string
* @param[in]     maxlen     :maximum length of inspection
* @retval        'number of characters in the string' or 'maxlen'
******************************************************************************/
size_t SCUX_StrNLen(const char_t* p_str, size_t maxlen)
{
    size_t ret_len;

    if (NULL == p_str)
    {
        /* character string does not exist */
        ret_len = 0;
    }
    else
    {
        /* ->IPA P1.3.1 Even if a sign is positive, even a negative is no problem. */
        for (ret_len = 0; (maxlen != 0U) && (*p_str != '\0'); ret_len++)
        /* <-IPA P1.3.1 */
        {
            maxlen--;
            p_str++;
        }
    }

    return ret_len;
}

/******************************************************************************
End of function SCUX_StrNLen
******************************************************************************/
#if(1) /* mbed */
/**************************************************************************//**
* Function Name: SCUX_CmnUnInitialize
* @brief         uninitialize driver infomation.
*
*                Description:<br>
*                
* @param[in]     None.
* @retval        ESUCCESS : Parameter is no problems.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/
static int_t SCUX_CmnUnInitialize(void)
{
    int_t    retval = ESUCCESS;
    osStatus sem_ercd;
    int_t    ssif_ch_count;
    uint32_t cpg_value;
    volatile uint8_t dummy_buf;

    /* software reset */
    SCUX.SWRSR_CIM &= ~SWRSR_CIM_SWRST_SET;

    for (ssif_ch_count = 0; ssif_ch_count < SCUX_SSIF_CH_NUM; ssif_ch_count++) 
    {
        if (NULL != gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access)
        {
            sem_ercd = osSemaphoreDelete(gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set error return value */
                 retval = EFAULT_RBSP;
            }

            gb_scux_ssif_info[ssif_ch_count].sem_ch_scux_ssif_access = NULL;
        }
    }

    if (NULL != gb_scux_info_drv.shared_info.sem_shared_access)
    {
        sem_ercd = osSemaphoreDelete(gb_scux_info_drv.shared_info.sem_shared_access);
        /* semaphore error check */
        if (osOK != sem_ercd)
        {
            /* set error return value */
            retval = EFAULT_RBSP;
        }

        gb_scux_info_drv.shared_info.sem_shared_access = NULL;
    }

    core_util_critical_section_enter();

    /* stop clock for SCUX */
    cpg_value = ((uint32_t)CPG.STBCR8 | CPG_STBCR8_BIT_MSTP81);
    CPG.STBCR8 = (uint8_t)cpg_value;
    dummy_buf = CPG.STBCR8;
#if(1) /* mbed */
    (void)dummy_buf;
#endif

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function SCUX_CmnUnInitialize
******************************************************************************/
#endif /* end mbed */

