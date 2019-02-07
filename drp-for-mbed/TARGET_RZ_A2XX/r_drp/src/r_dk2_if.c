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
* File Name    : r_dk2_if.c
* Version      : $Rev: 114 $
* Device       : RZ
* Abstract     : Control software of DRP.
* Tool-Chain   : Renesas e2studio
* OS           : Not use
* H/W Platform : Renesas Starter Kit
* Description  : Interface of DRP Driver.
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
#if(1) /* for mbed */
#include <string.h>
#include "iodefine.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "mbed_critical.h"
#else
#include "iodefine.h"
#include "r_mmu_lld_rza2m.h"
#include "r_cache_l1_rza2m_asm.h"
#include "r_intc_lld_rza2m.h"
#include "r_os_abstraction_api.h"
#endif
#include "r_dk2_core.h"
#include "r_dk2_if.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
#define USE_UNCACHE_AREA

#define STATUS_UNINITIALIZED    (0)
#define STATUS_INITIALIZED      (1)
#define STANBYOUT_WAIT_COUNT    (10000)
#define TILE_PATTERN_NUM        (11)
#define MAX_FREQ                (66000)
#define MIN_FREQ                (2063)
#define INVALID_FREQ            (0xFFFFFFFF)
#define ORIGINAL_FREQ           (264000)
#define ROUND_UP_DIV(x, y)      (((x) + (y) - 1) / (y))

/* for config_parser */
#define CONFIG_ALIGN_MASK       (0x1F)
#define SECTION_ID_LEN          (4)
#define COMMON_SECTION_ID       ("dk2c")
#define STP_SECTION_ID          ("stpd")
#define BYTE_NUM_OF_BYTE        (1)
#define BYTE_NUM_OF_HWORD       (2)
#define BYTE_NUM_OF_WORD        (4)
#define TARGET_DEVIVCE_LEN      (16)
#define COMMON_RESERVE_LEN      (25)
#define CONTEXT_MAX             (64)
#define STATE_MAX               (256)

/* for CRC */
#define CRC32_GEN_POLYNOMIAL    (0xEDB88320u)
#define CRC32_BIT_LOOP_NUM      (8)
#define CRC32_INITIAL_VALUE     (0xFFFFFFFFu)
#define CRC32_XOR_VALUE         (0xFFFFFFFFu)

#define MUTEX_WAIT              (100)

/*******************************************************************************
Typedef definitions
*******************************************************************************/
typedef char dk2_char_t;

/*******************************************************************************
Imported global variables and functions (from other files)
*******************************************************************************/

/*******************************************************************************
Private variables and functions(prototypes)
*******************************************************************************/
static uint8_t status = STATUS_UNINITIALIZED;
static uint8_t aid[R_DK2_TILE_NUM];
static uint8_t adfc[R_DK2_TILE_NUM];
static uint32_t amax[R_DK2_TILE_NUM];
static uint32_t afreq[R_DK2_TILE_NUM];
static uint8_t astart[R_DK2_TILE_NUM];
static load_cb_t apload[R_DK2_TILE_NUM];
static int_cb_t apint[R_DK2_TILE_NUM];
#if(1) /* for mbed */
__attribute__ ((aligned(16), section("NC_BSS")))
static uint8_t work_load[R_DK2_TILE_NUM][16];

__attribute__ ((aligned(16), section("NC_BSS")))
static uint8_t work_start[R_DK2_TILE_NUM][32];

#else
#ifdef __GNUC__
#ifdef USE_UNCACHE_AREA
__attribute__ ((aligned(16), section("UNCACHED_BSS")))
#else
__attribute__ ((aligned(16)))
#endif
#endif
static uint8_t work_load[R_DK2_TILE_NUM][16];
#ifdef __GNUC__
#ifdef USE_UNCACHE_AREA
__attribute__ ((aligned(16), section("UNCACHED_BSS")))
#else
__attribute__ ((aligned(16)))
#endif
#endif
static uint8_t work_start[R_DK2_TILE_NUM][32];
#endif

/* for conf_parser */
static const uint8_t *pu8config;
static uint32_t config_index;
static uint32_t section_index;
static uint32_t section_size;

/* conf_parser result */
static uint64_t *pu64config;
static uint32_t u64config_num;
static uint16_t minor_ver_dat;
static uint8_t tile_num_dat;
static uint32_t freq_dat;
static uint8_t del_zero_dat;
static uint8_t dfc_dat;
static uint16_t context_dat;
static uint16_t state_dat;

#if(1) /* for mbed */
static osSemaphoreId_t sem_hdl;
osSemaphoreDef(drp_sem_01);
#else
static p_mutex_t mutex_hdl;
#endif

