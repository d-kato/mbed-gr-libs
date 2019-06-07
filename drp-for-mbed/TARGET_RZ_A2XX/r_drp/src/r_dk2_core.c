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
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* System Name  : DRP Driver
* File Name    : r_dk2_core.c
* Device       : RZ
* Abstract     : Control software of DRP.
* Tool-Chain   : Renesas e2studio
* OS           : Not use
* H/W Platform : Renesas Starter Kit
* Description  : Core part of DRP Driver.
* Limitation   : None
*******************************************************************************/
/*******************************************************************************
* History      : History is managed by Revision Control System.
*******************************************************************************/
/*******************************************************************************
Includes <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "drp_iodefine.h"
#include "r_dk2_core.h"
#include "r_dk2_if.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
#ifdef DEBUG
#define BUILD_TYPE  (0x80)
#else
#define BUILD_TYPE  (0)
#endif
#define MAJOR_VER   (0)
#define MINOR_VER   (9)
#define BUILD_NUM   (1)
#define VER         (((BUILD_TYPE) << 24) | ((MAJOR_VER) << 16) | ((MINOR_VER) << 8) | (BUILD_NUM))
#define CONTEXT_MAX (64)
#define STATE_MAX   (256)

#define WR32(addr, data) *(volatile long *)(addr) = (data)
#define WR64(addr, data) *(volatile long long *)(addr) = (data)

/*******************************************************************************
Typedef definitions
*******************************************************************************/

/*******************************************************************************
Imported global variables and functions (from other files)
*******************************************************************************/
static void dk2_init(void);
static void dk2_pre_load(unsigned char id, int zero_clear, int context_max, int state_max, unsigned char *pdesc, unsigned long config, unsigned long size);
static void dk2_load(unsigned char id, unsigned long tilepat, unsigned long tilesrc, unsigned long desc);
static void dk2_post_load(unsigned char id, int state_max);
static void dk2_activate(unsigned char id, unsigned long tilepat, unsigned char div);
static void dk2_pre_start(unsigned char id, unsigned char *pdesc, unsigned long param, unsigned long size);
static void dk2_start(unsigned char id, unsigned long desc);
static void dk2_unload(unsigned char id);
static void dk2_get_int(unsigned long *pdscc, unsigned long *pidif, unsigned long *podif);

static void read_evsel(int core, int state, int evidx, unsigned char *pretval, unsigned char *preaddata);
static void write_evsel(int core, int state, int evidx, unsigned char writeval, unsigned char baseval);
static void reg_write(unsigned char tile, unsigned long addr, unsigned long data);
static void get_tile_info(unsigned char id, int *pshift, int *pnum, unsigned long *pdma);
static void set_dma(unsigned int dma, int shift);

/*******************************************************************************
Private variables and functions(prototypes)
*******************************************************************************/
static uint8_t loaded_tiles = 0;
static uint8_t activated_tiles = 0;
static uint32_t current_tilepat = 0;

#ifdef __GNUC__
__attribute__ ((aligned(16)))
#endif
static volatile unsigned char disc[6][32];

/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_Initialize
* Description  : This function causes DRP to initialized.
* Arguments    : None.
* Return Value : None.
*******************************************************************************/
void R_DK2_CORE_Initialize(void)
{
    dk2_init();
    
    loaded_tiles = 0;
    activated_tiles = 0;
    current_tilepat = 0;
}
/*******************************************************************************
End of function R_DK2_CORE_Initialize
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_PreLoad
* Description  : This function prepares to load DRP configuration data.
* Arguments    : tile_num - Number of tiles.
                 top_tiles - Top tiles of DRP circuit to be loaded.
                 config - Address of configuration data.
                 size - Size of configuration data.
                 del_zero - Flag of deleting zero data.
                 context_num - Context number.
                 state_num - State number.
                 pwork - Address of work area.
                 pid - Address of ID.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_CORE_PreLoad(const uint8_t tile_num, const uint8_t top_tiles, const uint32_t config, const uint32_t size, const bool del_zero, const uint16_t context_num, const uint16_t state_num, void *const pwork, uint8_t *const pid)
{
    int32_t result = R_DK2_SUCCESS;
    int32_t tile_index;
    uint8_t id;
    uint8_t config_id;
    int32_t context;
    int32_t state;
    
    /* check pwork, config and size */
    if ((0 == tile_num) || (tile_num > R_DK2_TILE_NUM) ||
        (0 == top_tiles) || (0 != (top_tiles & ~0x3F)) ||
        (0 == config) || (0 != (config & 0x0000001F)) ||
        (0 == size) || (size > 0xFFFFF) || (0 != (size & 0x00000007)) ||
        (context_num > CONTEXT_MAX) || (state_num > STATE_MAX) ||
        (NULL == pwork) || (0 != ((uint32_t)pwork & 0x0000000F)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /* make id */
    id = 0;
    for (tile_index = 0; tile_index < tile_num; tile_index++)
    {
        id |= (uint8_t)(R_DK2_TILE_0 << tile_index);
    }
    
    if (0 == id)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /* make config_id */
    config_id = 0;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            if (0 == (config_id & (id << tile_index)))
            {
                config_id |= (uint8_t)(id << tile_index);
            }
            else
            {
                result = R_DK2_ERR_OVERWRITE; /* self intersection */
                goto func_end;
            }
        }
    }
    
    context = (0 == context_num) ? CONTEXT_MAX : context_num;
    state = (0 == state_num) ? STATE_MAX : state_num;
    
    dk2_pre_load(config_id, (false != del_zero), context, state, pwork, config, size);
    
    if (0 != pid)
    {
        *pid = id;
    }

func_end:
    return result;
}
/*******************************************************************************
End of function R_DK2_CORE_PreLoad
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_Load
* Description  : This function loads DRP configuration data.
* Arguments    : id - ID of configuration data.
                 top_tiles - Top tiles of DRP circuit to be loaded.
                 tile_pattern - Address of configuration data.
                 state_num - State number.
                 work - Address of work area.
                 paid - Address of ID array.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_CORE_Load(const uint8_t id, const uint8_t top_tiles, const uint32_t tile_pattern, const uint16_t state_num, const uint32_t work, uint8_t *const paid)
{
    int32_t result = R_DK2_SUCCESS;
    int32_t tile_index;
    uint8_t config_id;
    uint32_t tilesrc;
    uint32_t tilesrc_value;
    int32_t state;
    int32_t aid_index;
    
    if ((0 == id) || (0 != (id & ~0x3F)) ||
        (0 == top_tiles) || (0 != (top_tiles & ~0x3F)) ||
        (state_num > STATE_MAX) ||
        (0 == work) || (0 != (work & 0x0000000F)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /* make config_id */
    config_id = 0;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            if (0 == (config_id & (id << tile_index)))
            {
                config_id |= (uint8_t)(id << tile_index);
            }
            else
            {
                result = R_DK2_ERR_OVERWRITE; /* self intersection */
                goto func_end;
            }
        }
    }
    
    /* check config_id */
    if ((0 == config_id) || (0 != (config_id & loaded_tiles)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /**** use user tile pattern ****/
    if (current_tilepat == 0)
    {
        current_tilepat = tile_pattern;
    }
    else
    {
        if (tile_pattern != current_tilepat)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
    /**** use user tile pattern ****/

    /* make tilesrc for copying application */
    tilesrc = 0;
    tilesrc_value = R_DK2_TILE_NUM;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (tilesrc_value < R_DK2_TILE_NUM)
        {
            tilesrc |= (tilesrc_value << (tile_index * 4));
        }
        else if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            tilesrc_value = 0;
            tilesrc |= (tilesrc_value << (tile_index * 4));
        }
        else
        {
            tilesrc |= (uint32_t)(tile_index << (tile_index * 4));
        }
        tilesrc_value++;
        if (0 == (id >> tilesrc_value))
        {
            tilesrc_value = R_DK2_TILE_NUM;
        }
    }
    
    dk2_load(config_id, current_tilepat, tilesrc, work);
    
    /* post load process */
    state = (0 == state_num) ? STATE_MAX : state_num;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            dk2_post_load((unsigned char)(id << tile_index), state);
        }
    }
    
    /* write paid */
    aid_index = 0;
    if (NULL != paid)
    {
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
            {
                paid[aid_index++] = (uint8_t)(id << tile_index);
            }
        }
    }
    
    loaded_tiles |= config_id;
    
