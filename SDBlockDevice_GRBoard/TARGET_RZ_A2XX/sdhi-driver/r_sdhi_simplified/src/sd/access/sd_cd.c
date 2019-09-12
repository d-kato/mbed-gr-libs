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
/*******************************************************************************
* System Name  : SDHI Driver
* File Name    : sd_cd.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  :
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 29.05.2019 1.20     Correspond to internal coding rules
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "r_sdif.h"
#include "sd.h"

#ifdef __CC_ARM
#pragma arm section code = "CODE_SDHI"
#pragma arm section rodata = "CONST_SDHI"
#pragma arm section rwdata = "DATA_SDHI"
#pragma arm section zidata = "BSS_SDHI"
#endif

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

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
 * Function Name: sd_cd_int
 * Description  : set card detect interrupt
 *              : if select SD_CD_INT_ENABLE, detect interrupt is enbled and
 *              : it is possible register callback function
 *              : if select SD_CD_INT_DISABLE, detect interrupt is disabled
 * Arguments    : int32_t sd_port               : channel no (0 or 1)
 *              : int32_t enable                : is enable or disable card detect interrupt?
 *              : int32_t (*callback)(int32_t, int32_t)
 *                                              : interrupt callback function
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_cd_int(int32_t sd_port, int32_t enable, int32_t (*callback)(int32_t, int32_t))
{
    uint64_t    info1;
    st_sdhndl_t *p_hndl;
    int32_t     layout;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }
    if ((SD_CD_INT_ENABLE != enable) && (SD_CD_INT_DISABLE != enable))
    {
        return SD_ERR;  /* parameter error */
    }

    /* is change interrupt disable to enable? */
    if ((p_hndl->int_info1_mask & (SD_INFO1_MASK_DET_DAT3 | SD_INFO1_MASK_DET_CD)) == 0)
    {
        sddev_loc_cpu(sd_port);

        /* Cast to an appropriate type */
        info1 = SD_INP(p_hndl, SD_INFO1) ;

        /* Cast to an appropriate type */
        info1 &= (uint64_t)~SD_INFO1_MASK_DET_DAT3_CD;

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_INFO1, info1); /* clear insert and remove bits */
        sddev_unl_cpu(sd_port);
    }

    layout = sddev_cd_layout(sd_port);
    if (SD_OK == layout)
    {
        if (SD_CD_INT_ENABLE == enable)
        {
            /* enable insert and remove interrupts */
            if (SD_CD_SOCKET == p_hndl->cd_port)  /* CD */
            {
                /* Cast to an appropriate type */
                _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DET_CD, 0);
            }
            else    /* DAT3 */
            {
                /* Cast to an appropriate type */
                _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DET_DAT3, 0);
            }
        }
        else    /* case SD_CD_INT_DISABLE */
        {
            /* disable insert and remove interrupts */
            if (SD_CD_SOCKET == p_hndl->cd_port)  /* CD */
            {
                /* Cast to an appropriate type */
                _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DET_CD, 0);
            }
            else    /* DAT3 */
            {
                /* Cast to an appropriate type */
                _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DET_DAT3, 0);
            }
        }
    }

    /* ---- register callback function ---- */
    p_hndl->int_cd_callback = callback;

    return SD_OK;
}
/******************************************************************************
 End of function sd_cd_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_check_media
 * Description  : check card insertion
 *              : if card is inserted, return SD_OK
 *              : if card is not inserted, return SD_ERR
 *              : if SD handle is not initialized, return SD_ERR
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : card is inserted
 *              : SD_ERR: card is not inserted
 *****************************************************************************/
int32_t sd_check_media(int32_t sd_port)
{
    st_sdhndl_t  *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    return _sd_check_media(p_hndl);
}
/******************************************************************************
 End of function sd_check_media
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_check_media
 * Description  : check card insertion
 *              : if card is inserted, return SD_OK
 *              : if card is not inserted, return SD_ERR
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : card is inserted
 *              : SD_ERR: card is not inserted
 *****************************************************************************/
int32_t _sd_check_media(st_sdhndl_t *p_hndl)
{
    uint16_t reg;
    int32_t  layout;

    /* Cast to an appropriate type */
    layout = sddev_cd_layout((int32_t)(p_hndl->sd_port));
    if (SD_OK == layout)
    {
        /* Cast to an appropriate type */
        reg = SD_INPLL(p_hndl, SD_INFO1);
        if (SD_CD_SOCKET == p_hndl->cd_port)
        {
            /* Cast to an appropriate type */
            reg &= (uint16_t)SD_INFO1_MASK_STATE_CD;    /* check CD level   */
#if defined(TARGET_RZ_A2M_SBEV) /* mbed */
            if ((reg & SD_INFO1_MASK_STATE_CD) == 0) {
                reg |= SD_INFO1_MASK_STATE_CD;
            } else {
                reg &= ~SD_INFO1_MASK_STATE_CD;
            }
#endif
        }
        else
        {
            /* Cast to an appropriate type */
            reg &= (uint16_t)SD_INFO1_MASK_STATE_DAT3;  /* check DAT3 level */
        }
    }
    else
    {
        reg = SD_CD_DETECT;                             /* Always inserted */
    }

    if (reg)
    {
        return SD_OK;   /* inserted */
    }

    return SD_ERR;  /* no card */
}
/******************************************************************************
 End of function _sd_check_media
 *****************************************************************************/


/* End of File */