static const uint32_t apattern[TILE_PATTERN_NUM] = 
{
    R_DK2_TILE_PATTERN_1_1_1_1_1_1,
    R_DK2_TILE_PATTERN_2_1_1_1_1,
    R_DK2_TILE_PATTERN_2_2_1_1,
    R_DK2_TILE_PATTERN_2_2_2,
    R_DK2_TILE_PATTERN_3_1_1_1,
    R_DK2_TILE_PATTERN_3_2_1,
    R_DK2_TILE_PATTERN_3_3,
    R_DK2_TILE_PATTERN_4_1_1,
    R_DK2_TILE_PATTERN_4_2,
    R_DK2_TILE_PATTERN_5_1,
    R_DK2_TILE_PATTERN_6,
};
static const uint8_t ahead[TILE_PATTERN_NUM] =
{
    0x3F, /* R_DK2_TILE_PATTERN_1_1_1_1_1_1 */
    0x3D, /* R_DK2_TILE_PATTERN_2_1_1_1_1 */
    0x35, /* R_DK2_TILE_PATTERN_2_2_1_1 */
    0x15, /* R_DK2_TILE_PATTERN_2_2_2 */
    0x39, /* R_DK2_TILE_PATTERN_3_1_1_1 */
    0x29, /* R_DK2_TILE_PATTERN_3_2_1 */
    0x09, /* R_DK2_TILE_PATTERN_3_3 */
    0x31, /* R_DK2_TILE_PATTERN_4_1_1 */
    0x11, /* R_DK2_TILE_PATTERN_4_2 */
    0x21, /* R_DK2_TILE_PATTERN_5_1 */
    0x01, /* R_DK2_TILE_PATTERN_6 */
};
static const uint8_t atail[TILE_PATTERN_NUM] =
{
    0x3F, /* R_DK2_TILE_PATTERN_1_1_1_1_1_1 */
    0x3E, /* R_DK2_TILE_PATTERN_2_1_1_1_1 */
    0x3A, /* R_DK2_TILE_PATTERN_2_2_1_1 */
    0x2A, /* R_DK2_TILE_PATTERN_2_2_2 */
    0x3C, /* R_DK2_TILE_PATTERN_3_1_1_1 */
    0x34, /* R_DK2_TILE_PATTERN_3_2_1 */
    0x24, /* R_DK2_TILE_PATTERN_3_3 */
    0x38, /* R_DK2_TILE_PATTERN_4_1_1 */
    0x28, /* R_DK2_TILE_PATTERN_4_2 */
    0x30, /* R_DK2_TILE_PATTERN_5_1 */
    0x20, /* R_DK2_TILE_PATTERN_6 */
};

/* Prototypes */
static int32_t get_tile_index(const uint8_t id);
static int32_t get_status(const uint8_t id);
static int32_t analyze_config(const void *const pconfig);
static int32_t init_config_parser(const void *const pconfig);
static int32_t analyze_common_section(void);
static int32_t analyze_stp_section(void);
static int32_t read_config_section(const dk2_char_t *const pid);
static int32_t read_config_word(uint32_t *const pword, const uint32_t *const pexpected);
static int32_t read_config_hword(uint16_t *const phword, const uint16_t *const pexpected);
static int32_t read_config_byte(uint8_t *const pbyte, const uint8_t *const pexpected);
static int32_t read_config_str(const dk2_char_t *const pexpeced, const uint32_t size);
static int32_t read_config_data(const uint32_t **const pdata, const uint32_t size);
#if(1) /* for mbed */
static void dk2_nmlint_isr(void);
#else
static void dk2_nmlint_isr(uint32_t int_sense);
#endif

/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_Initialize
* Description  : This function causes DRP to be initialized.
* Arguments    : None.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_Initialize(void)
{
    int32_t result = R_DK2_SUCCESS;
    int32_t tile_index;
    volatile uint8_t dummy_buf;
    int32_t wait_count;
    
    if (STATUS_UNINITIALIZED != status)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        aid[tile_index] = 0;
        adfc[tile_index] = 0;
        amax[tile_index] = MAX_FREQ;
        afreq[tile_index] = INVALID_FREQ;
        astart[tile_index] = 0;
        apload[tile_index] = NULL;
        apint[tile_index] = NULL;
    }
    
#if(1) /* for mbed */
    sem_hdl = osSemaphoreNew(1, 1, osSemaphore(drp_sem_01));
    if (NULL == sem_hdl)
#else
    mutex_hdl = R_OS_MutexCreate();
    if (NULL == mutex_hdl)
#endif
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
#if(1) /* for mbed */
    GIC_SetPriority(NMLINT_IRQn, 8);
    GIC_SetConfiguration(NMLINT_IRQn, 1);
    InterruptHandlerRegister(NMLINT_IRQn, &dk2_nmlint_isr);
    GIC_EnableIRQ(NMLINT_IRQn);
#else
    R_INTC_SetPriority(INTC_ID_DRP_NMLINT, 8);
    R_INTC_RegistIntFunc(INTC_ID_DRP_NMLINT, &dk2_nmlint_isr);
    R_INTC_Enable(INTC_ID_DRP_NMLINT);
#endif
    
    /* DRP Clock on */
    CPG.STBCR9.BIT.MSTP90 = 0;
    dummy_buf = CPG.STBCR9.BYTE; /* Dummy read */
#if(1) /* for mbed */
    (void)dummy_buf;
#endif
    
    /* DRP standby out */
    wait_count = 0;
    CPG.STBREQ2.BIT.STBRQ24 = 0;
    while(0 != CPG.STBACK2.BIT.STBAK24)
    {
        if (STANBYOUT_WAIT_COUNT < wait_count)
        {
            result = R_DK2_ERR_STATUS;
            goto func_end;
        }
        wait_count++;
    }
    
    R_DK2_CORE_Initialize();
    
    status = STATUS_INITIALIZED;
    
    goto func_end;
    
