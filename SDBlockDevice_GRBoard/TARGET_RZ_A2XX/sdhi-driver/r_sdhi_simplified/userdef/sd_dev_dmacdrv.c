/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/******************************************************************************
* System Name  : SDHI Driver
* File Name    : sd_dev_dmacdrv.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : RZ/A2M SD Sample Program - DMAC Sample Program (Main)
* Operation    : 
* Limitations  : None
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 16.03.2018 1.00    First Release
*         : 14.12.2018 1.01    Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02    Support for OS
*         : 29.05.2019 1.20    Correspond to internal coding rules
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#if(1) // mbed
#include "cpg_iodefine.h"
#else
#include "r_mmu_lld_rza2m.h"
#include "r_stb_lld_rza2m.h"
#endif
#include "r_sdif.h"
#include "sd.h"
#include "sd_dev_dmacdrv.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#if(1) // mbed
#define     UNUSED_PARAM(param)             (void)(param)
#endif

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
* Function Name: sd_DMAC_PeriReqInit
* Description  : Initialize DMAC.
* Arguments    : int32_t  sd_port : channel no (0 or 1)
*              : uint32_t buff    : buffer addrees to transfer datas
*              : int32_t  dir     : direction to transfer
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sd_DMAC_PeriReqInit(int32_t sd_port, uint32_t buff, int32_t dir)
{
    st_sdhndl_t *p_hndl;
    uint32_t    paddr;
#if(1) /* mbed */
#else
    e_mmu_err_t e_mmu_err;
#endif

    if ((0 != sd_port) && (1 != sd_port))
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    
    /* Cast to an appropriate type */
    if (NULL == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* set DMA controller */
    /* W (CC_EXT_MODE, H'0000_0002) */
    SD_OUTP(p_hndl, CC_EXT_MODE, (uint64_t)(SD_INP(p_hndl, CC_EXT_MODE) | CC_EXT_MODE_DMASDRW));

    if (SD_TRANS_READ == dir)
    {
        /* W (DM_CM_DTRAN_MODE, "channel01", "bus width") */
        SD_OUTP(p_hndl, DM_CM_DTRAN_MODE, DM_CM_DTRAN_MODE_READ);
    }
    else
    {
        /* W (DM_CM_DTRAN_MODE, "channel00", "bus width") */
        SD_OUTP(p_hndl, DM_CM_DTRAN_MODE, DM_CM_DTRAN_MODE_WRITE);
    }

    /* W (DM_DTRAN_ADDR, "address") */
#if(1) /* mbed */
    paddr = buff;
    SD_OUTP(p_hndl, DM_DTRAN_ADDR, paddr);
#else
    e_mmu_err = R_MMU_VAtoPA(buff, &paddr);

    if (MMU_SUCCESS == e_mmu_err)
    {
        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, DM_DTRAN_ADDR, paddr);
    }
    else
    {
        return SD_ERR;
    }
#endif

    return SD_OK;
}
/*******************************************************************************
 End of function sd_DMAC_PeriReqInit
 ******************************************************************************/

/******************************************************************************
* Function Name: sd_DMAC_Open
* Description  : Start DMAC.
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t dir     : direction to transfer
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sd_DMAC_Open(int32_t sd_port, int32_t dir)
{
    st_sdhndl_t  *p_hndl;

    if ((0 != sd_port) && (1 != sd_port))
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);

    /* Cast to an appropriate type */
    if (NULL == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* enable dtranend and dtranerr */
    if (SD_TRANS_READ == dir)
    {
        /* Cast to an appropriate type */
        (void)_sd_set_int_dm_mask(p_hndl, DM_CM_INFO1_MASK_DTRANEND1, DM_CM_INFO2_MASK_DTRANERR1);
    }
    else
    {
        /* Cast to an appropriate type */
        (void)_sd_set_int_dm_mask(p_hndl, DM_CM_INFO1_MASK_DTRANEND0, DM_CM_INFO2_MASK_DTRANERR0);
    }

    /* W (DM_CM_DTRAN_CTRL, H'0000_0001) */
    SD_OUTP(p_hndl, DM_CM_DTRAN_CTRL, DM_CM_DTRAN_CTRL_DM_START);

    return SD_OK;
}
/*******************************************************************************
 End of function sd_DMAC_Open
 ******************************************************************************/

