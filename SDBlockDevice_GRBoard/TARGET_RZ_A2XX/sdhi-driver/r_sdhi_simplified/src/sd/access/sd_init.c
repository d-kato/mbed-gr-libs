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
* File Name    : sd_init.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : SD Driver initialize
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
st_sdhndl_t *gp_sdhandle[NUM_PORT];

static int32_t _sd_init_error(int32_t sd_port, int32_t ret);

/******************************************************************************
 * Function Name: sd_init
 * Description  : initialize SD Driver (more than 2ports).
 *              : initialize SD Driver work memory started from SDHI register
 *              : base
 *              : address specified by argument (base)
 *              : initialize port specified by argument (cd_port)
 *              : work memory is allocated octlet boundary
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : uint32_t base   : SDHI register base address
 *              : void *workarea  : SD Driver work memory
 *              : int32_t cd_port : card detect port
 *              :   SD_CD_SOCKET  : card detect by CD pin
 *              :   SD_CD_DAT3    : card detect by DAT3 pin
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *              : SD_ERR_CPU_IF : CPU-IF function error
 *****************************************************************************/
int32_t sd_init(int32_t sd_port, uint32_t base, void *workarea, int32_t cd_port)
{
    int32_t     i;
    uint64_t    info1;
    uint8_t     *p_ptr;
    st_sdhndl_t *p_hndl;
    int32_t     ret;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    if ((SD_SCC_IP0_BASE_ADDR != base) && (SD_SCC_IP1_BASE_ADDR != base))
    {
        return SD_ERR;
    }

    /* ==== initialize work memory  ==== */
    if ((uint32_t)workarea == 0)
    {
        ret = SD_ERR;
        return _sd_init_error(sd_port, ret);
    }

    /* ==== work memory boundary check (octlet unit) ==== */
    if ((uint32_t)workarea & 0x7u)
    {
        ret = SD_ERR;
        return _sd_init_error(sd_port, ret);
    }

    /* ==== check card detect port ==== */
    if ((SD_CD_SOCKET != cd_port) && (SD_CD_DAT3 != cd_port))
    {
        ret = SD_ERR;
        return _sd_init_error(sd_port, ret);
    }

    /* card detect port is fixed at CD pin */
    cd_port = SD_CD_SOCKET;

    /* ==== initialize peripheral module ==== */
    if (sddev_init(sd_port) != SD_OK)
    {
        ret = SD_ERR_CPU_IF;
        return _sd_init_error(sd_port, ret);
    }

    /* disable all interrupts */
    sddev_loc_cpu(sd_port);

    /* Cast to an appropriate type */
    p_hndl = (st_sdhndl_t *)workarea;

    gp_sdhandle[sd_port] = p_hndl;

    /* ---- clear work memory zero value --- */
    p_ptr = (uint8_t *)p_hndl;
    for (i = sizeof(st_sdhndl_t); i > 0 ; i--)
    {
        *p_ptr++ = 0;
    }

    /* ---- set SDHI register address ---- */
    p_hndl->reg_base = base;

    /* Cast to an appropriate type */
    p_hndl->cd_port = (uint8_t)cd_port;

    /* ---- initialize maximum block count ---- */
    p_hndl->trans_sectors = 256;
    p_hndl->trans_blocks  = 32;

    p_hndl->sd_port = sd_port;

    /* return to select port0 */
    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* ==== initialize SDHI ==== */
    SD_OUTP(p_hndl, SD_INFO1_MASK, SD_INFO1_MASK_ALL);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2_MASK, SD_INFO2_MASK_ALLP);

    /* Cast to an appropriate type */
    info1 = SD_INP(p_hndl, SD_INFO1);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO1, (uint64_t)(info1 & ~SD_INFO1_MASK_TRNS_RESP));

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2, 0x0000);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SOFT_RST, SOFT_RST_SDRST_RESET);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SOFT_RST, SOFT_RST_SDRST_RELEASED);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO1_MASK, DM_CM_INFO1_MASK_ALLP);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO2_MASK, DM_CM_INFO2_MASK_ALLP);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO1, (uint64_t)0);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO2, (uint64_t)0);

    /* initialize DMAC */
    ret = sddev_reset_dma(sd_port);
    if (SD_OK != ret)
    {
        return ret;
    }

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, HOST_MODE, HOST_MODE_64BIT_ACCESS);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_OPTION, SD_OPTION_INIT);

    /* enable all interrupts */
    sddev_unl_cpu(sd_port);

    return SD_OK;
}
/******************************************************************************
 End of function sd_init
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_init_error
 * Description  : initialize SD Driver error.
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 *              : int32_t ret     : return value
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *              : SD_ERR_CPU_IF : CPU-IF function error
 *****************************************************************************/
static int32_t _sd_init_error(int32_t sd_port, int32_t ret)
{
    gp_sdhandle[sd_port] = 0;  /* relese SD handle */
    return ret;
}
/******************************************************************************
 End of function _sd_init_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: sd_finalize
 * Description  : do finish operation of SD Driver (2ports).
 *              : finish SD Driver
 *              : reset SDHI include card detection/removal
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : after this function finished, SD handle is unavailable
 *****************************************************************************/