func_end:
    return result;
}
/*******************************************************************************
End of function R_DK2_Initialize
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_Uninitialize
* Description  : This function causes DRP to be uninitialized.
* Arguments    : None.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_Uninitialize(void)
{
    /* Future planned implementation */
    return R_DK2_ERR_INTERNAL;
}
/*******************************************************************************
End of function R_DK2_Uninitialize
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_Load
* Description  : This function loads DRP circuit to DRP.
* Arguments    : pconfig - Address of configuration data.
                 top_tiles - Top tiles of DRP circuit to be loaded.
                 tile_pat - Pattern of tile layout.
                 pload - Callback function to notify completion of loading.
                 pint - Callback function to notify interrupt from DRP circuit.
                 pid - Array represents tile layout of DRP circuit.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_Load(const void *const pconfig, const uint8_t top_tiles, const uint32_t tile_pattern, const load_cb_t pload, const int_cb_t pint, uint8_t *const paid)
{
    int32_t result;
    uint8_t bottom_tiles;
    int32_t index;
    int32_t tile_index;
    int32_t tile_pos;
    int32_t tile_config;
    void *work_uc;
    uint8_t id;
    uint8_t loaded_id[R_DK2_TILE_NUM];
    int32_t loaded_id_index;
#if(1) /* for mbed */
    osStatus sem_result;
    uint32_t sem_timeout = MUTEX_WAIT;
#else
    bool mutex_result;
#endif
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
#if(1) /* for mbed */
    if (core_util_is_isr_active() != false) {
        sem_timeout = 0;
    }
    sem_result = osSemaphoreAcquire (sem_hdl, sem_timeout);
    if (osOK != sem_result)
#else
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
#endif
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    /* write paid before loading */
    if (NULL != paid)
    {
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            paid[tile_index] = aid[tile_index];
        }
    }
    
    if ((NULL == pconfig) || (0 != ((uint32_t)pconfig & 0x0000001F)) || (0 == top_tiles) || (0 != (top_tiles & ~0x3F)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /***** Unimplemented *****/
    if (NULL != pload)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }
    
    /* analyze configuration data */
    pu64config = NULL;
    u64config_num = 0;
    minor_ver_dat = 0;
    tile_num_dat = 0;
    freq_dat = 0;
    del_zero_dat = 0;
    dfc_dat = 0;
    context_dat = 0;
    state_dat = 0;
    result = analyze_config(pconfig);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    if ((NULL == pu64config) || (0 == u64config_num) ||
        (0 == tile_num_dat) || (tile_num_dat > R_DK2_TILE_NUM) ||
        (del_zero_dat > 1) || (dfc_dat > 1) || (context_dat > CONTEXT_MAX) || (state_dat > STATE_MAX))
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }
    
    /* check tile pattern */
    bottom_tiles = (uint8_t)(top_tiles << (tile_num_dat - 1));
    for (index = 0; index < TILE_PATTERN_NUM; index++)
    {
        if (tile_pattern == apattern[index])
        {
            if ((top_tiles & ahead[index]) != top_tiles)
            {
                result = R_DK2_ERR_TILE_PATTERN;
                goto func_end;
            }
            if ((bottom_tiles & atail[index]) != bottom_tiles)
            {
                result = R_DK2_ERR_TILE_PATTERN;
                goto func_end;
            }
            break;
        }
    }
    if (index >= TILE_PATTERN_NUM)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /* get tile position */
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (0 != ((top_tiles >> tile_index) & 1))
        {
            break;
        }
    }
    if (tile_index >= R_DK2_TILE_NUM)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    tile_pos = tile_index;
    
    /* check free tiles  */
    tile_config = R_DK2_TILE_NUM;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (tile_config < R_DK2_TILE_NUM)
        {
            if (0 != aid[tile_index])
            {
                result = R_DK2_ERR_OVERWRITE;
                goto func_end;
            }
        }
        else if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            if (0 != aid[tile_index])
            {
                result = R_DK2_ERR_OVERWRITE;
                goto func_end;
            }
            tile_config = tile_index;
        }
        tile_config++;
        if (tile_config >= tile_pos + tile_num_dat)
        {
            tile_config = R_DK2_TILE_NUM;
        }
        else if (0 != ((top_tiles >> tile_config) & R_DK2_TILE_0))
        {
            result = R_DK2_ERR_OVERWRITE;
            goto func_end;
        }
    }
    
#ifdef USE_UNCACHE_AREA
    work_uc = (void *)&work_load[tile_pos][0];
#else
    work_uc = (void *)((uint32_t)&work_load[tile_pos][0] | 0x2000000);
