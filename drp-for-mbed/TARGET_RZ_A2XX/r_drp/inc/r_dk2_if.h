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
* File Name    : r_dk2_if.h
* Version      : $Rev: 100 $
* Device       : RZ
* Abstract     : Control software of DRP.
* Tool-Chain   : Renesas e2 studio
* OS           : Not use
* H/W Platform : Renesas Starter Kit
* Description  : Interface of DRP Driver.
* Limitation   : None
*******************************************************************************/
/*******************************************************************************
* History      : History is managed by Revision Control System.
*******************************************************************************/

#ifndef R_DK2_IF_H
#define R_DK2_IF_H

/*******************************************************************************
Includes <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
Macro definitions
*******************************************************************************/
#define R_DK2_SUCCESS                   (0)
#define R_DK2_ERR_ARG                   (-1)
#define R_DK2_ERR_FORMAT                (-2)
#define R_DK2_ERR_CRC                   (-3)
#define R_DK2_ERR_DEVICE                (-4)
#define R_DK2_ERR_BUSY                  (-5)
#define R_DK2_ERR_INTERNAL              (-6)
#define R_DK2_ERR_OVERWRITE             (-7)
#define R_DK2_ERR_OS                    (-8)
#define R_DK2_ERR_STATUS                (-9)
#define R_DK2_ERR_TILE_PATTERN          (-10)
#define R_DK2_ERR_STOPPED               (-11)
#define R_DK2_ERR_FREQ                  (-12) /* for future reservations */

#define R_DK2_TILE_NUM                  (6)
#define R_DK2_TILE_0                    (0x01)
#define R_DK2_TILE_1                    (0x02)
#define R_DK2_TILE_2                    (0x04)
#define R_DK2_TILE_3                    (0x08)
#define R_DK2_TILE_4                    (0x10)
#define R_DK2_TILE_5                    (0x20)

#define R_DK2_TILE_PATTERN_1_1_1_1_1_1  (0x00)
#define R_DK2_TILE_PATTERN_2_1_1_1_1    (0x10)
#define R_DK2_TILE_PATTERN_2_2_1_1      (0x11)
#define R_DK2_TILE_PATTERN_2_2_2        (0x12)
#define R_DK2_TILE_PATTERN_3_1_1_1      (0x20)
#define R_DK2_TILE_PATTERN_3_2_1        (0x21)
#define R_DK2_TILE_PATTERN_3_3          (0x22)
#define R_DK2_TILE_PATTERN_4_1_1        (0x30)
#define R_DK2_TILE_PATTERN_4_2          (0x31)
#define R_DK2_TILE_PATTERN_5_1          (0x40)
#define R_DK2_TILE_PATTERN_6            (0x50)

