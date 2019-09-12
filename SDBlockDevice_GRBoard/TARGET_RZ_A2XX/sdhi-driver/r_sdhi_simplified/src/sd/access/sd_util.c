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
* File Name    : sd_util.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Function setting
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
static uint32_t s_next;       /* Volume ID Number */

static int32_t _sd_standby_error(st_sdhndl_t *p_hndl);
static int32_t _sd_active_error(st_sdhndl_t *p_hndl);
static int32_t _sd_inactive_error(st_sdhndl_t *p_hndl);
static int32_t _sd_reget_reg_error(st_sdhndl_t *p_hndl);
static int32_t _sd_lock_unlock_error(st_sdhndl_t *p_hndl);
static int32_t _sd_set_tmpwp_error(st_sdhndl_t *p_hndl);
static uint8_t _sd_calc_crc(uint8_t *data, int32_t len);

/******************************************************************************
 * Function Name: _sd_set_clock
 * Description  : control SD clock.
 *              : supply or halt SD clock
 *              : if enable is SD_CLOCK_ENABLE, supply SD clock
 *              : if enable is SD_CLOCK_DISKABLE, halt SD clock
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t clock       : SD clock frequency
 *              : int32_t enable      : supply or halt SD clock
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_set_clock(st_sdhndl_t *p_hndl, int32_t clock, int32_t enable)
{
    uint32_t div;
    int32_t  i;

    if (SD_CLOCK_ENABLE == enable)
    {
        /* convert clock frequency to clock divide ratio */
        div = sddev_get_clockdiv(p_hndl->sd_port, clock);

        if ((div > SD_DIV_512) && (SD_DIV_1 != div))
        {
            _sd_set_err(p_hndl, SD_ERR_CPU_IF);
            return SD_ERR;
        }

        /* SCLKEN = 0 */
        SD_OUTP(p_hndl, SD_CLK_CTRL, (SD_INP(p_hndl, SD_CLK_CTRL) & (~SD_CLK_CTRL_SCLKEN)));

        /* write DIV[7:0] */
        SD_OUTP(p_hndl, SD_CLK_CTRL, ((SD_INP(p_hndl, SD_CLK_CTRL) & (~0x00FFuL)) | div));

        /* SCLKEN = 1 */
        SD_OUTP(p_hndl, SD_CLK_CTRL, (SD_INP(p_hndl, SD_CLK_CTRL) | SD_CLK_CTRL_SCLKEN));

    }
    else
    {
        for (i = 0; i < SCLKDIVEN_LOOP_COUNT; i++)
        {
#ifdef USE_INFO2_CBSY
            /* Cast to an appropriate type */
            if ( (SD_INP(p_hndl, SD_INFO2) & SD_INFO2_MASK_CBSY) == 0 )
            {
                break;
            }
#else
            /* Cast to an appropriate type */
            if (SD_INP(p_hndl, SD_INFO2) & SD_INFO2_MASK_SCLKDIVEN)
            {
                break;
            }
#endif
        }
        if (SCLKDIVEN_LOOP_COUNT == i)
        {
            p_hndl->error = SD_ERR_CBSY_ERROR;
        }

        /* SCLKEN = 0  halt */
        SD_OUTP(p_hndl, SD_CLK_CTRL, (SD_INP(p_hndl, SD_CLK_CTRL) & (~SD_CLK_CTRL_SCLKEN)));

    }
    return SD_OK;
}
/******************************************************************************
 End of function _sd_set_clock
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_set_port
 * Description  : control data bus width.
 *              : change data bus width
 *              : if port is SD_PORT_SERIAL, set data bus width 1bit
 *              : if port is SD_PORT_PARALEL, set data bus width 4bits
 *              : change between 1bit and 4bits by ACMD6
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t port        : setting bus with
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : before execute this function, check card supporting bus
 *              : width
 *              : SD memory card is 4bits support mandatory
 *****************************************************************************/
int32_t _sd_set_port(st_sdhndl_t *p_hndl, int32_t port)
{
    uint64_t reg;
    uint16_t arg;

    if (p_hndl->media_type & SD_MEDIA_SD)    /* SD */
    {
        /* ---- check card state ---- */
        if ((p_hndl->resp_status & RES_STATE) == STATE_TRAN)  /* transfer state */
        {
            if (SD_PORT_SERIAL == port)
            {
                arg = ARG_ACMD6_1BIT;
            }
            else
            {
                arg = ARG_ACMD6_4BIT;
            }

            /* ==== change card bus width (issue ACMD6) ==== */
            if (_sd_send_acmd(p_hndl, ACMD6, 0, arg) != SD_OK)
            {
                return SD_ERR;
            }
            if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
            {
                return SD_ERR;
            }
        }
    }

    /* ==== change SDHI bus width ==== */
    if (SD_PORT_SERIAL == port) /* 1bit */
    {
        sddev_set_port(p_hndl->sd_port, port);

        /* Cast to an appropriate type */
        reg = SD_INP(p_hndl, SD_OPTION);

        /* Cast to an appropriate type */
        reg |= SD_OPTION_WIDTH;

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_OPTION, reg);
    }
    else    /* 4bits */
    {
        /* Cast to an appropriate type */
        reg = SD_INP(p_hndl, SD_OPTION);

        /* Cast to an appropriate type */
        reg &= (~SD_OPTION_WIDTH_MASK);

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_OPTION, reg);
        sddev_set_port(p_hndl->sd_port, port);
    }

    /* Cast to an appropriate type */
    p_hndl->if_mode = (uint8_t)port;

    return SD_OK;
}
/******************************************************************************
 End of function _sd_set_port
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_iswp
 * Description  : check hardware write protect.
 *              : check hardware write protect refer to SDHI register
 *              : if WP pin is disconnected to SDHI, return value has no
 *              : meaning
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : p_hndl->write_protect : write protected state
 *****************************************************************************/