#endif
    id = 0;

    {
        uint32_t paddr;

#if(1) /* for mbed */
        paddr = (uint32_t)pu64config;
#else
        R_MMU_VAtoPA((uint32_t)pu64config, &paddr);
#endif
        result = R_DK2_CORE_PreLoad(tile_num_dat, top_tiles, paddr, u64config_num * 8, (0 != del_zero_dat), context_dat, state_dat, work_uc, &id);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
    
#if(1) /* for mbed */
        paddr = (uint32_t)&work_load[tile_pos][0];
#else
        R_MMU_VAtoPA((uint32_t)&work_load[tile_pos][0], &paddr);
#endif
        result = R_DK2_CORE_Load(id, top_tiles, tile_pattern, state_dat, paddr, &loaded_id[0]);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
    }
    
    /* write paid after loading */
    loaded_id_index = 0;
    tile_pos = R_DK2_TILE_NUM;
    if ((1 <= minor_ver_dat) && (0 != freq_dat))
    {
        freq_dat = 1000000000 / freq_dat;
    }
    else
    {
        freq_dat = MAX_FREQ;
    }
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if ((tile_pos < R_DK2_TILE_NUM) && (tile_index - tile_pos < tile_num_dat))
        {
            aid[tile_index] = aid[tile_index - 1];
            adfc[tile_index] = dfc_dat;
            amax[tile_index] = freq_dat;
            if (NULL != pload)
            {
                apload[tile_index] = pload;
            }
            if (NULL != pint)
            {
                apint[tile_index] = pint;
            }
            if (NULL != paid)
            {
                paid[tile_index] = aid[tile_index];
            }
        }
        else if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            tile_pos = tile_index;
            aid[tile_index] = loaded_id[loaded_id_index++];
            adfc[tile_index] = dfc_dat;
            amax[tile_index] = freq_dat;
            if (NULL != pload)
            {
                apload[tile_index] = pload;
            }
            if (NULL != pint)
            {
                apint[tile_index] = pint;
            }
            if (NULL != paid)
            {
                paid[tile_index] = aid[tile_index];
            }
        }
    }

    goto func_end;
    
func_end:
#if(1) /* for mbed */
    if (osOK == sem_result)
    {
        (void)osSemaphoreRelease(sem_hdl);
    }
#else
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
#endif
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Load = %d: pconfig = %08Xh, top_tiles = %02X, tile_pattern = %02X\n  id =", result, (uint32_t)pconfig, top_tiles, tile_pattern);
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            printf(" %02X", paid[tile_index]);
        }
        printf("\n");
    }
#endif /* DRP_DEBUG */

    return result;
}
/*******************************************************************************
End of function R_DK2_Load
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_Unload
* Description  : This function unloads DRP circuit from DRP.
* Arguments    : id - ID of DRP circuit to be unloaded.
                 pid - Array represents tile layout of DRP circuits.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_Unload(const uint8_t id, uint8_t *const paid)
{
    int32_t result;
    int32_t tile_index;
#if(1) /* for mbed */
    osStatus sem_result;
    uint32_t sem_timeout = MUTEX_WAIT;
#else
    bool mutex_result;
#endif
    int32_t id_index;
    int32_t id_num;
    uint8_t id_target;
    uint8_t aid_target[R_DK2_TILE_NUM] = { 0, 0, 0, 0, 0, 0 };
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    /* Exclusion control */
#if(1) /* for mbed */
    if (core_util_is_isr_active() != false) {
        sem_timeout = 0;
    }
    sem_result = osSemaphoreAcquire (sem_hdl, sem_timeout);
    if (osOK != sem_result)
#else
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
#endif
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    /* write paid before unloading */
    if (NULL != paid)
    {
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            paid[tile_index] = aid[tile_index];
        }
    }
    
    /* Search target ID. */
    if (0 == id) /* target is all circuits. */
    {
        /* Search target from all circuits. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (0 != aid[tile_index])
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    id_num++;
                }
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
    else /* Target is specified by argument id. */
    {
        /* Search target from argument id. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if ((0 != aid[tile_index]) && ((id & aid[tile_index]) == aid[tile_index]))
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    id_num++;
                }
            }
            else if ((id & aid[tile_index]) != 0)
            {
                result = R_DK2_ERR_ARG;
                goto func_end;
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
    
    /* Make target ID */
    id_target = 0;
    for (id_index = 0; id_index < id_num; id_index++)
    {
        id_target |= aid_target[id_index];
    }
    if ((0 != id) && (id != id_target))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    for (id_index = 0; id_index < id_num; id_index++)
    {
        result = R_DK2_CORE_Unload(aid_target[id_index]);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
        
        /* write paid after unloading */
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (aid[tile_index] == aid_target[id_index])
            {
                aid[tile_index] = 0;
                adfc[tile_index] = 0;
                amax[tile_index] = MAX_FREQ;
                afreq[tile_index] = INVALID_FREQ;
                astart[tile_index] = 0;
                apload[tile_index] = NULL;
                apint[tile_index] = NULL;
                if (NULL != paid)
                {
                    paid[tile_index] = 0;
                }
            }
            else
            {
                paid[tile_index] = aid[tile_index];
            }
        }
    }
    
func_end:
#if(1) /* for mbed */
    if (osOK == sem_result)
    {
        (void)osSemaphoreRelease(sem_hdl);
    }
#else
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
#endif
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Unload = %d: id = %02Xh\n  id =", result, id);
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            printf(" %02X", paid[tile_index]);
        }
        printf("\n");
    }