func_end:
    return result;
}
/*******************************************************************************
End of function R_DK2_CORE_Load
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_Activate
* Description  : This function activates DRP.
* Arguments    : id - ID of configuration data.
                 div - Division ratio.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_CORE_Activate(const uint8_t id, const uint8_t div)
{
    int32_t result = R_DK2_SUCCESS;
    
    if ((0 == id) || (0 != (id & activated_tiles)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    if (0 == div)
    {
        dk2_activate(id, current_tilepat, 0);
    }
    else if ((div < 4) || (div > 128))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    else
    {
        dk2_activate(id, current_tilepat, (unsigned char)(div - 1));
    }
    
    activated_tiles |= id;

func_end:
    return result;
}
/*******************************************************************************
End of function R_DK2_CORE_Activate
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_PreStart
* Description  : This function prepares to start DRP.
* Arguments    : id - ID of configuration data.
                 pwork - Address of work area.
                 param - Address of parameters.
                 size - Size of parameters.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_CORE_PreStart(const uint8_t id, void *const pwork, const uint32_t param, const uint32_t size)
{
    int32_t result = R_DK2_SUCCESS;
    
    if ((0 == id) || (0 != (id & ~0x3F)) || (NULL == pwork) || (0 == param) || (0 == size) || (size > 0xFFFFFF))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    dk2_pre_start(id, pwork, param, size);
    
func_end:
    return result;
}
/*******************************************************************************
End of function R_DK2_CORE_PreStart
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_Start
* Description  : This function starts DRP.
* Arguments    : id - ID of configuration data.
                 work - Address of work area.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_CORE_Start(const uint8_t id, const uint32_t work)
{
    int32_t result = R_DK2_SUCCESS;
    
    if ((0 == id) || (0 != (id & ~0x3F)) || (0 == work) || (0 != (work & 0x0000000F)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    dk2_start(id, work);
    
func_end:
    return result;
}
/*******************************************************************************
End of function R_DK2_CORE_Start
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_Unload
* Description  : This function unloads DRP.
* Arguments    : id - ID of configuration data.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_CORE_Unload(const uint8_t id)
{
    int32_t result = R_DK2_SUCCESS;
    
    if ((0 == id) || (0 != (id & ~0x3F)) || (id != (id & loaded_tiles)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    dk2_unload(id);
    
    activated_tiles &= (uint8_t)~id;
    loaded_tiles &= (uint8_t)~id;
    if (0 == loaded_tiles)
    {
        current_tilepat = 0;
    }
    
func_end:
    return result;
}
/*******************************************************************************
End of function R_DK2_CORE_Unload
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_GetInt
* Description  : This function gets interrupt status of DRP.
* Arguments    : None.
* Return Value : Interrupt status of DRP.
*******************************************************************************/
uint32_t R_DK2_CORE_GetInt(void)
{
    uint32_t dscc;
    uint32_t idif;
    uint32_t odif;

    dk2_get_int(&dscc, &idif, &odif);

    return (odif << 16);
}
/*******************************************************************************
End of function R_DK2_CORE_GetInt
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_CORE_GetVersion
* Description  : This function gets version of DRP driver.
* Arguments    : None.
* Return Value : Version of DRP.
*******************************************************************************/
uint32_t R_DK2_CORE_GetVersion(void)
{
    return VER;
}
/*******************************************************************************
End of function R_DK2_CORE_GetVersion
*******************************************************************************/

