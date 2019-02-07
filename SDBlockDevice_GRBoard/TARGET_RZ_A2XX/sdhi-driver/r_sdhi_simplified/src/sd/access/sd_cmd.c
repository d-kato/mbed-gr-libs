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
* File Name    : sd_cmd.c
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : command issue, response receive and register check
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
/* ==== response errors ==== */
        /* for internal error detail    */
static const int32_t RespErrTbl[]={
    SD_ERR_OUT_OF_RANGE,                /* b31 : OUT_OF_RANGE                   */
    SD_ERR_ADDRESS_ERROR,               /* b30 : ADDRESS_ERROR                  */
    SD_ERR_BLOCK_LEN_ERROR,             /* b29 : BLOCK_LEN_ERROR                */
    SD_ERR_CARD_ERASE,                  /* b28 : ERASE_SEQ_ERROR                */
    SD_ERR_CARD_ERASE,                  /* b27 : ERASE_PARAM                    */
    SD_ERR_WP,                          /* b26 : WP_VIOLATION                   */
    SD_ERR_CARD_LOCK,                   /* b25 : CARD_IS_LOCKED                 */
    SD_ERR_CARD_UNLOCK,                 /* b24 : LOCK_UNLOCK_FAILED             */
    SD_ERR_HOST_CRC,                    /* b23 : COM_CRC_ERROR                  */
    SD_ERR_ILLEGAL_COMMAND,             /* b22 : ILLEGAL_COMMAND                */
    SD_ERR_CARD_ECC,                    /* b21 : CARD_ECC_FAILED                */
    SD_ERR_CARD_CC,                     /* b20 : CC_ERROR                       */
    SD_ERR_CARD_ERROR,                  /* b19 : ERROR                          */
    SD_ERR_RESERVED_ERROR18,            /* b18 : (reserved)                     */
    SD_ERR_RESERVED_ERROR17,            /* b17 : (reserved)                     */
    SD_ERR_OVERWRITE,                   /* b16 : CSD_OVERWRITE                  */
};

/* ==== SD_INFO2 errors table ==== */
static const int32_t Info2ErrTbl[]={
    SD_ERR_ILL_ACCESS,                  /* b15 : Illegal Access Error           */
    SD_OK,                              /* b14 :                                */
    SD_OK,                              /* b13 :                                */
    SD_OK,                              /* b12 :                                */
    SD_OK,                              /* b11 :                                */
    SD_OK,                              /* b10 :                                */
    SD_OK,                              /* b9  :                                */
    SD_OK,                              /* b8  :                                */
    SD_OK,                              /* b7  :                                */
    SD_ERR_RES_TOE,                     /* b6  : Response Timeout               */
    SD_ERR_ILL_READ,                    /* b5  : SD_BUF Illegal Read Access     */
    SD_ERR_ILL_WRITE,                   /* b4  : SD_BUF Illegal Write Access    */
    SD_ERR_CARD_TOE,                    /* b3  : Data Timeout                   */
    SD_ERR_END_BIT,                     /* b2  : END Error                      */
    SD_ERR_CRC,                         /* b1  : CRC Error                      */
    SD_ERR_CMD_ERROR,                   /* b0  : CMD Error                      */
};

/* ==== transfer speed table ==== */
static const uint16_t TranSpeed[8]={
    1,      // 100kbit/s
    10,     // 1Mbit/s
    100,    // 10Mbit/s
    1000,   // 100Mbit/s
    1000,   // reserved
    1000,   // reserved
    1000,   // reserved
    1000,   // reserved
};
static const uint8_t TimeValue[16]={
    0,10,12,13,15,20,25,30,35,40,45,50,55,60,70,80
};

static void _sd_get_info2(SDHNDL *hndl);

 /*****************************************************************************
 * ID           :
 * Summary      : issue SD command
 * Include      : 
 * Declaration  : int32_t _sd_send_cmd(SDHNDL *hndl, uint16_t cmd);
 * Functions    : issue SD command, hearafter wait recive response
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 *              : uint16_t cmd : command code
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : not get response and check response errors
 *****************************************************************************/