#endif /* DRP_DEBUG */

    return result;
}
/*******************************************************************************
End of function R_DK2_Unload
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_Activate
* Description  : This function activates and set up DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit to be started.
                 freq - Frequency of DRP circuit to be started.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_Activate(const uint8_t id, const uint32_t freq)
{
    int32_t result;
    int32_t tile_index;
    uint8_t div;
#if(1) /* for mbed */
    osStatus sem_result;
    uint32_t sem_timeout = MUTEX_WAIT;
#else
    bool mutex_result;
#endif
    int32_t id_index;
    int32_t id_num;
    uint8_t id_target;
    uint8_t aid_target[R_DK2_TILE_NUM] = { 0, 0, 0, 0, 0, 0 };
    int32_t aindex_target[R_DK2_TILE_NUM] = { 0, 0, 0, 0, 0, 0 };
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    /* Exclusion control */
#if(1) /* for mbed */
    if (core_util_is_isr_active() != false) {
        sem_timeout = 0;
    }
    sem_result = osSemaphoreAcquire (sem_hdl, sem_timeout);
    if (osOK != sem_result)
#else
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
#endif
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    /* Search target ID. */
    if (0 == id) /* target is all circuits. */
    {
        /* Search target from all circuits. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if ((0 != aid[tile_index]) && (R_DK2_STATUS_LOADED == get_status(aid[tile_index])))
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    aindex_target[0] = tile_index;
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    aindex_target[id_num] = tile_index;
                    id_num++;
                }
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_STATUS;
            goto func_end;
        }
    }
    else /* Target is specified by argument id. */
    {
        /* Search target from argument id. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if ((0 != aid[tile_index]) && ((id & aid[tile_index]) == aid[tile_index]))
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    aindex_target[0] = tile_index;
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    aindex_target[id_num] = tile_index;
                    id_num++;
                }
            }
            else if ((id & aid[tile_index]) != 0)
            {
                result = R_DK2_ERR_ARG;
                goto func_end;
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
        
        /* Check status of circuits. */
        for (id_index = 0; id_index < id_num; id_index++)
        {
            if (R_DK2_STATUS_LOADED != get_status(aid_target[id_index]))
            {
                result = R_DK2_ERR_STATUS;
                goto func_end;
            }
        }
    }
    
    /* Make target ID */
    id_target = 0;
    for (id_index = 0; id_index < id_num; id_index++)
    {
        id_target |= aid_target[id_index];
    }
    if ((0 != id) && (id != id_target))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /* Check frequency and culculate divison factor */
    if (0 == freq)
    {
        /* dfc check */
        for (id_index = 0; id_index < id_num; id_index++)
        {
            if (0 == adfc[aindex_target[id_index]])
            {
                result = R_DK2_ERR_FORMAT;
                goto func_end;
            }
        }
        
        div = 0;
    }
    else if (freq < MIN_FREQ)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    else
    {
        uint32_t max_freq = MAX_FREQ;
        uint32_t u32div;
        
        /* decide maximum of frequency */
        for (id_index = 0; id_index < id_num; id_index++)
        {
            if (max_freq > amax[aindex_target[id_index]])
            {
                max_freq = amax[aindex_target[id_index]];
            }
        }
        
        if (freq > max_freq)
        {
            result = R_DK2_ERR_FREQ;
            goto func_end;
        }
        
        u32div = ROUND_UP_DIV(ORIGINAL_FREQ, freq);
        if ((128 < u32div) || (4 > u32div))
        {
            result = R_DK2_ERR_FREQ;
            goto func_end;
        }
        div = (uint8_t)u32div;
    }
    
    for (id_index = 0; id_index < id_num; id_index++)
    {
        result = R_DK2_CORE_Activate(aid_target[id_index], div);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
        
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (aid[tile_index] == aid_target[id_index])
            {
                afreq[tile_index] = freq;
            }
        }
    }

func_end:
#if(1) /* for mbed */
    if (osOK == sem_result)
    {
        (void)osSemaphoreRelease(sem_hdl);
    }
#else
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
#endif
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Activate = %d: id = %02Xh, freq = %d\n", result, id, freq);
    }
#endif /* DRP_DEBUG */

    return result;
}
/*******************************************************************************
End of function R_DK2_Activate
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_Inactivate
* Description  : This function inactivates DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit to be started.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_Inactivate(const uint8_t id)
{
    /* Future planned implementation */
    return R_DK2_ERR_INTERNAL;
}
/*******************************************************************************
End of function R_DK2_Inactivate
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_Start
* Description  : This function starts DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit.
                 pparam - Address of parameters.
                 size - Size of parameters.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_Start(const uint8_t id, const void *const pparam, const uint32_t size)
{
    int32_t result;
    int32_t circuit_status;
    int32_t tile_index;
    int32_t tile_pos;
    void *work_uc;
#if(1) /* for mbed */
    osStatus sem_result;
    uint32_t sem_timeout = MUTEX_WAIT;
#else
    bool mutex_result;
#endif
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
#if(1) /* for mbed */
    if (core_util_is_isr_active() != false) {
        sem_timeout = 0;
    }
    sem_result = osSemaphoreAcquire (sem_hdl, sem_timeout);
    if (osOK != sem_result)