int32_t sd_iswp(int32_t sd_port)
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

    /* Cast to an appropriate type */
    return (int32_t)p_hndl->write_protect;
}
/******************************************************************************
 End of function sd_iswp
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_iswp
 * Description  : check hardware write protect refer to SDHI register
 *              : if WP pin is disconnected to SDHI, return value has no
 *              : meaning
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_WP_OFF (0): not write protected
 *              : SD_WP_HW  (1): write protected
 * Remark       : don't check CSD write protect bits and ROM card
 *****************************************************************************/
int32_t _sd_iswp(st_sdhndl_t *p_hndl)
{
    int32_t wp;
    int32_t layout;

    /* Cast to an appropriate type */
    layout = sddev_wp_layout((int32_t)(p_hndl->sd_port));

    if (SD_OK == layout)
    {
        /* ===== check SD_INFO1 WP bit ==== */
        wp = (int32_t)(((~SD_INPLL(p_hndl, SD_INFO1)) & SD_INFO1_MASK_WP) >> 7);
    }
    else
    {
        /* Cast to an appropriate type */
        wp = (int32_t)SD_WP_OFF;
    }
    return wp;
}
/******************************************************************************
 End of function _sd_iswp
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_bit_search
 * Description  : get bit information.
 *              : check every bits of argument (data) from LSB
 *              : return first bit whose value is 1'b
 *              : bit number is big endian (MSB is 0)
 * Arguments    : uint16_t data : checked data
 * Return Value : not less than 0 : bit number has 1'b
 *              : -1 : no bit has 1'b
 * Remark       : just 16bits value can be applied
 *****************************************************************************/
int32_t _sd_bit_search(uint16_t data)
{
    int32_t i;

    for (i = 15; i >= 0 ; i--)
    {
        if (data & 1u)
        {
            return i;
        }
        data >>= 1;
    }

    return -1;
}
/******************************************************************************
 End of function _sd_bit_search
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_set_err
 * Description  : set errors information.
 *              : set error information (=error) to SD Handle member
 *              : (=p_hndl->error)
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t error       : setting error information
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if p_hndl->error was already set, no overwrite it
 *****************************************************************************/
int32_t _sd_set_err(st_sdhndl_t *p_hndl, int32_t error)
{
    if (SD_OK == p_hndl->error)
    {
        p_hndl->error = error;
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_set_err
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_stop
 * Description  : stop operations compulsory.
 *              : set flag (=SD handle member stop) stop operations compulsory
 *              : if this flag is set, read, write operations is stopped
 *              : this flag is used for card detect/removal interrupt detection
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : none
 *****************************************************************************/
void sd_stop(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return; /* not initilized */
    }

    p_hndl->stop = 1;
}
/******************************************************************************
 End of function sd_stop
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_type
 * Description  : get card type.
 *              : get mounting card type, current and supported speed mode
 *              : and capacity type
 *              : (if SD memory card)
 *              : following card types are defined
 *              : SD_MEDIA_UNKNOWN : unknown media
 *              : SD_MEDIA_MMC     : MMC card
 *              : SD_MEDIA_SD      : SD Memory card
 * Arguments    : int32_t  sd_port : channel no (0 or 1)
 *              : uint16_t *type   : mounting card type
 *              : uint16_t *speed  : speed mode
 *              : uint8_t  *capa   : card capacity
 *              :   Standard capacity:0, High capacity:1
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the value isn't returned
 *              : only SD memory card, speed mode has meaning
 *****************************************************************************/
int32_t sd_get_type(int32_t sd_port, uint16_t *type, uint16_t *speed, uint8_t *capa)
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

    if (type)
    {
        *type = p_hndl->media_type;
    }
    if (speed)
    {
        *speed = p_hndl->speed_mode;
    }
    if (capa)
    {
        *capa = p_hndl->csd_structure;
    }
    return SD_OK;
}
/******************************************************************************
 End of function sd_get_type
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_size
 * Description  : get card size.
 *              : get total sectors of user area calculated from CSD
 *              : get total sectors of protect area calculated from CSD and
 *              : SD STAUS
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *              : uint32_t *user    : user area size
 *              : uint32_t *protect : protect area size
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the value isn't returned
 *              : return just the number of all sectors
 *              : only SD memory card, protect area size has meaning
 *****************************************************************************/
