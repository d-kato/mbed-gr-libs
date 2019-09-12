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
* File Name    : sd_mount.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Card mount
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02     Support for OS
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
static uint16_t s_stat_buff[NUM_PORT][64 / sizeof(uint16_t)];

static int32_t _sd_mount_error(st_sdhndl_t *p_hndl);
static int32_t _sd_card_init_get_rca(st_sdhndl_t *p_hndl);
static int32_t _sd_mem_mount_error(st_sdhndl_t *p_hndl);
static int32_t _sd_read_byte_error(st_sdhndl_t *p_hndl);
static int32_t _sd_write_byte_error(st_sdhndl_t *p_hndl);

/******************************************************************************
 * Function Name: sd_mount
 * Description  : mount SD card.
 *              : mount SD memory card user area
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
 * Arguments    : int32_t sd_port  : channel no (0 or 1)
 *              : uint32_t mode    : SD Driver operation mode
 *              : uint32_t voltage : operation voltage
 * Return Value : p_hndl->error    : SD handle error value
 *              : SD_OK : end of succeed
 *              : other : end of error
 * Remark       : user area should be mounted
 *****************************************************************************/
int32_t sd_mount(int32_t sd_port, uint32_t mode, uint32_t voltage)
{
    st_sdhndl_t *p_hndl;
    uint64_t    info1_back;
    uint16_t    sd_spec;
    uint16_t    sd_spec3;

    if ((0 != sd_port) && (1 != sd_port))
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* ==== check work buffer is allocated ==== */
    if (0 == p_hndl->p_rw_buff)
    {
        return SD_ERR;  /* not allocated yet */
    }

    /* ==== initialize parameter ==== */
    _sd_init_hndl(p_hndl, mode, voltage);
    p_hndl->error = SD_OK;

    /* ==== is card inserted? ==== */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);
        return p_hndl->error;     /* not inserted */
    }

    /* ==== power on sequence ==== */
    /* ---- turn on voltage ---- */
    if (sddev_power_on(sd_port) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return _sd_mount_error(p_hndl);
    }

    /* ---- set single port ---- */
    _sd_set_port(p_hndl, SD_PORT_SERIAL);

    /* ---- supply clock (card-identification ratio) ---- */
    if (_sd_set_clock(p_hndl, SD_CLK_400KHZ, SD_CLOCK_ENABLE) != SD_OK)
    {
        return p_hndl->error;     /* not inserted */
    }

    sddev_int_wait(sd_port, 2); /* add wait function  */

    sddev_loc_cpu(sd_port);

    /* Cast to an appropriate type */
    info1_back = SD_INP(p_hndl, SD_INFO1);

    /* Cast to an appropriate type */
    info1_back &= (uint64_t)0xfff8;

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO1, (uint64_t)info1_back);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2, (uint64_t)0);

    /* Clear DMA Enable because of CPU Transfer */
    SD_OUTP(p_hndl, CC_EXT_MODE, (uint64_t)(SD_INP(p_hndl, CC_EXT_MODE) & ~CC_EXT_MODE_DMASDRW)); /* disable DMA  */

    sddev_unl_cpu(sd_port);

    /* ==== initialize card and distinguish card type ==== */
    if (_sd_card_init(p_hndl) != SD_OK)
    {
        return _sd_mount_error(p_hndl);  /* failed card initialize */
    }

    if (p_hndl->media_type & SD_MEDIA_MEM)    /* with memory part */
    {
        /* ==== check card registers ==== */
        /* ---- check CSD register ---- */
        if (_sd_check_csd(p_hndl) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }

        /* ---- no check other registers (to be create) ---- */

        /* get user area size */
        if (_sd_get_size(p_hndl, SD_USER_AREA) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }

        /* check write protect */
        p_hndl->write_protect |= (uint8_t)_sd_iswp(p_hndl);
    }

    if (p_hndl->media_type & SD_MEDIA_MEM)    /* with memory part */
    {
        if (_sd_mem_mount(p_hndl) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }
        if (SD_ERR_CARD_LOCK == p_hndl->error)
        {
            p_hndl->mount = (SD_CARD_LOCKED | SD_MOUNT_LOCKED_CARD);

            /* ---- halt clock ---- */
            _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
            return SD_OK_LOCKED_CARD;
        }
    }

    /* if SD memory card, get SCR register */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        if (_sd_card_get_scr(p_hndl) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }

        if (SD_SPEC_20 == p_hndl->sd_spec)
        {
            /* Cast to an appropriate type */
            sd_spec = (uint16_t)(p_hndl->scr[0] & SD_SPEC_REGISTER_MASK);

            /* Cast to an appropriate type */
            sd_spec3 = (uint16_t)(p_hndl->scr[1] & SD_SPEC_30_REGISTER);
            if ((SD_SPEC_20_REGISTER == sd_spec) && (SD_SPEC_30_REGISTER == sd_spec3))
            {
                /* ---- more than phys spec ver3.00 ---- */
                p_hndl->sd_spec = SD_SPEC_30;
            }
            else     /* ---- phys spec ver2.00 ---- */
            {
                p_hndl->sd_spec = SD_SPEC_20;
            }
        }
        else
        {
            /* Cast to an appropriate type */
            sd_spec = (uint16_t)(p_hndl->scr[0] & SD_SPEC_REGISTER_MASK);
            if (SD_SPEC_11_REGISTER == sd_spec)   /* ---- phys spec ver1.10 ---- */
            {
                p_hndl->sd_spec = SD_SPEC_11;
            }
            else    /* ---- phys spec ver1.00 or ver1.01 ---- */
            {
                p_hndl->sd_spec = SD_SPEC_10;
            }
        }

        /* Cast to an appropriate type */
        (void)_sd_calc_erase_sector(p_hndl);
    }

    /* ---- set mount flag ---- */
    p_hndl->mount = SD_MOUNT_UNLOCKED_CARD;

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
    return p_hndl->error;
}
/******************************************************************************
 End of function sd_mount
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_mount_error
 * Description  : mount SD card error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : p_hndl->error  : SD handle error value
 *              : SD_OK : end of succeed
 *              : other : end of error
 *****************************************************************************/
