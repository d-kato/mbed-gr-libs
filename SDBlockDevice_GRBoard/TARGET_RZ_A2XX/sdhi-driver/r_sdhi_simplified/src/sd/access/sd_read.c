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
* File Name    : sd_read.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Card read
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
static int32_t _sd_read_sect_error(st_sdhndl_t *p_hndl, int32_t mode);
static int32_t _sd_single_read(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t psn, int32_t mode);
static int32_t _sd_single_read_error(st_sdhndl_t *p_hndl, int32_t mode);

/******************************************************************************
 * Function Name: sd_read_sect
 * Description  : read sector data from card.
 *              : read sector data from physical sector number (=psn) by the
 *              : number of sectors (=cnt)
 *              : if SD Driver mode is SD_MODE_SW, data transfer by
 *              : sddev_read_data function
 *              : if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint8_t *buff   : read data buffer
 *              : uint32_t psn    : read physical sector number
 *              : int32_t cnt     : number of read sectors
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_read_sect(int32_t sd_port, uint8_t *buff, uint32_t psn, int32_t cnt)
{

    st_sdhndl_t *p_hndl;
    int32_t     i;
    int32_t     j;
    int32_t     ret;
    int32_t     mode = SD_MODE_SW;
    int32_t     mmc_lastsect = 0;
    uint64_t    info1_back;
    uint64_t    opt_back;

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
    if (NULL == buff)
    {
        return SD_ERR;
    }

    p_hndl->error = SD_OK;

    /* ---- check card is mounted ---- */
    if (SD_MOUNT_UNLOCKED_CARD != p_hndl->mount)
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error; /* not mounted yet */
    }

    /* ---- is stop compulsory? ---- */
    if (p_hndl->stop)
    {
        p_hndl->stop = 0;
        _sd_set_err(p_hndl, SD_ERR_STOP);
        return SD_ERR_STOP;
    }

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
        return SD_ERR_NO_CARD;
    }

    /* access area check */
    if ((psn >= p_hndl->card_sector_size) || ((psn + cnt) > p_hndl->card_sector_size))
    {
        _sd_set_err(p_hndl, SD_ERR);
        return p_hndl->error; /* out of area */
    }

    /* if DMA transfer, buffer boundary is octlet unit */
    if ((p_hndl->trans_mode & SD_MODE_DMA) && (((uint32_t)buff & 0x07u) == 0))
    {
        mode = SD_MODE_DMA; /* set DMA mode */
    }

    /* transfer size is fixed (512 bytes) */
    SD_OUTP(p_hndl, SD_SIZE, (uint64_t)512);

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return p_hndl->error;
    }

    /* ==== check status precede read operation ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000)
            == SD_OK)
    {
        if ((p_hndl->resp_status & RES_STATE) != STATE_TRAN)  /* not transfer state */
        {
            p_hndl->error = SD_ERR;
            return _sd_read_sect_error(p_hndl, mode);
        }
    }
    else    /* SDHI error */
    {
        return _sd_read_sect_error(p_hndl, mode);
    }

    /* ==== execute multiple transfer by 256 sectors ==== */
    for (i = cnt; i > 0 ;
            i -= TRANS_SECTORS, psn += TRANS_SECTORS, buff += (TRANS_SECTORS * 512))
    {

        /* ---- is card existed? ---- */
        if (_sd_check_media(p_hndl) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_NO_CARD);  /* no card */
            return _sd_read_sect_error(p_hndl, mode);
        }

        /* set transfer sector numbers to SD_SECCNT */
        cnt = i - TRANS_SECTORS;
        if (cnt < 0)    /* remaining sectors are less than TRANS_SECTORS */
        {
            cnt = i;
        }
        else
        {
            cnt = TRANS_SECTORS;
        }

        if (cnt <= 2)
        {
            /* disable SD_SECCNT */
            SD_OUTP(p_hndl, SD_STOP, 0x0000);
            for (j = cnt; j > 0; j--, psn++, buff += 512)
            {
                ret = _sd_single_read(p_hndl, buff, psn, mode);
                if (SD_OK != ret)
                {
                    /* Cast to an appropriate type */
                    opt_back = SD_INP(p_hndl, SD_OPTION);

                    /* Cast to an appropriate type */
                    SD_OUTP(p_hndl, SOFT_RST, SOFT_RST_SDRST_RESET);

                    /* Cast to an appropriate type */
                    SD_OUTP(p_hndl, SOFT_RST, SOFT_RST_SDRST_RELEASED);

                    /* Cast to an appropriate type */
                    SD_OUTP(p_hndl, SD_OPTION, opt_back);
                    break;
                }
            }

            /* ---- halt clock ---- */
            _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

            return p_hndl->error;
        }

        /* enable SD_SECCNT */
        SD_OUTP(p_hndl, SD_STOP, (uint64_t)0x0100);

        /* issue CMD12 not automatically, if MMC last sector access */
        mmc_lastsect = 0;
        if ((SD_MEDIA_MMC == (p_hndl->media_type)) && ((psn + cnt) == (p_hndl->card_sector_size)))
        {
            mmc_lastsect = 1;
        }

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_SECCNT, (uint64_t)cnt);

        /* ---- enable RespEnd and ILA ---- */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, 0);

        /* issue CMD18 (READ_MULTIPLE_BLOCK) */
        if (0 != mmc_lastsect)   /* MMC last sector access */
        {
            if (_sd_send_mcmd(p_hndl, CMD18 | SDR104_READ_CMD, SET_ACC_ADDR) != SD_OK)
            {
                return _sd_read_sect_error(p_hndl, mode);
            }
        }
        else
        {
            if (_sd_send_mcmd(p_hndl, CMD18, SET_ACC_ADDR) != SD_OK)
            {
                return _sd_read_sect_error(p_hndl, mode);
            }
        }

        /* ---- disable RespEnd and ILA ---- */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

        if (SD_MODE_SW == mode) /* ==== PIO ==== */
        {
            /* enable All end, BRE and errors */
            _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

            /* software data transfer */
            ret = _sd_software_trans(p_hndl, buff, cnt, SD_TRANS_READ);
        }
        else    /* ==== DMA ==== */
        {
            /* disable card ins&rem interrupt for FIFO */
            info1_back = (uint64_t)(p_hndl->int_info1_mask & SD_INFO1_MASK_DET_CD);

            /* Cast to an appropriate type */
            _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DET_CD, 0);

            /* enable All end and errors */
            _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR);

            /* ---- initialize DMAC ---- */
            if (sddev_init_dma(sd_port, (uint32_t)buff, SD_TRANS_READ) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR_CPU_IF);
                return _sd_read_sect_error(p_hndl, mode);
            }

            /* DMA data transfer */
            ret = _sd_dma_trans(p_hndl, cnt);

            _sd_set_int_mask(p_hndl, info1_back, 0);
        }

        if (SD_OK != ret)
        {
            return _sd_read_sect_error(p_hndl, mode);
        }

        /* ---- wait All end interrupt ---- */
        if (sddev_int_wait(sd_port, SD_TIMEOUT_RESP) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
            return _sd_read_sect_error(p_hndl, mode);
        }

        /* ---- check errors ---- */
        if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
        {
            _sd_check_info2_err(p_hndl);
            return _sd_read_sect_error(p_hndl, mode);
        }

        /* clear All end bit */
        _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);

        /* disable All end, BRE and errors */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

        if (mmc_lastsect)
        {
            if (_sd_card_send_cmd_arg(p_hndl, 12, SD_RSP_R1B, 0, 0) != SD_OK)
            {
                /* check OUT_OF_RANGE error */
                /* ignore errors during last block access */
                if (p_hndl->resp_status & 0xffffe008ul)
                {
                    if ((psn + cnt) != p_hndl->card_sector_size)
                    {
                        return _sd_read_sect_error(p_hndl, mode);  /* but for last block */
                    }
                    if (p_hndl->resp_status & 0x7fffe008ul)
                    {
                        return _sd_read_sect_error(p_hndl, mode);  /* not OUT_OF_RANGE error */
                    }

                    /* clear OUT_OF_RANGE error */
                    p_hndl->resp_status &= 0x1f00u;
                    p_hndl->error = SD_OK;
                }
                else    /* SDHI error, ex)timeout error so on */
                {
                    return _sd_read_sect_error(p_hndl, mode);
                }
            }
        }

        /* ==== check status after read operation ==== */
        if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000)
                != SD_OK)
        {
            /* check OUT_OF_RANGE error */
            /* ignore errors during last block access */
            if (p_hndl->resp_status & 0xffffe008ul)
            {
                if ((psn + cnt) != p_hndl->card_sector_size)
                {
                    return _sd_read_sect_error(p_hndl, mode);  /* but for last block */
                }
                if (p_hndl->resp_status & 0x7fffe008ul)
                {
                    return _sd_read_sect_error(p_hndl, mode);  /* not OUT_OF_RANGE error */
                }

                /* clear OUT_OF_RANGE error */
                p_hndl->resp_status &= 0x1f00u;
                p_hndl->error = SD_OK;
            }
            else    /* SDHI error, ex)timeout error so on */
            {
                return _sd_read_sect_error(p_hndl, mode);
            }
        }

        if ((p_hndl->resp_status & RES_STATE) != STATE_TRAN)
        {
            p_hndl->error = SD_ERR;
            return _sd_read_sect_error(p_hndl, mode);
        }

        /* ---- is stop compulsory? ---- */
        if (p_hndl->stop)
        {
            p_hndl->stop = 0;

            /* data transfer stop (issue CMD12) */
            SD_OUTP(p_hndl, SD_STOP, (uint64_t)0x0001);
            i = 0;  /* set zero to break loop */
            _sd_set_err(p_hndl, SD_ERR_STOP);
        }
    }

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function sd_read_sect
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_read_sect_error
 * Description  : read sector data error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t mode        : data transfer mode
 *              :   SD_MODE_SW  : software
 *              :   SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_read_sect_error(st_sdhndl_t *p_hndl, int32_t mode)
{
    int32_t  error;
    uint64_t sd_option;
    uint64_t sd_clk_ctrl;

    if (SD_MODE_DMA == mode)
    {
        /* Cast to an appropriate type */
        (void)sddev_disable_dma((int32_t)(p_hndl->sd_port)); /* disable DMA */

        /* reset DMAC */
        error = sddev_reset_dma((int32_t)(p_hndl->sd_port));
        if (SD_OK != error)
        {
            /* DO NOTHING */
            ;
        }
    }
    mode = p_hndl->error;

    /* ---- clear error bits ---- */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- disable all interrupts ---- */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* Cast to an appropriate type */
    if ((SD_INP(p_hndl, SD_INFO2) & SD_INFO2_MASK_CBSY) == SD_INFO2_MASK_CBSY)
    {
        /* ---- enable All end ---- */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0);

        /* ---- data transfer stop (issue CMD12) ---- */
        SD_OUTP(p_hndl, SD_STOP, (uint64_t)0x0001);

        /* ---- wait All end ---- */
        sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP);

        /* Cast to an appropriate type */
        _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

        /* Cast to an appropriate type */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0);

        sddev_loc_cpu(p_hndl->sd_port);

        /* Cast to an appropriate type */
        sd_option   = SD_INP(p_hndl, SD_OPTION);

        /* Cast to an appropriate type */
        sd_clk_ctrl = SD_INP(p_hndl, SD_CLK_CTRL);

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SOFT_RST, SOFT_RST_SDRST_RESET);

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SOFT_RST, SOFT_RST_SDRST_RELEASED);

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_STOP, 0x0000);

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_OPTION, sd_option);

        /* Cast to an appropriate type */
        SD_OUTP(p_hndl, SD_CLK_CTRL, sd_clk_ctrl);
        sddev_unl_cpu(p_hndl->sd_port);
    }

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_STOP, (uint64_t)0x0001);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_STOP, 0x0000);

    /* Check Current State */
    if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000) == SD_OK)
    {
        /* not transfer state? */
        if ((p_hndl->resp_status & RES_STATE) != STATE_TRAN)
        {
            /* if not tran state, issue CMD12 to transit the SD card to tran state */
            _sd_card_send_cmd_arg(p_hndl, CMD12, SD_RSP_R1B, p_hndl->rca[0], 0x0000);

            /* not check error because already checked */
        }
    }

    p_hndl->error = mode;

    /* Cast to an appropriate type */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_read_sect_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_single_read
 * Description  : read sector data from card by single block transfer.
 *              : read sector data from physical sector number (=psn) by the
 *              : single block transfer
 *              : if SD Driver mode is SD_MODE_SW, data transfer by
 *              : sddev_read_data function
 *              : if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint8_t *buff       : read data buffer
 *              : uint32_t psn        : read physical sector number
 *              : int32_t mode        : data transfer mode
 *              :   SD_MODE_SW  : software
 *              :   SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_single_read(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t psn,
                                int32_t mode)
{
    int32_t  ret;
    uint64_t info1_back;

    /* ---- enable RespEnd and ILA ---- */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

    /* issue CMD17 (READ_SINGLE_BLOCK) */
    if (_sd_send_mcmd(p_hndl, CMD17, SET_ACC_ADDR) != SD_OK)
    {
        return _sd_single_read_error(p_hndl, mode);
    }

    /* ---- disable RespEnd and ILA ---- */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

    if (SD_MODE_SW == mode) /* ==== PIO ==== */
    {
        /* enable All end, BRE and errors */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

        /* software data transfer */
        ret = _sd_software_trans(p_hndl, buff, 1, SD_TRANS_READ);
    }
    else    /* ==== DMA ==== */
    {
        /* disable card ins&rem interrupt for FIFO */
        info1_back = (uint64_t)(p_hndl->int_info1_mask & SD_INFO1_MASK_DET_CD);

        /* Cast to an appropriate type */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DET_CD, 0);

        /* enable All end and errors */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR);

        /* ---- initialize DMAC ---- */
        if (sddev_init_dma(p_hndl->sd_port, (uint32_t)buff, SD_TRANS_READ) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_CPU_IF);
            return _sd_single_read_error(p_hndl, mode);
        }

        /* DMA data transfer */
        ret = _sd_dma_trans(p_hndl, 1);

        _sd_set_int_mask(p_hndl, info1_back, 0);
    }

    if (SD_OK != ret)
    {
        return _sd_single_read_error(p_hndl, mode);
    }

    /* ---- wait All end interrupt ---- */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_single_read_error(p_hndl, mode);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_single_read_error(p_hndl, mode);
    }

    /* clear All end bit */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);

    /* disable All end, BRE and errors */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);


    /* ==== check status after read operation ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000) != SD_OK)
    {
        /* check OUT_OF_RANGE error */
        /* ignore errors during last block access */
        if (p_hndl->resp_status & 0xffffe008ul)
        {
            if ((psn + 1) != p_hndl->card_sector_size)
            {
                return _sd_single_read_error(p_hndl, mode);  /* but for last block */
            }
            if (p_hndl->resp_status & 0x7fffe008ul)
            {
                return _sd_single_read_error(p_hndl, mode);  /* not OUT_OF_RANGE error */
            }

            /* clear OUT_OF_RANGE error */
            p_hndl->resp_status &= 0x1f00u;
            p_hndl->error = SD_OK;
        }
        else    /* SDHI error, ex)timeout error so on */
        {
            return _sd_single_read_error(p_hndl, mode);
        }
    }

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_single_read
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_single_read_error
 * Description  : read sector data error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t mode        : data transfer mode
 *              :   SD_MODE_SW  : software
 *              :   SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_single_read_error(st_sdhndl_t *p_hndl, int32_t mode)
{
    int32_t error;

    if (SD_MODE_DMA == mode)
    {
        /* Cast to an appropriate type */
        (void)sddev_disable_dma((int32_t)(p_hndl->sd_port));   /* disable DMA */

        /* reset DMAC */
        error = sddev_reset_dma((int32_t)(p_hndl->sd_port));
        if (SD_OK != error)
        {
            /* DO NOTHING */
            ;
        }
    }

    error = p_hndl->error;

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* Cast to an appropriate type */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* Cast to an appropriate type */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0);

    /* Cast to an appropriate type */
    _sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000);

    /* Cast to an appropriate type */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    p_hndl->error = error;

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_single_read_error
 *****************************************************************************/

/* End of File */