int32_t sd_get_size(int32_t sd_port, uint32_t *user, uint32_t *protect)
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

    if (user)
    {
        *user = p_hndl->card_sector_size;
    }
    if (protect)
    {
        *protect = p_hndl->prot_sector_size;
    }
    return SD_OK;
}
/******************************************************************************
 End of function sd_get_size
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_get_size
 * Description  : get card size.
 *              : get memory card size
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t area   : memory area (bit0:user area, bit1:protect area)
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : protect area is just the number of all sectors
 *****************************************************************************/
int32_t _sd_get_size(st_sdhndl_t *p_hndl, uint32_t area)
{
    uint32_t c_mult;
    uint32_t c_size;
    uint32_t read_bl_len;

    /* ---- READ BL LEN ---- */
    read_bl_len = (p_hndl->csd[3] & 0x0f00u) >> 8;

    /* ---- C_SIZE_MULT ---- */
    c_mult = ((p_hndl->csd[5] & 0x0380u) >> 7);

    if (area & SD_PROT_AREA)
    {
        /* calculate the number of all sectors */
        if ((SD_MODE_VER2X == p_hndl->sup_ver) && (0x01 == p_hndl->csd_structure))
        {
            /* Cast to an appropriate type */
            p_hndl->prot_sector_size = (((uint32_t)p_hndl->sdstatus[2] << 16u) |

                                        /* Cast to an appropriate type */
                                        ((uint32_t)p_hndl->sdstatus[3])) / 512;
        }
        else
        {
            /* Cast to an appropriate type */
            p_hndl->prot_sector_size = (p_hndl->sdstatus[3] *

                                        /* Cast to an appropriate type */
                                        ((uint32_t)1 << (c_mult + 2)) *

                                        /* Cast to an appropriate type */
                                        ((uint32_t)1 << read_bl_len)) / 512;
        }
    }

    if (area & SD_USER_AREA)
    {
        if ((SD_MODE_VER2X == p_hndl->sup_ver) && (0x01 == p_hndl->csd_structure))
        {
            /* Cast to an appropriate type */
            c_size = ((((uint32_t)p_hndl->csd[4] & 0x3fffu) << 8u) |

                    /* Cast to an appropriate type */
                    (((uint32_t)p_hndl->csd[5] & 0xff00u) >> 8u));

            /* memory capacity = C_SIZE*512K byte */
            /* sector_size = memory capacity/512 */
            p_hndl->card_sector_size = ((c_size + 1) << 10u);
        }
        else
        {
            /* ---- C_SIZE ---- */
            c_size = ((p_hndl->csd[3] & 0x0003u) << 10) |
                        ((p_hndl->csd[4] & 0xffc0u) >> 6);

            /* calculate the number of all sectors */
            p_hndl->card_sector_size = ((uint32_t)(c_size + 1) *

                                    /* Cast to an appropriate type */
                                    ((uint32_t)1 << (c_mult + 2)) * ((uint32_t)1
                                        << read_bl_len)) / 512;
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_get_size
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_error
 * Description  : get SD Driver errors (=p_hndl->error) and return it
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : p_hndl->error
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_get_error(int32_t sd_port)
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
    return p_hndl->error;
}
/******************************************************************************
 End of function sd_get_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_reg
 * Description  : get card register.
 *              : get card register value
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *ocr    : OCR register address
 *              : uint8_t *cid    : CID register address
 *              : uint8_t *csd    : CSD register address
 *              : uint8_t *dsr    : DSR register address
 *              : uint8_t *scr    : SCR register address
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_reg(int32_t sd_port, uint8_t *ocr, uint8_t *cid, uint8_t *csd,
                    uint8_t *dsr, uint8_t *scr)
{
    st_sdhndl_t *p_hndl;
    uint32_t    i;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    if (ocr)
    {
        for (i = 0; i < 2; ++i)
        {
            /* Cast to an appropriate type */
            *ocr++ = (uint8_t)(p_hndl->ocr[i] >> 8);

            /* Cast to an appropriate type */
            *ocr++ = (uint8_t)p_hndl->ocr[i];
        }
    }
    if (cid)
    {
        for (i = 0; i < 8; ++i)
        {
            /* Cast to an appropriate type */
            *cid++ = (uint8_t)(p_hndl->cid[i] >> 8);

            /* Cast to an appropriate type */
            *cid++ = (uint8_t)p_hndl->cid[i];
        }
    }
    if (csd)
    {
        for (i = 0; i < 8; ++i)
        {
            /* Cast to an appropriate type */
            *csd++ = (uint8_t)(p_hndl->csd[i] >> 8);

            /* Cast to an appropriate type */
            *csd++ = (uint8_t)p_hndl->csd[i];
        }
    }
    if (dsr)
    {
        /* Cast to an appropriate type */
        *dsr++ = (uint8_t)(p_hndl->dsr[0] >> 8);

        /* Cast to an appropriate type */
        *dsr++ = (uint8_t)p_hndl->dsr[0];
    }
    if (scr)
    {
        for (i = 0; i < 4; ++i)
        {
            /* Cast to an appropriate type */
            *scr++ = (uint8_t)(p_hndl->scr[i] >> 8);

            /* Cast to an appropriate type */
            *scr++ = (uint8_t)p_hndl->scr[i];
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function sd_get_reg
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_rca
 * Description  : get rca register.
 *              : get card register value
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *rca    : RCA register address
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_rca(int32_t sd_port, uint8_t *rca)
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

    if (rca)    /* return high 16bits */
    {
        /* Cast to an appropriate type */
        *rca++ = (uint8_t)(p_hndl->rca[0] >> 8);

        /* Cast to an appropriate type */
        *rca++ = (uint8_t)p_hndl->rca[0];
    }

    return SD_OK;
}
/******************************************************************************
 End of function sd_get_rca
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_sdstatus
 * Description  : get SD Status register value
 * Arguments    : int32_t sd_port   : channel no (0 or 1)
 *              : uint8_t *sdstatus : SD STATUS address
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_sdstatus(int32_t sd_port, uint8_t *sdstatus)
{
    st_sdhndl_t *p_hndl;
    uint32_t    i;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    if (sdstatus)
    {
        for (i = 0; i < 8; ++i)
        {
            /* Cast to an appropriate type */
            *sdstatus++ = (uint8_t)(p_hndl->sdstatus[i] >> 8);

            /* Cast to an appropriate type */
            *sdstatus++ = (uint8_t)p_hndl->sdstatus[i];
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function sd_get_sdstatus
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_speed
 * Description  : get card speed.
 *              : get card speed class and performance move
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *clss   : speed class
 *              : uint8_t *move   : performance move
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the register value isn't returned
 *****************************************************************************/
int32_t sd_get_speed(int32_t sd_port, uint8_t *clss, uint8_t *move)
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

    if (clss)
    {
        *clss = p_hndl->speed_class;
    }

    if (move)
    {
        *move = p_hndl->perform_move;
    }

    return SD_OK;
}
/******************************************************************************
 End of function sd_get_speed
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_set_seccnt
 * Description  : set maximum block count per multiple command
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : int16_t sectors : block count
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : maximam block count is constrained from 3 to 32767(0x7fff)
 *****************************************************************************/