#else
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
#endif
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    if ((0 == id) || (NULL == pparam) || (0 == size))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    circuit_status = get_status(id);
    if (0 > circuit_status)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    if (R_DK2_STATUS_ACTIVATED != circuit_status)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    tile_pos = get_tile_index(id);
    if (tile_pos >= R_DK2_TILE_NUM)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
#ifdef USE_UNCACHE_AREA
    work_uc = (void *)&work_start[tile_pos][0];
#else
    work_uc = (void *)((uint32_t)&work_start[tile_pos][0] | 0x2000000);
#endif
    {
        uint32_t paddr;

#if(1) /* for mbed */
        paddr = (uint32_t)pparam;
#else
        R_MMU_VAtoPA((uint32_t)pparam, &paddr);
#endif
        result = R_DK2_CORE_PreStart(id, work_uc, paddr, size);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
    }

    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (id == aid[tile_index])
        {
            astart[tile_index] = id;
        }
    }
    
    {
        uint32_t paddr;

#if(1) /* for mbed */
        paddr = (uint32_t)&work_start[tile_pos][0];
#else
        R_MMU_VAtoPA((uint32_t)&work_start[tile_pos][0], &paddr);
#endif
        result = R_DK2_CORE_Start(id, paddr);
        if (R_DK2_SUCCESS != result)
        {
            for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
            {
                if (id == aid[tile_index])
                {
                    astart[tile_index] = 0;
                }
            }
            goto func_end;
        }
    }
    
func_end:
#if(1) /* for mbed */
    if (osOK == sem_result)
    {
        (void)osSemaphoreRelease(sem_hdl);
    }
#else
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
#endif
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Start = %d: id = %02Xh, pparam = %08X, size = %d\n", result, id, (uint32_t)pparam, size);
    }
#endif /* DRP_DEBUG */

    return result;
}
/*******************************************************************************
End of function R_DK2_Start
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_GetStatus
* Description  : This function gets status of DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_GetStatus(const uint8_t id)
{
    int32_t result;
#if(1) /* for mbed */
    osStatus sem_result;
    uint32_t sem_timeout = MUTEX_WAIT;
#else
    bool mutex_result;
#endif
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
#if(1) /* for mbed */
    if (core_util_is_isr_active() != false) {
        sem_timeout = 0;
    }
    sem_result = osSemaphoreAcquire (sem_hdl, sem_timeout);
    if (osOK != sem_result)
#else
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
#endif
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    result = get_status(id);
    
    goto func_end;
    
func_end:
#if(1) /* for mbed */
    if (osOK == sem_result)
    {
        (void)osSemaphoreRelease(sem_hdl);
    }
#else
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
#endif
    
    return result;
}
/*******************************************************************************
End of function R_DK2_GetStatus
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_GetInfo
* Description  : This function gets infomation of DRP configuration data.
* Arguments    : pconfig - Address of DRP configuration data.
                 pinfo - Address of infomation of DRP configuration data.
                 crc_check - Whether or not to check CRC.
* Return Value : Error code.
*******************************************************************************/
int32_t R_DK2_GetInfo(const void *const pconfig, config_info_t *const pinfo, const bool crc_check)
{
    /* Future planned implementation */
    return R_DK2_ERR_INTERNAL;
}
/*******************************************************************************
End of function R_DK2_GetInfo
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DK2_GetVersion
* Description  : This function gets version of DRP driver.
* Arguments    : None.
* Return Value : Version of DRP driver.
*******************************************************************************/
uint32_t R_DK2_GetVersion(void)
{
    return R_DK2_CORE_GetVersion();
}
/*******************************************************************************
End of function R_DK2_GetVersion
*******************************************************************************/

/*******************************************************************************
Private functions
*******************************************************************************/

/*******************************************************************************
* Function Name: get_tile_index
* Description  : This function gets index of DRP circuit.
* Arguments    : id - ID of circuit.
* Return Value : Index of DRP circuit.
*******************************************************************************/
static int32_t get_tile_index(const uint8_t id)
{
    int32_t tile_index;

    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (aid[tile_index] == id)
        {
            break;
        }
    }

    return tile_index;
}
/*******************************************************************************
End of function get_tile_index
*******************************************************************************/

/*******************************************************************************
* Function Name: get_status
* Description  : This function gets status of DRP circuit.
* Arguments    : id - ID of circuit.
* Return Value : Status of DRP circuit.
*******************************************************************************/
static int32_t get_status(const uint8_t id)
{
    int32_t result;
    int32_t tile_index;
    
    tile_index = get_tile_index(id);
    
    if ((0 == id) || (tile_index >= R_DK2_TILE_NUM))
    {
        result = R_DK2_ERR_ARG;
    }
    else if (INVALID_FREQ == afreq[tile_index])
    {
        result = R_DK2_STATUS_LOADED;
    }
    else if (0 == astart[tile_index])
    {
        result = R_DK2_STATUS_ACTIVATED;
    }
    else
    {
        result = R_DK2_STATUS_STARTED;
    }
    
    return result;
}
/*******************************************************************************
End of function get_status
*******************************************************************************/