/*******************************************************************************
Private functions
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_init
* Description  : This function causes to initialize DRP at H/W level.
* Arguments    : None.
* Return Value : None.
*******************************************************************************/
static void dk2_init(void)
{
    DRP_DCLKINACT         = 0x00000001; // 0xEAFFD820
    
    DRP_CMREG_TILEWE      = 0x000000FF; // 0xEAFD6000
    DRP_CMREG_TILESRC     = 0x76543210; // 0xEAFD6004
    
    DRP_regSftRst0        = 0x00000000; // 0xEAFD4060
    DRP_regSftRst1        = 0x00000000; // 0xEAFD4068
    DRP_regSftRst2        = 0x00000000; // 0xEAFD4070
    DRP_regSftRst3        = 0x00000000; // 0xEAFD4078
    DRP_regSftRst4        = 0x00000000; // 0xEAFD4080
    DRP_regSftRst5        = 0x00000000; // 0xEAFD4088
    DRP_regClkRst0        = 0x00000000; // 0xEAFD4020
    DRP_regClkRst1        = 0x00000000; // 0xEAFD4028
    DRP_regClkRst2        = 0x00000000; // 0xEAFD4030
    DRP_regClkRst3        = 0x00000000; // 0xEAFD4038
    DRP_regClkRst4        = 0x00000000; // 0xEAFD4040
    DRP_regClkRst5        = 0x00000000; // 0xEAFD4048
#ifdef DRP_USE_STBY
    DRP_regStandby        = 0x0000003F; // 0xEAFD4018
    while (0x3F != (DRP_StandbyOut & 0x3F)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWait    = 0x0000003F; // 0xEAFD40F0
#endif
    DRP_T0_DFCCTRL        = 0x00000000; // 0xEA1D2518
    DRP_T1_DFCCTRL        = 0x00000000; // 0xEA3D2518
    DRP_T2_DFCCTRL        = 0x00000000; // 0xEA5D2518
    DRP_T3_DFCCTRL        = 0x00000000; // 0xEA7D2518
    DRP_T4_DFCCTRL        = 0x00000000; // 0xEA9D2518
    DRP_T5_DFCCTRL        = 0x00000000; // 0xEABD2518
    DRP_T0_DIV            = 0x00000003; // 0xEA1D2510
    DRP_T1_DIV            = 0x00000003; // 0xEA3D2510
    DRP_T2_DIV            = 0x00000003; // 0xEA5D2510
    DRP_T3_DIV            = 0x00000003; // 0xEA7D2510
    DRP_T4_DIV            = 0x00000003; // 0xEA9D2510
    DRP_T5_DIV            = 0x00000003; // 0xEABD2510
    DRP_T0_TILECLKE       = 0x00000001; // 0xEA1D2508
    DRP_T1_TILECLKE       = 0x00000001; // 0xEA3D2508
    DRP_T2_TILECLKE       = 0x00000001; // 0xEA5D2508
    DRP_T3_TILECLKE       = 0x00000001; // 0xEA7D2508
    DRP_T4_TILECLKE       = 0x00000001; // 0xEA9D2508
    DRP_T5_TILECLKE       = 0x00000001; // 0xEABD2508
#ifdef DRP_USE_STBY
    DRP_regStandby        = 0x00000000; // 0xEAFD4018
    while (0 != (DRP_StandbyOut & 0x3F)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWaitClr = 0x0000003F; // 0xEAFD40F8
#endif
    DRP_T0_RegBResetZ     = 0x00000001; // 0xEA1B0070
    DRP_T1_RegBResetZ     = 0x00000001; // 0xEA3B0070
    DRP_T2_RegBResetZ     = 0x00000001; // 0xEA5B0070
    DRP_T3_RegBResetZ     = 0x00000001; // 0xEA7B0070
    DRP_T4_RegBResetZ     = 0x00000001; // 0xEA9B0070
    DRP_T5_RegBResetZ     = 0x00000001; // 0xEABB0070
    
    DRP_SFTRST            = 0x00000000; // 0xEAFFD800
    DRP_CLKE              = 0x01FF03FF; // 0xEAFFD810
    
#ifdef DRP_USE_STBY
    DRP_regStandby        = 0x0000003F; // 0xEAFD4018
    while (0x3F != (DRP_StandbyOut & 0x3F)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWait    = 0x0000003F; // 0xEAFD40F0
#endif
}
/*******************************************************************************
End of function dk2_init
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_pre_load
* Description  : This function prepares to load DRP at H/W level.
* Arguments    : id - ID of configuration data.
                 zero_clear - Flag of clearing zero data.
                 context_max - Maximum of context.
                 state_max - Maximum of state.
                 pdesc - Address of descriptor.
                 config - Address of configuration data.
                 size - Size of configuration data.
* Return Value : None.
*******************************************************************************/
static void dk2_pre_load(unsigned char id, int zero_clear, int context_max, int state_max, unsigned char *pdesc, unsigned long config, unsigned long size)
{
    int shift;
    unsigned long offset;
    int i;
    int j;
    
    shift = 0;
    while (0 == ((id >> shift) & 1))
    {
        shift++;
    }
    
    if (0 != zero_clear)
    {
        DRP_CMREG_TILEWE   = 0xC0 | id; // 0xEAFD6000
        
        if (0 != (id & 0x01))
        {
            DRP_CMREG_TILESRC  = 0x76000000; // 0xEAFD6004
            offset = 0x000000;
        }
        else if (0 != (id & 0x02))
        {
            DRP_CMREG_TILESRC  = 0x76111110; // 0xEAFD6004
            offset = 0x200000;
        }
        else if (0 != (id & 0x04))
        {
            DRP_CMREG_TILESRC  = 0x76222210; // 0xEAFD6004
            offset = 0x400000;
        }
        else if (0 != (id & 0x08))
        {
            DRP_CMREG_TILESRC  = 0x76333210; // 0xEAFD6004
            offset = 0x600000;
        }
        else if (0 != (id & 0x10))
        {
            DRP_CMREG_TILESRC  = 0x76443210; // 0xEAFD6004
            offset = 0x800000;
        }
        else if (0 != (id & 0x20))
        {
            DRP_CMREG_TILESRC  = 0x76543210; // 0xEAFD6004
            offset = 0xA00000;
        }
        
        WR32(0xEA1B0010 | offset, 0x9);
        
        // Pelu
        for (i = 0; i < 32; i++)
        {
            for (j = 0; j < context_max; j++)
            {
                WR64(0xEA000000 | offset | ((unsigned long)i << 11) | ((unsigned long)j << 5), 0);
            }
        }
        
        // Sblu,Stclu
        for (j = 0; j < 2; j++)
        {
            WR64(0xEA010000 | offset | ((unsigned long)j << 3), 0);
        }
        
        // PeRfu
        for (j = 0; j < 7; j++)
        {
            WR64(0xEA020000 | offset | ((unsigned long)j << 7), 0);
        }
        
        // StcSTbl
        for (i = 0; i < 24; i++)
        {
            for (j = 0; j < state_max; j++)
            {
                WR64(0xEA090000 | offset | ((unsigned long)i << 11) | ((unsigned long)j << 3), 0);
            }
        }
        
        WR32(0xEA1B0010 | offset, 0x0);
        
        DRP_CMREG_TILEWE   = 0xFF; // 0xEAFD6000
        DRP_CMREG_TILESRC  = 0x76543210; // 0xEAFD6004
    }
    
    if (0 != (id & 0x01))
    {
        DRP_regSftRst0 = 0x00000000; /* DRPKSFTRST0 EAFD4060 */
    }
    if (0 != (id & 0x02))
    {
        DRP_regSftRst1 = 0x00000000; /* DRPKSFTRST1 EAFD4068 */
    }
    if (0 != (id & 0x04))
    {
        DRP_regSftRst2 = 0x00000000; /* DRPKSFTRST2 EAFD4070 */
    }
    if (0 != (id & 0x08))
    {
        DRP_regSftRst3 = 0x00000000; /* DRPKSFTRST3 EAFD4078 */
    }
    if (0 != (id & 0x10))
    {
        DRP_regSftRst4 = 0x00000000; /* DRPKSFTRST4 EAFD4080 */
    }
    if (0 != (id & 0x20))
    {
        DRP_regSftRst5 = 0x00000000; /* DRPKSFTRST5 EAFD4088 */
    }
    
    if (0 != (id & 0x01))
    {
        DRP_regClkRst0 = 0x00000000; /* CLKGENRST0 EAFD4020 */
    }
    if (0 != (id & 0x02))
    {
        DRP_regClkRst1 = 0x00000000; /* CLKGENRST1 EAFD4028 */
    }
    if (0 != (id & 0x04))
    {
        DRP_regClkRst2 = 0x00000000; /* CLKGENRST2 EAFD4030 */
    }
    if (0 != (id & 0x08))
    {
        DRP_regClkRst3 = 0x00000000; /* CLKGENRST3 EAFD4038 */
    }
    if (0 != (id & 0x10))
    {
        DRP_regClkRst4 = 0x00000000; /* CLKGENRST4 EAFD4040 */
    }
    if (0 != (id & 0x20))
    {
        DRP_regClkRst5 = 0x00000000; /* CLKGENRST5 EAFD4048 */
    }
    
#ifdef DRP_USE_STBY
    DRP_regStandby = ((DRP_regStandby & ~0xC0uL) | id); // 0xEAFD4018
    while (id != (DRP_StandbyOut & id)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWait = ((DRP_regStandbyWait & ~0xC0uL) | id); // 0xEAFD40F0
#endif
    
    if (0 != (id & 0x01))
    {
        DRP_T0_DFCCTRL = 0; // 0xEA1D2518
        DRP_T0_DIV = 0x00000003; // 0xEA1D2510
    }
    if (0 != (id & 0x02))
    {
        DRP_T1_DFCCTRL = 0; // 0xEA3D2518
        DRP_T1_DIV = 0x00000003; // 0xEA3D2510
    }
    if (0 != (id & 0x04))
    {
        DRP_T2_DFCCTRL = 0; // 0xEA5D2518
        DRP_T2_DIV = 0x00000003; // 0xEA5D2510
    }
    if (0 != (id & 0x08))
    {
        DRP_T3_DFCCTRL = 0; // 0xEA7D2518
        DRP_T3_DIV = 0x00000003; // 0xEA7D2510
    }
    if (0 != (id & 0x10))
    {
        DRP_T4_DFCCTRL = 0; // 0xEA9D2518
        DRP_T4_DIV = 0x00000003; // 0xEA9D2510
    }
    if (0 != (id & 0x20))
    {
        DRP_T5_DFCCTRL = 0; // 0xEABD2518
        DRP_T5_DIV = 0x00000003; // 0xEABD2510
    }
    if (0 != (id & 0x01))
    {
        DRP_T0_TILECLKE = 0x00000001; // 0xEA1D2508
    }
    if (0 != (id & 0x02))
    {
        DRP_T1_TILECLKE = 0x00000001; // 0xEA3D2508
    }
    if (0 != (id & 0x04))
    {
        DRP_T2_TILECLKE = 0x00000001; // 0xEA5D2508
    }
    if (0 != (id & 0x08))
    {
        DRP_T3_TILECLKE = 0x00000001; // 0xEA7D2508
    }
    if (0 != (id & 0x10))
    {
        DRP_T4_TILECLKE = 0x00000001; // 0xEA9D2508
    }
    if (0 != (id & 0x20))
    {
        DRP_T5_TILECLKE = 0x00000001; // 0xEABD2508
    }
    
#ifdef DRP_USE_STBY
    DRP_regStandby = (DRP_regStandby & ~(0xC0uL | id)); // 0xEAFD4018
    while (0 != (DRP_StandbyOut & id)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWaitClr = id; // 0xEAFD40F8
#endif
    
    if (0 != (id & 0x01))
    {
        DRP_T0_RegBResetZ = 0x00000001; // 0xEA1B0070
    }
    if (0 != (id & 0x02))
    {
        DRP_T1_RegBResetZ = 0x00000001; // 0xEA3B0070
    }
    if (0 != (id & 0x04))
    {
        DRP_T2_RegBResetZ = 0x00000001; // 0xEA5B0070
    }
    if (0 != (id & 0x08))
    {
        DRP_T3_RegBResetZ = 0x00000001; // 0xEA7B0070
    }
    if (0 != (id & 0x10))
    {
        DRP_T4_RegBResetZ = 0x00000001; // 0xEA9B0070
    }
    if (0 != (id & 0x20))
    {
        DRP_T5_RegBResetZ = 0x00000001; // 0xEABB0070
    }
    
    DRP_SFTRST &= ~(((unsigned long)id << 16) | (unsigned long)id); /* DMACSFTRST EAFFD800 */
    DRP_CLKE   |= (((unsigned long)id << 16) | (unsigned long)id); /* DMACCLKE EAFFD810 */
    
    // Load Config.dat
    pdesc[ 0] = 0x00;                                         //   0-  7
    pdesc[ 1] = 0x00;                                         //   8- 15
    pdesc[ 2] = 0x00;                                         //  16- 23
    pdesc[ 3] = 0x13;                                         //  24- 31
    pdesc[ 4] = (unsigned char)( config & 0x000000E0       ); //  32- 39
    pdesc[ 5] = (unsigned char)((config & 0x0000FF00) >>  8); //  40- 47
    pdesc[ 6] = (unsigned char)((config & 0x00FF0000) >> 16); //  48- 55
    pdesc[ 7] = (unsigned char)((config & 0xFF000000) >> 24); //  56- 63
    pdesc[ 8] = (unsigned char)(   size & 0x000000F8       ); //  64- 71
    pdesc[ 9] = (unsigned char)((  size & 0x0000FF00) >>  8); //  72- 79
    pdesc[10] = (unsigned char)((  size & 0x000F0000) >> 16); //  80- 87
    pdesc[11] = 0x00;                                         //  88- 95
    pdesc[12] = 0x00;                                         //  96-103
    pdesc[13] = 0x00;                                         // 104-111
    pdesc[14] = 0x00;                                         // 112-119
    pdesc[15] = 0x00;                                         // 120-127
}
/*******************************************************************************
End of function dk2_pre_load
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_load
* Description  : This function loads DRP configuration data at H/W level.
* Arguments    : id - ID of configuration data.
                 tilepat - Tile pattern.
                 tilesrc - Tile source.
                 desc - Address of descriptor.
* Return Value : None.
*******************************************************************************/
static void dk2_load(unsigned char id, unsigned long tilepat, unsigned long tilesrc, unsigned long desc)
{
    int shift;
    
    shift = 0;
    while (0 == ((id >> shift) & 1))
    {
        shift++;
    }
    
//    DRP_CMREG_TILEWE  = 0xC0 | id;            // 0xEAFD6000
//    DRP_CMREG_TILESRC = 0x76543210;           // 0xEAFD6004
    
#if 1 /* CFG debug */
    DRP_CFGL_INT_STS  = 0x00000001;           // 0xEAFF0000
#else
    DRP_DSCC_INT      = (1 << shift);         // 0xEAFF8000 DMA int clear
#endif
    
    DRP_TILEPAT       = tilepat;              // 0xEAFD5100
    DRP_CMCFG_TILEWE  = 0xC0 | id;            // 0xEAFFE000
    DRP_CMCFG_TILESRC = 0x76000000 | tilesrc; // 0xEAFFE004
    
    DRP_IDIF_DMACNTCW = 0x00040001;           // 0xEAFF9900
    
    if (0 != (id & 0x01))
    {
        DRP_DSCC_DCTLI0 = 0x00000000; // 0xEAFF8100
        DRP_DSCC_DPAI0  = desc;       // 0xEAFF8108
        DRP_DSCC_DCTLI0 = 0x00000001; // 0xEAFF8100
    }
    else if (0 != (id & 0x02))
    {
        DRP_DSCC_DCTLI1 = 0x00000000; // 0xEAFF8140
        DRP_DSCC_DPAI1  = desc;       // 0xEAFF8148
        DRP_DSCC_DCTLI1 = 0x00000001; // 0xEAFF8140
    }
    else if (0 != (id & 0x04))
    {
        DRP_DSCC_DCTLI2 = 0x00000000; // 0xEAFF8180
        DRP_DSCC_DPAI2  = desc;       // 0xEAFF8188
        DRP_DSCC_DCTLI2 = 0x00000001; // 0xEAFF8180
    }
    else if (0 != (id & 0x08))
    {
        DRP_DSCC_DCTLI3 = 0x00000000; // 0xEAFF81C0
        DRP_DSCC_DPAI3  = desc;       // 0xEAFF81C8
        DRP_DSCC_DCTLI3 = 0x00000001; // 0xEAFF81C0
    }
    else if (0 != (id & 0x10))
    {
        DRP_DSCC_DCTLI4 = 0x00000000; // 0xEAFF8200
        DRP_DSCC_DPAI4  = desc;       // 0xEAFF8208
        DRP_DSCC_DCTLI4 = 0x00000001; // 0xEAFF8200
    }
    else if (0 != (id & 0x20))
    {
        DRP_DSCC_DCTLI5 = 0x00000000; // 0xEAFF8240
        DRP_DSCC_DPAI5  = desc;       // 0xEAFF8248
        DRP_DSCC_DCTLI5 = 0x00000001; // 0xEAFF8240
    }
    
#if 1 /* CFG debug */
    while (0 == DRP_CFGL_INT_STS) // 0xEAFFD000
    {
        ;
    }
#else
    while (0 == (DRP_DSCC_INT & (1 << shift))) // 0xEAFF8000
    {
        ;
    }
#endif
}
/*******************************************************************************
End of function dk2_load
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_post_load
* Description  : This function post-processes loading DRP configuration data at H/W level.
* Arguments    : id - ID of configuration data.
                 state_max - Maximum of state.
* Return Value : None.
*******************************************************************************/
static void dk2_post_load(unsigned char id, int state_max)
{
    unsigned char readdata;
    unsigned char ev_val;
    int state;
    int ev_idx;
    
    for (state = 0; state < state_max; state++)
    {
        for (ev_idx = 0; ev_idx < 4; ev_idx++)
        {
            if (0x38 == id) /* 3Tile app: Tile0 -> Tile3 */
            {
                read_evsel(3, state, ev_idx, &ev_val, &readdata);
                if (0x8 == ev_val)
                {
                    write_evsel(3, state, ev_idx, 0xB, readdata);
                }
                read_evsel(5, state, ev_idx, &ev_val, &readdata);
                if (0x9 == ev_val)
                {
                    write_evsel(5, state, ev_idx, 0x8, readdata);
                }
            }
            else if (0x0C == id) /* 2Tile app: Tile0 -> Tile2 */
            {
                read_evsel(2, state, ev_idx, &ev_val, &readdata);
                if (0x8 == ev_val)
                {
                    write_evsel(2, state, ev_idx, 0xA, readdata);
                }
                read_evsel(3, state, ev_idx, &ev_val, &readdata);
                if (0x8 == ev_val)
                {
                    write_evsel(3, state, ev_idx, 0xA, readdata);
                }
            }
            else if (0x18 == id) /* 2Tile app: Tile0 -> Tile3 */
            {
                read_evsel(3, state, ev_idx, &ev_val, &readdata);
                if (0x8 == ev_val)
                {
                    write_evsel(3, state, ev_idx, 0xB, readdata);
                }
                read_evsel(4, state, ev_idx, &ev_val, &readdata);
                if (0x8 == ev_val)
                {
                    write_evsel(4, state, ev_idx, 0xB, readdata);
                }
            }
        }
    }
    
#ifdef DRP_USE_STBY
    DRP_regStandby = ((DRP_regStandby & ~0xC0uL) | id); // 0xEAFD4018
    while (id != (DRP_StandbyOut & id)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWait = ((DRP_regStandbyWait & ~0xC0uL) | id); // 0xEAFD40F0
#endif
}
/*******************************************************************************
End of function dk2_post_load
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_activate
* Description  : This function activates DRP at H/W level.
* Arguments    : id - ID of configuration data.
                 tilepat - Tile pattern.
                 div - Division ratio.
* Return Value : None.
*******************************************************************************/
static void dk2_activate(unsigned char id, unsigned long tilepat, unsigned char div)
{
    volatile unsigned int t0_dma,t1_dma,t2_dma,t3_dma,t4_dma,t5_dma;
    int shift;
    unsigned long dma;
    unsigned long reg_run_sel;
    
    get_tile_info(id, &shift, 0, &dma);
    
    reg_run_sel = (0x8 | (unsigned long)shift);
    
//    DRP_CMREG_TILEWE  = 0xC0 | id;  // 0xEAFD6000
//    DRP_CMREG_TILESRC = 0x76543210; // 0xEAFD6004
    
#ifdef DRP_USE_STBY
    DRP_regStandby = (DRP_regStandby & ~(0xC0uL | id)); // 0xEAFD4018
    while (0 != (DRP_StandbyOut & id)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWaitClr = id; // 0xEAFD40F8
#endif
    
    t0_dma = 0;
    t1_dma = 0;
    t2_dma = 0;
    t3_dma = 0;
    t4_dma = 0;
    t5_dma = 0;
    if (tilepat == 0x00)
    {
        if((id & 0x01) == 0x01){
            DRP_T0_DMACHE = 0x00000011;
            t0_dma = 0x00000011;
        }
        if((id & 0x02) == 0x02){
            DRP_T1_DMACHE = 0x00000011;
            t1_dma = 0x00000011;
        }
        if((id & 0x04) == 0x04){
            DRP_T2_DMACHE = 0x00000011;
            t2_dma = 0x00000011;
        }
        if((id & 0x08) == 0x08){
            DRP_T3_DMACHE = 0x00000011;
            t3_dma = 0x00000011;
        }
        if((id & 0x10) == 0x10){
            DRP_T4_DMACHE = 0x00000011;
            t4_dma = 0x00000011;
        }
        if((id & 0x20) == 0x20){
            DRP_T5_DMACHE = 0x00000011;
            t5_dma = 0x00000011;
        }
    }
    if (tilepat == 0x10)
    {
        if((id & 0x03) == 0x03){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t1_dma = 0x00000030;
        }
        if((id & 0x04) == 0x04){
            DRP_T2_DMACHE = 0x00000011;
            t2_dma = 0x00000011;
        }
        if((id & 0x08) == 0x08){
            DRP_T3_DMACHE = 0x00000011;
            t3_dma = 0x00000011;
        }
        if((id & 0x10) == 0x10){
            DRP_T4_DMACHE = 0x00000011;
            t4_dma = 0x00000011;
        }
        if((id & 0x20) == 0x20){
            DRP_T5_DMACHE = 0x00000011;
            t5_dma = 0x00000011;
        }
    }
    if (tilepat == 0x11)
    {
        if((id & 0x03) == 0x03){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t1_dma = 0x00000030;
        }
        if((id & 0x0C) == 0x0C){
            DRP_T2_DMACHE = 0x00000003;
            DRP_T3_DMACHE = 0x00000030;
            t2_dma = 0x00000003;
            t3_dma = 0x00000030;
        }
        if((id & 0x10) == 0x10){
            DRP_T4_DMACHE = 0x00000011;
            t4_dma = 0x00000011;
        }
        if((id & 0x20) == 0x20){
            DRP_T5_DMACHE = 0x00000011;
            t5_dma = 0x00000011;
        }
    }
    if (tilepat == 0x12)
    {
        if((id & 0x03) == 0x03){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t1_dma = 0x00000030;
        }
        if((id & 0x0C) == 0x0C){
            DRP_T2_DMACHE = 0x00000003;
            DRP_T3_DMACHE = 0x00000030;
            t2_dma = 0x00000003;
            t3_dma = 0x00000030;
        }
        if((id & 0x30) == 0x30){
            DRP_T4_DMACHE = 0x00000003;
            DRP_T5_DMACHE = 0x00000030;
            t4_dma = 0x00000003;
            t5_dma = 0x00000030;
        }
    }
    if (tilepat == 0x20)
    {
        if((id & 0x07) == 0x07){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000000;
            DRP_T2_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t2_dma = 0x00000030;
        }
        if((id & 0x08) == 0x08){
            DRP_T3_DMACHE = 0x00000011;
            t3_dma = 0x00000011;
        }
        if((id & 0x10) == 0x10){
            DRP_T4_DMACHE = 0x00000011;
            t4_dma = 0x00000011;
        }
        if((id & 0x20) == 0x20){
            DRP_T5_DMACHE = 0x00000011;
            t5_dma = 0x00000011;
        }
    }
    if (tilepat == 0x21)
    {
        if((id & 0x07) == 0x07){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000000;
            DRP_T2_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t2_dma = 0x00000030;
        }
        if((id & 0x18) == 0x18){
            DRP_T3_DMACHE = 0x00000003;
            DRP_T4_DMACHE = 0x00000030;
            t3_dma = 0x00000003;
            t4_dma = 0x00000030;
        }
        if((id & 0x20) == 0x20){
            DRP_T5_DMACHE = 0x00000011;
            t5_dma = 0x00000011;
        }
    }
    if (tilepat == 0x22)
    {
        if((id & 0x07) == 0x07){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000000;
            DRP_T2_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t2_dma = 0x00000030;
        }
        if((id & 0x38) == 0x38){
            DRP_T3_DMACHE = 0x00000003;
            DRP_T4_DMACHE = 0x00000000;
            DRP_T5_DMACHE = 0x00000030;
            t3_dma = 0x00000003;
            t5_dma = 0x00000030;
        }
    }
    if (tilepat == 0x30)
    {
        if((id & 0x0F) == 0x0F){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000000;
            DRP_T2_DMACHE = 0x00000000;
            DRP_T3_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t3_dma = 0x00000030;
        }
        if((id & 0x10) == 0x10){
            DRP_T4_DMACHE = 0x00000011;
            t4_dma = 0x00000011;
        }
        if((id & 0x20) == 0x20){
            DRP_T5_DMACHE = 0x00000011;
            t5_dma = 0x00000011;
        }
    }
    if (tilepat == 0x31)
    {
        if((id & 0x0F) == 0x0F){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000000;
            DRP_T2_DMACHE = 0x00000000;
            DRP_T3_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t3_dma = 0x00000030;
        }
        if((id & 0x30) == 0x30){
            DRP_T4_DMACHE = 0x00000003;
            DRP_T5_DMACHE = 0x00000030;
            t4_dma = 0x00000003;
            t5_dma = 0x00000030;
        }
    }
    if (tilepat == 0x40)
    {
        if((id & 0x1F) == 0x1F){
            DRP_T0_DMACHE = 0x00000003;
            DRP_T1_DMACHE = 0x00000000;
            DRP_T2_DMACHE = 0x00000000;
            DRP_T3_DMACHE = 0x00000000;
            DRP_T4_DMACHE = 0x00000030;
            t0_dma = 0x00000003;
            t4_dma = 0x00000030;
        }
        if((id & 0x20) == 0x20){
            DRP_T5_DMACHE = 0x00000011;
            t5_dma = 0x00000011;
        }
    }
    if (tilepat == 0x50)
    {
        if((id & 0x3F) == 0x3F){
            DRP_T0_DMACHE = 0x0000000F;
            DRP_T1_DMACHE = 0x00000000;
            DRP_T2_DMACHE = 0x00000000;
            DRP_T3_DMACHE = 0x00000000;
            DRP_T4_DMACHE = 0x00000000;
            DRP_T5_DMACHE = 0x000000F0;
            t0_dma = 0x0000000F;
            t5_dma = 0x000000F0;
        }
    }
    
// Tile 0 set
    if ((id & 0x01) != 0)
    {
        DRP_T0_RegCfgmReadEn = 0x00000001; // 0xEA118040
        DRP_T0_RegCfgmReadEn = 0x00000000; // 0xEA118040
        if((id & 0x01) == 1){
            DRP_T0_RegConfigEn = 0x00000001; // 0xEA118048
        }else{
            DRP_T0_RegConfigEn = 0x00000000; // 0xEA118048
        }
        DRP_T0_RegReset = 0x00000000; // 0xEA118010
        if((id & 0x01) == 1){
            DRP_T0_RegRunSel = reg_run_sel; // 0xEA118078
        }else{
            DRP_T0_RegRunSel = 0x00000000; // 0xEA118078
        }
    }
// Tile 1 set
    if ((id & 0x02) != 0)
    {
        DRP_T1_RegCfgmReadEn = 0x00000001; // 0xEA318040
        DRP_T1_RegCfgmReadEn = 0x00000000; // 0xEA318040
        if(((id & 0x02) >> 1) == 1){
            DRP_T1_RegConfigEn = 0x00000001; // 0xEA318048
        }else{
            DRP_T1_RegConfigEn = 0x00000000; // 0xEA318048
        }
        DRP_T1_RegReset = 0x00000000; // 0xEA318010
        if(((id & 0x02) >> 1) == 1){
            DRP_T1_RegRunSel = reg_run_sel; // 0xEA318078
        }else{
            DRP_T1_RegRunSel = 0x00000000; // 0xEA318078
        }
    }
// Tile 2 set
    if ((id & 0x04) != 0)
    {
        DRP_T2_RegCfgmReadEn = 0x00000001; // 0xEA518040
        DRP_T2_RegCfgmReadEn = 0x00000000; // 0xEA518040
        if(((id & 0x04) >> 2) == 1){
            DRP_T2_RegConfigEn = 0x00000001; // 0xEA518048
        }else{
            DRP_T2_RegConfigEn = 0x00000000; // 0xEA518048
        }
        DRP_T2_RegReset = 0x00000000; // 0xEA518010
        if(((id & 0x04) >> 2) == 1){
            DRP_T2_RegRunSel = reg_run_sel; // 0xEA518078
        }else{
            DRP_T2_RegRunSel = 0x00000000; // 0xEA518078
        }
    }
// Tile 3 set
    if ((id & 0x08) != 0)
    {
        DRP_T3_RegCfgmReadEn = 0x00000001; // 0xEA718040
        DRP_T3_RegCfgmReadEn = 0x00000000; // 0xEA718040
        if(((id & 0x08) >> 3) == 1){
            DRP_T3_RegConfigEn = 0x00000001; // 0xEA718048
        }else{
            DRP_T3_RegConfigEn = 0x00000000; // 0xEA718048
        }
        DRP_T3_RegReset = 0x00000000; // 0xEA718010
        if(((id & 0x08) >> 3) == 1){
            DRP_T3_RegRunSel = reg_run_sel; // 0xEA718078
        }else{
            DRP_T3_RegRunSel = 0x00000000; // 0xEA718078
        }
    }
// Tile 4 set
    if ((id & 0x10) != 0)
    {
        DRP_T4_RegCfgmReadEn = 0x00000001; // 0xEA918040
        DRP_T4_RegCfgmReadEn = 0x00000000; // 0xEA918040
        if(((id & 0x10) >> 4) == 1){
            DRP_T4_RegConfigEn = 0x00000001; // 0xEA918048
        }else{
            DRP_T4_RegConfigEn = 0x00000000; // 0xEA918048
        }
        DRP_T4_RegReset = 0x00000000; // 0xEA918010
        if(((id & 0x10) >> 4) == 1){
            DRP_T4_RegRunSel = reg_run_sel; // 0xEA918078
        }else{
            DRP_T4_RegRunSel = 0x00000000; // 0xEA918078
        }
    }
// Tile 5 set
    if ((id & 0x20) != 0)
    {
        DRP_T5_RegCfgmReadEn = 0x00000001; // 0xEAB18040
        DRP_T5_RegCfgmReadEn = 0x00000000; // 0xEAB18040
        if(((id & 0x20) >> 5) == 1){
            DRP_T5_RegConfigEn = 0x00000001; // 0xEAB18048
        }else{
            DRP_T5_RegConfigEn = 0x00000000; // 0xEAB18048
        }
        DRP_T5_RegReset = 0x00000000; // 0xEAB18010
        if(((id & 0x20) >> 5) == 1){
            DRP_T5_RegRunSel = reg_run_sel; // 0xEAB18078
        }else{
            DRP_T5_RegRunSel = 0x00000000; // 0xEAB18078
        }
    }
    
    set_dma(t0_dma, shift);
    set_dma(t1_dma, shift);
    set_dma(t2_dma, shift);
    set_dma(t3_dma, shift);
    set_dma(t4_dma, shift);
    set_dma(t5_dma, shift);
    
    DRP_ODIF_INTMSKCLR = (1uL << shift); // 0xEAFFA004 Clear ODIF INT mask
    DRP_DSCC_INT       = (1uL << shift); // 0xEAFF8000 Clear DSCC INT
    
    /* set div or dfc */
#ifdef DRP_USE_STBY
    DRP_regStandby = ((DRP_regStandby & ~0xC0uL) | id); // 0xEAFD4018
    while (id != (DRP_StandbyOut & id)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWait = ((DRP_regStandbyWait & ~0xC0uL) | id); // 0xEAFD40F0
#endif
    if (0 != div)
    {
        if (0 != (id & 0x01))
        {
            DRP_T0_DFCCTRL = 0; // 0xEA1D2518
            DRP_T0_DIV = div; // 0xEA1D2510
        }
        if (0 != (id & 0x02))
        {
            DRP_T1_DFCCTRL = 0; // 0xEA3D2518
            DRP_T1_DIV = div; // 0xEA3D2510
        }
        if (0 != (id & 0x04))
        {
            DRP_T2_DFCCTRL = 0; // 0xEA5D2518
            DRP_T2_DIV = div; // 0xEA5D2510
        }
        if (0 != (id & 0x08))
        {
            DRP_T3_DFCCTRL = 0; // 0xEA7D2518
            DRP_T3_DIV = div; // 0xEA7D2510
        }
        if (0 != (id & 0x10))
        {
            DRP_T4_DFCCTRL = 0; // 0xEA9D2518
            DRP_T4_DIV = div; // 0xEA9D2510
        }
        if (0 != (id & 0x20))
        {
            DRP_T5_DFCCTRL = 0; // 0xEABD2518
            DRP_T5_DIV = div; // 0xEABD2510
        }
    }
    else
    {
        if (0 != (id & 0x01))
        {
            DRP_T0_DFCCTRL = 1; // 0xEA1D2518
        }
        if (0 != (id & 0x02))
        {
            DRP_T1_DFCCTRL = 1; // 0xEA3D2518
        }
        if (0 != (id & 0x04))
        {
            DRP_T2_DFCCTRL = 1; // 0xEA5D2518
        }
        if (0 != (id & 0x08))
        {
            DRP_T3_DFCCTRL = 1; // 0xEA7D2518
        }
        if (0 != (id & 0x10))
        {
            DRP_T4_DFCCTRL = 1; // 0xEA9D2518
        }
        if (0 != (id & 0x20))
        {
            DRP_T5_DFCCTRL = 1; // 0xEABD2518
        }
    }
#ifdef DRP_USE_STBY
    DRP_regStandby = (DRP_regStandby & ~(0xC0uL | id)); // 0xEAFD4018
    while (0 != (DRP_StandbyOut & id)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWaitClr = id; // 0xEAFD40F8
#endif
    
    if ((id & 0x01) != 0)
    {
        DRP_T0_RegRun = 1; // 0xEA118008
    }
    else if ((id & 0x02) != 0)
    {
        DRP_T1_RegRun = 1; // 0xEA318008
    }
    else if ((id & 0x04) != 0)
    {
        DRP_T2_RegRun = 1; // 0xEA518008
    }
    else if ((id & 0x08) != 0)
    {
        DRP_T3_RegRun = 1; // 0xEA718008
    }
    else if ((id & 0x10) != 0)
    {
        DRP_T4_RegRun = 1; // 0xEA918008
    }
    else if ((id & 0x20) != 0)
    {
        DRP_T5_RegRun = 1; // 0xEAB18008
    }
}
/*******************************************************************************
End of function dk2_activate
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_pre_start
* Description  : This function prepares to start DRP at H/W level.
* Arguments    : id - ID of configuration data.
                 pdesc - Address of descriptor.
                 param - Address of parameters.
                 size - Size of parameters.
* Return Value : None.
*******************************************************************************/
static void dk2_pre_start(unsigned char id, unsigned char *pdesc, unsigned long param, unsigned long size)
{
    int shift;
    
    get_tile_info(id, &shift, 0, 0);
    
    /* DRP start */
    pdesc[ 0] = 0x07;                                        //   0-  7
    pdesc[ 1] = 0x00;                                        //   8- 15
    pdesc[ 2] = 0x00;                                        //  16- 23
    pdesc[ 3] = 0x01;                                        //  24- 31
    pdesc[ 4] = (unsigned char)( param & 0x000000FF       ); //  32- 39
    pdesc[ 5] = (unsigned char)((param & 0x0000FF00) >>  8); //  40- 47
    pdesc[ 6] = (unsigned char)((param & 0x00FF0000) >> 16); //  48- 55
    pdesc[ 7] = (unsigned char)((param & 0xFF000000) >> 24); //  56- 63
    pdesc[ 8] = (unsigned char)(  size & 0x000000FF       ); //  64- 71
    pdesc[ 9] = (unsigned char)(( size & 0x0000FF00) >>  8); //  72- 79
    pdesc[10] = (unsigned char)(( size & 0x00FF0000) >> 16); //  80- 87
    pdesc[11] = 0x00;                                        //  88- 95
    pdesc[12] = 0x00;                                        //  96-103
    pdesc[13] = 0x00;                                        // 104-111
    pdesc[14] = 0x00;                                        // 112-119
    pdesc[15] = 0x00;                                        // 120-127
    
    pdesc[16] = 0x00;                                        //   0-  7
    pdesc[17] = 0x80;                                        //   8- 15
    pdesc[18] = 0x00;                                        //  16- 23
    pdesc[19] = 0x03;                                        //  24- 31
    pdesc[20] = 0x00;                                        //  32- 39
    pdesc[21] = (unsigned char)(0x91 + shift);               //  40- 47
    pdesc[22] = 0xFF;                                        //  48- 55
    pdesc[23] = 0xF0;                                        //  56- 63
    pdesc[24] = 0x01;                                        //  64- 71
    pdesc[25] = 0x01;                                        //  72- 79
    pdesc[26] = 0x04;                                        //  80- 87
    pdesc[27] = 0x00;                                        //  88- 95
    pdesc[28] = 0x00;                                        //  96-103
    pdesc[29] = 0x00;                                        // 104-111
    pdesc[30] = 0x00;                                        // 112-119
    pdesc[31] = 0x00;                                        // 120-127
}
/*******************************************************************************
End of function dk2_pre_start
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_start
* Description  : This function starts DRP at H/W level.
* Arguments    : id - ID of configuration data.
                 desc - Address of descriptor.
* Return Value : None.
*******************************************************************************/
static void dk2_start(unsigned char id, unsigned long desc)
{
    int shift;
    
    get_tile_info(id, &shift, 0, 0);
    
//    DRP_CMREG_TILEWE  = 0xC0 | id;    // 0xEAFD6000
//    DRP_CMREG_TILESRC = 0x76543210;   // 0xEAFD6004
    
    DRP_ODIF_INT      = (1uL << shift); // 0xEAFFA000 Clear ODIF INT
    DRP_DSCC_INT      = (1uL << shift); // 0xEAFF8000 Clear DSCC INT
    
    if ((id & 0x01) != 0)
    {
//        DRP_DSCC_DPFCI0 = 0xFFFF0001; // debug
        
        DRP_IDIF_DMACNTI0 = 0x00040001; // 0xEAFF9100
        DRP_DSCC_DCTLI0   = 0x00000000; // 0xEAFF8100
        DRP_DSCC_DPAI0    = desc;       // 0xEAFF8108
        DRP_DSCC_DCTLI0   = 0x00000001; // 0xEAFF8100
    }
    else if ((id & 0x02) != 0)
    {
//        DRP_DSCC_DPFCI1 = 0xFFFF0001; // debug
        
        DRP_IDIF_DMACNTI1 = 0x00040001; // 0xEAFF9200
        DRP_DSCC_DCTLI1   = 0x00000000; // 0xEAFF8140
        DRP_DSCC_DPAI1    = desc;       // 0xEAFF8148
        DRP_DSCC_DCTLI1   = 0x00000001; // 0xEAFF8140
    }
    else if ((id & 0x04) != 0)
    {
//        DRP_DSCC_DPFCI2 = 0xFFFF0001; // debug
        
        DRP_IDIF_DMACNTI2 = 0x00040001; // 0xEAFF9300
        DRP_DSCC_DCTLI2   = 0x00000000; // 0xEAFF8180
        DRP_DSCC_DPAI2    = desc;       // 0xEAFF8188
        DRP_DSCC_DCTLI2   = 0x00000001; // 0xEAFF8180
    }
    else if ((id & 0x08) != 0)
    {
//        DRP_DSCC_DPFCI3 = 0xFFFF0001; // debug
        
        DRP_IDIF_DMACNTI3 = 0x00040001; // 0xEAFF9400
        DRP_DSCC_DCTLI3   = 0x00000000; // 0xEAFF81C0
        DRP_DSCC_DPAI3    = desc;       // 0xEAFF81C8
        DRP_DSCC_DCTLI3   = 0x00000001; // 0xEAFF81C0
    }
    else if ((id & 0x10) != 0)
    {
//        DRP_DSCC_DPFCI4 = 0xFFFF0001; // debug
        
        DRP_IDIF_DMACNTI4 = 0x00040001; // 0xEAFF9500
        DRP_DSCC_DCTLI4   = 0x00000000; // 0xEAFF8200
        DRP_DSCC_DPAI4    = desc;       // 0xEAFF8208
        DRP_DSCC_DCTLI4   = 0x00000001; // 0xEAFF8200
    }
    else if ((id & 0x20) != 0)
    {
//        DRP_DSCC_DPFCI5 = 0xFFFF0001; // debug
        
        DRP_IDIF_DMACNTI5 = 0x00040001; // 0xEAFF9600
        DRP_DSCC_DCTLI5   = 0x00000000; // 0xEAFF8240
        DRP_DSCC_DPAI5    = desc;       // 0xEAFF8248
        DRP_DSCC_DCTLI5   = 0x00000001; // 0xEAFF8240
    }
    
    while(0 == (DRP_DSCC_INT & (1uL << shift))) // 0xEAFF8000
    {
        ;
    }
}
/*******************************************************************************
End of function dk2_start
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_unload
* Description  : This function unloads DRP configuration data at H/W level.
* Arguments    : id - ID of configuration data.
* Return Value : None.
*******************************************************************************/
static void dk2_unload(unsigned char id)
{
    int shift;
    unsigned long reg;
    
    shift = 0;
    while (((id >> shift) & 1) == 0)
    {
        shift++;
    }
    
//    DRP_CMREG_TILEWE  = 0xC0 | id;  // 0xEAFD6000
//    DRP_CMREG_TILESRC = 0x76543210; // 0xEAFD6004
    
    /* Stop fetching descriptor */
    
    if (0 != (id & 0x01))
    {
        DRP_DSCC_DCTLI0 = 0x00000000; /* DSCEN = 0 */
        while (0 != (DRP_DSCC_DCTLI0 & 0x00000002)) /* waiting ACT = 0 */
        {
            ;
        }
        
        DRP_IDIF_DMACNTI0 = 0x00000101; /* REQEN = 0 */
        DRP_IDIF_DMACNTI0 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_IDIF_DMACNTI0;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
        
        DRP_ODIF_DMACNTO0 = 0x00000101; /* REQEN = 0 */
        DRP_ODIF_DMACNTO0 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_ODIF_DMACNTO0;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
    }
    if (0 != (id & 0x02))
    {
        DRP_DSCC_DCTLI1 = 0x00000000; /* DSCEN = 0 */
        while (0 != (DRP_DSCC_DCTLI1 & 0x00000002)) /* waiting ACT = 0 */
        {
            ;
        }
        
        DRP_IDIF_DMACNTI1 = 0x00000101; /* REQEN = 0 */
        DRP_IDIF_DMACNTI1 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_IDIF_DMACNTI1;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
        
        DRP_ODIF_DMACNTO1 = 0x00000101; /* REQEN = 0 */
        DRP_ODIF_DMACNTO1 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_ODIF_DMACNTO1;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
    }
    if (0 != (id & 0x04))
    {
        DRP_DSCC_DCTLI2 = 0x00000000; /* DSCEN = 0 */
        while (0 != (DRP_DSCC_DCTLI2 & 0x00000002)) /* waiting ACT = 0 */
        {
            ;
        }
        
        DRP_IDIF_DMACNTI2 = 0x00000101; /* REQEN = 0 */
        DRP_IDIF_DMACNTI2 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_IDIF_DMACNTI2;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
        
        DRP_ODIF_DMACNTO2 = 0x00000101; /* REQEN = 0 */
        DRP_ODIF_DMACNTO2 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_ODIF_DMACNTO2;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
    }
    if (0 != (id & 0x08))
    {
        DRP_DSCC_DCTLI3 = 0x00000000; /* DSCEN = 0 */
        while (0 != (DRP_DSCC_DCTLI3 & 0x00000002)) /* waiting ACT = 0 */
        {
            ;
        }
        
        DRP_IDIF_DMACNTI3 = 0x00000101; /* REQEN = 0 */
        DRP_IDIF_DMACNTI3 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_IDIF_DMACNTI3;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
        
        DRP_ODIF_DMACNTO3 = 0x00000101; /* REQEN = 0 */
        DRP_ODIF_DMACNTO3 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_ODIF_DMACNTO3;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
    }
    if (0 != (id & 0x10))
    {
        DRP_DSCC_DCTLI4 = 0x00000000; /* DSCEN = 0 */
        while (0 != (DRP_DSCC_DCTLI4 & 0x00000002)) /* waiting ACT = 0 */
        {
            ;
        }
        
        DRP_IDIF_DMACNTI4 = 0x00000101; /* REQEN = 0 */
        DRP_IDIF_DMACNTI4 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_IDIF_DMACNTI4;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
        
        DRP_ODIF_DMACNTO4 = 0x00000101; /* REQEN = 0 */
        DRP_ODIF_DMACNTO4 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_ODIF_DMACNTO4;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
    }
    if (0 != (id & 0x20))
    {
        DRP_DSCC_DCTLI5 = 0x00000000; /* DSCEN = 0 */
        while (0 != (DRP_DSCC_DCTLI5 & 0x00000002)) /* waiting ACT = 0 */
        {
            ;
        }
        
        DRP_IDIF_DMACNTI5 = 0x00000101; /* REQEN = 0 */
        DRP_IDIF_DMACNTI5 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_IDIF_DMACNTI5;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
        
        DRP_ODIF_DMACNTO5 = 0x00000101; /* REQEN = 0 */
        DRP_ODIF_DMACNTO5 = 0x00000100; /* DEN = 0 */
        do
        {
            reg = DRP_ODIF_DMACNTO5;
        }
        while ((0 != (reg & 0x00000002)) && (0 == (reg & 0x00080000))); /* waiting DMON = 0 or STOPMON = 1 */
    }
    
    /* Soft reset */
    
    DRP_SFTRST |= (((unsigned long)id << 16) | (unsigned long)id); /* DMACSFTRST EAFFD800 */
    
    if (0 != (id & 0x01))
    {
        DRP_regSftRst0 = 0x00000001; /* DRPKSFTRST0 EAFD4060 */
    }
    if (0 != (id & 0x02))
    {
        DRP_regSftRst1 = 0x00000001; /* DRPKSFTRST1 EAFD4068 */
    }
    if (0 != (id & 0x04))
    {
        DRP_regSftRst2 = 0x00000001; /* DRPKSFTRST2 EAFD4070 */
    }
    if (0 != (id & 0x08))
    {
        DRP_regSftRst3 = 0x00000001; /* DRPKSFTRST3 EAFD4078 */
    }
    if (0 != (id & 0x10))
    {
        DRP_regSftRst4 = 0x00000001; /* DRPKSFTRST4 EAFD4080 */
    }
    if (0 != (id & 0x20))
    {
        DRP_regSftRst5 = 0x00000001; /* DRPKSFTRST5 EAFD4088 */
    }
    
    DRP_CLKE &= ~(((unsigned long)id << 16) | (unsigned long)id); /* DMACCLKE EAFFD810 */
    
#ifdef DRP_USE_STBY
    DRP_regStandby = ((DRP_regStandby & ~0xC0uL) | id); // 0xEAFD4018
    while (id != (DRP_StandbyOut & id)) // 0xEAFD40A0
    {
        ;
    }
#else
    DRP_regStandbyWait = ((DRP_regStandbyWait & ~0xC0uL) | id); // 0xEAFD40F0
#endif
}
/*******************************************************************************
End of function dk2_unload
*******************************************************************************/

/*******************************************************************************
* Function Name: dk2_get_int
* Description  : This function gets interrupt status of DRP and clears it.
* Arguments    : pdscc - Status of DSCC interrupts.
                 pidif - Status of IDIF interrupts.
                 podif - Status of ODIF interrupts.
* Return Value : None.
*******************************************************************************/
static void dk2_get_int(unsigned long *pdscc, unsigned long *pidif, unsigned long *podif)
{
    unsigned long odif_int;
    volatile unsigned long dummy;

    odif_int = (DRP_ODIF_INT & 0x3F); /* get   ODIF INT */
    DRP_ODIF_INT = odif_int;          /* clear ODIF INT */
    dummy = DRP_ODIF_INT;             /* dummy read */

    if (0 != podif)
    {
        *podif = odif_int;
    }
}
/*******************************************************************************
End of function dk2_get_int
*******************************************************************************/

/*******************************************************************************
* Function Name: read_evsel
* Description  : This function reads EVSEL.
* Arguments    : core - Number of core.
                 state - Number of state.
                 evidx - Index of EVSEL.
                 pretval - Address of return value.
                 preaddata - Address of read data.
* Return Value : None.
*******************************************************************************/
static void read_evsel(int core, int state, int evidx, unsigned char *pretval, unsigned char *preaddata)
{
    unsigned long raddr;
    unsigned char readdata;
    
    reg_write((unsigned char)(1 << core), REG_CFGMREADEN, 0x00000001);
    
    if (0 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xBuL << 11) | ((unsigned long)state << 3));
        readdata = *(unsigned char *)raddr;
        *pretval = ((readdata >> 4) & 0xF); /* readdata[7:4] */
    }
    else if (1 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xCuL << 11) | ((unsigned long)state << 3));
        readdata = *(unsigned char *)raddr;
        *pretval = (readdata & 0xF); /* readdata[3:0] */
    }
    else if (2 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xCuL << 11) | ((unsigned long)state << 3));
        readdata = *(unsigned char *)raddr;
        *pretval = ((readdata >> 4) & 0xF); /* readdata[7:4] */
    }
    else if (3 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xDuL << 11) | ((unsigned long)state << 3));
        readdata = *(unsigned char *)raddr;
        *pretval = (readdata & 0xF); /* readdata[3:0] */
    }
    *preaddata = readdata;
    
    reg_write((unsigned char)(1 << core), REG_CFGMREADEN, 0x00000000);
}
/*******************************************************************************
End of function read_evsel
*******************************************************************************/

/*******************************************************************************
* Function Name: write_evsel
* Description  : This function writes EVSEL.
* Arguments    : core - Number of core.
                 state - Number of state.
                 evidx - Index of EVSEL.
                 writeval - Write value.
                 baseval - Base value.
* Return Value : None.
*******************************************************************************/
static void write_evsel(int core, int state, int evidx, unsigned char writeval, unsigned char baseval)
{
    unsigned long raddr;
    unsigned char writedata;
    
    if (0 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xBuL << 11) | ((unsigned long)state << 3));
        writedata = (unsigned char)((baseval & 0x0F) | ((writeval & 0x0F) << 4));
    }
    else if (1 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xCuL << 11) | ((unsigned long)state << 3));
        writedata = (unsigned char)((baseval & 0xF0) | ((writeval & 0x0F) << 0));
    }
    else if (2 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xCuL << 11) | ((unsigned long)state << 3));
        writedata = (unsigned char)((baseval & 0x0F) | ((writeval & 0x0F) << 4));
    }
    else if (3 == evidx)
    {
        raddr = DRPK_BASE_ADDR + (((unsigned long)core << 21) | (0x9uL << 16) | (0xDuL << 11) | ((unsigned long)state << 3));
        writedata = (unsigned char)((baseval & 0xF0) | ((writeval & 0x0F) << 0));
    }
    *(unsigned char *)raddr = writedata;
}
/*******************************************************************************
End of function write_evsel
*******************************************************************************/

