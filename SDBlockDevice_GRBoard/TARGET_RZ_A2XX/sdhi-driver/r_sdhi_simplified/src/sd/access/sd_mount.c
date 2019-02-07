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
* File Name    : sd_mount.c
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : Card mount
* Operation    : 
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02     Support for OS
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
static uint16_t stat_buff[NUM_PORT][64/sizeof(uint16_t)];

static int32_t _sd_mount_error(SDHNDL *hndl);
static int32_t _sd_card_init_get_rca(SDHNDL *hndl);
static int32_t _sd_mem_mount_error(SDHNDL *hndl);
static int32_t _sd_read_byte_error(SDHNDL *hndl);
static int32_t _sd_write_byte_error(SDHNDL *hndl);

/*****************************************************************************
 * ID           :
 * Summary      : mount SD card
 * Include      : 
 * Declaration  : int32_t sd_mount(int32_t sd_port, uint32_t mode, uint32_t voltage);
 * Functions    : mount SD memory card user area
 *              : can be access user area after this function is finished
 *              : without errors
 *              : turn on power
 *              : 
 *              : following is available SD Driver mode
 *              : SD_MODE_POLL     : software polling
 *              : SD_MODE_HWINT    : hardware interrupt
 *              : SD_MODE_SW       : software data transfer (SD_BUF)
 *              : SD_MODE_DMA      : DMA data transfer (SD_BUF)
 *              : SD_MODE_MEM      : only memory cards
 *              : SD_MODE_DS       : only default speed
 *              : SD_MODE_VER1X    : ver1.1 host
 *              : SD_MODE_VER2X    : ver2.x host
 * Argument     : int32_t sd_port  : channel no (0 or 1)
 *              : uint32_t mode    : SD Driver operation mode
 *              : uint32_t voltage : operation voltage
 * Return       : hndl->error      : SD handle error value
 *              : SD_OK : end of succeed
 *              : other : end of error
 * Remark       : user area should be mounted
 *****************************************************************************/