/* for future reservations */
#define R_DK2_FREQ_66000KHZ             (66000)
#define R_DK2_FREQ_52800KHZ             (52800)
#define R_DK2_FREQ_44000KHZ             (44000)
#define R_DK2_FREQ_37715KHZ             (37715)
#define R_DK2_FREQ_33000KHZ             (33000)
#define R_DK2_FREQ_29334KHZ             (29334)
#define R_DK2_FREQ_26400KHZ             (26400)
#define R_DK2_FREQ_24000KHZ             (24000)
#define R_DK2_FREQ_22000KHZ             (22000)
#define R_DK2_FREQ_20308KHZ             (20308)
#define R_DK2_FREQ_18858KHZ             (18858)
#define R_DK2_FREQ_17600KHZ             (17600)
#define R_DK2_FREQ_16500KHZ             (16500)
#define R_DK2_FREQ_15530KHZ             (15530)
#define R_DK2_FREQ_14667KHZ             (14667)
#define R_DK2_FREQ_13895KHZ             (13895)
#define R_DK2_FREQ_13200KHZ             (13200)
#define R_DK2_FREQ_12572KHZ             (12572)
#define R_DK2_FREQ_12000KHZ             (12000)
#define R_DK2_FREQ_11479KHZ             (11479)
#define R_DK2_FREQ_11000KHZ             (11000)
#define R_DK2_FREQ_10560KHZ             (10560)
#define R_DK2_FREQ_10154KHZ             (10154)
#define R_DK2_FREQ_09778KHZ             (9778)
#define R_DK2_FREQ_09429KHZ             (9429)
#define R_DK2_FREQ_09104KHZ             (9104)
#define R_DK2_FREQ_08800KHZ             (8800)
#define R_DK2_FREQ_08517KHZ             (8517)
#define R_DK2_FREQ_08250KHZ             (8250)
#define R_DK2_FREQ_08000KHZ             (8000)
#define R_DK2_FREQ_07765KHZ             (7765)
#define R_DK2_FREQ_07543KHZ             (7543)
#define R_DK2_FREQ_07334KHZ             (7334)
#define R_DK2_FREQ_07136KHZ             (7136)
#define R_DK2_FREQ_06948KHZ             (6948)
#define R_DK2_FREQ_06770KHZ             (6770)
#define R_DK2_FREQ_06600KHZ             (6600)
#define R_DK2_FREQ_06440KHZ             (6440)
#define R_DK2_FREQ_06286KHZ             (6286)
#define R_DK2_FREQ_06140KHZ             (6140)
#define R_DK2_FREQ_06000KHZ             (6000)
#define R_DK2_FREQ_05867KHZ             (5867)
#define R_DK2_FREQ_05740KHZ             (5740)
#define R_DK2_FREQ_05618KHZ             (5618)
#define R_DK2_FREQ_05500KHZ             (5500)
#define R_DK2_FREQ_05388KHZ             (5388)
#define R_DK2_FREQ_05280KHZ             (5280)
#define R_DK2_FREQ_05177KHZ             (5177)
#define R_DK2_FREQ_05077KHZ             (5077)
#define R_DK2_FREQ_04982KHZ             (4982)
#define R_DK2_FREQ_04889KHZ             (4889)
#define R_DK2_FREQ_04800KHZ             (4800)
#define R_DK2_FREQ_04715KHZ             (4715)
#define R_DK2_FREQ_04632KHZ             (4632)
#define R_DK2_FREQ_04552KHZ             (4552)
#define R_DK2_FREQ_04475KHZ             (4475)
#define R_DK2_FREQ_04400KHZ             (4400)
#define R_DK2_FREQ_04328KHZ             (4328)
#define R_DK2_FREQ_04259KHZ             (4259)
#define R_DK2_FREQ_04191KHZ             (4191)
#define R_DK2_FREQ_04125KHZ             (4125)
#define R_DK2_FREQ_04062KHZ             (4062)
#define R_DK2_FREQ_04000KHZ             (4000)
#define R_DK2_FREQ_03941KHZ             (3941)
#define R_DK2_FREQ_03883KHZ             (3883)
#define R_DK2_FREQ_03827KHZ             (3827)
#define R_DK2_FREQ_03772KHZ             (3772)
#define R_DK2_FREQ_03719KHZ             (3719)
#define R_DK2_FREQ_03667KHZ             (3667)
#define R_DK2_FREQ_03617KHZ             (3617)
#define R_DK2_FREQ_03568KHZ             (3568)
#define R_DK2_FREQ_03520KHZ             (3520)
#define R_DK2_FREQ_03474KHZ             (3474)
#define R_DK2_FREQ_03429KHZ             (3429)
#define R_DK2_FREQ_03385KHZ             (3385)
#define R_DK2_FREQ_03342KHZ             (3342)
#define R_DK2_FREQ_03300KHZ             (3300)
#define R_DK2_FREQ_03260KHZ             (3260)
#define R_DK2_FREQ_03220KHZ             (3220)
#define R_DK2_FREQ_03181KHZ             (3181)
#define R_DK2_FREQ_03143KHZ             (3143)
#define R_DK2_FREQ_03106KHZ             (3106)
#define R_DK2_FREQ_03070KHZ             (3070)
#define R_DK2_FREQ_03035KHZ             (3035)
#define R_DK2_FREQ_03000KHZ             (3000)
#define R_DK2_FREQ_02967KHZ             (2967)
#define R_DK2_FREQ_02934KHZ             (2934)
#define R_DK2_FREQ_02902KHZ             (2902)
#define R_DK2_FREQ_02870KHZ             (2870)
#define R_DK2_FREQ_02839KHZ             (2839)
#define R_DK2_FREQ_02809KHZ             (2809)
#define R_DK2_FREQ_02779KHZ             (2779)
#define R_DK2_FREQ_02750KHZ             (2750)
#define R_DK2_FREQ_02722KHZ             (2722)
#define R_DK2_FREQ_02694KHZ             (2694)
#define R_DK2_FREQ_02667KHZ             (2667)
#define R_DK2_FREQ_02640KHZ             (2640)
#define R_DK2_FREQ_02614KHZ             (2614)
#define R_DK2_FREQ_02589KHZ             (2589)
#define R_DK2_FREQ_02564KHZ             (2564)
#define R_DK2_FREQ_02539KHZ             (2539)
#define R_DK2_FREQ_02515KHZ             (2515)
#define R_DK2_FREQ_02491KHZ             (2491)
#define R_DK2_FREQ_02468KHZ             (2468)
#define R_DK2_FREQ_02445KHZ             (2445)
#define R_DK2_FREQ_02423KHZ             (2423)
#define R_DK2_FREQ_02400KHZ             (2400)
#define R_DK2_FREQ_02379KHZ             (2379)
#define R_DK2_FREQ_02358KHZ             (2358)
#define R_DK2_FREQ_02337KHZ             (2337)
#define R_DK2_FREQ_02316KHZ             (2316)
#define R_DK2_FREQ_02296KHZ             (2296)
#define R_DK2_FREQ_02276KHZ             (2276)
#define R_DK2_FREQ_02257KHZ             (2257)
#define R_DK2_FREQ_02238KHZ             (2238)
#define R_DK2_FREQ_02219KHZ             (2219)
#define R_DK2_FREQ_02200KHZ             (2200)
#define R_DK2_FREQ_02182KHZ             (2182)
#define R_DK2_FREQ_02164KHZ             (2164)
#define R_DK2_FREQ_02147KHZ             (2147)
#define R_DK2_FREQ_02130KHZ             (2130)
#define R_DK2_FREQ_02112KHZ             (2112)
#define R_DK2_FREQ_02096KHZ             (2096)
#define R_DK2_FREQ_02079KHZ             (2079)
#define R_DK2_FREQ_02063KHZ             (2063)

