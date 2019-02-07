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
* File Name    : sd_util.c
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : Function setting
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
static uint32_t next;       /* Volume ID Number */

static int32_t _sd_standby_error(SDHNDL *hndl);
static int32_t _sd_active_error(SDHNDL *hndl);
static int32_t _sd_inactive_error(SDHNDL *hndl);
static int32_t _sd_reget_reg_error(SDHNDL *hndl);
static int32_t _sd_lock_unlock_error(SDHNDL *hndl);
static int32_t _sd_set_tmpwp_error(SDHNDL *hndl);
static uint8_t _sd_calc_crc(uint8_t *data,int32_t len);

/*****************************************************************************
 * ID           :
 * Summary      : control SD clock
 * Include      : 
 * Declaration  : int32_t _sd_set_clock(SDHNDL *hndl, int32_t clock, int32_t enable)
 * Functions    : supply or halt SD clock
 *              : if enable is SD_CLOCK_ENABLE, supply SD clock
 *              : if enable is SD_CLOCK_DISKABLE, halt SD clock
 *              : 
 * Argument     : SDHNDL *hndl   : SD handle
 *              : int32_t clock  : SD clock frequency
 *              : int32_t enable : supply or halt SD clock
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_set_clock(SDHNDL *hndl, int32_t clock, int32_t enable)
{
    uint32_t div;
    int32_t  i;
    
    if(enable == SD_CLOCK_ENABLE){
        /* convert clock frequency to clock divide ratio */
        div = sddev_get_clockdiv(hndl->sd_port, clock);

        if((div > SD_DIV_512) && (div != SD_DIV_1)){
            _sd_set_err(hndl,SD_ERR_CPU_IF);
            return SD_ERR;
        }

        /* SCLKEN = 0 */
        SD_OUTP(hndl,SD_CLK_CTRL,(SD_INP(hndl,SD_CLK_CTRL)&(~SD_CLK_CTRL_SCLKEN)));
        /* write DIV[7:0] */
        SD_OUTP(hndl,SD_CLK_CTRL,((SD_INP(hndl,SD_CLK_CTRL)&(~0x00FFuL))|div));
        /* SCLKEN = 1 */
        SD_OUTP(hndl,SD_CLK_CTRL,(SD_INP(hndl,SD_CLK_CTRL)|SD_CLK_CTRL_SCLKEN));

    }
    else{
        for(i=0; i<SCLKDIVEN_LOOP_COUNT; i++){
        #ifdef USE_INFO2_CBSY
            if( (SD_INP(hndl,SD_INFO2) & SD_INFO2_MASK_CBSY) == 0 ){
                break;
            }
        #else
            if(SD_INP(hndl,SD_INFO2) & SD_INFO2_MASK_SCLKDIVEN){
                break;
            }
        #endif
        }
        if(i==SCLKDIVEN_LOOP_COUNT){
            hndl->error = SD_ERR_CBSY_ERROR;
        }
        /* SCLKEN = 0  halt */
        SD_OUTP(hndl,SD_CLK_CTRL,(SD_INP(hndl,SD_CLK_CTRL)&(~SD_CLK_CTRL_SCLKEN)));

    }
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : control data bus width
 * Include      : 
 * Declaration  : int32_t _sd_set_port(SDHNDL *hndl, int32_t port)
 * Functions    : change data bus width
 *              : if port is SD_PORT_SERIAL, set data bus width 1bit
 *              : if port is SD_PORT_PARALEL, set data bus width 4bits
 *              : change between 1bit and 4bits by ACMD6
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 *              : int32_t port : setting bus with
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : before execute this function, check card supporting bus 
 *              : width
 *              : SD memory card is 4bits support mandatory
 *****************************************************************************/
int32_t _sd_set_port(SDHNDL *hndl, int32_t port)
{
    uint64_t reg;
    uint16_t arg;

    if(hndl->media_type & SD_MEDIA_SD){    /* SD */
        /* ---- check card state ---- */
        if((hndl->resp_status & RES_STATE) == STATE_TRAN){  /* transfer state */
            if(port == SD_PORT_SERIAL){
                arg = ARG_ACMD6_1bit;
            }
            else{
                arg = ARG_ACMD6_4bit;
            }
            /* ==== change card bus width (issue ACMD6) ==== */
            if(_sd_send_acmd(hndl,ACMD6,0,arg) != SD_OK){
                return SD_ERR;
            }
            if(_sd_get_resp(hndl,SD_RSP_R1) != SD_OK){
                return SD_ERR;
            }
        }
    }
    
    /* ==== change SDHI bus width ==== */
    if(port == SD_PORT_SERIAL){ /* 1bit */
        sddev_set_port(hndl->sd_port, port);
        reg = SD_INP(hndl,SD_OPTION);
        reg |= SD_OPTION_WIDTH;
        SD_OUTP(hndl,SD_OPTION,reg);
    }
    else{   /* 4bits */
        reg = SD_INP(hndl,SD_OPTION);
        reg &= ~SD_OPTION_WIDTH_MASK;
        SD_OUTP(hndl,SD_OPTION,reg);
        sddev_set_port(hndl->sd_port, port);
    }
    
    hndl->if_mode = (uint8_t)port;
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : check hardware write protect
 * Include      : 
 * Declaration  : int32_t sd_iswp(int32_t sd_port);
 * Functions    : check hardware write protect refer to SDHI register
 *              : if WP pin is disconnected to SDHI, return value has no
 *              : meaning
 *              : 
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : hndl->write_protect : write protected state
 * Remark       : 
 *****************************************************************************/
int32_t sd_iswp(int32_t sd_port)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    return (int32_t)hndl->write_protect;
}