int32_t sd_set_seccnt(int32_t sd_port, int16_t sectors)
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

    if (sectors <= 2)
    {
        /* need more than 3 continuous transfer */
        return SD_ERR;  /* undefined value */
    }

    p_hndl->trans_sectors = sectors;

    return SD_OK;
}
/******************************************************************************
 End of function sd_set_seccnt
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_seccnt
 * Description  : get block count.
 *              : get maximum block count per multiple command
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : not less than 0 : block count
 *              : SD_ERR: end of error
 * Remark       : maximam block count are constrained from 1 to 65535
 *****************************************************************************/
int32_t sd_get_seccnt(int32_t sd_port)
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

    /* Cast to an appropriate type */
    return (int32_t)p_hndl->trans_sectors;
}
/******************************************************************************
 End of function sd_get_seccnt
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_get_ver
 * Description  : get SDHI and SD Driver versions.
 *              : get SDHI version from VERSION register
 *              : get SD Driver version from constant DRIVER NAME
 * Arguments    : int32_t sd_port    : channel no (0 or 1)
 *              : uint16_t *sdhi_ver : SDHI version
 *              : char_t *sddrv_ver  : SD Driver version
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if pointer has NULL ,the value isn't returned
 *****************************************************************************/
int32_t sd_get_ver(int32_t sd_port, uint16_t *sdhi_ver, char_t *sddrv_ver)
{
    st_sdhndl_t *p_hndl;
    int32_t     i;

    /* Cast to an appropriate type */
    char_t  *p_name = (char_t*)DRIVER_NAME;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    if (sdhi_ver)
    {
        /* Cast to an appropriate type */
        *sdhi_ver = SD_INPLL(p_hndl, VERSION);
    }

    if (sddrv_ver)
    {
        for (i = 0; i < 32; ++i)
        {
            *sddrv_ver++ = *p_name++;
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function sd_get_ver
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_set_cdtime
 * Description  : set card detect time.
 *              : set card detect time equal to IMCLK*2^(10+cdtime)
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint16_t cdtime : card detect time interval
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_set_cdtime(int32_t sd_port, uint16_t cdtime)
{
    st_sdhndl_t *p_hndl;
    uint64_t    reg;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    if (cdtime >= 0x000f)
    {
        return SD_ERR;  /* undefined value */
    }

    /* Cast to an appropriate type */
    reg = SD_INP(p_hndl, SD_OPTION);

    /* Cast to an appropriate type */
    reg &= (~(uint64_t)0x000f);

    /* Cast to an appropriate type */
    reg |= (uint64_t)(cdtime & 0x000fu);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_OPTION, reg);
    return SD_OK;
}
/******************************************************************************
 End of function sd_set_cdtime
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_set_responsetime
 * Description  : set response timeout.
 *              : set response timeout equal to IMCLK*2^(13+cdtime)
 * Arguments    : int32_t sd_port       : channel no (0 or 1)
 *              : uint16_t responsetime : response timeout interval
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_set_responsetime(int32_t sd_port, uint16_t responsetime)
{
    st_sdhndl_t *p_hndl;
    uint64_t    reg;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    if (responsetime >= 0x0010u)
    {
        return SD_ERR;  /* undefined value */
    }

    /* Cast to an appropriate type */
    reg = SD_INP(p_hndl, SD_OPTION);

    /* Cast to an appropriate type */
    reg &= (~SD_OPTION_TOP_MASK);

    if (0x000fu == responsetime)
    {
        /* Cast to an appropriate type */
        reg |= SD_OPTION_TOP_MAX;
    }
    else
    {
        /* Cast to an appropriate type */
        reg |= (uint64_t)(((uint64_t)responsetime & 0x000fu) << 4);
    }

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_OPTION, reg);
    return SD_OK;
}
/******************************************************************************
 End of function sd_set_responsetime
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_set_buffer
 * Description  : initialize SD driver work buffer.
 *              : this buffer is used for mainly MKB process
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : void *buff      : work buffer address
 *              : uint32_t size   : work buffer size
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : if applied to CPRM, allocating more than 8K bytes
 *****************************************************************************/