static int32_t _sd_mount_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_mount_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_init
 * Description  : initialize card.
 *              : initialize card from idle state to stand-by
 *              : distinguish card type (SD, MMC, IO or COMBO)
 *              : get CID, RCA, CSD from the card
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_card_init(st_sdhndl_t *p_hndl)
{
    int32_t  ret;
    int32_t  i;
    uint16_t if_cond_0;
    uint16_t if_cond_1;

    p_hndl->media_type = SD_MEDIA_UNKNOWN;
    if_cond_0 = p_hndl->if_cond[0];
    if_cond_1 = p_hndl->if_cond[1];

    /* ==== transfer idle state (issue CMD0) ==== */
    for (i = 0; i < 3; i++)
    {
        ret = _sd_send_cmd(p_hndl, CMD0);
        if (SD_OK == ret)
        {
            break;
        }
    }

    if (SD_OK != ret)
    {
        return SD_ERR;  /* error for CMD0 */
    }

    /* clear error by reissuing CMD0 */
    p_hndl->error = SD_OK;

    p_hndl->media_type |= SD_MEDIA_SD;

    if (SD_MODE_VER2X == p_hndl->sup_ver)
    {
        ret = _sd_card_send_cmd_arg(p_hndl, CMD8, SD_RSP_R7, if_cond_0, if_cond_1);
        if (SD_OK == ret)
        {
            /* check R7 response */
            if (p_hndl->if_cond[0] & 0xf000)
            {
                p_hndl->error = SD_ERR_IFCOND_VER;
                return SD_ERR;
            }
            if ((p_hndl->if_cond[1] & 0x00ff) != 0x00aa)
            {
                p_hndl->error = SD_ERR_IFCOND_ECHO;
                return SD_ERR;
            }
            p_hndl->sd_spec = SD_SPEC_20;         /* cmd8 have response.              */

            /* because of (phys spec ver2.00)   */
        }
        else
        {
            /* ==== clear illegal command error for CMD8 ==== */
            for (i = 0; i < 3; i++)
            {
                ret = _sd_send_cmd(p_hndl, CMD0);
                if (SD_OK == ret)
                {
                    break;
                }
            }
            p_hndl->error = SD_OK;
            p_hndl->sd_spec = SD_SPEC_10;         /* cmd8 have no response.                   */

            /* because of (phys spec ver1.01 or 1.10)   */
        }
    }
    else
    {
        p_hndl->sd_spec = SD_SPEC_10;             /* cmd8 have response.                      */

        /* because of (phys spec ver1.01 or 1.10)   */
    }

    /* set OCR (issue ACMD41) */
    ret = _sd_card_send_ocr(p_hndl, (int32_t)p_hndl->media_type);

    /* clear error due to card distinction */
    p_hndl->error = SD_OK;

    if (SD_OK != ret)
    {
        /* softreset for error clear (issue CMD0) */
        for (i = 0; i < 3; i++)
        {
            ret = _sd_send_cmd(p_hndl, CMD0);
            if (SD_OK == ret)
            {
                break;
            }
        }
        if (SD_OK != ret)
        {
            return SD_ERR;  /* error for CMD0 */
        }

        /* clear error by reissuing CMD0 */
        p_hndl->error = SD_OK;

        /* ---- get OCR (issue CMD1) ---- */
        if (SD_OK == (ret = _sd_card_send_ocr(p_hndl, SD_MEDIA_MMC)))
        {
            /* MMC */
            p_hndl->media_type = SD_MEDIA_MMC;
            p_hndl->error = SD_OK;
        }
        else
        {
            /* unknown card */
            p_hndl->media_type = SD_MEDIA_UNKNOWN;
            _sd_set_err(p_hndl, SD_ERR_CARD_TYPE);
            return SD_ERR;
        }
    }

    /* ---- get CID (issue CMD2) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, CMD2, SD_RSP_R2_CID, 0, 0) != SD_OK)
    {
        return SD_ERR;
    }
    return _sd_card_init_get_rca(p_hndl);
}
/******************************************************************************
 End of function _sd_card_init
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_init_get_rca
 * Description  : initialize card.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_card_init_get_rca(st_sdhndl_t *p_hndl)
{
    int32_t  i;

    /* ---- get RCA (issue CMD3) ---- */
    if (p_hndl->media_type & SD_MEDIA_SD)  /* SD */
    {
        for (i = 0; i < 3; i++)
        {
            if (_sd_card_send_cmd_arg(p_hndl, CMD3, SD_RSP_R6, 0, 0) != SD_OK)
            {
                return SD_ERR;
            }
            if (0x00 != p_hndl->rca[0])
            {
                break;
            }
        }

        /* illegal RCA */
        if (3 == i)
        {
            _sd_set_err(p_hndl, SD_ERR_CARD_CC);
            return SD_ERR;
        }
    }
    else
    {
        p_hndl->rca[0] = 1;   /* fixed 1 */
        if (_sd_card_send_cmd_arg(p_hndl, CMD3, SD_RSP_R1, p_hndl->rca[0], 0x0000)
                != SD_OK)
        {
            return SD_ERR;
        }
    }

    /* ---- get CSD (issue CMD9) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, CMD9, SD_RSP_R2_CSD, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        return SD_ERR;
    }

    p_hndl->dsr[0] = 0x0000;

    if (p_hndl->media_type & SD_MEDIA_MEM)
    {
        /* is DSR implimented? */
        if (p_hndl->csd[3] & 0x0010u) /* implimented */
        {
            /* set DSR (issue CMD4) */
            p_hndl->dsr[0] = 0x0404;
            if (_sd_card_send_cmd_arg(p_hndl, CMD4, SD_RSP_NON, p_hndl->dsr[0], 0x0000)
                    != SD_OK)
            {
                return SD_ERR;
            }
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_card_init_get_rca
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_mem_mount
 * Description  : mount memory card.
 *              : mount memory part from stand-by to transfer state
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_mem_mount(st_sdhndl_t *p_hndl)
{
    /* ==== data-transfer mode(Transfer State) ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD7, SD_RSP_R1B, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        return _sd_mem_mount_error(p_hndl);
    }

    if ((p_hndl->resp_status & 0x02000000))
    {
        _sd_set_err(p_hndl, SD_ERR_CARD_LOCK);
        return SD_OK;
    }

    /* ---- set block length (issue CMD16) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, CMD16, SD_RSP_R1, 0x0000, 0x0200) != SD_OK)
    {
        return _sd_mem_mount_error(p_hndl);
    }

    /* if 4bits transfer supported (SD memory card mandatory), change bus width 4bits */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        _sd_set_port(p_hndl, p_hndl->sup_if_mode);
    }

    /* clear pull-up DAT3 */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        if (_sd_send_acmd(p_hndl, ACMD42, 0, 0) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }

        /* check R1 resp */
        if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }
    }

    /* if SD memory card, get SD Status */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        if (_sd_card_get_status(p_hndl) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }

        /* get protect area size */
        if (_sd_get_size(p_hndl, SD_PROT_AREA) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_mem_mount
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_mem_mount_error
 * Description  : mount memory card error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_mem_mount_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_mem_mount_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_unmount
 * Description  : unmount card.
 *              : turn off power
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t sd_unmount(int32_t sd_port)
{
    st_sdhndl_t  *p_hndl;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initialized */
    }

    /* ---- clear mount flag ---- */
    p_hndl->mount = SD_UNMOUNT_CARD;

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    /* ---- set single port ---- */
    sddev_set_port(sd_port, SD_PORT_SERIAL);

    /* ---- turn off power ---- */
    if (sddev_power_off(sd_port) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return p_hndl->error;
    }

    /* ---- initilaize SD handle ---- */
    _sd_init_hndl(p_hndl, 0, p_hndl->voltage);

    return SD_OK;
}
/******************************************************************************
 End of function sd_unmount
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_get_status
 * Description  : get SD Status (issue ACMD13)
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_card_get_status(st_sdhndl_t *p_hndl)
{
    int32_t  ret;
    int32_t  i;
    uint8_t  *p_rw_buff;

    /* Cast to an appropriate type */
    p_rw_buff = (uint8_t *)&s_stat_buff[p_hndl->sd_port][0];

    /* ---- get SD Status (issue ACMD13) ---- */
    if (_sd_read_byte(p_hndl, ACMD13, 0, 0, p_rw_buff, SD_STATUS_BYTE) != SD_OK)
    {
        return SD_ERR;
    }

    /* ---- distinguish SD ROM card ---- */
    if ((p_rw_buff[2] & 0xffu) == 0x00) /* [495:488] = 0x00 */
    {
        ret = SD_OK;
        if ((p_rw_buff[3] & 0xffu) == 0x01)
        {
            p_hndl->write_protect |= SD_WP_ROM;
        }
    }
    else
    {
        ret = SD_ERR;
        _sd_set_err(p_hndl, SD_ERR_CARD_ERROR);
    }

    p_hndl->speed_class = p_rw_buff[8];
    p_hndl->perform_move = p_rw_buff[9];

    /* ---- save SD STATUS ---- */
    for (i = 0; i < (16 / sizeof(uint16_t)); i++)
    {
        p_hndl->sdstatus[i] = (s_stat_buff[p_hndl->sd_port][i] << 8) | (s_stat_buff[p_hndl->sd_port][i] >> 8);
    }

    return ret;
}
/******************************************************************************
 End of function _sd_card_get_status
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_get_scr
 * Description  : get SCR register (issue ACMD51).
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_card_get_scr(st_sdhndl_t *p_hndl)
{
    uint8_t  *p_rw_buff;

    /* Cast to an appropriate type */
    p_rw_buff = (uint8_t *)&s_stat_buff[p_hndl->sd_port][0];

    /* ---- get SCR register (issue ACMD51) ---- */
    if (_sd_read_byte(p_hndl, ACMD51, 0, 0, p_rw_buff, SD_SCR_REGISTER_BYTE) != SD_OK)
    {
        return SD_ERR;
    }

    /* ---- save SCR register ---- */
    p_hndl->scr[0] = (s_stat_buff[p_hndl->sd_port][0] << 8) | (s_stat_buff[p_hndl->sd_port][0] >> 8);
    p_hndl->scr[1] = (s_stat_buff[p_hndl->sd_port][1] << 8) | (s_stat_buff[p_hndl->sd_port][1] >> 8);
    p_hndl->scr[2] = (s_stat_buff[p_hndl->sd_port][2] << 8) | (s_stat_buff[p_hndl->sd_port][2] >> 8);
    p_hndl->scr[3] = (s_stat_buff[p_hndl->sd_port][3] << 8) | (s_stat_buff[p_hndl->sd_port][3] >> 8);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_card_get_scr
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_read_byte
 * Description  : read byte data from card
 *              : issue byte data read command and read data from SD_BUF
 *              : using following commands
 *              : SD STATUS(ACMD13),SCR(ACMD51),NUM_WRITE_BLOCK(ACMD22),
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint16_t cmd        : command code
 *              : uint16_t h_arg      : command argument high [31:16]
 *              : uint16_t l_arg      : command argument low [15:0]
 *              : uint8_t *readbuff   : read data buffer
 *              : uint16_t byte       : the number of read bytes
 * Return Value : SD_OK : end of succeed
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_read_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg,
                        uint16_t l_arg, uint8_t *readbuff, uint16_t byte)
{
    /* ---- disable SD_SECCNT ---- */
    SD_OUTP(p_hndl, SD_STOP, 0x0000);

    /* ---- set transfer bytes ---- */
    SD_OUTP(p_hndl, SD_SIZE, (uint64_t)byte);

    /* ---- issue command ---- */
    if (cmd & 0x0040u)  /* ACMD13, ACMD22 and ACMD51 */
    {
        if (_sd_send_acmd(p_hndl, cmd, h_arg, l_arg) != SD_OK)
        {
            if ((SD_ERR_END_BIT == p_hndl->error) ||
                    (SD_ERR_CRC == p_hndl->error))
            {
                /* continue */
                ;
            }
            else
            {
                return _sd_read_byte_error(p_hndl);
            }
        }
    }
    else
    {
        _sd_set_arg(p_hndl, h_arg, l_arg);
        if (_sd_send_cmd(p_hndl, cmd) != SD_OK)
        {
            return SD_ERR;
        }
    }

    /* ---- check R1 response ---- */
    if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
    {
        return _sd_read_byte_error(p_hndl);
    }

    /* enable All end, BRE and errors */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

    /* ---- wait BRE interrupt ---- */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_read_byte_error(p_hndl);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_read_byte_error(p_hndl);
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_RE); /* clear BRE bit */

    /* transfer data */
    if (sddev_read_data(p_hndl->sd_port, readbuff, (uint32_t)(p_hndl->reg_base + SD_BUF0), (int32_t)byte) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return _sd_read_byte_error(p_hndl);
    }

    /* wait All end interrupt */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_read_byte_error(p_hndl);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_read_byte_error(p_hndl);
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR); /* clear All end bit */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_read_byte
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_read_byte_error
 * Description  : read byte data error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle.
 * Return Value : SD_ERR: end of error.
 *****************************************************************************/