/*******************************************************************************
* Function Name: reg_write
* Description  : This function writes register of tile.
* Arguments    : tile - Number of tile.
                 addr - Address.
                 data - Data.
* Return Value : None.
*******************************************************************************/
static void reg_write(unsigned char tile, unsigned long addr, unsigned long data)
{
    unsigned long drpk_tile_baseaddr;
    int tile_idx;
    
    drpk_tile_baseaddr = DRPK_TILE0_ADDR;
    for (tile_idx = 0; tile_idx < 6; tile_idx++)
    {
        if (1 == ((tile >> tile_idx) & 1))
        {
            *(unsigned long *)(drpk_tile_baseaddr + (addr & 0x003FFFFF)) = data;
        }
        drpk_tile_baseaddr += 0x00200000;
    }
}
/*******************************************************************************
End of function reg_write
*******************************************************************************/

/*******************************************************************************
* Function Name: get_tile_info
* Description  : This function gets information of tile.
* Arguments    : id - ID of DRP configuration data.
                 pshift - Address of tile shift number.
                 pnum - Address of tile number.
                 pdma - Address of DMA channel.
* Return Value : None.
*******************************************************************************/
static void get_tile_info(unsigned char id, int *pshift, int *pnum, unsigned long *pdma)
{
    int shift;
    int num;
    unsigned long dma;
    
    shift = 0;
    while (0 == ((id >> shift) & 1))
    {
        shift++;
    }
    
    num = 0;
    while (0 != (id >> (shift + num)))
    {
        num++;
    }
    
    if (6 == num)
    {
        dma = 0xF;
    }
    else if (num > 1)
    {
        dma = 0x3;
    }
    else
    {
        dma = 0x1;
    }
    
    if (0 != pshift)
    {
        *pshift = shift;
    }
    if (0 != pnum)
    {
        *pnum = num;
    }
    if (0 != pdma)
    {
        *pdma = dma;
    }
}
/*******************************************************************************
End of function get_tile_info
*******************************************************************************/