/******************************************************************************
* Function Name: sd_DMAC_Close
* Description  : Stop DMAC.
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sd_DMAC_Close(int32_t sd_port)
{
    st_sdhndl_t  *p_hndl;

    if ((0 != sd_port) && (1 != sd_port))
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);

    /* Cast to an appropriate type */
    if (NULL == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* W (DM_CM_INFO1, H'0000_0000) */
    SD_OUTP(p_hndl, DM_CM_INFO1, (uint64_t)0);

    /* SD_BUF disable DMA transfer */
    /* W (CC_EXT_MODE, H'0000_0000) */
    SD_OUTP(p_hndl, CC_EXT_MODE, (uint64_t)(SD_INP(p_hndl, CC_EXT_MODE) & ~CC_EXT_MODE_DMASDRW));

    /* disable dtransend and dtranerr */
    (void)_sd_clear_int_dm_mask(p_hndl,

                                /* Cast to an appropriate type */
                                (DM_CM_INFO1_MASK_DTRANEND0|DM_CM_INFO1_MASK_DTRANEND1),

                                /* Cast to an appropriate type */
                                (DM_CM_INFO2_MASK_DTRANERR0|DM_CM_INFO2_MASK_DTRANERR1));

    /* Clear dm_cm_info */
    (void)_sd_clear_dm_info(p_hndl,

                            /* Cast to an appropriate type */
                            (DM_CM_INFO1_MASK_DTRANEND0|DM_CM_INFO1_MASK_DTRANEND1),

                            /* Cast to an appropriate type */
                            (DM_CM_INFO2_MASK_DTRANERR0|DM_CM_INFO2_MASK_DTRANERR1));

    return SD_OK;
}
/*******************************************************************************
 End of function sd_DMAC_Close
 ******************************************************************************/

/******************************************************************************
* Function Name: sd_DMAC_Reset
* Description  : Soft resets of the SDHI module built-in DMAC.
* Arguments    : int32_t  sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sd_DMAC_Reset(int32_t sd_port)
{
#if(1) /* mbed */
    int32_t               ret;
    st_sdhndl_t           *p_hndl;
    st_sdhi_info_dev_ch_t *p_ch;
    volatile uint8_t      reg_read_8;
    volatile uint64_t     dummy_read_64;

    ret  = SD_OK;

    if ((sd_port != 0) && (sd_port != 1)) {
        ret = SD_ERR;
    } else {
        p_hndl = SD_GET_HNDLS(sd_port);
        if (p_hndl == NULL) {
            ret = SD_ERR;  /* not initilized */
        } else {
            p_ch = sddev_get_dev_ch_instance(sd_port);
            if (p_ch != NULL) {
                /* (1) Transition to Software Reset State */
                /* (b) Procedure for transition to the software reset state of modules for which the STBREQ register
                   has a corresponding bit */
                /* 1. Set the corresponding bit in the STBREQ register to 1 to generate a request to stop the module. */
                /* 2. Confirm that the corresponding bit in the STBACK register has been set to 1.  */
                if (sd_port == 0) {
                    while (1) {
                        CPG.STBREQ1.BYTE |= 0x04;
                        reg_read_8 = CPG.STBREQ1.BYTE; /* dummy read */
                        reg_read_8 = CPG.STBACK1.BYTE;
                        if ((reg_read_8 & 0x04) != 0) {
                            break;
                        }
                    }
                } else {
                    while (1) {
                        CPG.STBREQ1.BYTE |= 0x02;
                        reg_read_8 = CPG.STBREQ1.BYTE;
                        if ((reg_read_8 & 0x02) != 0) {
                            break;
                        }
                    }
                }

                /* 3. Set the SRST bit of the corresponding module to 1, then dummy-read the same register. */
                /* Logic reversed */
                SD_OUTP(p_hndl, DM_CM_RST, DM_CM_RST_RESET);
                dummy_read_64 = SD_INP(p_hndl, DM_CM_RST);
                (void)dummy_read_64;
            } else { /* if (p_ch != NULL) */
                ret = SD_ERR;
            }
        }
    }
    return ret;
#else
    int32_t               ret;
    st_sdhndl_t           *p_hndl;
    st_sdhi_info_dev_ch_t *p_ch;
    e_stb_err_t           e_stb_err;
    volatile uint64_t     dummy_read_64;

    ret  = SD_OK;

    if ((0 != sd_port) && (1 != sd_port))
    {
        ret = SD_ERR;
    }
    else
    {
        p_hndl = SD_GET_HNDLS(sd_port);

        /* Cast to an appropriate type */
        if (NULL == p_hndl)
        {
            ret = SD_ERR;  /* not initilized */
        }
        else
        {
            p_ch = sddev_get_dev_ch_instance(sd_port);

            /* Cast to an appropriate type */
            if (NULL != p_ch)
            {
                /*
                   (1) Transition to Software Reset State
                   (b) Procedure for transition to the software reset state of modules for which the STBREQ register
                    has a corresponding bit
                   1. Set the corresponding bit in the STBREQ register to 1 to generate a request to stop the module.
                   2. Confirm that the corresponding bit in the STBACK register has been set to 1.
                */
                e_stb_err = STB_AGAIN;
                while (STB_AGAIN == e_stb_err)
                {
                    e_stb_err = R_STB_RequestModuleStop(p_ch->stb.stb_ch);
                }
                if (STB_SUCCESS == e_stb_err)
                {
                    /* 3. Set the SRST bit of the corresponding module to 1, then dummy-read the same register. */
                    /* Logic reversed *//* Cast to an appropriate type */
                    SD_OUTP(p_hndl, DM_CM_RST, DM_CM_RST_RESET);

                    /* Cast to an appropriate type */
                    dummy_read_64 = SD_INP(p_hndl, DM_CM_RST);

                    /* Cast to an appropriate type */
                    UNUSED_PARAM(dummy_read_64);
                }
                else
                {
                    ret = SD_ERR;
                }
            }
            else /* if (NULL != p_ch) */
            {
                ret = SD_ERR;
            }
        }
    }
    return ret;
#endif
}
/*******************************************************************************
 End of function sd_DMAC_Reset
 ******************************************************************************/

