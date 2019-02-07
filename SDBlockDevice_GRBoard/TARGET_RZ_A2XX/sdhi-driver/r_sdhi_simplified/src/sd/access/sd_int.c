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
* http://www.renesas.com/disclaimer *
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* System Name  : SDHI Driver
* File Name    : sd_int.c
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : SD_INFO1 and SD_INFO2 interrupt
* Operation    : 
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
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


/*****************************************************************************
 * ID           :
 * Summary      : set SD_INFO1 and SD_INFO2 interrupt mask
 * Include      : 
 * Declaration  : int32_t _sd_set_int_mask(SDHNDL *hndl, uint64_t mask1,
 *              :     uint64_t mask2)
 * Functions    : set int_info1_mask and int_info2_mask depend on the mask bits
 *              : value
 *              : if mask bit is one, it is enabled
 *              : if mask bit is zero, it is disabled
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : uint64_t mask1 : SD_INFO1_MASK1 bits value
 *              : uint64_t mask2 : SD_INFO1_MASK2 bits value
 * Return       : SD_OK : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_set_int_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(hndl->sd_port);

    /* ---- set int_info1_mask and int_info2_mask ---- */
    hndl->int_info1_mask |= mask1;
    hndl->int_info2_mask |= mask2;

    /* ---- set hardware mask ---- */
    SD_OUTP(hndl,SD_INFO1_MASK,(uint64_t)~(hndl->int_info1_mask));
    SD_OUTP(hndl,SD_INFO2_MASK,(uint64_t)~(hndl->int_info2_mask));

    sddev_unl_cpu(hndl->sd_port);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : clear SD_INFO1 and SD_INFO2 interrupt mask
 * Include      : 
 * Declaration  : int32_t _sd_clear_int_mask(SDHNDL *hndl, uint64_t mask1,
 *              :   uint64_t mask2)
 * Functions    : clear int_cc_status_mask depend on the mask bits value
 *              : if mask bit is one, it is disabled
 *              : if mask bit is zero, it is enabled
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : uint64_t mask1 : SD_INFO1_MASK1 bits value
 *              : uint64_t mask2 : SD_INFO1_MASK2 bits value
 * Return       : SD_OK : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_clear_int_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(hndl->sd_port);
    
    /* ---- clear int_info1_mask and int_info2_mask ---- */
    hndl->int_info1_mask &= (uint64_t)~mask1;
    hndl->int_info2_mask &= (uint64_t)~mask2;
    
    /* ---- clear hardware mask ---- */
    SD_OUTP(hndl,SD_INFO1_MASK,(uint64_t)~(hndl->int_info1_mask));
    SD_OUTP(hndl,SD_INFO2_MASK,(uint64_t)~(hndl->int_info2_mask));
    
    sddev_unl_cpu(hndl->sd_port);
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : clear int_info bits
 * Include      : 
 * Declaration  : int32_t _sd_clear_info(SDHNDL *hndl, uint64_t clear_info1,
 *              :   uint64_t clear_info2)
 * Functions    : clear int_info1 and int_info2 depend on the clear value
 *              : 
 * Argument     : SDHNDL *hndl         : SD handle
 *              : uint64_t clear_info1 : int_info1 clear bits value
 *              : uint64_t clear_info2 : int_info2 clear bits value
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : SD_INFO1 and SD_INFO2 bits are not cleared
 *****************************************************************************/
