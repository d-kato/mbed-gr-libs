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
* File Name    : sd_int.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : SD_INFO1 and SD_INFO2 interrupt
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
 * Function Name: _sd_set_int_mask
 * Description  : set SD_INFO1 and SD_INFO2 interrupt mask.
 *              : set int_info1_mask and int_info2_mask depend on the mask bits
 *              : value
 *              : if mask bit is one, it is enabled
 *              : if mask bit is zero, it is disabled
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint64_t mask1      : SD_INFO1_MASK1 bits value
 *              : uint64_t mask2      : SD_INFO1_MASK2 bits value
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_set_int_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- set int_info1_mask and int_info2_mask ---- */
    p_hndl->int_info1_mask |= mask1;
    p_hndl->int_info2_mask |= mask2;

    /* ---- set hardware mask ---- */
    SD_OUTP(p_hndl, SD_INFO1_MASK, (uint64_t)~(p_hndl->int_info1_mask));

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2_MASK, (uint64_t)~(p_hndl->int_info2_mask));

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_set_int_mask
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_clear_int_mask
 * Description  : clear SD_INFO1 and SD_INFO2 interrupt mask.
 *              : clear int_cc_status_mask depend on the mask bits value
 *              : if mask bit is one, it is disabled
 *              : if mask bit is zero, it is enabled
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint64_t mask1      : SD_INFO1_MASK1 bits value
 *              : uint64_t mask2      : SD_INFO1_MASK2 bits value
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_clear_int_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- clear int_info1_mask and int_info2_mask ---- */
    p_hndl->int_info1_mask &= (uint64_t)~mask1;

    /* Cast to an appropriate type */
    p_hndl->int_info2_mask &= (uint64_t)~mask2;

    /* ---- clear hardware mask ---- */
    SD_OUTP(p_hndl, SD_INFO1_MASK, (uint64_t)~(p_hndl->int_info1_mask));

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2_MASK, (uint64_t)~(p_hndl->int_info2_mask));

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_clear_int_mask
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_clear_info
 * Description  : clear int_info bits.
 *              : clear int_info1 and int_info2 depend on the clear value
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 *              : uint64_t clear_info1 : int_info1 clear bits value
 *              : uint64_t clear_info2 : int_info2 clear bits value
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : SD_INFO1 and SD_INFO2 bits are not cleared
 *****************************************************************************/
int32_t _sd_clear_info(st_sdhndl_t *p_hndl, uint64_t clear_info1, uint64_t clear_info2)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- clear int_info1 and int_info2 ---- */
    p_hndl->int_info1 &= (uint64_t)~clear_info1;

    /* Cast to an appropriate type */
    p_hndl->int_info2 &= (uint64_t)~clear_info2;

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_clear_info
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_get_int
 * Description  : get SD_INFO1 and SD_INFO2 interrupt elements.
 *              : get SD_INFO1 and SD_INFO2 bits
 *              : examine enabled elements
 *              : hearafter, clear SD_INFO1 and SD_INFO2 bits
 *              : save those bits to int_info1 or int_info2
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_get_int(st_sdhndl_t *p_hndl)
{
    uint64_t info1;
    uint64_t info2;

    /* get SD_INFO1 and SD_INFO2 bits */
    info1 = (uint64_t)(SD_INP(p_hndl, SD_INFO1) & p_hndl->int_info1_mask);

    /* Cast to an appropriate type */
    info2 = (uint64_t)(SD_INP(p_hndl, SD_INFO2) & p_hndl->int_info2_mask);

    /* clear SD_INFO1 and SD_INFO2 bits */
    SD_OUTP(p_hndl, SD_INFO1, (uint64_t)~info1);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2, (uint64_t)~info2);

    /* save enabled elements */
    p_hndl->int_info1 |= info1;
    p_hndl->int_info2 |= info2;
    if (info1 || info2)
    {
        return SD_OK;   /* any interrupt occured */
    }

    return SD_ERR;  /* no interrupt occured */
}
/******************************************************************************
 End of function _sd_get_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_check_int
 * Description  : check SD_INFO1 and SD_INFO2 interrupt elements
 *              : if any interrupt is detected, return SD_OK
 *              : if no interrupt is detected, return SD_ERR
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_check_int(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    if (p_hndl->int_mode)
    {
        /* ---- hardware interrupt mode ---- */
        if (p_hndl->int_info1 || p_hndl->int_info2)
        {
            return SD_OK;
        }
        else
        {
            return SD_ERR;
        }
    }

    /* ---- polling mode ---- */
    return _sd_get_int(p_hndl);
}
/******************************************************************************
 End of function sd_check_int
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_int_handler
 * Description  : SD_INFO1 and SD_INFO2 interrupt handler
 *              : examine the relevant elements (without masked)
 *              : save those elements to int_info1 or int_info2
 *              : if a callback function is registered, call it
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : none
 *****************************************************************************/