/*****************************************************************************
 * ID           :
 * Summary      : check hardware write protect
 * Include      : 
 * Declaration  : int32_t _sd_iswp(SDHNDL *hndl)
 * Functions    : check hardware write protect refer to SDHI register
 *              : if WP pin is disconnected to SDHI, return value has no
 *              : meaning
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_WP_OFF (0): not write protected
 *              : SD_WP_HW  (1): write protected
 * Remark       : don't check CSD write protect bits and ROM card
 *****************************************************************************/
int32_t _sd_iswp(SDHNDL *hndl)
{
    int32_t wp;
    int32_t layout;

    layout = sddev_wp_layout((int32_t)(hndl->sd_port));

    if(layout == SD_OK){
        /* ===== check SD_INFO1 WP bit ==== */
        wp = (int32_t)((~SD_INPLL(hndl, SD_INFO1) & SD_INFO1_MASK_WP) >> 7);
    }
    else{
        wp = (int32_t)SD_WP_OFF;
    }
    return wp;
}

/*****************************************************************************
 * ID           :
 * Summary      : get bit information
 * Include      : 
 * Declaration  : int32_t _sd_bit_search(uint16_t data)
 * Functions    : check every bits of argument (data) from LSB
 *              : return first bit whose value is 1'b
 *              : bit number is big endian (MSB is 0)
 * Argument     : uint16_t data : checked data
 * Return       : not less than 0 : bit number has 1'b
 *              : -1 : no bit has 1'b
 * Remark       : just 16bits value can be applied
 *****************************************************************************/
int32_t _sd_bit_search(uint16_t data)
{
    int32_t i;
    
    for(i=15;i >= 0 ; i--){
        if(data & 1u){
            return i;
        }
        data >>=1;
    }
    
    return -1;
}

/*****************************************************************************
 * ID           :
 * Summary      : set errors information
 * Include      : 
 * Declaration  : int32_t _sd_set_err(SDHNDL *hndl, int32_t error)
 * Functions    : set error information (=error) to SD Handle member
 *              : (=hndl->error)
 * Argument     : SDHNDL *hndl  : SD handle
 *              : int32_t error : setting error information
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if hndl->error was already set, no overwrite it
 *****************************************************************************/
int32_t _sd_set_err(SDHNDL *hndl, int32_t error)
{
    if(hndl->error == SD_OK){
        hndl->error = error;
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : stop operations compulsory
 * Include      : 
 * Declaration  : void sd_stop(int32_t sd_port);
 * Functions    : set flag (=SD handle member stop) stop operations compulsory
 *              : if this flag is set, read, write operations is stopped
 *              : this flag is used for card detect/removal interrupt detection
 *              : 
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : none
 * Remark       : 
 *****************************************************************************/
void sd_stop(int32_t sd_port)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return; /* not initilized */
    }
    
    hndl->stop = 1;
}

/*****************************************************************************
 * ID           :
 * Summary      : get card type
 * Include      : 
 * Declaration  : int32_t sd_get_type(int32_t sd_port, uint16_t *type, uint16_t *speed, uint8_t *capa)
 * Functions    : get mounting card type, current and supported speed mode
 *              : and capacity type
 *              : (if SD memory card)
 *              : following card types are defined
 *              : SD_MEDIA_UNKNOWN : unknown media
 *              : SD_MEDIA_MMC     : MMC card
 *              : SD_MEDIA_SD      : SD Memory card
 * Argument     : int32_t  sd_port : channel no (0 or 1)
 *              : uint16_t *type   : mounting card type
 *              : uint16_t *speed  : speed mode
 *              : uint8_t  *capa   : card capacity
 *              :   Standard capacity:0, High capacity:1
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the value isn't returned
 *              : only SD memory card, speed mode has meaning
 *****************************************************************************/
int32_t sd_get_type(int32_t sd_port, uint16_t *type, uint16_t *speed, uint8_t *capa)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(type){
        *type = hndl->media_type;
    }
    if(speed){
        *speed = hndl->speed_mode;
    }
    if(capa){
        *capa = hndl->csd_structure;
    }
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get card size
 * Include      : 
 * Declaration  : int32_t sd_get_size(int32_t sd_port, uint32_t *user, uint32_t *protect);
 * Functions    : get total sectors of user area calculated from CSD 
 *              : get total sectors of protect area calculated from CSD and 
 *              : SD STAUS
 * Argument     : int32_t sd_port   : channel no (0 or 1)
 *              : uint32_t *user    : user area size
 *              : uint32_t *protect : protect area size
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the value isn't returned
 *              : return just the number of all sectors
 *              : only SD memory card, protect area size has meaning
 *****************************************************************************/
