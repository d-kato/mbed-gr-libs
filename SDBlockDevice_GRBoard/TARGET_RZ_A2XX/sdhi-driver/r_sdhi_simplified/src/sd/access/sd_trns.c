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
* File Name    : sd_trns.c
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : Data transfer
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
 * Summary      : transfer data by software
 * Include      : 
 * Declaration  : int32_t _sd_software_trans(SDHNDL *hndl, uint8_t *buff,
 *              :   int32_t cnt, int32_t dir)
 * Functions    : transfer data to/from card by software
 *              : this operations are used multiple command data phase
 *              : if dir is SD_TRANS_READ, data is from card to host
 *              : if dir is SD_TRANS_WRITE, data is from host to card
 *              : 
 * Argument     : SDHNDL *hndl  : SD handle
 *              : uint8_t *buff : destination/source data buffer
 *              : int32_t cnt   : number of transfer bytes
 *              : int32_t dir   : transfer direction
 * Return       : hndl->error : SD handle error value
 *              : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer finished, check CMD12 sequence refer to All end
 *****************************************************************************/
int32_t _sd_software_trans(SDHNDL *hndl, uint8_t *buff, int32_t cnt, int32_t dir)
{
    int32_t j;
    int32_t (*func)(int32_t, uint8_t *, uint32_t, int32_t);
    
    if(dir == SD_TRANS_READ){
        func = sddev_read_data;
    }
    else{
        func = sddev_write_data;
    }
    
    for(j=cnt; j>0 ;j--){
        /* ---- wait BWE/BRE interrupt ---- */
        if(sddev_int_wait(hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK){
            _sd_set_err(hndl,SD_ERR_HOST_TOE);
            break;
        }   

        if(hndl->int_info2&SD_INFO2_MASK_ERR){
            _sd_check_info2_err(hndl);
            break;
        }

        if(dir == SD_TRANS_READ){
            _sd_clear_info(hndl,0x0000,SD_INFO2_MASK_RE);   /* clear BRE and errors bit */
        }
        else{
            _sd_clear_info(hndl,0x0000,SD_INFO2_MASK_WE);   /* clear BWE and errors bit */
        }
    
        /* write/read to/from SD_BUF by 1 sector */
        if((*func)(hndl->sd_port, buff,(uint32_t)hndl->reg_base+SD_BUF0,512) != SD_OK){
            _sd_set_err(hndl,SD_ERR_CPU_IF);
            break;
        }
        
        /* update buffer */
        buff+=512;
    
    }

    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer data by DMA
 * Include      : 
 * Declaration  : int32_t _sd_dma_trans(SDHNDL *hndl, int32_t cnt)
 * Functions    : transfer data to/from card by DMA
 *              : this operations are multiple command data phase
 *              :
 * Argument     : SDHNDL *hndl : SD handle
 *              : int32_t cnt  : number of transfer bytes
 * Return       : hndl->error : SD handle error value
 *              : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer finished, check CMD12 sequence refer to All end
 *****************************************************************************/
int32_t _sd_dma_trans(SDHNDL *hndl, int32_t cnt)
{
    /* ---- check DMA transfer end  --- */
    /* timeout value is depend on transfer size */
    if(sddev_wait_dma_end((int32_t)(hndl->sd_port), cnt*512) != SD_OK){
        (void)sddev_disable_dma((int32_t)(hndl->sd_port));   /* disable DMAC */
        (void)_sd_set_err(hndl,SD_ERR_CPU_IF);
        return hndl->error;
    }
    
    /* ---- disable DMAC ---- */
    if(sddev_disable_dma((int32_t)(hndl->sd_port)) != SD_OK){
        (void)_sd_set_err(hndl,SD_ERR_CPU_IF);
        return hndl->error;
    }
    return hndl->error;
}

/* End of File */