int32_t _sd_send_cmd(SDHNDL *hndl, uint16_t cmd)
{
    int32_t time;
    int32_t i;
    
    hndl->error = SD_OK;

    if(cmd == CMD38){   /* erase command */
        time = SD_TIMEOUT_ERASE_CMD;    /* extend timeout 1 sec */
    }
    else if(cmd == ACMD46){ /* ACMD46 */
        time = SD_TIMEOUT_MULTIPLE; /* same as write timeout */
    }
    else if(cmd == CMD7){
        time = SD_TIMEOUT_RESP; /* same as write timeout */
    }
    else if(cmd == CMD12){
        time = SD_TIMEOUT_RESP; /* same as write timeout */
    }
    else if(cmd == CMD43){
        time = SD_TIMEOUT_RESP;
    }
    else if(cmd == CMD44){
        time = SD_TIMEOUT_RESP;
    }
    else if(cmd == CMD45){
        time = SD_TIMEOUT_RESP;
    }
    else{
        time = SD_TIMEOUT_CMD;
    }

    /* enable resp end and illegal access interrupts */
    _sd_set_int_mask(hndl,SD_INFO1_MASK_RESP,0);

    for(i=0; i<SCLKDIVEN_LOOP_COUNT; i++){
        if(SD_INP(hndl,SD_INFO2) & SD_INFO2_MASK_SCLKDIVEN){
            break;
        }
    }
    if(i==SCLKDIVEN_LOOP_COUNT){
        _sd_set_err(hndl,SD_ERR_CBSY_ERROR);        /* treate as CBSY ERROR */
        return hndl->error;
    }

    /* ---- issue command ---- */

    SD_OUTP(hndl,SD_CMD,(uint64_t)cmd);
    
    /* ---- wait resp end ---- */
    if(sddev_int_wait(hndl->sd_port, time) != SD_OK){
        _sd_set_err(hndl,SD_ERR_HOST_TOE);
        _sd_clear_int_mask(hndl,SD_INFO1_MASK_RESP,SD_INFO2_MASK_ILA);
        return hndl->error;
    }

    /* disable resp end and illegal access interrupts */
    _sd_clear_int_mask(hndl,SD_INFO1_MASK_RESP,SD_INFO2_MASK_ILA);

    _sd_get_info2(hndl);    /* get SD_INFO2 register */

    _sd_check_info2_err(hndl);  /* check SD_INFO2 error bits */

    if(!(hndl->int_info1 & SD_INFO1_MASK_RESP)){
        _sd_set_err(hndl,SD_ERR_NO_RESP_ERROR);     /* no response */
    }

    /* ---- clear previous errors ---- */
    _sd_clear_info(hndl,SD_INFO1_MASK_RESP,SD_INFO2_MASK_ERR);

    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : issue application specific command
 * Include      : 
 * Declaration  : int32_t _sd_send_acmd(SDHNDL *hndl, uint16_t cmd,
 *              :   uint16_t h_arg, uint16_t l_arg)
 * Functions    : issue application specific command, hearafter wait recive
 *              : response
 *              : issue CMD55 preceide application specific command
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : uint16_t cmd   : command code
 *              : uint16_t h_arg : command argument high [31:16]
 *              : uint16_t l_arg : command argument low [15:0]
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_send_acmd(SDHNDL *hndl, uint16_t cmd, uint16_t h_arg,
    uint16_t l_arg)
{
    /* ---- issue CMD 55 ---- */
    _sd_set_arg(hndl,hndl->rca[0],0);
    if(_sd_send_cmd(hndl, CMD55) != SD_OK){
        return SD_ERR;
    }
    
    if(_sd_get_resp(hndl,SD_RSP_R1) != SD_OK){
        return SD_ERR;
    }
    
    /* ---- issue ACMD ---- */
    _sd_set_arg(hndl,h_arg,l_arg);
    if(_sd_send_cmd(hndl, cmd) != SD_OK){
        return SD_ERR;
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : issue multiple command
 * Include      : 
 * Declaration  : int32_t _sd_send_mcmd(SDHNDL *hndl, uint16_t cmd,
 *              :   uint32_t startaddr)
 * Functions    : issue multiple command (CMD18 or CMD25)
 *              : wait response
 *              : set read start address to startaddr
 *              : after this function finished, start data transfer
 *              : 
 * Argument     : SDHNDL *hndl       : SD handle
 *              : uint16_t cmd       : command code (CMD18 or CMD25)
 *              : uint32_t startaddr : data address (command argument)
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_send_mcmd(SDHNDL *hndl, uint16_t cmd, uint32_t startaddr)
{
    int32_t i;

    _sd_set_arg(hndl,(uint16_t)(startaddr>>16),(uint16_t)startaddr);

    for(i=0; i<SCLKDIVEN_LOOP_COUNT; i++){
        if(SD_INP(hndl,SD_INFO2) & SD_INFO2_MASK_SCLKDIVEN){
            break;
        }
    }
    if(i==SCLKDIVEN_LOOP_COUNT){
        _sd_set_err(hndl,SD_ERR_CBSY_ERROR);        /* treate as CBSY ERROR */
        return hndl->error;
    }

    /* ---- issue command ---- */
    SD_OUTP(hndl,SD_CMD,(uint64_t)cmd);

    /* ---- wait resp end ---- */
    if(sddev_int_wait(hndl->sd_port, SD_TIMEOUT_CMD) != SD_OK){
        _sd_set_err(hndl,SD_ERR_HOST_TOE);
        return hndl->error;
    }

    _sd_get_info2(hndl);    /* get SD_INFO2 register */

    _sd_check_info2_err(hndl);  /* check SD_INFO2 error bits */

    if(hndl->int_info1 & SD_INFO1_MASK_RESP){
        if(!hndl->error){
            _sd_get_resp(hndl,SD_RSP_R1);  /* check R1 resp */
        }
    }
    else{
        _sd_set_err(hndl,SD_ERR_NO_RESP_ERROR);     /* no response */
    }

    /* ---- clear previous errors ---- */
    _sd_clear_info(hndl,SD_INFO1_MASK_RESP,SD_INFO2_MASK_ERR);

    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : issue general SD command
 * Include      : 
 * Declaration  : int32_t _sd_card_send_cmd_arg(SDHNDL *hndl, uint16_t cmd, 
 *              :   int32_t resp, uint16_t h_arg, uint16_t l_arg)
 * Functions    : issue command specified cmd code
 *              : get and check response
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : uint16_t cmd   : command code (CMD18 or CMD25)
 *              : int32_t  resp  : command response
 *              : uint16_t h_arg : command argument high [31:16]
 *              : uint16_t l_arg : command argument low [15:0]
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_card_send_cmd_arg(SDHNDL *hndl, uint16_t cmd, int32_t resp,
    uint16_t h_arg, uint16_t l_arg)
{
    int32_t ret;

    _sd_set_arg(hndl,h_arg,l_arg);
    
    /* ---- issue command ---- */
    if((ret =_sd_send_cmd(hndl,cmd)) == SD_OK){
        ret = _sd_get_resp(hndl,resp);  /* get and check response */
    }
    return ret;
}

/*****************************************************************************
 * ID           :
 * Summary      : set command argument
 * Include      : 
 * Declaration  : void _sd_set_arg(SDHNDL *hndl, uint16_t h_arg,
 *              :   uint16_t l_arg)
 * Functions    : set command argument to SDHI
 *              : h_arg means higher 16bits [31:16] and  set SD_ARG0
 *              : l_arg means lower 16bits [15:0] and set SD_ARG1
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : uint16_t h_arg : command argument high [31:16]
 *              : uint16_t l_arg : command argument low [15:0]
 * Return       : none
 *              : 
 * Remark       : SD_ARG0 and SD_ARG1 are like little endian order
 *****************************************************************************/
void _sd_set_arg(SDHNDL *hndl, uint16_t h_arg, uint16_t l_arg)
{
    SD_OUTPLL(hndl,SD_ARG,l_arg);
    SD_OUTPLL(hndl,SD_ARG1,h_arg);
}

/*****************************************************************************
 * ID           :
 * Summary      : get OCR register
 * Include      : 
 * Declaration  : int32_t _sd_card_send_ocr(SDHNDL *hndl, int32_t type)
 * Functions    : get OCR register and check card operation voltage
 *              : if type is SD_MEDIA_SD, issue ACMD41
 *              : if type is SD_MEDIA_MMC, issue CMD1
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 *              : int32_t type : card type
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_card_send_ocr(SDHNDL *hndl, int32_t type)
{
    int32_t ret,i;
    int32_t j=0;

    /* ===== distinguish card type issuing ACMD41 or CMD1 ==== */
    for(i=0; i < 200; i++){
        switch(type){
        case SD_MEDIA_SD:
            if(hndl->sup_ver == SD_MODE_VER2X){
                if( hndl->sd_spec & SD_SPEC_20 ){
                    /* cmd8 have response   */  /* set HCS bit */
                    hndl->voltage |= 0x40000000;
                }
            }
            /* ---- issue ACMD41 ---- */
            ret = _sd_send_acmd(hndl,ACMD41,
                (uint16_t)(hndl->voltage>>16),
                    (uint16_t)hndl->voltage);
            break;
        
        case SD_MEDIA_MMC:  /* MMC */
            /* ---- issue CMD1 ---- */
            _sd_set_arg(hndl,(uint16_t)(hndl->voltage>>16),
                (uint16_t)hndl->voltage);
            ret = _sd_send_cmd(hndl,CMD1);
            break;

        default:
            hndl->resp_status = 0;
            /* for internal error detail    */
            /* but not need to change       */
            hndl->error = SD_ERR_INTERNAL;
            return SD_ERR;
        }
        
        if(ret == SD_OK){
            _sd_get_resp(hndl,SD_RSP_R3);  /* check R3 resp */
            /* ---- polling busy bit ---- */
            if(hndl->ocr[0] & 0x8000){  /* busy cleared */
                break;
            }
            else{
                ret = SD_ERR;   /* busy */
                sddev_int_wait(hndl->sd_port, 5);   /* add wait function because retry interval is too short */
            }
        }
        
        /* if more than 3 times response timeout occured, retry stop quick distinction to MMC */ 
        if(hndl->error == SD_ERR_RES_TOE){
            ++j;
            if( j == 3){
                break;
            }
        }
        else{
            j = 0;
        } 
    }
    return ret;
}

/*****************************************************************************
 * ID           :
 * Summary      : check R1 type response errors
 * Include      : 
 * Declaration  : int32_t _sd_check_resp_error(SDHNDL *hndl)
 * Functions    : distinguish error bit from R1 response
 *              : set the error bit to hndl->error
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : no error detected
 *              : SD_ERR: any errors detected
 * Remark       : 
 *****************************************************************************/
int32_t _sd_check_resp_error(SDHNDL *hndl)
{
    uint16_t status;
    int32_t  bit;
    

    /* SD or MMC card */
    status = (uint16_t)((hndl->resp_status >>16 ) & 0xfdffu);

    /* ---- search R1 error bit ---- */
    bit = _sd_bit_search(status);
    
    if(bit != -1){
        /* R1 resp errors bits but for AKE_SEQ_ERROR */
        _sd_set_err(hndl,RespErrTbl[bit]);
        return SD_ERR;
    }
    else if(hndl->resp_status & RES_AKE_SEQ_ERROR){
        /* authentication process sequence error */
        _sd_set_err(hndl,SD_ERR_AKE_SEQ);
        return SD_ERR;
    }
    else{
        /* NOP */
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get response and check response errors
 * Include      : 
 * Declaration  : int32_t _sd_get_resp(SDHNDL *hndl, int32_t resp)
 * Functions    : get response value from RESP register
 *              : R1, R2, R3,(R4, R5) and R6 types are available
 *              : specify response type by the argument resp
 *              : set response value to SD handle member
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 *              : int32_t resp : response type
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_get_resp(SDHNDL *hndl, int32_t resp)
{
    uint32_t status;
    uint16_t *ptr;
    
    /* select RESP register depend on the response type */
    switch(resp){
    case SD_RSP_NON:   /* no response */
        /* NOP */
        break;
    case SD_RSP_R1:    /* nomal response (32bits length) */
    case SD_RSP_R1b:   /* nomal response with an optional busy signal */
        status = SD_INPLL(hndl,SD_RSP1);
        status <<=16;
        status |= SD_INPLL(hndl,SD_RSP10);
        hndl->resp_status = status;
        
        if(status & 0xfdffe008){        /* ignore card locked status    */
            /* any status error */
            return _sd_check_resp_error(hndl);
        }
        
        break;
        
    case SD_RSP_R1_SCR:    /* nomal response with an optional busy signal */
        hndl->scr[0] = SD_INPLL(hndl,SD_RSP1);
        hndl->scr[1] = SD_INPLL(hndl,SD_RSP10);
        break;
        
    case SD_RSP_R2_CID:    /* CID register (128bits length) */
        ptr = hndl->cid;
        *ptr++ = SD_INPLL(hndl,SD_RSP7);
        *ptr++ = SD_INPLL(hndl,SD_RSP76);
        *ptr++ = SD_INPLL(hndl,SD_RSP5);
        *ptr++ = SD_INPLL(hndl,SD_RSP54);
        *ptr++ = SD_INPLL(hndl,SD_RSP3);
        *ptr++ = SD_INPLL(hndl,SD_RSP32);
        *ptr++ = SD_INPLL(hndl,SD_RSP1);
        *ptr++ = SD_INPLL(hndl,SD_RSP10);
        break;
        
    case SD_RSP_R2_CSD:    /* CSD register (128bits length) */
        ptr = hndl->csd;
        *ptr++ = SD_INPLL(hndl,SD_RSP7);
        *ptr++ = SD_INPLL(hndl,SD_RSP76);
        *ptr++ = SD_INPLL(hndl,SD_RSP5);
        *ptr++ = SD_INPLL(hndl,SD_RSP54);
        *ptr++ = SD_INPLL(hndl,SD_RSP3);
        *ptr++ = SD_INPLL(hndl,SD_RSP32);
        *ptr++ = SD_INPLL(hndl,SD_RSP1);
        *ptr++ = SD_INPLL(hndl,SD_RSP10);
        break;
        
    case SD_RSP_R3:    /* OCR register (32bits length) */
        hndl->ocr[0] = SD_INPLL(hndl,SD_RSP1);
        hndl->ocr[1] = SD_INPLL(hndl,SD_RSP10);
        break;

    case SD_RSP_R6:        /* Published RCA response (32bits length) */
        hndl->rca[0] = SD_INPLL(hndl,SD_RSP1);
        hndl->rca[1] = SD_INPLL(hndl,SD_RSP10);
        break;
        
     case SD_RSP_R7:       /* IF_COND response */
        hndl->if_cond[0] = SD_INPLL(hndl,SD_RSP1);
        hndl->if_cond[1] = SD_INPLL(hndl,SD_RSP10);
        break;
    
    default:
        /* unknown type */
        hndl->resp_status = 0;
        hndl->error = SD_ERR_INTERNAL;
        return SD_ERR;
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : check CSD register
 * Include      : 
 * Declaration  : int32_t _sd_check_csd(SDHNDL *hndl)
 * Functions    : check CSD register and get following information
 *              : Transfer Speed
 *              : Command Class
 *              : Read Block Length
 *              : Copy Bit
 *              : Write Protect Bit
 *              : File Format Group
 *              : Number of Erase Sector
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_check_csd(SDHNDL *hndl)
{
    uint32_t transpeed,timevalue;
    uint32_t erase_sector_size,erase_group_size;

    /* ---- CSD Structure ---- */
    if(hndl->media_type == SD_MEDIA_MMC){
        hndl->csd_structure = 0;
    }
    else{
        hndl->csd_structure = (uint8_t)((hndl->csd[0] & 0x00c0u) >> 6u);
        if( hndl->csd_structure == 1 ){
            if((hndl->sd_spec == SD_SPEC_10) || (hndl->sd_spec == SD_SPEC_11)){
                /* if csd_structure is ver1.00 or 1.10, sd_spec has to be phys spec ver1.00 or 1.10 */
                _sd_set_err(hndl, SD_ERR_CSD_VER);
                return SD_ERR;
            }
        }
    }

    /* ---- TAAC/NSAC ---- */
    /* no check, to be obsolete */

    /* ---- TRAN_SPEED  ---- */
    transpeed = (hndl->csd[2] & 0x0700u) >> 8u;
    timevalue = (hndl->csd[2] & 0x7800u) >> 11u;
    
    transpeed = (uint32_t)(TranSpeed[transpeed] * TimeValue[timevalue]);
    
    /* ---- set transfer speed (memory access) ---- */
    if(transpeed >= 5000){
        hndl->csd_tran_speed = SD_CLK_50MHz;
    }
    else if(transpeed >= 2500){
        hndl->csd_tran_speed = SD_CLK_25MHz;
    }
    else if(transpeed >= 2000){
        hndl->csd_tran_speed = SD_CLK_20MHz;
    }
    else if(transpeed >= 1000){
        hndl->csd_tran_speed = SD_CLK_10MHz;
    }
    else if(transpeed >= 500){
        hndl->csd_tran_speed = SD_CLK_5MHz;
    }
    else if(transpeed >= 100){
        hndl->csd_tran_speed = SD_CLK_1MHz;
    }
    else{
        hndl->csd_tran_speed = SD_CLK_400kHz;
    }
    
    /* ---- CCC  ---- */
    hndl->csd_ccc = (uint16_t)(((hndl->csd[2] & 0x00ffu) << 4u) | 
        ((hndl->csd[3] & 0xf000u) >> 12u));
    
    
    /* ---- COPY ---- */
    hndl->csd_copy = (uint8_t)(hndl->csd[7] & 0x0040u);
    
    /* ---- PERM/TMP_WRITE_PROTECT ---- */
    hndl->write_protect |= (uint8_t)((hndl->csd[7] & 0x0030u)>>3u);
    
    /* ---- FILE_FORMAT ---- */
    hndl->csd_file_format = (uint8_t)(hndl->csd[7] & 0x008cu);
    if(hndl->csd_file_format & 0x80u){
        _sd_set_err(hndl, SD_ERR_FILE_FORMAT);
        return SD_ERR;
    }
    
    /* ---- calculate the number of erase sectors ---- */
    if(hndl->media_type & SD_MEDIA_SD){
        erase_sector_size = ((hndl->csd[5] & 0x003fu) << 1u) | 
            ((hndl->csd[6] & 0x8000) >> 15);
        erase_group_size = (hndl->csd[6] & 0x7f00u) >> 8u; 
    }
    else{
        erase_sector_size = (hndl->csd[5] & 0x007cu) >> 2u;
        erase_group_size = ((hndl->csd[5] & 0x0003u) << 3u) | 
            ((hndl->csd[6] & 0xe000u) >> 13u); 
    }
    hndl->erase_sect = (erase_sector_size+1) * (erase_group_size+1);
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : check SD_INFO2 register errors
 * Include      : 
 * Declaration  : int32_t _sd_check_info2_err(SDHNDL *hndl)
 * Functions    : check error bit of SD_INFO2 register
 *              : set the error bit to hndl->error
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_check_info2_err(SDHNDL *hndl)
{
    uint16_t info2;
    int32_t  bit;
    
    info2 = (uint16_t)(hndl->int_info2 & SD_INFO2_MASK_ERR);
    
    /* ---- search error bit ---- */
    bit = _sd_bit_search(info2);

    if(bit != -1){
        _sd_set_err(hndl,Info2ErrTbl[bit]);
    }

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get SD_INFO2 register
 * Include      : 
 * Declaration  : static void _sd_get_info2(SDHNDL *hndl)
 * Functions    : get SD_INFO2 register
 *              : set the register value to hndl->int_info2
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : none
 * Remark       : 
 *****************************************************************************/
static void _sd_get_info2(SDHNDL *hndl)
{
    uint64_t info2_reg;

    info2_reg = (uint64_t)((SD_INP(hndl,SD_INFO2) & SD_INFO2_MASK_ERR));
    SD_OUTP(hndl,SD_INFO2,(uint64_t)~info2_reg);
    hndl->int_info2 = (uint64_t)(hndl->int_info2 | info2_reg);
}

/* End of File */
