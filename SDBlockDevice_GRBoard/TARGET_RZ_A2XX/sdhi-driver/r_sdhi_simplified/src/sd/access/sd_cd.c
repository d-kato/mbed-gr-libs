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
* File Name    : sd_cd.c
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : 
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
 * Summary      : set card detect interrupt
 * Include      : 
 * Declaration  : int32_t sd_cd_int(int32_t sd_port, int32_t enable, int32_t (*callback)(int32_t, int32_t));
 * Functions    : set card detect interrupt
 *              : if select SD_CD_INT_ENABLE, detect interrupt is enbled and
 *              : it is possible register callback function
 *              : if select SD_CD_INT_DISABLE, detect interrupt is disabled
 *              : 
 * Argument     : int32_t sd_port               : channel no (0 or 1)
 *              : int32_t enable                : is enable or disable card detect interrupt?
 *              : (*callback)(int32_t, int32_t) : interrupt callback function
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_cd_int(int32_t sd_port, int32_t enable, int32_t (*callback)(int32_t, int32_t))
{
    uint64_t info1;
    SDHNDL   *hndl;
    int32_t  layout;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    if((enable != SD_CD_INT_ENABLE) && (enable != SD_CD_INT_DISABLE)){
        return SD_ERR;  /* parameter error */
    }
    /* is change interrupt disable to enable? */
    if((hndl->int_info1_mask & (SD_INFO1_MASK_DET_DAT3 
        | SD_INFO1_MASK_DET_CD)) == 0){
        sddev_loc_cpu(sd_port);
        info1 = SD_INP(hndl,SD_INFO1) ;
        info1 &= (uint64_t)~SD_INFO1_MASK_DET_DAT3_CD;
        SD_OUTP(hndl,SD_INFO1,info1);   /* clear insert and remove bits */
        sddev_unl_cpu(sd_port);
    }

    layout = sddev_cd_layout(sd_port);
    if(layout == SD_OK){
        if(enable == SD_CD_INT_ENABLE){
            /* enable insert and remove interrupts */
            if(hndl->cd_port == SD_CD_SOCKET){  /* CD */
                _sd_set_int_mask(hndl,SD_INFO1_MASK_DET_CD,0);
            }
            else{   /* DAT3 */
                _sd_set_int_mask(hndl,SD_INFO1_MASK_DET_DAT3,0);
            }
        }
        else{   /* case SD_CD_INT_DISABLE */
            /* disable insert and remove interrupts */
            if(hndl->cd_port == SD_CD_SOCKET){  /* CD */
                _sd_clear_int_mask(hndl,SD_INFO1_MASK_DET_CD,0);
            }
            else{   /* DAT3 */
                _sd_clear_int_mask(hndl,SD_INFO1_MASK_DET_DAT3,0);
            }
        }
    }

    /* ---- register callback function ---- */
    hndl->int_cd_callback = callback;
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : check card insertion
 * Include      : 
 * Declaration  : int32_t sd_check_media(int32_t sd_port);
 * Functions    : check card insertion
 *              : if card is inserted, return SD_OK
 *              : if card is not inserted, return SD_ERR
 *              : if SD handle is not initialized, return SD_ERR
 *              : 
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : SD_OK : card is inserted
 *              : SD_ERR: card is not inserted
 * Remark       : 
 *****************************************************************************/
int32_t sd_check_media(int32_t sd_port)
{
    SDHNDL  *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    return _sd_check_media(hndl);
}

/*****************************************************************************
 * ID           :
 * Summary      : check card insertion
 * Include      : 
 * Declaration  : int32_t _sd_check_media(SDHNDL *hndl);
 * Functions    : check card insertion
 *              : if card is inserted, return SD_OK
 *              : if card is not inserted, return SD_ERR
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : card is inserted
 *              : SD_ERR: card is not inserted
 * Remark       : 
 *****************************************************************************/
int32_t _sd_check_media(SDHNDL *hndl)
{
    uint16_t reg;
    int32_t  layout;

    layout = sddev_cd_layout((int32_t)(hndl->sd_port));
    if(layout == SD_OK){
        reg = SD_INPLL(hndl,SD_INFO1);
        if(hndl->cd_port == SD_CD_SOCKET){
            reg &= (uint16_t)SD_INFO1_MASK_STATE_CD;    /* check CD level   */
#if defined(TARGET_RZ_A2M_SBEV) /* mbed */
            if ((reg & SD_INFO1_MASK_STATE_CD) == 0) {
                reg |= SD_INFO1_MASK_STATE_CD;
            } else {
                reg &= ~SD_INFO1_MASK_STATE_CD;
            }
#endif
        }
        else{
            reg &= (uint16_t)SD_INFO1_MASK_STATE_DAT3;  /* check DAT3 level */
        }
    }
    else{
        reg = SD_CD_DETECT;                             /* Always inserted */
    }

    if(reg){
        return SD_OK;   /* inserted */
    }
    
    return SD_ERR;  /* no card */
}


/* End of File */