int32_t _sd_clear_info(SDHNDL *hndl, uint64_t clear_info1, uint64_t clear_info2)
{
    sddev_loc_cpu(hndl->sd_port);
    
    /* ---- clear int_info1 and int_info2 ---- */
    hndl->int_info1 &= (uint64_t)~clear_info1;
    hndl->int_info2 &= (uint64_t)~clear_info2;

    sddev_unl_cpu(hndl->sd_port);
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get SD_INFO1 and SD_INFO2 interrupt elements
 * Include      : 
 * Declaration  : int32_t _sd_get_int(SDHNDL *hndl)
 * Functions    : get SD_INFO1 and SD_INFO2 bits
 *              : examine enabled elements
 *              : hearafter, clear SD_INFO1 and SD_INFO2 bits
 *              : save those bits to int_info1 or int_info2
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_get_int(SDHNDL *hndl)
{
    uint64_t info1;
    uint64_t info2;

    /* get SD_INFO1 and SD_INFO2 bits */
    info1 = (uint64_t)(SD_INP(hndl,SD_INFO1) & hndl->int_info1_mask);
    info2 = (uint64_t)(SD_INP(hndl,SD_INFO2) & hndl->int_info2_mask);

    /* clear SD_INFO1 and SD_INFO2 bits */
    SD_OUTP(hndl,SD_INFO1,(uint64_t)~info1);
    SD_OUTP(hndl,SD_INFO2,(uint64_t)~info2);

    /* save enabled elements */
    hndl->int_info1 |= info1;
    hndl->int_info2 |= info2;
    if(info1 || info2){
        return SD_OK;   /* any interrupt occured */
    }
    
    return SD_ERR;  /* no interrupt occured */
}

/*****************************************************************************
 * ID           :
 * Summary      : check SD_INFO1 and SD_INFO2 interrupt elements
 * Include      : 
 * Declaration  : int32_t sd_check_int(int32_t sd_port);
 * Functions    : check SD_INFO1 and SD_INFO2 interrupt elements
 *              : if any interrupt is detected, return SD_OK
 *              : if no interrupt is detected, return SD_ERR
 *              : 
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_check_int(int32_t sd_port)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(hndl->int_mode){
        /* ---- hardware interrupt mode ---- */
        if(hndl->int_info1 || hndl->int_info2){
            return SD_OK;
        }
        else{
            return SD_ERR;
        }
    }

    /* ---- polling mode ---- */
    return _sd_get_int(hndl);
}

/*****************************************************************************
 * ID           :
 * Summary      : SD_INFO1 and SD_INFO2 interrupt handler
 * Include      : 
 * Declaration  : void sd_int_handler(int32_t sd_port);
 * Functions    : SD_INFO1 and SD_INFO2 interrupt handler
 *              : examine the relevant elements (without masked)
 *              : save those elements to int_info1 or int_info2
 *              : if a callback function is registered, call it
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : none
 * Remark       : 
 *****************************************************************************/
void sd_int_handler(int32_t sd_port)
{
    SDHNDL  *hndl;
    int32_t cd;

    if( (sd_port != 0) && (sd_port != 1) ){
        return;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return;
    }

    if(_sd_get_int(hndl) == SD_OK){
        /* is card detect interrupt? */
        if(hndl->int_info1 & (SD_INFO1_MASK_DET_DAT3 | SD_INFO1_MASK_DET_CD)){
            if(hndl->int_cd_callback){
                if(hndl->int_info1 & (SD_INFO1_MASK_INS_DAT3 | SD_INFO1_MASK_INS_CD)){
                    cd = 1; /* insert */
                }
                else{
                    cd = 0; /* remove */
                }
                (*hndl->int_cd_callback)(sd_port, cd);
            }
            hndl->int_info1 &= (uint64_t)~(SD_INFO1_MASK_DET_DAT3 | SD_INFO1_MASK_DET_CD);
        }
        else{
            if(hndl->int_callback){
                (*hndl->int_callback)(sd_port, 0);  /* arguments to be defined */
            }
        }
    }
}