int32_t sd_finalize(int32_t sd_port)
{
    st_sdhndl_t *p_hndl;
    uint64_t    info1;
    int32_t     ret;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    if (0 == gp_sdhandle[sd_port])
    {
        return SD_ERR;  /* not initilized */
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* reset SDHI */
    SD_OUTP(p_hndl, SOFT_RST, SOFT_RST_SDRST_RESET);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO1_MASK, SD_INFO1_MASK_ALL);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2_MASK, SD_INFO2_MASK_ALLP);

    /* Cast to an appropriate type */
    info1 = SD_INP(p_hndl, SD_INFO1);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO1, (uint64_t)(info1 & ~SD_INFO1_MASK_TRNS_RESP));

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, SD_INFO2, 0x0000);

    /* reset DMAC */
    ret = sddev_finalize_dma(sd_port);
    if (SD_OK != ret)
    {
        return ret;
    }

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO1_MASK, DM_CM_INFO1_MASK_ALLP);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO2_MASK, DM_CM_INFO2_MASK_ALLP);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO1, (uint64_t)0);

    /* Cast to an appropriate type */
    SD_OUTP(p_hndl, DM_CM_INFO2, (uint64_t)0);

    /* ==== finish peripheral module ==== */
    ret = sddev_finalize(sd_port);

    gp_sdhandle[sd_port] = 0;  /* destruct SD Handle */

    return ret;
}
/******************************************************************************
 End of function sd_finalize
 *****************************************************************************/


/******************************************************************************
 * Function Name: _sd_init_hndl
 * Description  : initialize SD handle.
 *              : initialize following SD handle members
 *              : media_type       : card type
 *              : write_protect    : write protect
 *              : resp_status      : R1/R1b response status
 *              : error            : error detail information
 *              : stop             : compulsory stop flag
 *              : prot_sector_size : sector size (protect area)
 *              : card registers   : ocr, cid, csd, dsr, rca, scr, sdstatus and
 *              : status_data
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint32_t mode       : driver mode
 *              : uint32_t voltage    : working voltage
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_init_hndl(st_sdhndl_t *p_hndl, uint32_t mode, uint32_t voltage)
{
    int32_t i;

    p_hndl->media_type = SD_MEDIA_UNKNOWN;
    p_hndl->write_protect = 0;
    p_hndl->resp_status = STATE_IDEL;
    p_hndl->error = SD_OK;
    p_hndl->stop = 0;
    p_hndl->prot_sector_size = 0;
    p_hndl->voltage = voltage;
    p_hndl->speed_mode = 0;

    /* Cast to an appropriate type */
    p_hndl->int_mode = (uint8_t)(mode & 0x1u);

    /* Cast to an appropriate type */
    p_hndl->trans_mode = (uint8_t)(mode & SD_MODE_DMA);

    /* Cast to an appropriate type */
    p_hndl->sup_card = (uint8_t)(mode & 0x30u);

    /* Cast to an appropriate type */
    p_hndl->sup_speed = (uint16_t)(mode & 0xF040u);

    /* Cast to an appropriate type */
    p_hndl->sup_ver = (uint8_t)(mode & 0x80u);
    if (mode & SD_MODE_1BIT)
    {
        p_hndl->sup_if_mode = SD_PORT_SERIAL;
    }
    else
    {
        p_hndl->sup_if_mode = SD_PORT_PARALLEL;
    }

    /* initialize card registers */
    for (i = 0; i < (4 / sizeof(uint16_t)); ++i)
    {
        p_hndl->ocr[i] = 0;
    }
    for (i = 0; i < (16 / sizeof(uint16_t)); ++i)
    {
        p_hndl->cid[i] = 0;
    }
    for (i = 0; i < (16 / sizeof(uint16_t)); ++i)
    {
        p_hndl->csd[i] = 0;
    }
    for (i = 0; i < (2 / sizeof(uint16_t)); ++i)
    {
        p_hndl->dsr[i] = 0;
    }
    for (i = 0; i < (4 / sizeof(uint16_t)); ++i)
    {
        p_hndl->rca[i] = 0;
    }
    for (i = 0; i < (8 / sizeof(uint16_t)); ++i)
    {
        p_hndl->scr[i] = 0;
    }
    for (i = 0; i < (14 / sizeof(uint16_t)); ++i)
    {
        p_hndl->sdstatus[i] = 0;
    }
    for (i = 0; i < (18 / sizeof(uint16_t)); ++i)
    {
        p_hndl->status_data[i] = 0;
    }
    for (i = 0; i < (4 / sizeof(uint16_t)); ++i)
    {
        p_hndl->if_cond[i] = 0;
    }

    if (SD_MODE_VER2X == p_hndl->sup_ver)
    {
        p_hndl->if_cond[0] = 0;
        p_hndl->if_cond[1] = 0x00aa;
        if (p_hndl->voltage & 0x00FF8000)
        {
            p_hndl->if_cond[1] |= 0x0100; /* high volatege : 2.7V-3.6V */
        }
        if (p_hndl->voltage & 0x00000F00)
        {
            p_hndl->if_cond[1] |= 0x0200; /* low volatege : 1.65V-1.95V */
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_init_hndl
 *****************************************************************************/


/* End of File */