void sd_int_handler(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;
    int32_t     cd;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return;
    }

    if (_sd_get_int(p_hndl) == SD_OK)
    {
        /* is card detect interrupt? */
        if (p_hndl->int_info1 & (SD_INFO1_MASK_DET_DAT3 | SD_INFO1_MASK_DET_CD))
        {
            if (p_hndl->int_cd_callback)
            {
                /* Cast to an appropriate type */
                if (p_hndl->int_info1 & (SD_INFO1_MASK_INS_DAT3 | SD_INFO1_MASK_INS_CD))
                {
                    cd = 1; /* insert */
                }
                else
                {
                    cd = 0; /* remove */
                }
                (*p_hndl->int_cd_callback)(sd_port, cd);
            }

            /* Cast to an appropriate type */
            p_hndl->int_info1 &= (uint64_t)~(SD_INFO1_MASK_DET_DAT3 | SD_INFO1_MASK_DET_CD);
        }
        else
        {
            if (p_hndl->int_callback)
            {
                (*p_hndl->int_callback)(sd_port, 0);  /* arguments to be defined */
            }
        }
    }
}
/******************************************************************************
 End of function sd_int_handler
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_set_intcallback
 * Description  : register SD_INFO1 or SD_INFO2 interrupt callback function.
 *              : register callback function
 *              : if SD_INFO1 or SD_INFO2 interrupt are occured, call callback
 *              : function
 * Arguments    : int32_t sd_port                       : channel no (0 or 1)
 *              : int32_t (*callback)(int32_t, int32_t) : callback function
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t))
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->int_callback = callback;

    return SD_OK;
}
/******************************************************************************
 End of function sd_set_intcallback
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_set_int_dm_mask
 * Description  : set DM_CM_INFO1_MASK and DM_CM_INFO2_MASK interrupt mask.
 *              : set int_dm_info1_mask and int_dm_info2_mask depend on the mask bits
 *              : value
 *              : if mask bit is one, it is enabled
 *              : if mask bit is zero, it is disabled
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint64_t mask1      : DM_CM_INFO1_MASK bits value
 *              : uint64_t mask2      : DM_CM_INFO2_MASK bits value
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_set_int_dm_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- set int_dm_info1_mask and int_dm_info2_mask ---- */
    p_hndl->int_dm_info1_mask |= mask1;
    p_hndl->int_dm_info2_mask |= mask2;

    /* ---- set hardware mask ---- */
    SD_OUTP(p_hndl, DM_CM_INFO1_MASK, (uint64_t)~(p_hndl->int_dm_info1_mask));

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO2_MASK, (uint64_t)~(p_hndl->int_dm_info2_mask));

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_set_int_dm_mask
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_clear_int_dm_mask
 * Description  : clear DM_CM_INFO1_MASK and DM_CM_INFO2_MASK interrupt mask.
 *              : clear int_dm_info1_mask and int_dm_info2_mask depend on the mask bits value
 *              : if mask bit is one, it is disabled
 *              : if mask bit is zero, it is enabled
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint64_t mask1      : DM_CM_INFO1_MASK bits value
 *              : uint64_t mask2      : DM_CM_INFO2_MASK bits value
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_clear_int_dm_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- clear int_dm_info1_mask and int_dm_info2_mask ---- */
    p_hndl->int_dm_info1_mask &= (uint64_t)~mask1;

    /* Cast to an appropriate type */
    p_hndl->int_dm_info2_mask &= (uint64_t)~mask2;

    /* ---- clear hardware mask ---- */
    SD_OUTP(p_hndl, DM_CM_INFO1_MASK, (uint64_t)~(p_hndl->int_dm_info1_mask));

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO2_MASK, (uint64_t)~(p_hndl->int_dm_info2_mask));

    sddev_unl_cpu(p_hndl->sd_port);


    return SD_OK;
}
/******************************************************************************
 End of function _sd_clear_int_dm_mask
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_clear_dm_info
 * Description  : clear int_dm_info bits.
 *              : clear int_dm_info1 and int_dm_info2 depend on the clear value
 * Arguments    : st_sdhndl_t *p_hndl  : SD handle
 *              : uint64_t clear_info1 : int_dm_info1 clear bits value
 *              : uint64_t clear_info2 : int_dm_info2 clear bits value
 * Return Value : SD_OK : end of succeed
 * Remark       : DM_CM_INFO1 and DM_CM_INFO2 bits are not cleared
 *****************************************************************************/