/*****************************************************************************
 * ID           :
 * Summary      : register SD_INFO1 or SD_INFO2 interrupt callback function
 * Include      : 
 * Declaration  : int32_t sd_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t));
 * Functions    : register callback function
 *              : if SD_INFO1 or SD_INFO2 interrupt are occured, call callback
 *              : function
 *              : 
 * Argument     : int32_t sd_port                       : channel no (0 or 1)
 *              : int32_t (*callback)(int32_t, int32_t) : callback function
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t))
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    hndl->int_callback = callback;

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : set DM_CM_INFO1_MASK and DM_CM_INFO2_MASK interrupt mask
 * Include      : 
 * Declaration  : int32_t _sd_set_int_dm_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2)
 * Functions    : set int_dm_info1_mask and int_dm_info2_mask depend on the mask bits
 *              : value
 *              : if mask bit is one, it is enabled
 *              : if mask bit is zero, it is disabled
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : uint64_t mask1 : DM_CM_INFO1_MASK bits value
 *              : uint64_t mask2 : DM_CM_INFO2_MASK bits value
 * Return       : SD_OK : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_set_int_dm_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(hndl->sd_port);

    /* ---- set int_dm_info1_mask and int_dm_info2_mask ---- */
    hndl->int_dm_info1_mask |= mask1;
    hndl->int_dm_info2_mask |= mask2;

    /* ---- set hardware mask ---- */
    SD_OUTP(hndl,DM_CM_INFO1_MASK,(uint64_t)~(hndl->int_dm_info1_mask));
    SD_OUTP(hndl,DM_CM_INFO2_MASK,(uint64_t)~(hndl->int_dm_info2_mask));

    sddev_unl_cpu(hndl->sd_port);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : clear DM_CM_INFO1_MASK and DM_CM_INFO2_MASK interrupt mask
 * Include      : 
 * Declaration  : int32_t _sd_clear_int_dm_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2)
 * Functions    : clear int_dm_info1_mask and int_dm_info2_mask depend on the mask bits value
 *              : if mask bit is one, it is disabled
 *              : if mask bit is zero, it is enabled
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : uint64_t mask1 : DM_CM_INFO1_MASK bits value
 *              : uint64_t mask2 : DM_CM_INFO2_MASK bits value
 * Return       : SD_OK : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_clear_int_dm_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2)
{
    sddev_loc_cpu(hndl->sd_port);
    
    /* ---- clear int_dm_info1_mask and int_dm_info2_mask ---- */
    hndl->int_dm_info1_mask &= (uint64_t)~mask1;
    hndl->int_dm_info2_mask &= (uint64_t)~mask2;
    
    /* ---- clear hardware mask ---- */
    SD_OUTP(hndl,DM_CM_INFO1_MASK,(uint64_t)~(hndl->int_dm_info1_mask));
    SD_OUTP(hndl,DM_CM_INFO2_MASK,(uint64_t)~(hndl->int_dm_info2_mask));
    
    sddev_unl_cpu(hndl->sd_port);
    

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : clear int_dm_info bits
 * Include      : 
 * Declaration  : int32_t _sd_clear_dm_info(SDHNDL *hndl, uint64_t clear_info1, uint64_t clear_info2)
 * Functions    : clear int_dm_info1 and int_dm_info2 depend on the clear value
 *              : 
 * Argument     : SDHNDL *hndl         : SD handle
 *              : uint64_t clear_info1 : int_dm_info1 clear bits value
 *              : uint64_t clear_info2 : int_dm_info2 clear bits value
 * Return       : SD_OK : end of succeed
 * Remark       : DM_CM_INFO1 and DM_CM_INFO2 bits are not cleared
 *****************************************************************************/
int32_t _sd_clear_dm_info(SDHNDL *hndl, uint64_t clear_info1, uint64_t clear_info2)
{
    sddev_loc_cpu(hndl->sd_port);
    
    /* ---- clear int_dm_info1 and int_dm_info2 ---- */
    hndl->int_dm_info1 &= (uint64_t)~clear_info1;
    hndl->int_dm_info2 &= (uint64_t)~clear_info2;

    sddev_unl_cpu(hndl->sd_port);
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get DM_CM_INFO1 and DM_CM_INFO2 interrupt elements
 * Include      : 
 * Declaration  : int32_t _sd_get_int_dm(SDHNDL *hndl)
 * Functions    : get DM_CM_INFO1 and DM_CM_INFO2 bits
 *              : examine enabled elements
 *              : hearafter, clear DM_CM_INFO1 and DM_CM_INFO2 bits
 *              : save those bits to int_dm_info1 or int_dm_info2
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : any interrupt occured
 *              : SD_ERR: no interrupt occured
 * Remark       : 
 *****************************************************************************/
int32_t _sd_get_int_dm(SDHNDL *hndl)
{
    uint64_t info1;
    uint64_t info2;

    /* get DM_CM_INFO1 and DM_CM_INFO2 bits */
    info1 = (uint64_t)(SD_INP(hndl,DM_CM_INFO1) & hndl->int_dm_info1_mask);
    info2 = (uint64_t)(SD_INP(hndl,DM_CM_INFO2) & hndl->int_dm_info2_mask);

    /* clear DM_CM_INFO1 and DM_CM_INFO2 bits */
    SD_OUTP(hndl,DM_CM_INFO1,(uint64_t)~info1);
    SD_OUTP(hndl,DM_CM_INFO2,(uint64_t)~info2);

    /* save enabled elements */
    hndl->int_dm_info1 |= info1;
    hndl->int_dm_info2 |= info2;
    if(info1 || info2){
        return SD_OK;   /* any interrupt occured */
    }
    
    return SD_ERR;  /* no interrupt occured */
}

/*****************************************************************************
 * ID           :
 * Summary      : check DM_CM_INFO1 and DM_CM_INFO2 interrupt elements
 * Include      : 
 * Declaration  : int32_t sd_check_int_dm(int32_t sd_port)
 * Functions    : check DM_CM_INFO1 and DM_CM_INFO2 interrupt elements
 *              : if any interrupt is detected, return SD_OK
 *              : if no interrupt is detected, return SD_ERR
 *              : 
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : SD_OK : any interrupt occured
 *              : SD_ERR: no interrupt occured
 * Remark       : 
 *****************************************************************************/
int32_t sd_check_int_dm(int32_t sd_port)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(hndl->int_mode){
        /* ---- hardware interrupt mode ---- */
        if(hndl->int_dm_info1 || hndl->int_dm_info2){
            return SD_OK;
        }
        else{
            return SD_ERR;
        }
    }

    /* ---- polling mode ---- */
    return _sd_get_int_dm(hndl);
}

/*****************************************************************************
 * ID           :
 * Summary      : DM_CM_INFO1 and DM_CM_INFO2 interrupt handler
 * Include      : 
 * Declaration  : void sd_int_dm_handler(int32_t sd_port)
 * Functions    : DM_CM_INFO1 and DM_CM_INFO2 interrupt handler
 *              : examine the relevant elements (without masked)
 *              : save those elements to int_dm_info1 or int_dm_info2
 *              : if a callback function is registered, call it
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : none
 * Remark       : 
 *****************************************************************************/
void sd_int_dm_handler(int32_t sd_port)
{
    SDHNDL  *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return;
    }

    if(_sd_get_int_dm(hndl) == SD_OK){
        if(hndl->int_dma_callback){
            (*hndl->int_dma_callback)(sd_port, 0);  /* arguments to be defined */
        }
    }
}

/*****************************************************************************
 * ID           :
 * Summary      : register DM_CM_INFO1 or DM_CM_INFO2 interrupt callback function
 * Include      : 
 * Declaration  : int32_t sd_set_dma_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t))
 * Functions    : register callback function
 *              : if DM_CM_INFO1 or DM_CM_INFO2 interrupt are occured, call callback
 *              : function
 *              : 
 * Argument     : int32_t sd_port                       : channel no (0 or 1)
 *              : int32_t (*callback)(int32_t, int32_t) : callback function
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_set_dma_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t))
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    hndl->int_dma_callback = callback;

    return SD_OK;
}

/* End of File */