/*******************************************************************************
* Function Name: analyze_config
* Description  : This function stops DRP circuit loaded in DRP.
* Arguments    : pconf - Address of configuration data.
* Return Value : Error code.
*******************************************************************************/
static int32_t analyze_config(const void *const pconfig)
{
    int32_t result = R_DK2_SUCCESS;
    
    result = init_config_parser(pconfig);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    
    result = analyze_common_section();
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    
    result = analyze_stp_section();
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    goto func_end;
    
func_end:
    return result;
}
/*******************************************************************************
End of function analyze_config
*******************************************************************************/

/*******************************************************************************
* Function Name: analyze_common_section
* Description  : Read common section from configuration data.
* Arguments    : None.
* Return Value : Error code.
*******************************************************************************/
static int32_t analyze_common_section(void)
{
    int32_t result = R_DK2_SUCCESS;
    uint16_t expected_major_ver = 2;
    uint8_t expected_fixed_num = 0;
    uint8_t expected_reloc_num = 0;
    uint8_t expected_stp_num = 1;

    result = read_config_section(COMMON_SECTION_ID);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_hword(&minor_ver_dat, NULL); /* minor version */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_hword(NULL, &expected_major_ver); /* major version */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, BYTE_NUM_OF_WORD); /* vendor ID */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, TARGET_DEVIVCE_LEN); /* target device */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(NULL, &expected_fixed_num); /* number of fixed data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(NULL, &expected_reloc_num); /* number of relocatable data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(NULL, &expected_stp_num); /* number of STP-type data num */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, COMMON_RESERVE_LEN); /* reserved */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, BYTE_NUM_OF_WORD); /* CRC */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function analyze_common_section
*******************************************************************************/

/*******************************************************************************
* Function Name: analyze_stp_section
* Description  : Read STP-type data section from configuration data.
* Arguments    : None.
* Return Value : Error code.
*******************************************************************************/
static int32_t analyze_stp_section(void)
{
    int32_t result = R_DK2_SUCCESS;

    result = read_config_section(STP_SECTION_ID);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, BYTE_NUM_OF_WORD); /* application code */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_word(&u64config_num, NULL); /* number of data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(&tile_num_dat, NULL); /* number of tiles */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(&del_zero_dat, NULL); /* delete zero data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(&dfc_dat, NULL); /* dfc */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, 1); /* reserved */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_word(&freq_dat, NULL); /* frequency */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_hword(&context_dat, NULL); /* context */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_hword(&state_dat, NULL); /* state */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, 4); /* reserved */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_str(NULL, 32);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    pu64config = (uint64_t *)&pu8config[config_index];

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function analyze_stp_section
*******************************************************************************/