static int32_t _sd_read_byte_error(st_sdhndl_t *p_hndl)
{
    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_STOP, (uint64_t)0x0001);                       /* stop data transfer   */

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR); /* clear All end bit    */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

    return SD_ERR;
}
/******************************************************************************
 End of function _sd_read_byte_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_write_byte
 * Description  : write byte data to card
 *              : issue byte data write command and write data to SD_BUF
 *              : using following commands
 *              : (CMD27 and CMD42)
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint16_t cmd        : command code
 *              : uint16_t h_arg      : command argument high [31:16]
 *              : uint16_t l_arg      : command argument low [15:0]
 *              : uint8_t *writebuff  : write data buffer
 *              : uint16_t byte       : the number of write bytes
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_write_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg,
                        uint16_t l_arg, uint8_t *writebuff, uint16_t byte)
{
    int32_t time_out;

    /* ---- disable SD_SECCNT ---- */
    SD_OUTP(p_hndl, SD_STOP, 0x0000);

    /* ---- set transfer bytes ---- */
    SD_OUTP(p_hndl, SD_SIZE, (uint64_t)byte);

    /* ---- issue command ---- */
    _sd_set_arg(p_hndl, h_arg, l_arg);
    if (_sd_send_cmd(p_hndl, cmd) != SD_OK)
    {
        return SD_ERR;
    }

    /* ---- check R1 response ---- */
    if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
    {
        if (SD_ERR_CARD_LOCK == p_hndl->error)
        {
            p_hndl->error = SD_OK;
        }
        else
        {
            return _sd_write_byte_error(p_hndl);
        }
    }

    /* enable All end, BWE and errors */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

    /* ---- wait BWE interrupt ---- */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_write_byte_error(p_hndl);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_write_byte_error(p_hndl);
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_WE); /* clear BWE bit */

    /* transfer data */
    if (sddev_write_data(p_hndl->sd_port, writebuff, (uint32_t)(p_hndl->reg_base + SD_BUF0), (int32_t)byte) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return _sd_write_byte_error(p_hndl);
    }

    /* wait All end interrupt */
    if ( (CMD42 == cmd) && (1 == byte) )
    {
        /* force erase timeout  */
        time_out = SD_TIMEOUT_ERASE_CMD;
    }
    else
    {
        time_out = SD_TIMEOUT_RESP;
    }

    if (sddev_int_wait(p_hndl->sd_port, time_out) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_write_byte_error(p_hndl);
    }

    /* ---- check errors but for timeout ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        if ( SD_TIMEOUT_ERASE_CMD == time_out )
        {
            /* force erase  */
            if (SD_ERR_CARD_TOE == p_hndl->error)
            {
                /* force erase timeout  */
                _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ERR);
                if (_sd_wait_rbusy(p_hndl, 10000000) != SD_OK)
                {
                    return _sd_write_byte_error(p_hndl);
                }
            }
            else
            {
                return _sd_write_byte_error(p_hndl);
            }
        }
        else
        {
            return _sd_write_byte_error(p_hndl);
        }
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);  /* clear All end bit */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_write_byte
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_write_byte_error
 * Description  : write byte data error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_write_byte_error(st_sdhndl_t *p_hndl)
{
    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_STOP, (uint64_t)0x0001);               /* stop data transfer   */

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);  /* clear All end bit    */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

    return SD_ERR;
}
/******************************************************************************
 End of function _sd_write_byte_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_calc_erase_sector
 * Description  : calculate erase sector.
 *              : This function calculate erase sector for SD Phy Ver2.0.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_calc_erase_sector(st_sdhndl_t *p_hndl)
{
    uint16_t au;
    uint16_t erase_size;

    if ((p_hndl->scr[0] & 0x0f00) == 0x0200)
    {
        /* AU is not defined,set to fixed value */
        p_hndl->erase_sect = SD_ERASE_SECTOR;

        /* get AU size */
        au = p_hndl->sdstatus[5] >> 12;

        if ( (au > 0) && (au < 0x0a) )
        {
            /* get AU_SIZE(sectors) */
            p_hndl->erase_sect = ((8 * 1024) / 512) << au;

            /* get ERASE_SIZE */
            erase_size = (p_hndl->sdstatus[5] << 8) | (p_hndl->sdstatus[6] >> 8);
            if (0 != erase_size)
            {
                p_hndl->erase_sect *= erase_size;
            }
        }

    }
    else
    {
        /* If card is not Ver2.0,it use ERASE_BLK_LEN in CSD *//* DO NOTHING */
        ;
    }
    return SD_OK;
}
/******************************************************************************
 End of function _sd_calc_erase_sector
 *****************************************************************************/
/* End of File */