int32_t sd_set_buffer(int32_t sd_port, void *buff, uint32_t size)
{
    st_sdhndl_t  *p_hndl;

    /* check buffer boundary (octlet unit) */
    if ( 0 != ((uint32_t)buff & 0x00000007u) )
    {
        return SD_ERR;
    }

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* initialize buffer area */
    p_hndl->p_rw_buff = (uint8_t*)buff;

    /* initialize buffer size */
    p_hndl->buff_size = size;

    return SD_OK;
}
/******************************************************************************
 End of function sd_set_buffer
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_standby
 * Description  : transfer card to stand-by state.
 *              : transfer card from transfer state to stand-by state
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_standby(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;
    int32_t     ret;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }
    p_hndl->error = SD_OK;

    ret = _sd_standby(p_hndl);

    return ret;
}
/******************************************************************************
 End of function sd_standby
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_standby
 * Description  : transfer card to stand-by state.
 *              : transfer card from transfer state to stand-by state
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_standby(st_sdhndl_t *p_hndl)
{
    int32_t  ret;
    uint16_t de_rca;

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sd_standby_error(p_hndl);
    }

    /* set deselect RCA */
    de_rca = 0;

    /* ==== state transfer (transfer to stand-by) ==== */
    ret = _sd_card_send_cmd_arg(p_hndl, CMD7, SD_RSP_R1B, de_rca, 0x0000);

    /* timeout error occured due to no response or response busy */
    if ((SD_OK != ret) && (SD_ERR_RES_TOE != p_hndl->error))
    {
        return _sd_standby_error(p_hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_standby
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_standby_error
 * Description  : transfer card to stand-by state error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_standby_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_standby_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_active
 * Description  : transfer card to transfer state.
 *              : transfer card from stand-by state to transfer state
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_active(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;
    int32_t     ret;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }
    p_hndl->error = SD_OK;

    ret = _sd_active(p_hndl);

    return ret;
}
/******************************************************************************
 End of function sd_active
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_active
 * Description  : transfer card to transfer state.
 *              : transfer card from stand-by state to transfer state
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_active(st_sdhndl_t *p_hndl)
{
    uint64_t reg;

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sd_active_error(p_hndl);
    }

    if (SD_PORT_SERIAL == p_hndl->if_mode)    /* 1bit */
    {
        sddev_set_port(p_hndl->sd_port, SD_PORT_SERIAL);

        /* Cast to an appropriate type */
        reg = SD_INP(p_hndl, SD_OPTION);

        /* Cast to an appropriate type */
        reg |= SD_OPTION_WIDTH;

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_OPTION, reg);
    }
    else    /* 4bits */
    {

        /* Cast to an appropriate type */
        reg = SD_INP(p_hndl, SD_OPTION);

        /* Cast to an appropriate type */
        reg &= (~SD_OPTION_WIDTH_MASK);

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_OPTION, reg);
        sddev_set_port(p_hndl->sd_port, SD_PORT_PARALLEL);
    }

    /* ==== state transfer (stand-by to transfer) ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD7, SD_RSP_R1B, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        return _sd_active_error(p_hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_active
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_active_error
 * Description  : transfer card to transfer state error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_active_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_active_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_inactive
 * Description  : transfer card to inactive state.
 *              : transfer card from any state to inactive state
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_inactive(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;
    int32_t     ret;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }
    p_hndl->error = SD_OK;

    ret = _sd_inactive(p_hndl);

    return ret;
}
/******************************************************************************
 End of function sd_inactive
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_inactive
 * Description  : transfer card to inactive state.
 *              : transfer card from any state to inactive state
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_inactive(st_sdhndl_t *p_hndl)
{
    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sd_inactive_error(p_hndl);
    }

    /* ==== state transfer (transfer to stand-by) ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD15, SD_RSP_NON, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        return _sd_inactive_error(p_hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_inactive
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_inactive_error
 * Description  : transfer card to inactive state error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_inactive_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_inactive_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_reget_reg
 * Description  : reget register.
 *              : reget CID or CSD
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *reg    : reget CID or CSD register address
 *              : int32_t is_csd  : CID(=0) or CSD(=1)
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_reget_reg(int32_t sd_port, uint8_t *reg, int32_t is_csd)
{
    st_sdhndl_t *p_hndl;
    int32_t     i;
    uint16_t    *p_ptr;
    uint16_t    cmd;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->error = SD_OK;

    /* ---- transfer stand-by state ---- */
    if (_sd_standby(p_hndl) != SD_OK)
    {
        return _sd_reget_reg_error(p_hndl);
    }

    /* verify CID or CSD */
    if (0 == is_csd)
    {
        p_ptr = p_hndl->cid;
        cmd = CMD10;
    }
    else
    {
        p_ptr = p_hndl->csd;
        cmd = CMD9;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sd_reget_reg_error(p_hndl);
    }

    /* ---- reget CID or CSD (issue CMD10 or CMD9) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, cmd, SD_RSP_R2_CID, p_hndl->rca[0], 0x0000) != SD_OK)
    {
        return _sd_reget_reg_error(p_hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    if (_sd_active(p_hndl) != SD_OK)
    {
        return _sd_reget_reg_error(p_hndl);
    }

    for (i = 0; i < 8; ++i)
    {
        /* Cast to an appropriate type */
        *reg++ = (uint8_t)((*p_ptr) >> 8);

        /* Cast to an appropriate type */
        *reg++ = (uint8_t)(*p_ptr++);
    }

    return SD_OK;
}
/******************************************************************************
 End of function sd_reget_reg
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_reget_reg_error
 * Description  : reget register error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_reget_reg_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_reget_reg_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_lock_unlock
 * Description  : lock/unlock.
 *              : lock/unlock operation
 *              : passward length is up to 16 bytes
 *              : case of cahnge passward, total length is 32 bytes,that is
 *              : old and new passward maximum length
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t code    : operation code
 *              : uint8_t *pwd    : passward
 *              : uint8_t len     : passward length
 * Return Value : SD_OK : end of succeed.
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_lock_unlock(int32_t sd_port, uint8_t code, uint8_t *pwd, uint8_t len)
{
    st_sdhndl_t *p_hndl;
    uint16_t    cmd_len;   /* lock/unlock data length */
    char_t      data[32];

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->error = SD_OK;

    /* ---- check mount ---- */
    if ( (p_hndl->mount && (SD_MOUNT_LOCKED_CARD | SD_MOUNT_UNLOCKED_CARD)) == 0 )
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error; /* not mounted yet */
    }

    /* check suppoted command class */
    if (!(p_hndl->csd_ccc & 0x0080))  /* don't support lock/unlock */
    {
        _sd_set_err(p_hndl, SD_ERR_NOTSUP_CMD);
        return SD_ERR_NOTSUP_CMD;
    }

    /* Cast to an appropriate type */
    data[0] = (char_t)code;

    if (code & 0x08)    /* forcing erase */
    {
        cmd_len = 1;
    }
    else
    {
        if (code & 0x01)    /* set passward */
        {
            if (len > 16)
            {
                /* total passward length is not more than 32 bytes      */
                /* but the library prohibit change password operation   */
                return SD_ERR;
            }
            if (p_hndl->resp_status & 0x02000000)
            {
                /* prohibit set passward to lock card */
                _sd_set_err(p_hndl, SD_ERR_CARD_LOCK);
                return SD_ERR;
            }
        }
        else if (len > 16)  /* only lock or unlock */
        {
            /* one passward length is not more than 16 bytes */
            return SD_ERR;
        }
        else
        {
            /* DO NOTHING */
            ;
        }

        /* include code and total data length */
        cmd_len = (uint16_t)(len + 2);

        /* set lock/unlock command data */
        data[1] = (char_t)len;
        while (len)
        {
            data[cmd_len - len] = *pwd++;
            len--;
        }
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sd_lock_unlock_error(p_hndl);
    }

    /* ---- set block length (issue CMD16) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, CMD16, SD_RSP_R1, 0x0000, cmd_len) != SD_OK)
    {
        if (SD_ERR_CARD_LOCK == p_hndl->error)
        {
            p_hndl->error = SD_OK;
        }
        else
        {
            return _sd_lock_unlock_error(p_hndl);
        }
    }

    /* Cast to an appropriate type */
    if (_sd_write_byte(p_hndl, CMD42, 0x0000, 0x0000, (uint8_t*)data, cmd_len)
            != SD_OK)
    {
        return _sd_lock_unlock_error(p_hndl);
    }

    if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000)
            == SD_OK)
    {
        if ((p_hndl->resp_status & RES_STATE) != STATE_TRAN)  /* not transfer state */
        {
            p_hndl->error = SD_ERR;
            return _sd_lock_unlock_error(p_hndl);
        }
    }
    else    /* SDHI error */
    {
        return _sd_lock_unlock_error(p_hndl);
    }

    if ( (code & SD_LOCK_CARD) == SD_UNLOCK_CARD )
    {
        /* ---- clear locked status ---- */
        p_hndl->mount &= (~SD_CARD_LOCKED);

        if ( SD_MOUNT_UNLOCKED_CARD == p_hndl->mount )
        {
            /* the card is already mounted as unlock card   */

            /* ---- set block length (issue CMD16) ---- */
            if (_sd_card_send_cmd_arg(p_hndl, CMD16, SD_RSP_R1, 0x0000, 0x0200) != SD_OK)
            {
                /* ---- set locked status ---- */
                p_hndl->mount |=  SD_CARD_LOCKED;
                return _sd_lock_unlock_error(p_hndl);
            }
        }
    }
    else
    {
        /* ---- set locked status ---- */
        p_hndl->mount |=  SD_CARD_LOCKED;
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return SD_OK;
}
/******************************************************************************
 End of function sd_lock_unlock
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_lock_unlock_error
 * Description  : lock/unlock error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_lock_unlock_error(st_sdhndl_t *p_hndl)
{
    int32_t  temp_error;
    int32_t  loop;

    /* keep error   */
    temp_error = p_hndl->error;

    for ( loop = 0; loop < 3; loop++ )
    {
        /* ---- retrive block length ---- */
        if (_sd_card_send_cmd_arg(p_hndl, CMD16, SD_RSP_R1, 0x0000, 0x0200) == SD_OK)
        {
            break;
        }
    }

    _sd_set_err(p_hndl, temp_error);

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_lock_unlock_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_set_tmpwp
 * Description  : set tempolary write protect.
 *              : set temporary write protect programing csd
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : int32_t is_set  : set or clear
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_set_tmpwp(int32_t sd_port, int32_t is_set)
{
    st_sdhndl_t *p_hndl;
    int32_t     i;
    uint16_t    *p_ptr;     /* got csd */
    uint8_t     w_csd[16];  /* work csd */
    uint8_t     crc7;       /* calculated crc7 value */

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl->error = SD_OK;

    /* check suppoted command class */
    if (!(p_hndl->csd_ccc & 0x0010))  /* don't support block write */
    {
        _sd_set_err(p_hndl, SD_ERR_NOTSUP_CMD);
        return SD_ERR;
    }

    /* ---- make programing csd value ---- */
    /* set unprogramable fields */
    p_ptr = p_hndl->csd;
    for (i = 0; i < 14; i += 2)
    {
        /* Cast to an appropriate type */
        w_csd[i] = (uint8_t)(*p_ptr++);

        /* Cast to an appropriate type */
        w_csd[i + 1] = (uint8_t)(((*p_ptr) >> 8u));
    }

    /* set programing fields */
    w_csd[14] = (uint8_t)(*p_ptr);
    if (1 == is_set)    /* set write protect */
    {
        w_csd[14] |= 0x10;
    }
    else    /* clear write protect */
    {
        w_csd[14] &= (~0x10);
    }

    /* calculate crc7 for CSD */
    crc7 = _sd_calc_crc(w_csd, 15);

    /* set crc7 filelds */
    w_csd[15] = (uint8_t)((crc7 << 1u) | 0x01);

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return _sd_set_tmpwp_error(p_hndl);
    }

    if (_sd_write_byte(p_hndl, CMD27, 0x0000, 0x0000, w_csd, sizeof(w_csd)) != SD_OK)
    {
        return _sd_set_tmpwp_error(p_hndl);
    }

    if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000)
            == SD_OK)
    {
        if ((p_hndl->resp_status & RES_STATE) != STATE_TRAN)  /* not transfer state */
        {
            p_hndl->error = SD_ERR;
            return _sd_set_tmpwp_error(p_hndl);
        }
    }
    else    /* SDHI error */
    {
        return _sd_set_tmpwp_error(p_hndl);
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    if (1 == is_set)    /* set write protect */
    {
        /* Cast to an appropriate type */
        p_hndl->write_protect |= (uint8_t)SD_WP_TEMP;
    }
    else    /* clear write protect */
    {
        /* Cast to an appropriate type */
        p_hndl->write_protect &= (uint8_t)~SD_WP_TEMP;
    }

    return SD_OK;
}
/******************************************************************************
 End of function sd_set_tmpwp
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_set_tmpwp_error
 * Description  : set tempolary write protect error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_set_tmpwp_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_set_tmpwp_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_calc_crc
 * Description  : calculate crc7.
 *              : calculate crc7 value
 * Arguments    : uint8_t *data : input data
 *              : int32_t len   : input data length (byte unit)
 * Return Value : calculated crc7 value
 *****************************************************************************/
