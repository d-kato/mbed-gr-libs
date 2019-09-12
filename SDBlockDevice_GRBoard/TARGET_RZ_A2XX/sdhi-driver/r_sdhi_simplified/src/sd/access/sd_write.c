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
* File Name    : sd_write.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Card write
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
static int32_t _sd_write_sect_error(st_sdhndl_t *p_hndl, int32_t mode);
static int32_t _sd_single_write(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t psn, int32_t mode);
static int32_t _sd_single_write_error(st_sdhndl_t *p_hndl, int32_t mode);

/******************************************************************************
 * Function Name: sd_write_sect
 * Description  : write sector data to card.
 *              : write sector data from physical sector number (=psn) by the
 *              : number of sectors (=cnt)
 *              : if SD Driver mode is SD_MODE_SW, data transfer by
 *              : sddev_read_data function
 *              : if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : int32_t sd_port   : channel no (0 or 1).
 *              : uint8_t *buff     : write data buffer
 *              : uint32_t psn      : write physical sector number
 *              : int32_t cnt       : number of write sectors
 *              : int32_t writemode : memory card write mode
 *              :   SD_WRITE_WITH_PREERASE : pre-erease write
 *              :   SD_WRITE_OVERWRITE     : overwrite
 * Return Value : SD_OK : end of succeed.
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t sd_write_sect(int32_t sd_port, uint8_t *buff, uint32_t psn, int32_t cnt, int32_t writemode)
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

    /* ---- check write protect ---- */
    if (p_hndl->write_protect)
    {
        _sd_set_err(p_hndl, SD_ERR_WP);
        return p_hndl->error; /* write protect error */
    }

    /* ---- is stop compulsory? ---- */
    if (p_hndl->stop)
    {
        p_hndl->stop = 0;
        _sd_set_err(p_hndl, SD_ERR_STOP);
        return p_hndl->error;
    }

    /* ---- is card existed? ---- */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);
        return p_hndl->error;
    }

    /* ==== write sector data to card ==== */
    _sd_write_sect(p_hndl, buff, psn, cnt, writemode);

    return p_hndl->error;
}
/******************************************************************************
 End of function sd_write_sect
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_write_sect
 * Description  : write sector data to card.
 *              : write sector data from physical sector number (=psn) by the
 *              : number of sectors (=cnt)
 *              : if SD Driver mode is SD_MODE_SW, data transfer by
 *              : sddev_read_data function
 *              : if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint8_t *buff       : write data buffer
 *              : uint32_t psn        : write physical sector number
 *              : int32_t cnt         : number of write sectors
 *              : int32_t writemode   : memory card write mode
 *              :   SD_WRITE_WITH_PREERASE : pre-erease write
 *              :   SD_WRITE_OVERWRITE     : overwrite
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_write_sect(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t psn, int32_t cnt, int32_t writemode)
{
    int32_t  i;
    int32_t  j;
    int32_t  ret;
    int32_t  trans_ret;
    int32_t  mode = SD_MODE_SW;
    uint8_t  wb[4];
    uint32_t writeblock;
    uint64_t info1_back;
    uint64_t opt_back;

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

    /* ---- supply clock (data-transfer ratio) ---- */
    if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
    {
        return p_hndl->error;
    }

    /* ==== check status precede write operation ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000)
            == SD_OK)
    {
        if ((p_hndl->resp_status & RES_STATE) != STATE_TRAN)  /* not transfer state */
        {
            p_hndl->error = SD_ERR;
            return _sd_write_sect_error(p_hndl, mode);
        }
    }
    else    /* SDHI error */
    {
        return _sd_write_sect_error(p_hndl, mode);
    }

    /* ==== execute multiple transfer by 256 sectors ==== */
    for (i = cnt; i > 0 ;
            i -= TRANS_SECTORS, psn += TRANS_SECTORS, buff += (TRANS_SECTORS * 512))
    {

        /* ---- is card existed? ---- */
        if (_sd_check_media(p_hndl) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_NO_CARD);
            return _sd_write_sect_error(p_hndl, mode);
        }

        cnt = i - TRANS_SECTORS;
        if (cnt < 0)    /* remaining sectors is less than TRANS_SECTORS */
        {
            cnt = i;
        }
        else
        {
            cnt = TRANS_SECTORS;
        }

        /* if card is SD Memory card and pre-erease write, issue ACMD23 */
        if ((p_hndl->media_type & SD_MEDIA_SD) &&
                (SD_WRITE_WITH_PREERASE == writemode))
        {
            /* Cast to an appropriate type */
            if (_sd_send_acmd(p_hndl, ACMD23, 0, (uint16_t)cnt) != SD_OK)
            {
                return _sd_write_sect_error(p_hndl, mode);
            }
            if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
            {
                return _sd_write_sect_error(p_hndl, mode);
            }
        }

        /* transfer size is fixed (512 bytes) */
        SD_OUTP(p_hndl, SD_SIZE, (uint64_t)512);

        /* 1 or 2 blocks, apply single read */
        if (cnt <= 2)
        {
            /* disable SD_SECCNT */
            SD_OUTP(p_hndl, SD_STOP, 0x0000);
            for (j = cnt; j > 0; j--, psn++, buff += 512)
            {
                trans_ret = _sd_single_write(p_hndl, buff, psn, mode);
                if (SD_OK != trans_ret)
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

        /* set number of transfer sectors */
        SD_OUTP(p_hndl, SD_SECCNT, (uint64_t)cnt);

        /* ---- enable RespEnd and ILA ---- */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, 0);

        /* issue CMD25 (WRITE_MULTIPLE_BLOCK) */
        ret = _sd_send_mcmd(p_hndl, CMD25, SET_ACC_ADDR);
        if (SD_OK != ret)
        {
            return _sd_write_sect_error(p_hndl, mode);
        }

        /* ---- disable RespEnd and ILA ---- */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

        if (SD_MODE_SW == mode) /* ==== PIO ==== */
        {
            /* enable All end, BWE and errors */
            _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

            /* software data transfer */
            trans_ret = _sd_software_trans(p_hndl, buff, cnt, SD_TRANS_WRITE);
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
            if (sddev_init_dma(p_hndl->sd_port, (uint32_t)buff, SD_TRANS_WRITE) != SD_OK)
            {
                _sd_set_err(p_hndl, SD_ERR_CPU_IF);
                return _sd_write_sect_error(p_hndl, mode);
            }

            /* DMA data transfer */
            trans_ret = _sd_dma_trans(p_hndl, cnt);
        }

        /* ---- wait All end interrupt ---- */
        ret = sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP);

        if (SD_MODE_DMA == mode)
        {
            _sd_set_int_mask(p_hndl, info1_back, 0);
        }

        /* ---- check result of transfer ---- */
        if (SD_OK != trans_ret)
        {
            return _sd_write_sect_error(p_hndl, mode);
        }

        /* ---- check result of wait All end interrupt ---- */
        if (SD_OK != ret)
        {
            _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
            return _sd_write_sect_error(p_hndl, mode);
        }

        /* ---- check errors ---- */
        if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
        {
            _sd_check_info2_err(p_hndl);
            return _sd_write_sect_error(p_hndl, mode);
        }

        /* clear All end bit */
        _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);

        /* disable All end, BWE and errors */
        _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

        if (p_hndl->media_type & SD_MEDIA_SD)
        {
            if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
            {
                /* check number of write complete block */
                if (_sd_read_byte(p_hndl, ACMD22, 0, 0, wb, 4) != SD_OK)
                {
                    return _sd_write_sect_error(p_hndl, mode);
                }

                /* type cast (uint32_t) remove compiler dependence */
                writeblock = ((uint32_t)wb[0] << 24) | ((uint32_t)wb[1] << 16) |

                            /* Cast to an appropriate type */
                            ((uint32_t)wb[2] << 8)  | (uint32_t)wb[3];

                if (cnt != writeblock)  /* no write complete block */
                {
                    _sd_set_err(p_hndl, SD_ERR);
                    return _sd_write_sect_error(p_hndl, mode);
                }
            }
        }

        /* ==== check status after write operation ==== */
        if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000)
                != SD_OK)
        {
            /* check OUT_OF_RANGE error */
            /* ignore errors during last block access */
            if (p_hndl->resp_status & 0xffffe008ul)
            {
                if ((psn + cnt) != p_hndl->card_sector_size)
                {
                    return _sd_write_sect_error(p_hndl, mode);   /* but for last block */
                }
                if (p_hndl->resp_status & 0x7fffe008ul)
                {
                    return _sd_write_sect_error(p_hndl, mode);   /* not OUT_OF_RANGE error */
                }

                /* clear OUT_OF_RANGE error */
                p_hndl->resp_status &= 0x1f00u;
                p_hndl->error = SD_OK;
            }
            else    /* SDHI error, ex)timeout error so on */
            {
                return _sd_write_sect_error(p_hndl, mode);
            }
        }

        if ((p_hndl->resp_status & RES_STATE) != STATE_TRAN)
        {
            p_hndl->error = SD_ERR;
            return _sd_write_sect_error(p_hndl, mode);
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

    /*
        CMD12 may not be issued when an error occurs.
        When the SDHI does not execute the command sequence and the SD card is not in the transfer state,
        CMD12 must be issued.

        Conditions for error processing
        1.The SD card is not in the transfer state when CMD13 is issued.
        2.CMD13 issuing process is failed.

        3.DMA transfer processing is failed.
          The sequence is suspended by the implementation failure of the user-defined function or INFO2 error.
        4.Interrupt for access end or response end is not generated.
          Implementation failure of the user-defined function caused timeout during the command sequence execution.
        5.Error bit of INFO2 is set.
        6.ACMD22 is in error.
        7.The sector count for writing obtained by ACMD22 and the transmitted sector count are different.
        8.Error bit of CMD13 response is set.
        9.CMD13 issuing process is failed.
        10.The SD card is not in the transfer state when CMD13 is issued.
    */
}
/******************************************************************************
 End of function _sd_write_sect
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_write_sect_error
 * Description  : write sector data error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : int32_t mode        : data transfer mode
 *              :   SD_MODE_SW  : software
 *              :   SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_write_sect_error(st_sdhndl_t *p_hndl, int32_t mode)
{
    volatile int32_t error;
    uint64_t sd_option;
    uint64_t sd_clk_ctrl;

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

    p_hndl->error = error;

    /* Cast to an appropriate type */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;

}
/******************************************************************************
 End of function _sd_write_sect_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_single_write
 * Description  : write sector data to card by single block transfer.
 *              : read sector data from physical sector number (=psn) by the
 *              : single block transfer
 *              : if SD Driver mode is SD_MODE_SW, data transfer by
 *              : sddev_read_data function
 *              : if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint8_t *buff       : write data buffer
 *              : uint32_t psn        : write physical sector number
 *              : int32_t mode        : data transfer mode
 *              :   SD_MODE_SW  : software
 *              :   SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_single_write(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t psn,
                                int32_t mode)
{
    int32_t  ret;
    int32_t  trans_ret;
    uint64_t info1_back;

    /* ---- enable RespEnd and ILA ---- */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

    /* issue CMD24 (WRITE_SIGLE_BLOCK) */
    if (_sd_send_mcmd(p_hndl, CMD24, SET_ACC_ADDR) != SD_OK)
    {
        return _sd_single_write_error(p_hndl, mode);
    }

    /* ---- disable RespEnd and ILA ---- */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_RESP, SD_INFO2_MASK_ILA);

    if (SD_MODE_SW == mode) /* ==== PIO ==== */
    {
        /* enable All end, BWE and errors */
        _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

        /* software data transfer */
        trans_ret = _sd_software_trans(p_hndl, buff, 1, SD_TRANS_WRITE);
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
        if (sddev_init_dma(p_hndl->sd_port, (uint32_t)buff, SD_TRANS_WRITE) != SD_OK)
        {
            _sd_set_err(p_hndl, SD_ERR_CPU_IF);
            return _sd_single_write_error(p_hndl, mode);
        }

        /* DMA data transfer */
        trans_ret = _sd_dma_trans(p_hndl, 1);
    }

    /* ---- wait All end interrupt ---- */
    ret = sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP);

    if (SD_MODE_DMA == mode)
    {
        _sd_set_int_mask(p_hndl, info1_back, 0);
    }

    /* ---- check result of transfer ---- */
    if (SD_OK != trans_ret)
    {
        return _sd_single_write_error(p_hndl, mode);
    }

    /* ---- check result of wait All end interrupt ---- */
    if (SD_OK != ret)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_single_write_error(p_hndl, mode);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_single_write_error(p_hndl, mode);
    }

    /* clear All end bit */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);

    /* disable All end, BWE and errors */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

    /* ==== skip write complete block number check ==== */

    /* ==== check status after write operation ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        /* check OUT_OF_RANGE error */
        /* ignore errors during last block access */
        if (p_hndl->resp_status & 0xffffe008ul)
        {
            if ((psn + 1) != p_hndl->card_sector_size)
            {
                return _sd_single_write_error(p_hndl, mode);  /* but for last block */
            }
            if (p_hndl->resp_status & 0x7fffe008ul)
            {
                return _sd_single_write_error(p_hndl, mode);  /* not OUT_OF_RANGE error */
            }

            /* clear OUT_OF_RANGE error */
            p_hndl->resp_status &= 0x1f00u;
            p_hndl->error = SD_OK;
        }
        else    /* SDHI error, ex)timeout error so on */
        {
            return _sd_single_write_error(p_hndl, mode);
        }
    }

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_single_write
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_single_write_error
 * Description  : write sector data to card error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle.
 *              : int32_t mode        : data transfer mode
 *              :   SD_MODE_SW  : software
 *              :   SD_MODE_DMA : DMA
 * Return Value : SD_OK : end of succeed.
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_single_write_error(st_sdhndl_t *p_hndl, int32_t mode)
{
    int32_t temp_error;

    if (SD_MODE_DMA == mode)
    {
        /* Cast to an appropriate type */
        (void)sddev_disable_dma((int32_t)(p_hndl->sd_port));   /* disable DMA */

        /* reset DMAC */
        temp_error = sddev_reset_dma((int32_t)(p_hndl->sd_port));
        if (SD_OK != temp_error)
        {
            /* DO NOTHING */
            ;
        }
    }
    temp_error = p_hndl->error;

    /* ---- clear error bits ---- */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- disable all interrupts ---- */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- enable All end ---- */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0);

    /* ---- data transfer stop (issue CMD12) ---- */
    SD_OUTP(p_hndl, SD_STOP, (uint64_t)0x0001);

    /* ---- wait All end ---- */
    sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP);

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);
    _sd_card_send_cmd_arg(p_hndl, CMD13, SD_RSP_R1, p_hndl->rca[0], 0x0000);
    p_hndl->error = temp_error;

    /* Cast to an appropriate type */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ALL);

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_single_write_error
 *****************************************************************************/

/* End of File */