/*******************************************************************************
* Function Name: init_config_parser
* Description  : Initialize parser of configuration data.
* Arguments    : pconfig - Address of configuration data.
* Return Value : None.
*******************************************************************************/
static int32_t init_config_parser(const void *const pconfig)
{
    int32_t result = R_DK2_SUCCESS;
    
    if ((NULL == pconfig) || (0 != ((uint32_t)pconfig & CONFIG_ALIGN_MASK)))
    {
        result = R_DK2_ERR_ARG;
    }
    else
    {
        section_size = 0;
        config_index = 0;
        section_index = 0;
        pu8config = pconfig;
    }
    
    return result;
}
/*******************************************************************************
End of function init_config_parser
*******************************************************************************/
/*******************************************************************************
* Function Name: read_config_section
* Description  : Read a section of configuration data.
* Arguments    : pid - string of section ID.
* Return Value : Error code.
*******************************************************************************/
static int32_t read_config_section(const dk2_char_t *const pid)
{
    int32_t result;
    const uint8_t *pconfig = pu8config;

    if (NULL == pconfig)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    /* Skip inter-section gap */
    if (section_index < section_size)
    {
        config_index += (section_size - section_index);
    }

    if (0 != (config_index & CONFIG_ALIGN_MASK))
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    section_size = *(const uint32_t *)&pconfig[config_index];
    config_index += SECTION_ID_LEN;
    section_index = SECTION_ID_LEN;

    /* read section id */
    result = read_config_str(pid, SECTION_ID_LEN);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function read_config_section
*******************************************************************************/

/*******************************************************************************
* Function Name: read_config_word
* Description  : Read 32 bit data from configuration data.
* Arguments    : pword - Address of 32 bit data.
*                pexpeced - Address of expected data.
* Return Value : Error code.
*******************************************************************************/
static int32_t read_config_word(uint32_t *const pword, const uint32_t *const pexpected)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *pconfig = pu8config;
    uint32_t word;

    if (NULL == pconfig)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((section_index + sizeof(uint32_t)) > section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    word = *(const uint32_t *)&pconfig[config_index];

    if (NULL != pword)
    {
        *pword = word;
    }

    if (NULL != pexpected)
    {
        if (word != (*pexpected))
        {
            result = R_DK2_ERR_FORMAT;
            goto func_end;
        }
    }

    config_index += sizeof(uint32_t);
    section_index += sizeof(uint32_t);

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function read_config_word
*******************************************************************************/

/*******************************************************************************
* Function Name: read_config_hword
* Description  : Read 16 bit data from configuration data.
* Arguments    : pword - Address of 16 bit data.
*                pexpeced - Address of expected data.
* Return Value : Error code.
*******************************************************************************/
static int32_t read_config_hword(uint16_t *const phword, const uint16_t *const pexpected)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *pconfig = pu8config;
    uint16_t hword;

    if (NULL == pconfig)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((section_index + sizeof(uint16_t)) > section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    hword = *(const uint16_t *)&pconfig[config_index];

    if (NULL != phword)
    {
        *phword = hword;
    }

    if (NULL != pexpected)
    {
        if (hword != (*pexpected))
        {
            result = R_DK2_ERR_FORMAT;
            goto func_end;
        }
    }

    config_index += sizeof(uint16_t);
    section_index += sizeof(uint16_t);

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function read_config_hword
*******************************************************************************/

/*******************************************************************************
* Function Name: read_config_byte
* Description  : Read 8 bit data from configuration data.
* Arguments    : pword - Address of 8 bit data.
*                pexpeced - Address of expected data.
* Return Value : Error code.
*******************************************************************************/
static int32_t read_config_byte(uint8_t *const pbyte, const uint8_t *const pexpected)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *pconfig = pu8config;
    uint8_t byte;

    if (NULL == pconfig)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((section_index + sizeof(uint8_t)) > section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    byte = pconfig[config_index];

    if (NULL != pbyte)
    {
        *pbyte = byte;
    }

    if (NULL != pexpected)
    {
        if (byte != (*pexpected))
        {
            result = R_DK2_ERR_FORMAT;
            goto func_end;
        }
    }

    config_index += sizeof(uint8_t);
    section_index += sizeof(uint8_t);

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function read_config_byte
*******************************************************************************/

/*******************************************************************************
* Function Name: read_config_str
* Description  : Read string data from configuration data.
* Arguments    : pexpeced - Address of expected data.
*                size - Size of string data.
* Return Value : Error code.
*******************************************************************************/
static int32_t read_config_str(const dk2_char_t *const pexpeced, const uint32_t size)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *pconfig = pu8config;

    if (NULL == pconfig)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((section_index + size) > section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    /* if pexpeced is not NULL, compare with expected data. */
    if (NULL != pexpeced)
    {
        uint32_t index;

        for (index = 0;index < size; index++)
        {
            if (pconfig[config_index + index] != pexpeced[index])
            {
                result = R_DK2_ERR_FORMAT;
                goto func_end;
            }
        }
    }

    config_index += size;
    section_index += size;

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function read_config_str
*******************************************************************************/

/*******************************************************************************
* Function Name: read_config_data
* Description  : Get current address and skip "size" bytes.
* Arguments    : pdata - Address of configuration data pointer.
*                size - Size of skip.
* Return Value : Error code.
*******************************************************************************/
static int32_t read_config_data(const uint32_t **const pdata, const uint32_t size)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *pconfig = pu8config;

    if (NULL == pconfig)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((section_index + size) > section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    if (NULL != pdata)
    {
        *pdata = (const uint32_t *)&pconfig[config_index];
    }
    config_index += size;
    section_index += size;

    goto func_end;

func_end:
    return result;
}
/*******************************************************************************
End of function read_config_data
*******************************************************************************/

#if(1) /* for mbed */
#else
/*******************************************************************************
* Function Name: calc_crc
* Description  : Calculate CRC.
* Arguments    : buf - Data pointer.
*                len - Length of CRC data.
* Return Value : CRC value.
*******************************************************************************/
static uint32_t calc_crc(const uint8_t *const buf, const uint32_t len)
{
    uint32_t result = CRC32_INITIAL_VALUE;
    uint32_t index;
    uint32_t bit;

    for (index = 0; index < len; index++)
    {
        result = result ^ buf[index];
        for (bit = 0; bit < CRC32_BIT_LOOP_NUM; bit++)
        {
            if (0 != (result & 1))
            {
                result >>= 1;
                result ^= CRC32_GEN_POLYNOMIAL;
            }
            else
            {
                result >>= 1;
            }
        }
    }
    return result ^ CRC32_XOR_VALUE;
}
/*******************************************************************************
End of function calc_crc
*******************************************************************************/
#endif

/*******************************************************************************
* Function Name: dk2_nmlint_isr
* Description  : Interrupt function of DRP.
* Arguments    : int_sense - Sense of interrupt.
* Return Value : None.
*******************************************************************************/
#if(1) /* for mbed */
static void dk2_nmlint_isr(void)
#else
static void dk2_nmlint_isr(uint32_t int_sense)
#endif
{
    uint32_t int_status;
    uint8_t id;
    int32_t tile_index;
    
    int_status = R_DK2_CORE_GetInt();
    
    id = 0;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (0 != ((int_status >> (tile_index + 16)) & R_DK2_TILE_0))
        {
            id = astart[tile_index];
            astart[tile_index] = 0;
            
            /* callback here */
            if (NULL != apint[tile_index])
            {
                apint[tile_index](id);
            }
        }
        else if ((0 != id) && (astart[tile_index] == id))
        {
            astart[tile_index] = 0;
        }
    }
}
/*******************************************************************************
End of function dk2_nmlint_isr
*******************************************************************************/

/* End of File */