static uint8_t _sd_calc_crc(uint8_t *data, int32_t len)
{
    int32_t i;
    int32_t j;
    int32_t k;
    uint8_t p_crc[7];   /* previous crc value */
    uint8_t t_crc[7];   /* tentative crc value */
    uint8_t m_data;     /* input bit mask data */
    uint8_t crc7 = 0;   /* calculated crc7 */

    for (i = 0; i < (sizeof(p_crc)); i++)
    {
        p_crc[i] = 0;
        t_crc[i] = 0;
    }

    for (i = len; i > 0; i--, data++) /* byte loop */
    {
        for (j = 8; j > 0; j--) /* bit loop */
        {
            /* Cast to an appropriate type */
            m_data = (uint8_t)((((*data) >> (j - 1)) & 0x01));
            t_crc[6] = (p_crc[0] != m_data);
            t_crc[5] = p_crc[6];
            t_crc[4] = p_crc[5];
            t_crc[3] = (p_crc[4] != (p_crc[0] != m_data));
            t_crc[2] = p_crc[3];
            t_crc[1] = p_crc[2];
            t_crc[0] = p_crc[1];

            /* save tentative crc value */
            for (k = 0; k < (sizeof(p_crc)); k++)
            {
                p_crc[k] = t_crc[k];
            }
        }
    }

    /* convert bit to byte form */
    for (i = 0; i < (sizeof(p_crc)); i++)
    {
        /* Cast to an appropriate type */
        crc7 |= (uint8_t)((p_crc[i] << (6 - i)));
    }

    return crc7;
}
/******************************************************************************
 End of function _sd_calc_crc
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_memset
 * Description  : set memory.
 *              : fill memory filling data(=data) from start address(=p)
 *              : by filling size(=cnt)
 * Arguments    : uint8_t *p   : start address of memory
 *              : uint8_t data : filling data
 *              : uint32_t cnt : filling size
 * Return Value : 0 : end of succeed
 *****************************************************************************/