int32_t sd_mount(int32_t sd_port, uint32_t mode, uint32_t voltage)
{
    SDHNDL   *hndl;
    uint64_t info1_back;
    uint16_t sd_spec;
    uint16_t sd_spec3;

    if((sd_port != 0) && (sd_port != 1)){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }

    /* ==== check work buffer is allocated ==== */
    if(hndl->rw_buff == 0){ 
        return SD_ERR;  /* not allocated yet */
    }

    /* ==== initialize parameter ==== */
    _sd_init_hndl(hndl,mode,voltage);
    hndl->error = SD_OK;
    
    /* ==== is card inserted? ==== */
    if(_sd_check_media(hndl) != SD_OK){
        _sd_set_err(hndl,SD_ERR_NO_CARD);
        return hndl->error;     /* not inserted */
    }
    
    /* ==== power on sequence ==== */
    /* ---- turn on voltage ---- */
    if(sddev_power_on(sd_port) != SD_OK){
        _sd_set_err(hndl,SD_ERR_CPU_IF);
        return _sd_mount_error(hndl);
    }

    /* ---- set single port ---- */
    _sd_set_port(hndl,SD_PORT_SERIAL);

    /* ---- supply clock (card-identification ratio) ---- */
    if(_sd_set_clock(hndl,SD_CLK_400kHz,SD_CLOCK_ENABLE) != SD_OK){
        return hndl->error;     /* not inserted */
    }
    
    sddev_int_wait(sd_port, 2); /* add wait function  */

    sddev_loc_cpu(sd_port);
    info1_back = SD_INP(hndl,SD_INFO1);
    info1_back &= (uint64_t)0xfff8;
    SD_OUTP(hndl,SD_INFO1,(uint64_t)info1_back);
    SD_OUTP(hndl,SD_INFO2,(uint64_t)0);
    /* Clear DMA Enable because of CPU Transfer */
    SD_OUTP(hndl,CC_EXT_MODE,(uint64_t)(SD_INP(hndl,CC_EXT_MODE) & ~CC_EXT_MODE_DMASDRW));  /* disable DMA  */

    sddev_unl_cpu(sd_port);

    /* ==== initialize card and distinguish card type ==== */
    if(_sd_card_init(hndl) != SD_OK){
        return _sd_mount_error(hndl);  /* failed card initialize */
    }

    if(hndl->media_type & SD_MEDIA_MEM){    /* with memory part */
        /* ==== check card registers ==== */
        /* ---- check CSD register ---- */
        if(_sd_check_csd(hndl) != SD_OK){
            return _sd_mount_error(hndl);
        }
        
        /* ---- no check other registers (to be create) ---- */
        
        /* get user area size */
        if(_sd_get_size(hndl,SD_USER_AREA) != SD_OK){
            return _sd_mount_error(hndl);
        }
            
        /* check write protect */
        hndl->write_protect |= (uint8_t)_sd_iswp(hndl);
    }

    if(hndl->media_type & SD_MEDIA_MEM){    /* with memory part */
        if(_sd_mem_mount(hndl) != SD_OK){
            return _sd_mount_error(hndl);
        }
        if(hndl->error == SD_ERR_CARD_LOCK){
            hndl->mount = (SD_CARD_LOCKED | SD_MOUNT_LOCKED_CARD);
            /* ---- halt clock ---- */
            _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
            return SD_OK_LOCKED_CARD;
        }
    }

    /* if SD memory card, get SCR register */
    if(hndl->media_type & SD_MEDIA_SD){
        if(_sd_card_get_scr(hndl) != SD_OK){
            return _sd_mount_error(hndl);
        }
        
        if(hndl->sd_spec == SD_SPEC_20){
            sd_spec = (uint16_t)(hndl->scr[0] & SD_SPEC_REGISTER_MASK);
            sd_spec3 = (uint16_t)(hndl->scr[1] & SD_SPEC_30_REGISTER);
            if((sd_spec == SD_SPEC_20_REGISTER) && (sd_spec3 == SD_SPEC_30_REGISTER)){
                /* ---- more than phys spec ver3.00 ---- */
                hndl->sd_spec = SD_SPEC_30;
            }
            else{    /* ---- phys spec ver2.00 ---- */
                hndl->sd_spec = SD_SPEC_20;
            }
        }
        else{
            sd_spec = (uint16_t)(hndl->scr[0] & SD_SPEC_REGISTER_MASK);
            if(sd_spec == SD_SPEC_11_REGISTER){   /* ---- phys spec ver1.10 ---- */
                hndl->sd_spec = SD_SPEC_11;
            }
            else{   /* ---- phys spec ver1.00 or ver1.01 ---- */
                hndl->sd_spec = SD_SPEC_10;
            }
        }
        (void)_sd_calc_erase_sector(hndl);
    }

    /* ---- set mount flag ---- */
    hndl->mount = SD_MOUNT_UNLOCKED_CARD;

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : mount SD card error
 * Include      : 
 * Declaration  : static int32_t _sd_mount_error(SDHNDL *hndl)
 * Functions    : 
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : hndl->error : SD handle error value
 *              : SD_OK : end of succeed
 *              : other : end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_mount_error(SDHNDL *hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : initialize card
 * Include      : 
 * Declaration  : int32_t _sd_card_init(SDHNDL *hndl)
 * Functions    : initialize card from idle state to stand-by
 *              : distinguish card type (SD, MMC, IO or COMBO)
 *              : get CID, RCA, CSD from the card
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_card_init(SDHNDL *hndl)
{
    int32_t  ret;
    int32_t  i;
    uint16_t if_cond_0;
    uint16_t if_cond_1;

    hndl->media_type = SD_MEDIA_UNKNOWN;
    if_cond_0 = hndl->if_cond[0];
    if_cond_1 = hndl->if_cond[1];

    /* ==== transfer idle state (issue CMD0) ==== */
    for(i=0; i < 3; i++){
        ret = _sd_send_cmd(hndl,CMD0);
        if(ret == SD_OK){
            break;
        }
    }

    if(ret != SD_OK){
        return SD_ERR;  /* error for CMD0 */
    }

    /* clear error by reissuing CMD0 */
    hndl->error = SD_OK;

    hndl->media_type |= SD_MEDIA_SD;

    if(hndl->sup_ver == SD_MODE_VER2X){
        ret = _sd_card_send_cmd_arg(hndl,CMD8,SD_RSP_R7,if_cond_0,if_cond_1);
        if(ret == SD_OK){
            /* check R7 response */
            if(hndl->if_cond[0] & 0xf000){
                hndl->error = SD_ERR_IFCOND_VER;
                return SD_ERR;
            }
            if((hndl->if_cond[1] & 0x00ff) != 0x00aa){
                hndl->error = SD_ERR_IFCOND_ECHO;
                return SD_ERR;
            }
            hndl->sd_spec = SD_SPEC_20;         /* cmd8 have response.              */
                                                /* because of (phys spec ver2.00)   */
        }
        else{
            /* ==== clear illegal command error for CMD8 ==== */
            for(i=0; i < 3; i++){
                ret = _sd_send_cmd(hndl,CMD0);
                if(ret == SD_OK){
                    break;
                }
            }
            hndl->error = SD_OK;
            hndl->sd_spec = SD_SPEC_10;         /* cmd8 have no response.                   */
                                                /* because of (phys spec ver1.01 or 1.10)   */
        }
    }
    else{
        hndl->sd_spec = SD_SPEC_10;             /* cmd8 have response.                      */
                                                /* because of (phys spec ver1.01 or 1.10)   */
    }

    /* set OCR (issue ACMD41) */
    ret = _sd_card_send_ocr(hndl,(int32_t)hndl->media_type);

    /* clear error due to card distinction */
    hndl->error = SD_OK;
    
    if(ret != SD_OK){
        /* softreset for error clear (issue CMD0) */
        for(i=0; i < 3; i++){
            ret = _sd_send_cmd(hndl,CMD0);
            if(ret == SD_OK){
                break;
            }
        }
        if(ret != SD_OK){
            return SD_ERR;  /* error for CMD0 */
        }
        /* clear error by reissuing CMD0 */
        hndl->error = SD_OK;
        /* ---- get OCR (issue CMD1) ---- */
        if((ret = _sd_card_send_ocr(hndl,SD_MEDIA_MMC)) == SD_OK){
            /* MMC */
            hndl->media_type = SD_MEDIA_MMC;
            hndl->error = SD_OK;
        }
        else{
            /* unknown card */
            hndl->media_type = SD_MEDIA_UNKNOWN;
            _sd_set_err(hndl,SD_ERR_CARD_TYPE);
            return SD_ERR;
        }
    }

    /* ---- get CID (issue CMD2) ---- */
    if(_sd_card_send_cmd_arg(hndl,CMD2,SD_RSP_R2_CID,0,0) != SD_OK){
        return SD_ERR;
    }
    return _sd_card_init_get_rca(hndl);
}

/*****************************************************************************
 * ID           :
 * Summary      : initialize card
 * Include      : 
 * Declaration  : static int32_t _sd_card_init_get_rca(SDHNDL *hndl)
 * Functions    : 
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_card_init_get_rca(SDHNDL *hndl)
{
    int32_t  i;

    /* ---- get RCA (issue CMD3) ---- */
    if(hndl->media_type & SD_MEDIA_SD){  /* SD */
        for(i=0; i < 3; i++){
            if(_sd_card_send_cmd_arg(hndl,CMD3,SD_RSP_R6,0,0) != SD_OK){
                return SD_ERR;
            }
            if(hndl->rca[0] != 0x00){
                break;
            }
        }
        /* illegal RCA */
        if(i == 3){
            _sd_set_err(hndl,SD_ERR_CARD_CC);
            return SD_ERR;
        }
    }
    else{
        hndl->rca[0] = 1;   /* fixed 1 */
        if(_sd_card_send_cmd_arg(hndl,CMD3,SD_RSP_R1,hndl->rca[0],0x0000) 
            != SD_OK){
            return SD_ERR;
        }
    }

    /* ---- get CSD (issue CMD9) ---- */
    if(_sd_card_send_cmd_arg(hndl,CMD9,SD_RSP_R2_CSD,hndl->rca[0],0x0000) 
        != SD_OK){
        return SD_ERR;
    }

    hndl->dsr[0] = 0x0000;

    if(hndl->media_type & SD_MEDIA_MEM){
        /* is DSR implimented? */
        if(hndl->csd[3] & 0x0010u){ /* implimented */
            /* set DSR (issue CMD4) */
            hndl->dsr[0] = 0x0404;
            if(_sd_card_send_cmd_arg(hndl,CMD4,SD_RSP_NON,hndl->dsr[0],0x0000)
                != SD_OK){
                return SD_ERR;
            }
        }
    }

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : mount memory card
 * Include      : 
 * Declaration  : int32_t _sd_mem_mount(SDHNDL *hndl)
 * Functions    : mount memory part from stand-by to transfer state
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_mem_mount(SDHNDL *hndl)
{
    /* ==== data-transfer mode(Transfer State) ==== */
    if(_sd_card_send_cmd_arg(hndl,CMD7,SD_RSP_R1b,hndl->rca[0],0x0000) 
        != SD_OK){
        return _sd_mem_mount_error(hndl);
    }
    
    if((hndl->resp_status & 0x02000000)){
        _sd_set_err(hndl,SD_ERR_CARD_LOCK);
        return SD_OK;
    }
    
    /* ---- set block length (issue CMD16) ---- */
    if(_sd_card_send_cmd_arg(hndl,CMD16,SD_RSP_R1,0x0000,0x0200) != SD_OK){
        return _sd_mem_mount_error(hndl);
    }

    /* if 4bits transfer supported (SD memory card mandatory), change bus width 4bits */
    if(hndl->media_type & SD_MEDIA_SD){
        _sd_set_port(hndl,hndl->sup_if_mode);
    }
        
    /* clear pull-up DAT3 */
    if(hndl->media_type & SD_MEDIA_SD){
        if(_sd_send_acmd(hndl,ACMD42,0,0) != SD_OK){
            return _sd_mem_mount_error(hndl);
        }
        /* check R1 resp */
        if(_sd_get_resp(hndl,SD_RSP_R1) != SD_OK){
            return _sd_mem_mount_error(hndl);
        }
    }

    /* if SD memory card, get SD Status */
    if(hndl->media_type & SD_MEDIA_SD){
        if(_sd_card_get_status(hndl) != SD_OK){
            return _sd_mem_mount_error(hndl);
        }
        /* get protect area size */
        if(_sd_get_size(hndl,SD_PROT_AREA) != SD_OK){
            return _sd_mem_mount_error(hndl);
        }
    }

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : mount memory card error
 * Include      : 
 * Declaration  : static int32_t _sd_mem_mount_error(SDHNDL *hndl)
 * Functions    : 
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_mem_mount_error(SDHNDL *hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : unmount card
 * Include      : 
 * Declaration  : int32_t sd_unmount(int32_t sd_port);
 * Functions    : unmount card
 *              : turn off power
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : SD_OK : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t sd_unmount(int32_t sd_port)
{
    SDHNDL  *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initialized */
    }

    /* ---- clear mount flag ---- */
    hndl->mount = SD_UNMOUNT_CARD;

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    
    /* ---- set single port ---- */
    sddev_set_port(sd_port, SD_PORT_SERIAL);
    
    /* ---- turn off power ---- */
    if(sddev_power_off(sd_port) != SD_OK){
        _sd_set_err(hndl,SD_ERR_CPU_IF);
        return hndl->error;
    }
    
    /* ---- initilaize SD handle ---- */
    _sd_init_hndl(hndl,0,hndl->voltage);
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get SD Status
 * Include      : 
 * Declaration  : int32_t _sd_card_get_status(SDHNDL *hndl)
 * Functions    : get SD Status (issue ACMD13)
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_card_get_status(SDHNDL *hndl)
{
    int32_t  ret;
    int32_t  i;
    uint8_t  *rw_buff;

    rw_buff = (uint8_t *)&stat_buff[hndl->sd_port][0];

    /* ---- get SD Status (issue ACMD13) ---- */
    if(_sd_read_byte(hndl,ACMD13,0,0,rw_buff,SD_STATUS_BYTE) != SD_OK){
        return SD_ERR;
    }
    
    /* ---- distinguish SD ROM card ---- */
    if((rw_buff[2] & 0xffu) == 0x00){ /* [495:488] = 0x00 */
        ret = SD_OK;
        if((rw_buff[3] & 0xffu) == 0x01){
            hndl->write_protect |= SD_WP_ROM;
        }
    }
    else{
        ret = SD_ERR;
        _sd_set_err(hndl,SD_ERR_CARD_ERROR);
    }

    hndl->speed_class = rw_buff[8];
    hndl->perform_move = rw_buff[9];
    
    /* ---- save SD STATUS ---- */
    for(i = 0;i < 16/sizeof(uint16_t);i++){
        hndl->sdstatus[i] = (stat_buff[hndl->sd_port][i] << 8) | (stat_buff[hndl->sd_port][i] >> 8);
    }

    return ret;
}

/*****************************************************************************
 * ID           :
 * Summary      : get SCR register
 * Include      : 
 * Declaration  : int32_t _sd_card_get_scr(SDHNDL *hndl);
 * Functions    : get SCR register (issue ACMD51).
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_card_get_scr(SDHNDL *hndl)
{
    uint8_t  *rw_buff;

    rw_buff = (uint8_t *)&stat_buff[hndl->sd_port][0];

    /* ---- get SCR register (issue ACMD51) ---- */
    if(_sd_read_byte(hndl,ACMD51,0,0,rw_buff,SD_SCR_REGISTER_BYTE) != SD_OK){
        return SD_ERR;
    }

    /* ---- save SCR register ---- */
    hndl->scr[0] = (stat_buff[hndl->sd_port][0] << 8) | (stat_buff[hndl->sd_port][0] >> 8);
    hndl->scr[1] = (stat_buff[hndl->sd_port][1] << 8) | (stat_buff[hndl->sd_port][1] >> 8);
    hndl->scr[2] = (stat_buff[hndl->sd_port][2] << 8) | (stat_buff[hndl->sd_port][2] >> 8);
    hndl->scr[3] = (stat_buff[hndl->sd_port][3] << 8) | (stat_buff[hndl->sd_port][3] >> 8);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : read byte data from card
 * Include      : 
 * Declaration  : int32_t _sd_read_byte(SDHNDL *hndl, uint16_t cmd,
 *              :   uint16_t h_arg, uint16_t l_arg,
 *              :       uint8_t *readbuff, uint16_t byte)
 * Functions    : read byte data from card
 *              : issue byte data read command and read data from SD_BUF
 *              : using following commands
 *              : SD STATUS(ACMD13),SCR(ACMD51),NUM_WRITE_BLOCK(ACMD22),
 *              : 
 * Argument     : SDHNDL *hndl      : SD handle
 *              : uint16_t cmd      : command code
 *              : uint16_t h_arg    : command argument high [31:16]
 *              : uint16_t l_arg    : command argument low [15:0]
 *              : uint8_t *readbuff : read data buffer
 *              : uint16_t byte     : the number of read bytes
 * Return       : SD_OK : end of succeed
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_read_byte(SDHNDL *hndl, uint16_t cmd, uint16_t h_arg,
    uint16_t l_arg, uint8_t *readbuff, uint16_t byte)
{
    /* ---- disable SD_SECCNT ---- */
    SD_OUTP(hndl,SD_STOP,0x0000);

    /* ---- set transfer bytes ---- */
    SD_OUTP(hndl,SD_SIZE,(uint64_t)byte);

    /* ---- issue command ---- */
    if(cmd & 0x0040u){  /* ACMD13, ACMD22 and ACMD51 */
        if(_sd_send_acmd(hndl,cmd,h_arg,l_arg) != SD_OK){
            if((hndl->error == SD_ERR_END_BIT) ||
                (hndl->error == SD_ERR_CRC)){
                /* continue */
            }
            else{
                return _sd_read_byte_error(hndl);
            }
        }
    }
    else{
        _sd_set_arg(hndl,h_arg,l_arg);
        if(_sd_send_cmd(hndl,cmd) != SD_OK){
            return SD_ERR;
        }
    }
    /* ---- check R1 response ---- */
    if(_sd_get_resp(hndl,SD_RSP_R1) != SD_OK){
        return _sd_read_byte_error(hndl);
    }

    /* enable All end, BRE and errors */
    _sd_set_int_mask(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_BRE);

    /* ---- wait BRE interrupt ---- */
    if(sddev_int_wait(hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK){
        _sd_set_err(hndl,SD_ERR_HOST_TOE);
        return _sd_read_byte_error(hndl);
    }   

    /* ---- check errors ---- */
    if(hndl->int_info2&SD_INFO2_MASK_ERR){
        _sd_check_info2_err(hndl);
        return _sd_read_byte_error(hndl);
    }

    _sd_clear_info(hndl,0x0000,SD_INFO2_MASK_RE);   /* clear BRE bit */

    /* transfer data */
    if(sddev_read_data(hndl->sd_port, readbuff,(uint32_t)(hndl->reg_base+SD_BUF0),
        (int32_t)byte) != SD_OK){
        _sd_set_err(hndl,SD_ERR_CPU_IF);
        return _sd_read_byte_error(hndl);
    }

    /* wait All end interrupt */
    if(sddev_int_wait(hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK){
        _sd_set_err(hndl,SD_ERR_HOST_TOE);
        return _sd_read_byte_error(hndl);
    }

    /* ---- check errors ---- */
    if(hndl->int_info2&SD_INFO2_MASK_ERR){
        _sd_check_info2_err(hndl);
        return _sd_read_byte_error(hndl);
    }

    _sd_clear_info(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_ERR); /* clear All end bit */
    /* disable all interrupts */
    _sd_clear_int_mask(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_BRE);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : read byte data error
 * Include      : 
 * Declaration  : static int32_t _sd_read_byte_error(SDHNDL *hndl)
 * Functions    : 
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_read_byte_error(SDHNDL *hndl)
{
    SD_OUTP(hndl,SD_STOP,(uint64_t)0x0001);                         /* stop data transfer   */
    _sd_clear_info(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_ERR); /* clear All end bit    */
    /* disable all interrupts */
    _sd_clear_int_mask(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_BRE);

    return SD_ERR;
}

/*****************************************************************************
 * ID           :
 * Summary      : write byte data to card
 * Include      : 
 * Declaration  : int32_t _sd_write_byte(SDHNDL *hndl, uint16_t cmd,
 *              :   uint16_t h_arg, uint16_t l_arg,
 *              :       uint8_t *writebuff, uint16_t byte)
 * Functions    : write byte data to card
 *              : issue byte data write command and write data to SD_BUF
 *              : using following commands
 *              : (CMD27 and CMD42)
 *              : 
 * Argument     : SDHNDL *hndl       : SD handle
 *              : uint16_t cmd       : command code
 *              : uint16_t h_arg     : command argument high [31:16]
 *              : uint16_t l_arg     : command argument low [15:0]
 *              : uint8_t *writebuff : write data buffer
 *              : uint16_t byte      : the number of write bytes
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_write_byte(SDHNDL *hndl, uint16_t cmd, uint16_t h_arg,
    uint16_t l_arg, uint8_t *writebuff, uint16_t byte)
{
    int32_t time_out;

    /* ---- disable SD_SECCNT ---- */
    SD_OUTP(hndl,SD_STOP,0x0000);

    /* ---- set transfer bytes ---- */
    SD_OUTP(hndl,SD_SIZE,(uint64_t)byte);

    /* ---- issue command ---- */
    _sd_set_arg(hndl,h_arg,l_arg);
    if(_sd_send_cmd(hndl,cmd) != SD_OK){
        return SD_ERR;
    }
    
    /* ---- check R1 response ---- */
    if(_sd_get_resp(hndl,SD_RSP_R1) != SD_OK){
        if(hndl->error == SD_ERR_CARD_LOCK){
            hndl->error = SD_OK;
        }
        else{
            return _sd_write_byte_error(hndl);
        }
    }

    /* enable All end, BWE and errors */
    _sd_set_int_mask(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_BWE);

    /* ---- wait BWE interrupt ---- */
    if(sddev_int_wait(hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK){
        _sd_set_err(hndl,SD_ERR_HOST_TOE);
        return _sd_write_byte_error(hndl);
    }

    /* ---- check errors ---- */
    if(hndl->int_info2&SD_INFO2_MASK_ERR){
        _sd_check_info2_err(hndl);
        return _sd_write_byte_error(hndl);
    }

    _sd_clear_info(hndl,0x0000,SD_INFO2_MASK_WE);   /* clear BWE bit */

    /* transfer data */
    if(sddev_write_data(hndl->sd_port, writebuff,(uint32_t)(hndl->reg_base+SD_BUF0),
        (int32_t)byte) != SD_OK){
        _sd_set_err(hndl,SD_ERR_CPU_IF);
        return _sd_write_byte_error(hndl);
    }

    /* wait All end interrupt */
    if( (cmd == CMD42) && (byte == 1) ){
        /* force erase timeout  */
        time_out = SD_TIMEOUT_ERASE_CMD;
    }
    else{
        time_out = SD_TIMEOUT_RESP;
    }

    if(sddev_int_wait(hndl->sd_port, time_out) != SD_OK){
        _sd_set_err(hndl,SD_ERR_HOST_TOE);
        return _sd_write_byte_error(hndl);
    }

    /* ---- check errors but for timeout ---- */
    if(hndl->int_info2&SD_INFO2_MASK_ERR){
        _sd_check_info2_err(hndl);
        if( time_out == SD_TIMEOUT_ERASE_CMD ){
            /* force erase  */
            if(hndl->error == SD_ERR_CARD_TOE){
                /* force erase timeout  */
                _sd_clear_info(hndl,SD_INFO1_MASK_TRNS_RESP,SD_INFO2_MASK_ERR);
                if(_sd_wait_rbusy(hndl,10000000) != SD_OK){
                    return _sd_write_byte_error(hndl);
                }
            }
            else{
                return _sd_write_byte_error(hndl);
            }
        }
        else{
            return _sd_write_byte_error(hndl);
        }
    }

    _sd_clear_info(hndl,SD_INFO1_MASK_DATA_TRNS,0x0000);    /* clear All end bit */

    /* disable all interrupts */
    _sd_clear_int_mask(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_BWE);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : write byte data error
 * Include      : 
 * Declaration  : static int32_t _sd_write_byte_error(SDHNDL *hndl)
 * Functions    : 
 *              : 
 * Argument     : SDHNDL *hndl       : SD handle
 * Return       : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_write_byte_error(SDHNDL *hndl)
{
    SD_OUTP(hndl,SD_STOP,(uint64_t)0x0001);                 /* stop data transfer   */
    _sd_clear_info(hndl,SD_INFO1_MASK_DATA_TRNS,0x0000);    /* clear All end bit    */
    /* disable all interrupts */
    _sd_clear_int_mask(hndl,SD_INFO1_MASK_DATA_TRNS,SD_INFO2_MASK_BWE);

    return SD_ERR;
}

/*****************************************************************************
 * ID           :
 * Summary      : calculate erase sector
 * Include      : 
 * Declaration  : int32_t _sd_calc_erase_sector(SDHNDL *hndl);
 * Functions    : This function calculate erase sector for SD Phy Ver2.0.
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_calc_erase_sector(SDHNDL *hndl)
{
    uint16_t au,erase_size;
    
    if((hndl->scr[0] & 0x0f00) == 0x0200){
        /* AU is not defined,set to fixed value */
        hndl->erase_sect = SD_ERASE_SECTOR;

        /* get AU size */
        au = hndl->sdstatus[5] >> 12;

        if( (au > 0) && (au < 0x0a) ){
            /* get AU_SIZE(sectors) */
            hndl->erase_sect = (8*1024/512) << au;

            /* get ERASE_SIZE */ 
            erase_size = (hndl->sdstatus[5] << 8) | (hndl->sdstatus[6] >> 8);
            if(erase_size != 0){
                hndl->erase_sect *= erase_size;
            }
        }
        
    }
    else{
        /* If card is not Ver2.0,it use ERASE_BLK_LEN in CSD */
    }
    return SD_OK;
}
/* End of File */