/*******************************************************************************
* Function Name: set_dma
* Description  : This function sets DMA.
* Arguments    : dma - DMA channel.
                 shift - Tile shift number.
* Return Value : None.
*******************************************************************************/
static void set_dma(unsigned int dma, int shift)
{
    volatile unsigned long idif_dmactli = 0xEAFF9100;
    volatile unsigned long odif_dmactlo = 0xEAFFA100;
    idif_dmactli += (unsigned long)(shift * 0x100);
    odif_dmactlo += (unsigned long)(shift * 0x100);

    if((dma & 0x00000010) != 0)
    {
        *(volatile unsigned int *)odif_dmactlo = 0x00040101;
    }
    if((dma & 0x00000020) != 0)
    {
        *(volatile unsigned int *)(odif_dmactlo + 0x100) = 0x00040101;
    }
    if((dma & 0x00000040) != 0)
    {
        *(volatile unsigned int *)(odif_dmactlo + 0x200) = 0x00040101;
    }
    if((dma & 0x00000080) != 0)
    {
        *(volatile unsigned int *)(odif_dmactlo + 0x300) = 0x00040101;
    }
    if((dma & 0x00000001) != 0)
    {
        *(volatile unsigned int *)idif_dmactli = 0x00040101;
    }
    if((dma & 0x00000002) != 0)
    {
        *(volatile unsigned int *)(idif_dmactli + 0x100) = 0x00040101;
    }
    if((dma & 0x00000004) != 0)
    {
        *(volatile unsigned int *)(idif_dmactli + 0x200) = 0x00040101;
    }
    if((dma & 0x00000008) != 0)
    {
        *(volatile unsigned int *)(idif_dmactli + 0x300) = 0x00040101;
    }
}
/*******************************************************************************
End of function set_dma
*******************************************************************************/

/* End of File */