int32_t _sd_memset(uint8_t *p, uint8_t data, uint32_t cnt)
{
    while (cnt--)
    {
        *p++ = data;
    }

    return 0;
}
/******************************************************************************
 End of function _sd_memset
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_memcpy
 * Description  : copy memory.
 *              : copy data from source address(=src) to destination address
 *              : (=dst) by copy size(=cnt)
 * Arguments    : uint8_t *dst : destination address
 *              : uint8_t *src : source address
 *              : uint32_t cnt : copy size
 * Return Value : 0 : end of succeed
 *****************************************************************************/
int32_t _sd_memcpy(uint8_t *dst, uint8_t *src, uint32_t cnt)
{
    while (cnt--)
    {
        *dst++ = *src++;
    }

    return 0;
}
/******************************************************************************
 End of function _sd_memcpy
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_rand
 * Description  : create Volume ID Number.
 *              : get Volume ID Number
 *              : Volume ID Number is created by pseudo random number
 * Arguments    : none
 * Return Value : created Volume ID Number
 *****************************************************************************/
uint16_t _sd_rand(void)
{

    s_next = (s_next * 1103515245L) + 12345;

    /* Cast to an appropriate type */
    return (uint16_t)s_next;

}
/******************************************************************************
 End of function _sd_rand
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_srand
 * Description  : set initial value of Volume ID Number.
 * Arguments    : uint32_t seed : initial seting value
 * Return Value : none
 *****************************************************************************/
void _sd_srand(uint32_t seed)
{
    if (0 == s_next)
    {
        s_next = seed;
    }
}
/******************************************************************************
 End of function _sd_srand
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_wait_rbusy
 * Description  : wait response busy.
 *              : wait response busy finished
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t time        : response busy wait interval
 * Return Value : SD_OK : response busy finished
 *              : SD_ERR: response busy not finished
 *****************************************************************************/
int32_t _sd_wait_rbusy(st_sdhndl_t *p_hndl, int32_t time)
{
    int32_t i;


    for (i = 0; i < time; ++i)
    {
        if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000) == SD_OK)
        {
            if ((p_hndl->resp_status & RES_STATE) == STATE_TRAN)  /* transfer state */
            {
                return SD_OK;
            }
        }
        else    /* SDHI error */
        {
            break;
        }

        if (_sd_check_media(p_hndl) != SD_OK)
        {
            break;
        }

        sddev_int_wait(p_hndl->sd_port, 1);
    }

    _sd_set_err(p_hndl, SD_ERR_HOST_TOE);

    return SD_ERR;

}
/******************************************************************************
 End of function _sd_wait_rbusy
 *****************************************************************************/

/* End of File */