/******************************************************************************
* Function Name: sd_DMAC_Released
* Description  : Soft resets released of the SDHI module built-in DMAC.
* Arguments    : int32_t  sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sd_DMAC_Released(int32_t sd_port)
{
#if(1) /* mbed */
    int32_t               ret;
    st_sdhndl_t           *p_hndl;
    st_sdhi_info_dev_ch_t *p_ch;
    volatile uint8_t      reg_read_8;
    volatile uint64_t     dummy_read_64;

    ret  = SD_OK;

    if ((sd_port != 0) && (sd_port != 1)) {
        ret = SD_ERR;
    } else { /* if ((sd_port != 0) && (sd_port != 1)) */
        p_hndl = SD_GET_HNDLS(sd_port);
        if (p_hndl == NULL) {
            ret = SD_ERR;  /* not initilized */
        } else { /* if (p_hndl == NULL) */
            p_ch = sddev_get_dev_ch_instance(sd_port);
            if (p_ch != NULL) {
                /* (2) Canceling Software Reset */
                /* (b) Procedure for release from the software reset state of modules for which the STBREQ register
                       has a corresponding bit  */
                /* 1. Clear the SRST bit of the corresponding module to 0, then dummy-read the same register.   */
                /* Logic reversed */
                SD_OUTP(p_hndl, DM_CM_RST, DM_CM_RST_RELEASED);
                dummy_read_64 = SD_INP(p_hndl, DM_CM_RST);
                (void)dummy_read_64;

                /* 2. Clear the corresponding bit in the STBREQ register to 0 to cancel the request to stop the module. */
                /* 3. Confirm that the corresponding bit in the STBACK register has been cleared to 0. */
                if (sd_port == 0) {
                    while (1) {
                        CPG.STBREQ1.BYTE &= ~0x04;
                        reg_read_8 = CPG.STBREQ1.BYTE; /* dummy read */
                        reg_read_8 = CPG.STBACK1.BYTE;
                        if ((reg_read_8 & 0x04) == 0) {
                            break;
                        }
                    }
                } else {
                    while (1) {
                        CPG.STBREQ1.BYTE &= ~0x02;
                        reg_read_8 = CPG.STBREQ1.BYTE; /* dummy read */
                        reg_read_8 = CPG.STBACK1.BYTE;
                        if ((reg_read_8 & 0x02) == 0) {
                            break;
                        }
                    }
                }
            } else { /* if (p_ch != NULL) */
                ret = SD_ERR;
            }
        }
    }
    return ret;
#else
    int32_t               ret;
    st_sdhndl_t           *p_hndl;
    st_sdhi_info_dev_ch_t *p_ch;
    e_stb_err_t           e_stb_err;
    volatile uint64_t     dummy_read_64;

    ret  = SD_OK;

    if ((0 != sd_port) && (1 != sd_port))
    {
        ret = SD_ERR;
    }
    else /* if ((0 != sd_port) && (1 != sd_port)) */
    {
        p_hndl = SD_GET_HNDLS(sd_port);

        /* Cast to an appropriate type */
        if (NULL == p_hndl)
        {
            ret = SD_ERR;  /* not initilized */
        }
        else /* if (NULL == p_hndl) */
        {
            p_ch = sddev_get_dev_ch_instance(sd_port);

            /* Cast to an appropriate type */
            if (NULL != p_ch)
            {
                /* (2) Canceling Software Reset */
                /* (b) Procedure for release from the software reset state of modules for which the STBREQ register
                       has a corresponding bit  */
                /* 1. Clear the SRST bit of the corresponding module to 0, then dummy-read the same register.   */
                /* Logic reversed *//* Cast to an appropriate type */
                SD_OUTP(p_hndl, DM_CM_RST, DM_CM_RST_RELEASED);

                /* Cast to an appropriate type */
                dummy_read_64 = SD_INP(p_hndl, DM_CM_RST);

                /* Cast to an appropriate type */
                UNUSED_PARAM(dummy_read_64);

                /* 2. Clear the corresponding bit in the STBREQ register to 0 to cancel the request to stop the module.
                   3. Confirm that the corresponding bit in the STBACK register has been cleared to 0. */
                e_stb_err = STB_AGAIN;
                while (STB_AGAIN == e_stb_err)
                {
                    e_stb_err = R_STB_RequestModuleStart(p_ch->stb.stb_ch);
                }
                if (STB_SUCCESS != e_stb_err)
                {
                    ret = SD_ERR;
                }
            }
            else /* if (NULL != p_ch) */
            {
                ret = SD_ERR;
            }
        }
    }
    return ret;
#endif
}
/*******************************************************************************
 End of function sd_DMAC_Released
 ******************************************************************************/

/* End of File */