#define R_DK2_STATUS_UNLOADED           (0) /* for compatibility with past version */
#define R_DK2_STATUS_LOADED             (1)
#define R_DK2_STATUS_ACTIVATED          (2)
#define R_DK2_STATUS_STARTED            (3)

#if(1) /* for mbed */
#define INC_DRP_LIB(section, name) \
    __ASM volatile ( \
        ".section " #section "\n" \
        ".balign 32\n" \
        ".global g_drp_lib_" #name "\n" \
        "g_drp_lib_" #name ":\n" \
        ".incbin \"r_drp_" #name ".dat\"\n" \
        \
        ".global g_label_sizeof_" #name "\n" \
        ".set g_label_sizeof_" #name ", . - g_drp_lib_" #name "\n" \
        ".balign 4\n" \
        ".section .text\n" \
    ); \
    extern const uint8_t g_drp_lib_##name[]; \
    extern const uint32_t * g_label_sizeof_##name; \
    const uint32_t g_sizeof_##name = (uint32_t)&g_label_sizeof_##name;
#endif

/*******************************************************************************
Typedef definitions
*******************************************************************************/
typedef void (*load_cb_t)(uint8_t id);
typedef void (*int_cb_t)(uint8_t id);
typedef struct config_info_st
{
    uint8_t type;
    char *pname;
    uint32_t ver;
    uint32_t cid;
} config_info_t;

/*******************************************************************************
Public Functions
*******************************************************************************/
int32_t R_DK2_Initialize(void);
int32_t R_DK2_Uninitialize(void);
int32_t R_DK2_Load(const void *const pconfig, const uint8_t top_tiles, const uint32_t tile_pattern, const load_cb_t pload, const int_cb_t pint, uint8_t *const paid);
int32_t R_DK2_Unload(const uint8_t id, uint8_t *const paid);
int32_t R_DK2_Activate(const uint8_t id, const uint32_t freq);
int32_t R_DK2_Inactivate(const uint8_t id);
int32_t R_DK2_Start(const uint8_t id, const void *const pparam, const uint32_t size);
int32_t R_DK2_GetStatus(const uint8_t id);
int32_t R_DK2_GetInfo(const void *const pconfig, config_info_t *const pinfo, const bool crc_check);
uint32_t R_DK2_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* R_DK2_IF_H */