int32_t sd_get_size(int32_t sd_port, uint32_t *user, uint32_t *protect)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(user){
        *user = hndl->card_sector_size;
    }
    if(protect){
        *protect = hndl->prot_sector_size;
    }
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get card size
 * Include      : 
 * Declaration  : int32_t _sd_get_size(SDHNDL *hndl, int32_t area)
 * Functions    : get memory card size
 * Argument     : SDHNDL *hndl : SD handle
 *              : int32_t area : memory area (bit0:user area, bit1:protect area)
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : protect area is just the number of all sectors
 *****************************************************************************/
int32_t _sd_get_size(SDHNDL *hndl, uint32_t area)
{
    uint32_t c_mult,c_size;
    uint32_t read_bl_len;

    /* ---- READ BL LEN ---- */
    read_bl_len = (hndl->csd[3] & 0x0f00u) >> 8;
    
    /* ---- C_SIZE_MULT ---- */
    c_mult = ((hndl->csd[5] & 0x0380u) >> 7);
    
    if(area & SD_PROT_AREA){
        /* calculate the number of all sectors */
        if((hndl->sup_ver == SD_MODE_VER2X) && (hndl->csd_structure == 0x01)){
            hndl->prot_sector_size = (((uint32_t)hndl->sdstatus[2] << 16u) 
                | ((uint32_t)hndl->sdstatus[3]))/512;
        }
        else{
            hndl->prot_sector_size = hndl->sdstatus[3] * 
                ((uint32_t)1 << (c_mult + 2)) * 
                    ((uint32_t)1 << read_bl_len)/512;
        }
    }
    
    if(area & SD_USER_AREA){
        if((hndl->sup_ver == SD_MODE_VER2X) && (hndl->csd_structure == 0x01)){
            c_size = ((((uint32_t)hndl->csd[4] & 0x3fffu) << 8u) |
            (((uint32_t)hndl->csd[5] & 0xff00u) >> 8u));
            /* memory capacity = C_SIZE*512K byte */
            /* sector_size = memory capacity/512 */
            hndl->card_sector_size = ((c_size +1) << 10u);
        }
        else{
            /* ---- C_SIZE ---- */
            c_size = ((hndl->csd[3] & 0x0003u) << 10) 
                | ((hndl->csd[4] & 0xffc0u) >> 6);

            /* calculate the number of all sectors */
            hndl->card_sector_size = ((uint32_t)(c_size +1) * 
                ((uint32_t)1 << (c_mult+2))*((uint32_t)1 
                    << read_bl_len)) / 512;
        }
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get SD Driver errors
 * Include      : 
 * Declaration  : int32_t sd_get_error(int32_t sd_port);
 * Functions    : get SD Driver errors (=hndl->error) and return it
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : hndl->error
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_get_error(int32_t sd_port)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : get card register
 * Include      : 
 * Declaration  : int32_t sd_get_reg(int32_t sd_port, uint8_t *ocr, uint8_t *cid, uint8_t *csd, uint8_t *dsr, uint8_t *scr);
 * Functions    : get card register value
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *ocr    : OCR register address
 *              : uint8_t *cid    : CID register address
 *              : uint8_t *csd    : CSD register address
 *              : uint8_t *dsr    : DSR register address
 *              : uint8_t *scr    : SCR register address
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_reg(int32_t sd_port, uint8_t *ocr, uint8_t *cid, uint8_t *csd,
    uint8_t *dsr, uint8_t *scr)
{
    SDHNDL   *hndl;
    uint32_t i;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(ocr){
        for(i = 0; i < 2; ++i){
            *ocr++ = (uint8_t)(hndl->ocr[i] >> 8);
            *ocr++ = (uint8_t)hndl->ocr[i];
        }
    }
    if(cid){
        for(i = 0; i < 8; ++i){
            *cid++ = (uint8_t)(hndl->cid[i] >> 8);
            *cid++ = (uint8_t)hndl->cid[i];
        }
    }
    if(csd){
        for(i = 0; i < 8; ++i){
            *csd++ = (uint8_t)(hndl->csd[i] >> 8);
            *csd++ = (uint8_t)hndl->csd[i];
        }
    }
    if(dsr){
        *dsr++ = (uint8_t)(hndl->dsr[0] >> 8);
        *dsr++ = (uint8_t)hndl->dsr[0];
    }
    if(scr){
        for(i = 0; i < 4; ++i){
            *scr++ = (uint8_t)(hndl->scr[i] >> 8);
            *scr++ = (uint8_t)hndl->scr[i];
        }
    }

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get rca register
 * Include      : 
 * Declaration  : int32_t sd_get_rca(int32_t sd_port, uint8_t *rca);
 * Functions    : get card register value
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *rca    : RCA register address
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_rca(int32_t sd_port, uint8_t *rca)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(rca){    /* return high 16bits */
        *rca++ = (uint8_t)(hndl->rca[0] >> 8);
        *rca++ = (uint8_t)hndl->rca[0];
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get SD Status
 * Include      : 
 * Declaration  : int32_t sd_get_sdstatus(int32_t sd_port, uint8_t *sdstatus);
 * Functions    : get SD Status register value
 * Argument     : int32_t sd_port   : channel no (0 or 1)
 *              : uint8_t *sdstatus : SD STATUS address
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_sdstatus(int32_t sd_port, uint8_t *sdstatus)
{
    SDHNDL   *hndl;
    uint32_t i;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(sdstatus){
        for(i = 0; i < 8; ++i){
            *sdstatus++ = (uint8_t)(hndl->sdstatus[i] >> 8);
            *sdstatus++ = (uint8_t)hndl->sdstatus[i];
        }
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get card speed
 * Include      : 
 * Declaration  : int32_t sd_get_speed(int32_t sd_port, uint8_t *clss,
 *              :  uint8_t *move);
 * Functions    : get card speed class and performance move
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *clss   : speed class
 *              : uint8_t *move   : performance move
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_speed(int32_t sd_port, uint8_t *clss, uint8_t *move)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(clss){
        *clss = hndl->speed_class;
    }

    if(move){
        *move = hndl->perform_move;
    }

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : set block count
 * Include      : 
 * Declaration  : int32_t sd_set_seccnt(int32_t sd_port, int16_t sectors);
 * Functions    : set maximum block count per multiple command
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : int16_t sectors : block count
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : maximam block count is constrained from 3 to 32767(0x7fff)
 *****************************************************************************/
int32_t sd_set_seccnt(int32_t sd_port, int16_t sectors)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }

    if(sectors <= 2){
        /* need more than 3 continuous transfer */
        return SD_ERR;  /* undefined value */
    }

    hndl->trans_sectors = sectors;

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : get block count
 * Include      : 
 * Declaration  : int32_t sd_get_seccnt(int32_t sd_port);
 * Functions    : get maximum block count per multiple command
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : not less than 0 : block count
 *              : SD_ERR: end of error
 * Remark       : maximam block count are constrained from 1 to 65535
 *****************************************************************************/
int32_t sd_get_seccnt(int32_t sd_port)
{
    SDHNDL *hndl;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }

    return (int32_t)hndl->trans_sectors;
}

/*****************************************************************************
 * ID           :
 * Summary      : get SDHI and SD Driver versions
 * Include      : 
 * Declaration  : int32_t sd_get_ver(int32_t sd_port, uint16_t *sdhi_ver, char_t *sddrv_ver);
 * Functions    : get SDHI version from VERSION register
 *              : get SD Driver version from constant DRIVER NAME
 * Argument     : int32_t sd_port    : channel no (0 or 1)
 *              : uint16_t *sdhi_ver : SDHI version
 *              : char_t *sddrv_ver  : SD Driver version
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the value isn't returned
 *****************************************************************************/
int32_t sd_get_ver(int32_t sd_port, uint16_t *sdhi_ver, char_t *sddrv_ver)
{
    SDHNDL  *hndl;
    int32_t i;
    char_t  *name = (char_t*)DRIVER_NAME;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(sdhi_ver){
        *sdhi_ver = SD_INPLL(hndl,VERSION);
    }
    
    if(sddrv_ver){
        for(i = 0;i < 32; ++i){
            *sddrv_ver++ = *name++;
        }
    }
    
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : set card detect time
 * Include      : 
 * Declaration  : int32_t sd_set_cdtime(int32_t sd_port, uint16_t cdtime);
 * Functions    : set card detect time equal to IMCLK*2^(10+cdtime)
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : uint16_t cdtime : card detect time interval
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_set_cdtime(int32_t sd_port, uint16_t cdtime)
{
    SDHNDL   *hndl;
    uint64_t reg;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }

    if(cdtime >= 0x000f){
        return SD_ERR;  /* undefined value */
    }
    
    reg = SD_INP(hndl,SD_OPTION);
    reg &= ~(uint64_t)0x000f;
    reg |= (uint64_t)(cdtime & 0x000fu);
    SD_OUTP(hndl,SD_OPTION,reg);
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : set response timeout
 * Include      : 
 * Declaration  : int32_t sd_set_responsetime(int32_t sd_port, uint16_t responsetime);
 * Functions    : set response timeout equal to IMCLK*2^(13+cdtime)
 * Argument     : int32_t sd_port       : channel no (0 or 1)
 *              : uint16_t responsetime : response timeout interval
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_set_responsetime(int32_t sd_port, uint16_t responsetime)
{
    SDHNDL   *hndl;
    uint64_t reg;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    if(responsetime >= 0x0010u){
        return SD_ERR;  /* undefined value */
    }
    
    reg = SD_INP(hndl,SD_OPTION);
    reg &= (~SD_OPTION_TOP_MASK);

    if(responsetime == 0x000fu){
        reg |= SD_OPTION_TOP_MAX;
    }
    else{
        reg |= (uint64_t)(((uint64_t)responsetime & 0x000fu) << 4);
    }
    SD_OUTP(hndl,SD_OPTION,reg);
    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : initialize SD driver work buffer
 * Include      : 
 * Declaration  : int32_t sd_set_buffer(int32_t sd_port, void *buff, uint32_t size);
 * Functions    : initialize SD driver work buffer
 *              : this buffer is used for mainly MKB process
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : void *buff      : work buffer address
 *              : uint32_t size   : work buffer size
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if applied to CPRM, allocating more than 8K bytes
 *****************************************************************************/
int32_t sd_set_buffer(int32_t sd_port, void *buff, uint32_t size)
{
    SDHNDL  *hndl;

    /* check buffer boundary (octlet unit) */
    if( ((uint32_t)buff & 0x00000007u) != 0 ){
        return SD_ERR;
    }

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    /* initialize buffer area */
    hndl->rw_buff = (uint8_t*)buff;

    /* initialize buffer size */
    hndl->buff_size = size;

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to stand-by state
 * Include      : 
 * Declaration  : int32_t sd_standby(int32_t sd_port);
 * Functions    : transfer card from transfer state to stand-by state
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_standby(int32_t sd_port)
{
    SDHNDL  *hndl;
    int32_t ret;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    hndl->error = SD_OK;

    ret = _sd_standby(hndl);

    return ret;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to stand-by state
 * Include      : 
 * Declaration  : int32_t _sd_standby(SDHNDL *hndl)
 * Functions    : transfer card from transfer state to stand-by state
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_standby(SDHNDL *hndl)
{
    int32_t  ret;
    uint16_t de_rca;
    
    /* ---- supply clock (data-transfer ratio) ---- */
    if(_sd_set_clock(hndl,(int32_t)hndl->csd_tran_speed,SD_CLOCK_ENABLE) != SD_OK){
        return _sd_standby_error(hndl);
    }

    /* set deselect RCA */
    de_rca = 0;

    /* ==== state transfer (transfer to stand-by) ==== */
    ret = _sd_card_send_cmd_arg(hndl,CMD7,SD_RSP_R1b,de_rca,0x0000);
    /* timeout error occured due to no response or response busy */
    if((ret != SD_OK) && (hndl->error != SD_ERR_RES_TOE)){
        return _sd_standby_error(hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to stand-by state error
 * Include      : 
 * Declaration  : static int32_t _sd_standby_error(SDHNDL *hndl)
 * Functions    : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_standby_error(SDHNDL *hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to transfer state
 * Include      : 
 * Declaration  : int32_t sd_active(int32_t sd_port);
 * Functions    : transfer card from stand-by state to transfer state
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_active(int32_t sd_port)
{
    SDHNDL  *hndl;
    int32_t ret;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    hndl->error = SD_OK;

    ret = _sd_active(hndl);

    return ret;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to transfer state
 * Include      : 
 * Declaration  : int32_t _sd_active(SDHNDL *hndl)
 * Functions    : transfer card from stand-by state to transfer state
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_active(SDHNDL *hndl)
{
    uint64_t reg;

    /* ---- supply clock (data-transfer ratio) ---- */
    if(_sd_set_clock(hndl,(int32_t)hndl->csd_tran_speed,SD_CLOCK_ENABLE) != SD_OK){
        return _sd_active_error(hndl);
    }
    
    if(hndl->if_mode == SD_PORT_SERIAL){    /* 1bit */
        sddev_set_port(hndl->sd_port, SD_PORT_SERIAL);
        reg = SD_INP(hndl,SD_OPTION);
        reg |= SD_OPTION_WIDTH;
        SD_OUTP(hndl,SD_OPTION,reg);
    }
    else{   /* 4bits */
        reg = SD_INP(hndl,SD_OPTION);
        reg &= ~SD_OPTION_WIDTH_MASK;
        SD_OUTP(hndl,SD_OPTION,reg);
        sddev_set_port(hndl->sd_port, SD_PORT_PARALLEL);
    }

    /* ==== state transfer (stand-by to transfer) ==== */
    if(_sd_card_send_cmd_arg(hndl,CMD7,SD_RSP_R1b,hndl->rca[0],0x0000) 
        != SD_OK){
        return _sd_active_error(hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to transfer state error
 * Include      : 
 * Declaration  : static int32_t _sd_active_error(SDHNDL *hndl)
 * Functions    : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_active_error(SDHNDL *hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to inactive state
 * Include      : 
 * Declaration  : int32_t sd_inactive(int32_t sd_port);
 * Functions    : transfer card from any state to inactive state
 * Argument     : int32_t sd_port : channel no (0 or 1)
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_inactive(int32_t sd_port)
{
    SDHNDL  *hndl;
    int32_t ret;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    hndl->error = SD_OK;

    ret = _sd_inactive(hndl);

    return ret;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to inactive state
 * Include      : 
 * Declaration  : int32_t _sd_inactive(SDHNDL *hndl)
 * Functions    : transfer card from any state to inactive state
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t _sd_inactive(SDHNDL *hndl)
{
    /* ---- supply clock (data-transfer ratio) ---- */
    if(_sd_set_clock(hndl,(int32_t)hndl->csd_tran_speed,SD_CLOCK_ENABLE) != SD_OK){
        return _sd_inactive_error(hndl);
    }

    /* ==== state transfer (transfer to stand-by) ==== */
    if(_sd_card_send_cmd_arg(hndl,CMD15,SD_RSP_NON,hndl->rca[0],0x0000) 
        != SD_OK){
        return _sd_inactive_error(hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : transfer card to inactive state error
 * Include      : 
 * Declaration  : static int32_t _sd_inactive_error(SDHNDL *hndl)
 * Functions    : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_inactive_error(SDHNDL *hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : reget register
 * Include      : 
 * Declaration  : int32_t sd_reget_reg(int32_t sd_port, uint8_t *reg, int32_t is_csd);
 * Functions    : reget CID or CSD
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *reg    : reget CID or CSD register address
 *              : int32_t is_csd  : CID(=0) or CSD(=1)
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_reget_reg(int32_t sd_port, uint8_t *reg, int32_t is_csd)
{
    SDHNDL   *hndl;
    int32_t  i;
    uint16_t *ptr;
    uint16_t cmd;

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    hndl->error = SD_OK;
    
    /* ---- transfer stand-by state ---- */
    if(_sd_standby(hndl) != SD_OK){
        return _sd_reget_reg_error(hndl);
    }

    /* verify CID or CSD */
    if(is_csd == 0){
        ptr = hndl->cid;
        cmd = CMD10;
    }
    else{
        ptr = hndl->csd;
        cmd = CMD9;
    }
    
    /* ---- supply clock (data-transfer ratio) ---- */
    if(_sd_set_clock(hndl,(int32_t)hndl->csd_tran_speed,SD_CLOCK_ENABLE) != SD_OK){
        return _sd_reget_reg_error(hndl);
    }

    /* ---- reget CID or CSD (issue CMD10 or CMD9) ---- */
    if(_sd_card_send_cmd_arg(hndl,cmd,SD_RSP_R2_CID,hndl->rca[0],0x0000) 
        != SD_OK){
        return _sd_reget_reg_error(hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);

    if(_sd_active(hndl) != SD_OK){
        return _sd_reget_reg_error(hndl);
    }

    for(i = 0; i < 8; ++i){
        *reg++ = (uint8_t)(*ptr >> 8);
        *reg++ = (uint8_t)(*ptr++);
    }

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : reget register error
 * Include      : 
 * Declaration  : static int32_t _sd_reget_reg_error(SDHNDL *hndl)
 * Functions    : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_reget_reg_error(SDHNDL *hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : lock/unlock
 * Include      : 
 * Declaration  : int32_t sd_lock_unlock(int32_t sd_port, uint8_t code, uint8_t *pwd, uint8_t len);
 * Functions    : lock/unlock operation
 *              : passward length is up to 16 bytes
 *              : case of cahnge passward, total length is 32 bytes,that is 
 *              : old and new passward maximum length
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t code    : operation code
 *              : uint8_t *pwd    : passward
 *              : uint8_t len     : passward length
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_lock_unlock(int32_t sd_port, uint8_t code, uint8_t *pwd, uint8_t len)
{
    SDHNDL   *hndl;
    uint16_t cmd_len;   /* lock/unlock data length */
    char_t   data[32];

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }

    hndl->error = SD_OK;
    
    /* ---- check mount ---- */
    if( (hndl->mount && (SD_MOUNT_LOCKED_CARD | SD_MOUNT_UNLOCKED_CARD)) == 0 ){
        _sd_set_err(hndl,SD_ERR);
        return hndl->error; /* not mounted yet */
    }

    /* check suppoted command class */
    if(!(hndl->csd_ccc & 0x0080)){  /* don't support lock/unlock */
        _sd_set_err(hndl,SD_ERR_NOTSUP_CMD);
        return SD_ERR_NOTSUP_CMD;
    }

    data[0] = (char_t)code;

    if(code & 0x08){    /* forcing erase */
        cmd_len = 1;
    }
    else{
        if(code & 0x01){    /* set passward */
            if(len > 16){
                /* total passward length is not more than 32 bytes      */
                /* but the library prohibit change password operation   */
                return SD_ERR;
            }
            if(hndl->resp_status & 0x02000000){
                 /* prohibit set passward to lock card */ 
                _sd_set_err(hndl,SD_ERR_CARD_LOCK);
                 return SD_ERR;
            }
        }
        else if(len > 16){  /* only lock or unlock */
            /* one passward length is not more than 16 bytes */
            return SD_ERR;
        }

        /* include code and total data length */
        cmd_len = (uint16_t)(len + 2);

        /* set lock/unlock command data */
        data[1] = (char_t)len;
        while(len){
            data[cmd_len-len] = *pwd++;
            len--;
        }
    }
    
    /* ---- supply clock (data-transfer ratio) ---- */
    if(_sd_set_clock(hndl,(int32_t)hndl->csd_tran_speed,SD_CLOCK_ENABLE) != SD_OK){
        return _sd_lock_unlock_error(hndl);
    }
    
    /* ---- set block length (issue CMD16) ---- */
    if(_sd_card_send_cmd_arg(hndl,CMD16,SD_RSP_R1,0x0000,cmd_len) != SD_OK){
        if(hndl->error == SD_ERR_CARD_LOCK){
            hndl->error = SD_OK;
        }
        else{
            return _sd_lock_unlock_error(hndl);
        }
    }

    if(_sd_write_byte(hndl,CMD42,0x0000,0x0000,(uint8_t*)data,cmd_len) 
        != SD_OK){
        return _sd_lock_unlock_error(hndl);
    }

    if(_sd_card_send_cmd_arg(hndl,CMD13,SD_RSP_R1,hndl->rca[0],0x0000) 
        == SD_OK){
        if((hndl->resp_status & RES_STATE) != STATE_TRAN){  /* not transfer state */
             hndl->error = SD_ERR;
            return _sd_lock_unlock_error(hndl);
        }
    }
    else{   /* SDHI error */
        return _sd_lock_unlock_error(hndl);
    }

    if( (code & SD_LOCK_CARD) == SD_UNLOCK_CARD ){
        /* ---- clear locked status ---- */
        hndl->mount &= ~SD_CARD_LOCKED;

        if( hndl->mount == SD_MOUNT_UNLOCKED_CARD ){
            /* the card is already mounted as unlock card   */

            /* ---- set block length (issue CMD16) ---- */
            if(_sd_card_send_cmd_arg(hndl,CMD16,SD_RSP_R1,0x0000,0x0200) != SD_OK){
                /* ---- set locked status ---- */
                hndl->mount |=  SD_CARD_LOCKED;
                return _sd_lock_unlock_error(hndl);
            }
        }
    }
    else{
        /* ---- set locked status ---- */
        hndl->mount |=  SD_CARD_LOCKED;
    }

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : lock/unlock error
 * Include      : 
 * Declaration  : static int32_t _sd_lock_unlock_error(SDHNDL *hndl)
 * Functions    : 
 *              : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_lock_unlock_error(SDHNDL *hndl)
{
    int32_t  temp_error;
    int32_t  loop;

    /* keep error   */
    temp_error = hndl->error;

    for( loop = 0; loop < 3; loop++ ){
        /* ---- retrive block length ---- */
        if(_sd_card_send_cmd_arg(hndl,CMD16,SD_RSP_R1,0x0000,0x0200) == SD_OK){
            break;
        }
    }

    _sd_set_err(hndl,temp_error);

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);

    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : set tempolary write protect
 * Include      : 
 * Declaration  : int32_t sd_set_tmpwp(int32_t sd_port, int32_t is_set);
 * Functions    : set temporary write protect programing csd
 * Argument     : int32_t sd_port : channel no (0 or 1)
 *              : int32_t is_set  : set or clear
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
int32_t sd_set_tmpwp(int32_t sd_port, int32_t is_set)
{
    SDHNDL   *hndl;
    int32_t  i;
    uint16_t *ptr;  /* got csd */
    uint8_t  w_csd[16];  /* work csd */
    uint8_t  crc7;   /* calculated crc7 value */

    if( (sd_port != 0) && (sd_port != 1) ){
        return SD_ERR;
    }

    hndl = _sd_get_hndls(sd_port);
    if(hndl == 0){
        return SD_ERR;  /* not initilized */
    }
    
    hndl->error = SD_OK;
    
    /* check suppoted command class */
    if(!(hndl->csd_ccc & 0x0010)){  /* don't support block write */
        _sd_set_err(hndl,SD_ERR_NOTSUP_CMD);
        return SD_ERR;
    }

    /* ---- make programing csd value ---- */
    /* set unprogramable fields */
    ptr = hndl->csd;
    for(i = 0;i < 14;i += 2){
        w_csd[i] = (uint8_t)(*ptr++);
        w_csd[i+1] = (uint8_t)((*ptr >> 8u));
    }
    
    /* set programing fields */
    w_csd[14] = (uint8_t)(*ptr);
    if(is_set == 1){    /* set write protect */
        w_csd[14] |= 0x10;
    }
    else{   /* clear write protect */
        w_csd[14] &= ~0x10;
    }

    /* calculate crc7 for CSD */
    crc7 = _sd_calc_crc(w_csd,15);

    /* set crc7 filelds */
    w_csd[15] = (uint8_t)((crc7<<1u) | 0x01);
    
    /* ---- supply clock (data-transfer ratio) ---- */
    if(_sd_set_clock(hndl,(int32_t)hndl->csd_tran_speed,SD_CLOCK_ENABLE) != SD_OK){
        return _sd_set_tmpwp_error(hndl);
    }

    if(_sd_write_byte(hndl,CMD27,0x0000,0x0000,w_csd,sizeof(w_csd)) != SD_OK){
        return _sd_set_tmpwp_error(hndl);
    }

    if(_sd_card_send_cmd_arg(hndl,CMD13,SD_RSP_R1,hndl->rca[0],0x0000) 
        == SD_OK){
        if((hndl->resp_status & RES_STATE) != STATE_TRAN){  /* not transfer state */
             hndl->error = SD_ERR;
            return _sd_set_tmpwp_error(hndl);
        }
    }
    else{   /* SDHI error */
        return _sd_set_tmpwp_error(hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);

    if(is_set == 1){    /* set write protect */
        hndl->write_protect |= (uint8_t)SD_WP_TEMP;
    }
    else{   /* clear write protect */
        hndl->write_protect &= (uint8_t)~SD_WP_TEMP;
    }

    return SD_OK;
}

/*****************************************************************************
 * ID           :
 * Summary      : set tempolary write protect error
 * Include      : 
 * Declaration  : static int32_t _sd_set_tmpwp_error(SDHNDL *hndl)
 * Functions    : 
 * Argument     : SDHNDL *hndl : SD handle
 * Return       : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : 
 *****************************************************************************/
static int32_t _sd_set_tmpwp_error(SDHNDL *hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(hndl,0,SD_CLOCK_DISABLE);
    
    return hndl->error;
}

/*****************************************************************************
 * ID           :
 * Summary      : calculate crc7
 * Include      : 
 * Declaration  : uint8_t _sd_calc_crc(uint8_t *data, int32_t len)
 * Functions    : calculate crc7 value
 * Argument     : uint8_t *data : input data
 *              : int32_t len   : input data length (byte unit)
 * Return       : calculated crc7 value 
 * Remark       : 
 *****************************************************************************/
uint8_t _sd_calc_crc(uint8_t *data, int32_t len)
{
    int32_t i,j,k;
    uint8_t p_crc[7];   /* previous crc value */
    uint8_t t_crc[7];   /* tentative crc value */
    uint8_t m_data;     /* input bit mask data */
    uint8_t crc7 = 0;   /* calculated crc7 */

    for(i = 0;i < sizeof(p_crc);i++){
        p_crc[i] = 0;
        t_crc[i] = 0;
    }

    for(i = len;i > 0;i--,data++){  /* byte loop */
        for(j = 8;j > 0;j--){   /* bit loop */
            m_data = (uint8_t)(((*data >> (j-1)) & 0x01));
            t_crc[6] = (p_crc[0] != m_data);
            t_crc[5] = p_crc[6];
            t_crc[4] = p_crc[5];
            t_crc[3] = (p_crc[4] != (p_crc[0] != m_data));
            t_crc[2] = p_crc[3];
            t_crc[1] = p_crc[2];
            t_crc[0] = p_crc[1];

            /* save tentative crc value */
            for(k = 0;k < sizeof(p_crc);k++){
                p_crc[k] = t_crc[k];
            }
        }
    }
    
    /* convert bit to byte form */
    for(i = 0;i < sizeof(p_crc);i++){
        crc7 |= (uint8_t)((p_crc[i] << (6-i)));
    }

    return crc7;
}

/*****************************************************************************
 * ID           :
 * Summary      : set memory
 * Include      : 
 * Declaration  : int32_t _sd_memset(uint8_t *p, uint8_t data, uint32_t cnt)
 * Functions    : fill memory filling data(=data) from start address(=p)
 *              : by filling size(=cnt)
 * Argument     : uint8_t *p   : start address of memory
 *              : uint8_t data : filling data
 *              : uint32_t cnt : filling size
 * Return       : 0 : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_memset(uint8_t *p, uint8_t data, uint32_t cnt)
{
    while(cnt--){
        *p++ = data;
    }
    
    return 0;
}

/*****************************************************************************
 * ID           :
 * Summary      : copy memory
 * Include      : 
 * Declaration  : static int32_t _sd_memcpy(uint8_t *dst, uint8_t *src,
 *              :   uint32_t cnt)
 * Functions    : copy data from source address(=src) to destination address
 *              : (=dst) by copy size(=cnt)
 * Argument     : uint8_t *dst : destination address
 *              : uint8_t *src : source address
 *              : uint32_t cnt : copy size
 * Return       : 0 : end of succeed
 * Remark       : 
 *****************************************************************************/
int32_t _sd_memcpy(uint8_t *dst, uint8_t *src, uint32_t cnt)
{
    while(cnt--){
        *dst++ = *src++;
    }
    
    return 0;
}

/*****************************************************************************
 * ID           :
 * Summary      : create Volume ID Number
 * Include      : 
 * Declaration  : uint16_t _sd_rand(void)
 * Functions    : get Volume ID Number
 *              : Volume ID Number is created by pseudo random number
 * Argument     : none
 * Return       : created Volume ID Number
 * Remark       : 
 *****************************************************************************/
uint16_t _sd_rand(void)
{

    next = next * 1103515245L + 12345;
    return (uint16_t)next;
    
}

/*****************************************************************************
 * ID           :
 * Summary      : set initial value of Volume ID Number
 * Include      : 
 * Declaration  : void _sd_srand(uint32_t seed)
 * Functions    : set initial value of Volume ID Number
 * Argument     : uint32_t seed : initial seting value
 * Return       : none
 * Remark       : 
 *****************************************************************************/
void _sd_srand(uint32_t seed)
{
    if(next == 0){
        next = seed;
    }
}

/*****************************************************************************
 * ID           :
 * Summary      : wait response busy
 * Include      : 
 * Declaration  : int32_t _sd_wait_rbusy(SDHNDL *hndl, int32_t time)
 * Functions    : wait response busy finished
 * Argument     : SDHNDL *hndl : SD handle
 *              : int32_t time : response busy wait interval
 * Return       : SD_OK : response busy finished
 *              : SD_ERR: response busy not finished
 * Remark       :
 *****************************************************************************/
int32_t _sd_wait_rbusy(SDHNDL *hndl, int32_t time)
{
    int32_t i;


    for(i = 0;i < time;++i){
        if(_sd_card_send_cmd_arg(hndl,CMD13,SD_RSP_R1,hndl->rca[0],0x0000) == SD_OK){
            if((hndl->resp_status & RES_STATE) == STATE_TRAN){  /* transfer state */
                return SD_OK;
            }
        }
        else{   /* SDHI error */
            break;
        }

        if(_sd_check_media(hndl) != SD_OK){
            break;
        }

        sddev_int_wait(hndl->sd_port, 1);
    }

    _sd_set_err(hndl,SD_ERR_HOST_TOE);

    return SD_ERR;

}

/* End of File */