int32_t _sd_clear_dm_info(st_sdhndl_t *p_hndl, uint64_t clear_info1, uint64_t clear_info2)
{
    sddev_loc_cpu(p_hndl->sd_port);

    /* ---- clear int_dm_info1 and int_dm_info2 ---- */
    p_hndl->int_dm_info1 &= (uint64_t)~clear_info1;

    /* Cast to an appropriate type */
    p_hndl->int_dm_info2 &= (uint64_t)~clear_info2;

    sddev_unl_cpu(p_hndl->sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_clear_dm_info
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_get_int_dm
 * Description  : get DM_CM_INFO1 and DM_CM_INFO2 interrupt elements.
 *              : get DM_CM_INFO1 and DM_CM_INFO2 bits
 *              : examine enabled elements
 *              : hearafter, clear DM_CM_INFO1 and DM_CM_INFO2 bits
 *              : save those bits to int_dm_info1 or int_dm_info2
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : any interrupt occured
 *              : SD_ERR: no interrupt occured
 *****************************************************************************/
int32_t _sd_get_int_dm(st_sdhndl_t *p_hndl)
{
    uint64_t info1;
    uint64_t info2;

    /* get DM_CM_INFO1 and DM_CM_INFO2 bits */
    info1 = (uint64_t)(SD_INP(p_hndl, DM_CM_INFO1) & p_hndl->int_dm_info1_mask);

    /* Cast to an appropriate type */
    info2 = (uint64_t)(SD_INP(p_hndl, DM_CM_INFO2) & p_hndl->int_dm_info2_mask);

    /* clear DM_CM_INFO1 and DM_CM_INFO2 bits */
    SD_OUTP(p_hndl, DM_CM_INFO1, (uint64_t)~info1);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO2, (uint64_t)~info2);

    /* save enabled elements */
    p_hndl->int_dm_info1 |= info1;
    p_hndl->int_dm_info2 |= info2;
    if (info1 || info2)
    {
        return SD_OK;   /* any interrupt occured */
    }

    return SD_ERR;  /* no interrupt occured */
}
/******************************************************************************
 End of function _sd_get_int_dm
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_check_int_dm
 * Description  : check DM_CM_INFO1 and DM_CM_INFO2 interrupt elements
 *              : if any interrupt is detected, return SD_OK
 *              : if no interrupt is detected, return SD_ERR
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : any interrupt occured
 *              : SD_ERR: no interrupt occured
 *****************************************************************************/
int32_t sd_check_int_dm(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    if (p_hndl->int_mode)
    {
        /* ---- hardware interrupt mode ---- */
        if (p_hndl->int_dm_info1 || p_hndl->int_dm_info2)
        {
            return SD_OK;
        }
        else
        {
            return SD_ERR;
        }
    }

    /* ---- polling mode ---- */
    return _sd_get_int_dm(p_hndl);
}
/******************************************************************************
 End of function sd_check_int_dm
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_int_dm_handler
 * Description  : DM_CM_INFO1 and DM_CM_INFO2 interrupt handler
 *              : examine the relevant elements (without masked)
 *              : save those elements to int_dm_info1 or int_dm_info2
 *              : if a callback function is registered, call it
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : none
 *****************************************************************************/
void sd_int_dm_handler(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return;
    }

    if (_sd_get_int_dm(p_hndl) == SD_OK)
    {
        if (p_hndl->int_dma_callback)
        {
            (*p_hndl->int_dma_callback)(sd_port, 0);  /* arguments to be defined */
        }
    }
}
/******************************************************************************
 End of function sd_int_dm_handler
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_set_dma_intcallback
 * Description  : register DM_CM_INFO1 or DM_CM_INFO2 interrupt callback function.
 *              : register callback function
 *              : if DM_CM_INFO1 or DM_CM_INFO2 interrupt are occured, call callback
 *              : function
 * Arguments    : int32_t sd_port                       : channel no (0 or 1)
 *              : int32_t (*callback)(int32_t, int32_t) : callback function
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_set_dma_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t))
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->int_dma_callback = callback;

    return SD_OK;
}
/******************************************************************************
 End of function sd_set_dma_intcallback
 *****************************************************************************/

/* End of File */
