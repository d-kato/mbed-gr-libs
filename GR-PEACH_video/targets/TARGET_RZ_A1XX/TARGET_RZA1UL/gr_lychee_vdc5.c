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
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         gr_peach_vdc5.c
* @version
* $Rev:
* $Date::
* @brief        VDC5 driver API wrapper function in C interface
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdio.h>
#include    <string.h>

#include    "r_typedefs.h"
#include    "r_vdc5.h"
#include    "gr_board_vdc5.h"
#include    "r_ceu.h"

#include    "mbed_assert.h"
#include    "pinmap.h"
#include    "RZ_A1_Init.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define STP91_BIT   (0x02u)
#define STP90_BIT   (0x01u)
#define STBRQ25_BIT (0x20u)
#define STBAK25_BIT (0x20u)
#define STBRQ24_BIT (0x10u)
#define STBAK24_BIT (0x10u)
#define CLUT8_TABLE_NUM         (256u)
#define CLUT4_TABLE_NUM         (16u)
#define CLUT1_TABLE_NUM         (2u)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum {
    VDC5_CH0,
} VDC5Name;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/
static const PinMap PinMap_DV_INPUT_PIN[] = {
    {P7_8  , VDC5_CH0, 3}, /* DV0_CLK    */
    {P7_9  , VDC5_CH0, 3}, /* DV0_VSYNC  */
    {P7_10 , VDC5_CH0, 3}, /* DV0_HSYNC  */
    {P1_11 , VDC5_CH0, 4}, /* DV0_DATA7  */
    {P1_10 , VDC5_CH0, 4}, /* DV0_DATA6  */
    {P1_9  , VDC5_CH0, 4}, /* DV0_DATA5  */
    {P1_8  , VDC5_CH0, 4}, /* DV0_DATA4  */
    {P1_3  , VDC5_CH0, 4}, /* DV0_DATA3  */
    {P1_2  , VDC5_CH0, 4}, /* DV0_DATA2  */
    {P1_1  , VDC5_CH0, 4}, /* DV0_DATA1  */
    {P1_0  , VDC5_CH0, 4}, /* DV0_DATA0  */
    {NC    , NC   , 0}
};

static const PinMap PinMap_CEU_PIN[] = {
    {P3_14 , 0, 2}, /* VIO_CLK    */
    {P4_0  , 0, 2}, /* VIO_VD     */
    {P4_1  , 0, 2}, /* VIO_HD     */
    {P1_15 , 0, 4}, /* VIO_D7     */
    {P1_14 , 0, 4}, /* VIO_D6     */
    {P1_13 , 0, 4}, /* VIO_D5     */
    {P1_12 , 0, 4}, /* VIO_D4     */
    {P1_7  , 0, 4}, /* VIO_D3     */
    {P1_6  , 0, 4}, /* VIO_D2     */
    {P1_5  , 0, 4}, /* VIO_D1     */
    {P1_4  , 0, 4}, /* VIO_D0     */
    {NC    , NC   , 0}
};

static const PinMap PinMap_LCD_DISP_PIN[] = {
    {P6_15 , VDC5_CH0, 2}, /* LCD0_DATA23  */
    {P6_14 , VDC5_CH0, 2}, /* LCD0_DATA22  */
    {P6_13 , VDC5_CH0, 2}, /* LCD0_DATA21  */
    {P6_12 , VDC5_CH0, 2}, /* LCD0_DATA20  */
    {P6_11 , VDC5_CH0, 2}, /* LCD0_DATA19  */
    {P6_10 , VDC5_CH0, 2}, /* LCD0_DATA18  */
    {P6_9  , VDC5_CH0, 2}, /* LCD0_DATA17  */
    {P6_8  , VDC5_CH0, 2}, /* LCD0_DATA16  */
    {P6_7  , VDC5_CH0, 2}, /* LCD0_DATA15  */
    {P6_6  , VDC5_CH0, 2}, /* LCD0_DATA14  */
    {P6_5  , VDC5_CH0, 2}, /* LCD0_DATA13  */
    {P6_4  , VDC5_CH0, 2}, /* LCD0_DATA12  */
    {P6_3  , VDC5_CH0, 2}, /* LCD0_DATA11  */
    {P6_2  , VDC5_CH0, 2}, /* LCD0_DATA10  */
    {P6_1  , VDC5_CH0, 2}, /* LCD0_DATA9   */
    {P6_0  , VDC5_CH0, 2}, /* LCD0_DATA8   */
    {P3_7  , VDC5_CH0, 3}, /* LCD0_DATA7   */
    {P3_6  , VDC5_CH0, 3}, /* LCD0_DATA6   */
    {P3_5  , VDC5_CH0, 3}, /* LCD0_DATA5   */
    {P3_4  , VDC5_CH0, 3}, /* LCD0_DATA4   */
    {P3_3  , VDC5_CH0, 3}, /* LCD0_DATA3   */
    {P3_2  , VDC5_CH0, 3}, /* LCD0_DATA2   */
    {P3_1  , VDC5_CH0, 3}, /* LCD0_DATA1   */
    {P3_0  , VDC5_CH0, 3}, /* LCD0_DATA0   */
    {P7_5  , VDC5_CH0, 6}, /* LCD0_TCON4   */
    {P5_2  , VDC5_CH0, 5}, /* LCD0_TCON2   */
    {P5_1  , VDC5_CH0, 5}, /* LCD0_TCON1   */
    {P5_0  , VDC5_CH0, 5}, /* LCD0_TCON0   */
    {P7_4  , VDC5_CH0, 6}, /* LCD0_CLK     */
    {NC    , NC   , 0}
};

static const IRQn_Type vdc5_irq_set_tbl[] = {
    S0_VI_VSYNC0_IRQn,
    S0_LO_VSYNC0_IRQn,
    S0_VSYNCERR0_IRQn,
    GR3_VLINE0_IRQn,
    S0_VFIELD0_IRQn,
    IV1_VBUFERR0_IRQn,
    IV3_VBUFERR0_IRQn,
    IV5_VBUFERR0_IRQn,
    IV6_VBUFERR0_IRQn,
    S0_WLINE0_IRQn,
};

static const uint32_t color_table256[CLUT8_TABLE_NUM] = {
  0xFF000000u, 0xFF010101u, 0xFF020202u, 0xFF030303u, 0xFF040404u, 0xFF050505u, 0xFF060606u, 0xFF070707u, /*   0 -   7 */
  0xFF080808u, 0xFF090909u, 0xFF0A0A0Au, 0xFF0B0B0Bu, 0xFF0C0C0Cu, 0xFF0D0D0Du, 0xFF0E0E0Eu, 0xFF0F0F0Fu, /*   8 -  15 */
  0xFF101010u, 0xFF111111u, 0xFF121212u, 0xFF131313u, 0xFF141414u, 0xFF151515u, 0xFF161616u, 0xFF171717u, /*  16 -  23 */
  0xFF181818u, 0xFF191919u, 0xFF1A1A1Au, 0xFF1B1B1Bu, 0xFF1C1C1Cu, 0xFF1E1E1Eu, 0xFF1E1E1Eu, 0xFF1F1F1Fu, /*  24 -  31 */
  0xFF202020u, 0xFF212121u, 0xFF222222u, 0xFF232323u, 0xFF242424u, 0xFF252525u, 0xFF262626u, 0xFF272727u, /*  32 -  39 */
  0xFF282828u, 0xFF292929u, 0xFF2A2A2Au, 0xFF2B2B2Bu, 0xFF2C2C2Cu, 0xFF2E2E2Eu, 0xFF2E2E2Eu, 0xFF2F2F2Fu, /*  40 -  47 */
  0xFF303030u, 0xFF313131u, 0xFF323232u, 0xFF333333u, 0xFF343434u, 0xFF353535u, 0xFF363636u, 0xFF373737u, /*  48 -  55 */
  0xFF383838u, 0xFF393939u, 0xFF3A3A3Au, 0xFF3B3B3Bu, 0xFF3C3C3Cu, 0xFF3E3E3Eu, 0xFF3E3E3Eu, 0xFF3F3F3Fu, /*  56 -  63 */
  0xFF404040u, 0xFF414141u, 0xFF424242u, 0xFF434343u, 0xFF444444u, 0xFF454545u, 0xFF464646u, 0xFF474747u, /*  64 -  71 */
  0xFF484848u, 0xFF494949u, 0xFF4A4A4Au, 0xFF4B4B4Bu, 0xFF4C4C4Cu, 0xFF4E4E4Eu, 0xFF4E4E4Eu, 0xFF4F4F4Fu, /*  72 -  79 */
  0xFF505050u, 0xFF515151u, 0xFF525252u, 0xFF535353u, 0xFF545454u, 0xFF555555u, 0xFF565656u, 0xFF575757u, /*  80 -  87 */
  0xFF585858u, 0xFF595959u, 0xFF5A5A5Au, 0xFF5B5B5Bu, 0xFF5C5C5Cu, 0xFF5E5E5Eu, 0xFF5E5E5Eu, 0xFF5F5F5Fu, /*  88 -  95 */
  0xFF606060u, 0xFF616161u, 0xFF626262u, 0xFF636363u, 0xFF646464u, 0xFF656565u, 0xFF666666u, 0xFF676767u, /*  96 - 103 */
  0xFF686868u, 0xFF696969u, 0xFF6A6A6Au, 0xFF6B6B6Bu, 0xFF6C6C6Cu, 0xFF6E6E6Eu, 0xFF6E6E6Eu, 0xFF6F6F6Fu, /* 104 - 111 */
  0xFF707070u, 0xFF717171u, 0xFF727272u, 0xFF737373u, 0xFF747474u, 0xFF757575u, 0xFF767676u, 0xFF777777u, /* 112 - 119 */
  0xFF787878u, 0xFF797979u, 0xFF7A7A7Au, 0xFF7B7B7Bu, 0xFF7C7C7Cu, 0xFF7E7E7Eu, 0xFF7E7E7Eu, 0xFF7F7F7Fu, /* 120 - 127 */
  0xFF808080u, 0xFF818181u, 0xFF828282u, 0xFF838383u, 0xFF848484u, 0xFF858585u, 0xFF868686u, 0xFF878787u, /* 128 - 135 */
  0xFF888888u, 0xFF898989u, 0xFF8A8A8Au, 0xFF8B8B8Bu, 0xFF8C8C8Cu, 0xFF8E8E8Eu, 0xFF8E8E8Eu, 0xFF8F8F8Fu, /* 136 - 143 */
  0xFF909090u, 0xFF919191u, 0xFF929292u, 0xFF939393u, 0xFF949494u, 0xFF959595u, 0xFF969696u, 0xFF979797u, /* 144 - 151 */
  0xFF989898u, 0xFF999999u, 0xFF9A9A9Au, 0xFF9B9B9Bu, 0xFF9C9C9Cu, 0xFF9E9E9Eu, 0xFF9E9E9Eu, 0xFF9F9F9Fu, /* 152 - 159 */
  0xFFA0A0A0u, 0xFFA1A1A1u, 0xFFA2A2A2u, 0xFFA3A3A3u, 0xFFA4A4A4u, 0xFFA5A5A5u, 0xFFA6A6A6u, 0xFFA7A7A7u, /* 160 - 167 */
  0xFFA8A8A8u, 0xFFA9A9A9u, 0xFFAAAAAAu, 0xFFABABABu, 0xFFACACACu, 0xFFAEAEAEu, 0xFFAEAEAEu, 0xFFAFAFAFu, /* 168 - 175 */
  0xFFB0B0B0u, 0xFFB1B1B1u, 0xFFB2B2B2u, 0xFFB3B3B3u, 0xFFB4B4B4u, 0xFFB5B5B5u, 0xFFB6B6B6u, 0xFFB7B7B7u, /* 176 - 183 */
  0xFFB8B8B8u, 0xFFB9B9B9u, 0xFFBABABAu, 0xFFBBBBBBu, 0xFFBCBCBCu, 0xFFBEBEBEu, 0xFFBEBEBEu, 0xFFBFBFBFu, /* 184 - 191 */
  0xFFC0C0C0u, 0xFFC1C1C1u, 0xFFC2C2C2u, 0xFFC3C3C3u, 0xFFC4C4C4u, 0xFFC5C5C5u, 0xFFC6C6C6u, 0xFFC7C7C7u, /* 192 - 199 */
  0xFFC8C8C8u, 0xFFC9C9C9u, 0xFFCACACAu, 0xFFCBCBCBu, 0xFFCCCCCCu, 0xFFCECECEu, 0xFFCECECEu, 0xFFCFCFCFu, /* 200 - 207 */
  0xFFD0D0D0u, 0xFFD1D1D1u, 0xFFD2D2D2u, 0xFFD3D3D3u, 0xFFD4D4D4u, 0xFFD5D5D5u, 0xFFD6D6D6u, 0xFFD7D7D7u, /* 208 - 215 */
  0xFFD8D8D8u, 0xFFD9D9D9u, 0xFFDADADAu, 0xFFDBDBDBu, 0xFFDCDCDCu, 0xFFDEDEDEu, 0xFFDEDEDEu, 0xFFDFDFDFu, /* 216 - 223 */
  0xFFE0E0E0u, 0xFFE1E1E1u, 0xFFE2E2E2u, 0xFFE3E3E3u, 0xFFE4E4E4u, 0xFFE5E5E5u, 0xFFE6E6E6u, 0xFFE7E7E7u, /* 224 - 231 */
  0xFFE8E8E8u, 0xFFE9E9E9u, 0xFFEAEAEAu, 0xFFEBEBEBu, 0xFFECECECu, 0xFFEEEEEEu, 0xFFEEEEEEu, 0xFFEFEFEFu, /* 232 - 239 */
  0xFFF0F0F0u, 0xFFF1F1F1u, 0xFFF2F2F2u, 0xFFF3F3F3u, 0xFFF4F4F4u, 0xFFF5F5F5u, 0xFFF6F6F6u, 0xFFF7F7F7u, /* 240 - 247 */
  0xFFF8F8F8u, 0xFFF9F9F9u, 0xFFFAFAFAu, 0xFFFBFBFBu, 0xFFFCFCFCu, 0xFFFEFEFEu, 0xFFFEFEFEu, 0xFFFFFFFFu  /* 248 - 255 */
};

static const uint32_t color_table16[CLUT4_TABLE_NUM] = {
  0xFF000000u, 0xFF111111u, 0xFF222222u, 0xFF333333u, 0xFF444444u, 0xFF555555u, 0xFF666666u, 0xFF777777u, /*   0 -   7 */
  0xFF888888u, 0xFF999999u, 0xFFAAAAAAu, 0xFFBBBBBBu, 0xFFCCCCCCu, 0xFFDDDDDDu, 0xFFEEEEEEu, 0xFFFFFFFFu  /*   8 -  15 */
};

static const uint32_t color_table2[CLUT1_TABLE_NUM] = {
  0xFF000000u, 0xFFFFFFFFu
};

/******************************************************************************
Private global variables and functions
******************************************************************************/
static drv_video_input_sel_t _drv_video_input_sel;

static void init_func (const uint32_t user_num);
static void DRV_Graphics_Irq_Set(vdc5_int_type_t irq, uint32_t enable);

/**************************************************************************//**
 * @brief       User-defined function within R_VDC5_Initialize
 * @param[in]   user_num                : VDC5 channel
 * @retval      None
******************************************************************************/
static void init_func (const uint32_t user_num)
{
    uint32_t            reg_data;
    volatile uint8_t    dummy_read;

    if ((vdc5_channel_t)user_num == VDC5_CHANNEL_0) {

        /* Standby control register 9 (STBCR9)
            b1      ------0-;  MSTP91 : 0 : Video display controller channel 0 & LVDS enable */
        reg_data    = (uint32_t)CPG.STBCR9 & (uint32_t)~STP91_BIT;
        CPG.STBCR9  = (uint8_t)reg_data;
        /* In order to reflect the change, a dummy read should be done. */
        dummy_read = CPG.STBCR9;

        /* Standby Request Register 2 (STBREQ2)
            b5      --0-----;  STBRQ25 : The standby request to VDC5 channel 0 is invalid. */
        reg_data    = (uint32_t)CPG.STBREQ2 & (uint32_t)~STBRQ25_BIT;
        CPG.STBREQ2 = (uint8_t)reg_data;
        /* Standby Acknowledge Register 2 (STBACK2)
            b5      --*-----;  STBAK25 : Standby acknowledgement from VDC5 channel 0. */
        while (((uint32_t)CPG.STBACK2 & (uint32_t)STBAK25_BIT) != 0u) {
            /* Wait for the STBAK25 to be cleared to 0. */
        }

        /* Standby control register 9 (STBCR9)
            b0      -------0;  MSTP90 : 0 : Video display controller channel 1 enable */
        reg_data    = (uint32_t)CPG.STBCR9 & (uint32_t)~(STP91_BIT|STP90_BIT);
        CPG.STBCR9  = (uint8_t)reg_data;
        /* In order to reflect the change, a dummy read should be done. */
        dummy_read = CPG.STBCR9;
        (void)dummy_read; /* unused */

        /* Standby Request Register 2 (STBREQ2)
            b4      ---0----;  STBRQ24 : The standby request to VDC5 channel 1 is invalid. */
        reg_data    = (uint32_t)CPG.STBREQ2 & (uint32_t)~STBRQ24_BIT;
        CPG.STBREQ2 = (uint8_t)reg_data;
        /* Standby Acknowledge Register 2 (STBACK2)
            b4      ---*----;  STBAK24 : Standby acknowledgement from VDC5 channel 1. */
        while (((uint32_t)CPG.STBACK2 & (uint32_t)STBAK24_BIT) != 0u) {
            /* Wait for the STBAK24 to be cleared to 0. */
        }
    }
}   /* End of function init_func() */

/**************************************************************************//**
 * @brief       Interrupt service routine acquisition processing
 *
 *              Description:<br>
 *              This function returns the function pointer to the specified interrupt service routine.
 * @param[in]   irq                     : VDC5 interrupt type
 * @param[in]   enable                  : VDC5 interrupt enable
 * @retval      None
******************************************************************************/
static void DRV_Graphics_Irq_Set(vdc5_int_type_t irq, uint32_t enable)
{
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    IRQn_Type IRQn;
    IRQHandler handler;

    IRQn = vdc5_irq_set_tbl[irq];
    handler = R_VDC5_GetISR(ch, irq);

    if (enable) {
        InterruptHandlerRegister(IRQn, (void (*)(uint32_t))handler);
        GIC_SetPriority(IRQn, 5);
        GIC_EnableIRQ(IRQn);
    } else {
        GIC_DisableIRQ(IRQn);
    }
} /* End of function DRV_Graphics_Irq_Set() */

static void (* ceu_callback_func)(vdc5_int_type_t);
static void Ceu_Irq_Handler_wrapper(ceu_int_type_t ceu_int_type) {
    if ((ceu_callback_func != NULL) && ((ceu_int_type & CEU_INT_CPEIE) != 0)) {
        ceu_callback_func(VDC5_INT_TYPE_S0_VFIELD);
    }
}

/**************************************************************************//**
 * @brief       Interrupt callback setup
 *              This function performs the following processing:
 *              - Enables the interrupt when the pointer to the corresponding interrupt callback function is specified.
 *              - Registers the specified interrupt callback function.
 *              - Disables the interrupt when the pointer to the corresponding interrupt callback function is not
 *                specified.
 * @param[in]   irq                     : VDC5 interrupt type
 * @param[in]   num                     : Interrupt line number
 * @param[in]   * callback              : Interrupt callback function pointer
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Irq_Handler_Set(
    vdc5_int_type_t irq,
    uint16_t num,
    void (* callback)(vdc5_int_type_t)  )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;

    if ((_drv_video_input_sel == DRV_INPUT_SEL_CEU) && (irq == VDC5_INT_TYPE_S0_VFIELD)) {
        /* ceu wrapper */
        if( callback == NULL ) {
            R_CEU_InterruptDisable();
            ceu_callback_func = NULL;
        } else {
            ceu_callback_func = callback;
            R_CEU_InterruptEnable(CEU_INT_CPEIE, &Ceu_Irq_Handler_wrapper);
        }
    } else {
        vdc5_channel_t          ch          = VDC5_CHANNEL_0;
        vdc5_error_t            error;
        vdc5_int_t              interrupt;

        if( callback == NULL ) {
            DRV_Graphics_Irq_Set( irq, 0 );
        } else {
            DRV_Graphics_Irq_Set( irq, 1 );
        }

        /* Interrupt parameter */
        interrupt.type      = irq;        /* Interrupt type */
        interrupt.line_num  = num ;       /* Line number */

        /* Interrupt parameter */
        interrupt.callback = callback;    /* Callback function pointer */
        /* Set interrupt service routine */
        error = R_VDC5_CallbackISR(ch, &interrupt);
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }
    return drv_error ;
} /* End of function DRV_Graphics_Irq_Handler_Set() */

/**************************************************************************//**
 * @brief       LCD output port initialization processing
 * @param[in]   pin                     : Pin assign for LCD output
 * @param[in]   pin_count               : Total number of pin assign
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Lcd_Port_Init( PinName *pin, uint32_t pin_count )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    uint32_t count;

    for( count = 0 ; count < pin_count ; count++ ) {
        pinmap_peripheral(pin[count], PinMap_LCD_DISP_PIN);
        pinmap_pinout(pin[count], PinMap_LCD_DISP_PIN);
    }
    return drv_error;
} /* End of function DRV_Graphics_Lcd_Port_Init() */

/**************************************************************************//**
 * @brief       LVDS output port initialization processing
 * @param[in]   pin                     : Pin assign for LVDS output
 * @param[in]   pin_count               : Total number of pin assign
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Lvds_Port_Init( PinName *pin, uint32_t pin_count )
{
    return DRV_GRAPHICS_VDC5_ERR;
} /* End of function DRV_Graphics_Lvds_Port_Init() */

/**************************************************************************//**
 * @brief       Digital video inpout port initialization processing
 * @param[in]   pin                     : Pin assign for digital video input port
 * @param[in]   pin_count               : Total number of pin assign
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Dvinput_Port_Init( PinName *pin, uint32_t pin_count )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    uint32_t count;

    for( count = 0 ; count < pin_count ; count++ ) {
        pinmap_peripheral(pin[count], PinMap_DV_INPUT_PIN);
        pinmap_pinout(pin[count], PinMap_DV_INPUT_PIN);
    }
    return drv_error;
} /* End of function DRV_Graphics_Dvinput_Port_Init() */

/**************************************************************************//**
 * @brief       CEU inpout port initialization processing
 * @param[in]   pin                     : Pin assign for digital video input port
 * @param[in]   pin_count               : Total number of pin assign
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_CEU_Port_Init( PinName *pin, uint32_t pin_count )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    uint32_t count;

    for( count = 0 ; count < pin_count ; count++ ) {
        pinmap_peripheral(pin[count], PinMap_CEU_PIN);
        pinmap_pinout(pin[count], PinMap_CEU_PIN);
    }
    return drv_error;
} /* End of function DRV_Graphics_CEU_Port_Init() */

/**************************************************************************//**
 * @brief       Graphics initialization processing
 * @param[in]   drv_lcd_config          : LCD configuration
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Init( drv_lcd_config_t * drv_lcd_config )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    vdc5_error_t            error;
    vdc5_init_t             init;
    double                  InputClock;
    double                  OutputClock;
    double                  OutputClock_wk;
    double                  val;
    double                  val_last = 200.0;
    vdc5_panel_clk_dcdr_t   set_pram = VDC5_PANEL_CLKDIV_1_1;
    int wk_div;

    if (RZ_A1_IsClockMode0() == false) {
        InputClock = CM1_RENESAS_RZ_A1_P1_CLK / 1000000;
    } else {
        InputClock = CM0_RENESAS_RZ_A1_P1_CLK / 1000000;
    }

    /* Initialization parameter */
    init.panel_icksel   = VDC5_PANEL_ICKSEL_PERI;      /* Panel clock select */
    init.lvds           = NULL;                        /* LVDS parameter */
    OutputClock         = drv_lcd_config->outputClock;
    init.panel_dcdr     = VDC5_PANEL_CLKDIV_1_1;
    for (wk_div = 1; wk_div <= 32; wk_div++) {
        switch (wk_div) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 12:
            case 16:
            case 24:
            case 32:
                OutputClock_wk = InputClock / wk_div;
                if (OutputClock_wk > OutputClock) {
                    val = OutputClock_wk - OutputClock;
                } else {
                    val = OutputClock - OutputClock_wk;
                }
                if (val < val_last) {
                    init.panel_dcdr = set_pram;
                    val_last = val;
                }
                set_pram++;
                break;
            default:
                break;
        }
    }

    /* Initialize (Set module clock to VDC5) */
    error = R_VDC5_Initialize( ch, &init, &init_func, (uint32_t)ch );
    if (error != VDC5_OK) {
        drv_error = DRV_GRAPHICS_VDC5_ERR;
    }

    if ( drv_error == DRV_GRAPHICS_OK ) {
        vdc5_sync_ctrl_t        sync_ctrl;

        /* Sync signal control */
        sync_ctrl.res_vs_sel    = VDC5_ON;                              /* Vsync signal output select (free-running Vsync on/off control) */
        /* Sync signal output and full-screen enable signal select */
        sync_ctrl.res_vs_in_sel = VDC5_RES_VS_IN_SEL_SC0;
        sync_ctrl.res_fv        = drv_lcd_config->v_toatal_period-1;    /* Free-running Vsync period setting */
        sync_ctrl.res_fh        = drv_lcd_config->h_toatal_period-1;    /* Hsync period setting */
        sync_ctrl.res_vsdly     = (uint16_t)0u;                         /* Vsync signal delay control */
        /* Full-screen enable control */
        sync_ctrl.res_f.vs      = (drv_lcd_config->v_back_porch);
        sync_ctrl.res_f.vw      = (drv_lcd_config->v_disp_widht);
        sync_ctrl.res_f.hs      = (drv_lcd_config->h_back_porch);
        sync_ctrl.res_f.hw      = (drv_lcd_config->h_disp_widht);
        sync_ctrl.vsync_cpmpe   = NULL;                                 /* Vsync signal compensation */
        /* Sync control */
        error = R_VDC5_SyncControl( ch, &sync_ctrl );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }

    if ( drv_error == DRV_GRAPHICS_OK ) {
        vdc5_output_t           output;
        vdc5_lcd_tcon_timing_t  lcd_tcon_timing_VS;
        vdc5_lcd_tcon_timing_t  lcd_tcon_timing_VE;
        vdc5_lcd_tcon_timing_t  lcd_tcon_timing_HS;
        vdc5_lcd_tcon_timing_t  lcd_tcon_timing_HE;
        vdc5_lcd_tcon_timing_t  lcd_tcon_timing_DE;

        /* Output parameter */
        output.tcon_half        = (drv_lcd_config->h_toatal_period-1)/2; /* TCON reference timing, 1/2fH timing */
        output.tcon_offset      = 0;                                     /* TCON reference timing, offset Hsync signal timing */

        /* LCD TCON timing setting */
        if( drv_lcd_config->v_sync_port != DRV_LCD_TCON_PIN_NON ) {
            lcd_tcon_timing_VS.tcon_hsvs   = 0u;
            lcd_tcon_timing_VS.tcon_hwvw   = (drv_lcd_config->v_sync_width * 2u);
            lcd_tcon_timing_VS.tcon_md     = VDC5_LCD_TCON_POLMD_NORMAL;
            lcd_tcon_timing_VS.tcon_hs_sel = VDC5_LCD_TCON_REFSEL_HSYNC;
            lcd_tcon_timing_VS.tcon_inv    = (vdc5_sig_pol_t)drv_lcd_config->v_sync_port_polarity;
            lcd_tcon_timing_VS.tcon_pin    = (vdc5_lcd_tcon_pin_t)drv_lcd_config->v_sync_port;
            lcd_tcon_timing_VS.outcnt_edge = VDC5_EDGE_FALLING;
            output.outctrl[VDC5_LCD_TCONSIG_STVA_VS]   = &lcd_tcon_timing_VS;  /* STVA/VS: Vsync      */
        } else {
            output.outctrl[VDC5_LCD_TCONSIG_STVA_VS]   = NULL;                 /* STVA/VS: Vsync      */
        }

        if( drv_lcd_config->h_sync_port != DRV_LCD_TCON_PIN_NON ) {
            lcd_tcon_timing_HS.tcon_hsvs   = 0u;
            lcd_tcon_timing_HS.tcon_hwvw   = drv_lcd_config->h_sync_width;
            lcd_tcon_timing_HS.tcon_md     = VDC5_LCD_TCON_POLMD_NORMAL;
            lcd_tcon_timing_HS.tcon_hs_sel = VDC5_LCD_TCON_REFSEL_HSYNC;
            lcd_tcon_timing_HS.tcon_inv    = (vdc5_sig_pol_t)drv_lcd_config->h_sync_port_polarity;
            lcd_tcon_timing_HS.tcon_pin    = (vdc5_lcd_tcon_pin_t)drv_lcd_config->h_sync_port;
            lcd_tcon_timing_HS.outcnt_edge = VDC5_EDGE_FALLING;
            output.outctrl[VDC5_LCD_TCONSIG_STH_SP_HS]   = &lcd_tcon_timing_HS;  /* STH/SP/HS: Hsync       */
        } else {
            output.outctrl[VDC5_LCD_TCONSIG_STH_SP_HS]   = NULL;                 /* STH/SP/HS: Hsync       */
        }

        if( drv_lcd_config->de_port != DRV_LCD_TCON_PIN_NON ) {
            lcd_tcon_timing_VE.tcon_hsvs   = (drv_lcd_config->v_back_porch * 2u);
            lcd_tcon_timing_VE.tcon_hwvw   = (drv_lcd_config->v_disp_widht * 2u);
            lcd_tcon_timing_VE.tcon_md     = VDC5_LCD_TCON_POLMD_NORMAL;
            lcd_tcon_timing_VE.tcon_hs_sel = VDC5_LCD_TCON_REFSEL_HSYNC;
            lcd_tcon_timing_VE.tcon_inv    = (vdc5_sig_pol_t)drv_lcd_config->de_port_polarity;
            lcd_tcon_timing_VE.tcon_pin    = VDC5_LCD_TCON_PIN_NON;
            lcd_tcon_timing_VE.outcnt_edge = VDC5_EDGE_FALLING;
            output.outctrl[VDC5_LCD_TCONSIG_STVB_VE]   = &lcd_tcon_timing_VE;  /* STVB/VE: Not used   */

            lcd_tcon_timing_HE.tcon_hsvs   = drv_lcd_config->h_back_porch;
            lcd_tcon_timing_HE.tcon_hwvw   = drv_lcd_config->h_disp_widht;
            lcd_tcon_timing_HE.tcon_md     = VDC5_LCD_TCON_POLMD_NORMAL;
            lcd_tcon_timing_HE.tcon_hs_sel = VDC5_LCD_TCON_REFSEL_HSYNC;
            lcd_tcon_timing_HE.tcon_inv    = (vdc5_sig_pol_t)drv_lcd_config->de_port_polarity;
            lcd_tcon_timing_HE.tcon_pin    = VDC5_LCD_TCON_PIN_NON;
            lcd_tcon_timing_HE.outcnt_edge = VDC5_EDGE_FALLING;
            output.outctrl[VDC5_LCD_TCONSIG_STB_LP_HE] = &lcd_tcon_timing_HE;  /* STB/LP/HE: Not used */

            lcd_tcon_timing_DE.tcon_hsvs   = 0u;
            lcd_tcon_timing_DE.tcon_hwvw   = 0u;
            lcd_tcon_timing_DE.tcon_md     = VDC5_LCD_TCON_POLMD_NORMAL;
            lcd_tcon_timing_DE.tcon_hs_sel = VDC5_LCD_TCON_REFSEL_HSYNC;
            lcd_tcon_timing_DE.tcon_inv    = (vdc5_sig_pol_t)drv_lcd_config->de_port_polarity;
            lcd_tcon_timing_DE.tcon_pin    = (vdc5_lcd_tcon_pin_t)drv_lcd_config->de_port;
            lcd_tcon_timing_DE.outcnt_edge = VDC5_EDGE_FALLING;
            output.outctrl[VDC5_LCD_TCONSIG_DE]   = &lcd_tcon_timing_DE;  /* DE      */
        } else {
            output.outctrl[VDC5_LCD_TCONSIG_STVB_VE]   = NULL;            /* STVB/VE: Not used   */
            output.outctrl[VDC5_LCD_TCONSIG_STB_LP_HE] = NULL;            /* STB/LP/HE: Not used */
            output.outctrl[VDC5_LCD_TCONSIG_DE]        = NULL;            /* DE                  */
        }

        output.outctrl[VDC5_LCD_TCONSIG_CPV_GCK]   = NULL;
        output.outctrl[VDC5_LCD_TCONSIG_POLA]      = NULL;
        output.outctrl[VDC5_LCD_TCONSIG_POLB]      = NULL;

        output.outcnt_lcd_edge  = (vdc5_edge_t)drv_lcd_config->lcd_edge;  /* Output phase control of LCD_DATA23 to LCD_DATA0 pin */
        output.out_endian_on    = VDC5_OFF;                               /* Bit endian change on/off control */
        output.out_swap_on      = VDC5_ON;                                /* B/R signal swap on/off control */
        output.out_format       = (vdc5_lcd_outformat_t)drv_lcd_config->lcd_outformat;    /* Output format select */
        output.out_frq_sel      = VDC5_LCD_PARALLEL_CLKFRQ_1;             /* Clock frequency control */
        output.out_dir_sel      = VDC5_LCD_SERIAL_SCAN_FORWARD;           /* Scan direction select */
        output.out_phase        = VDC5_LCD_SERIAL_CLKPHASE_0;             /* Clock phase adjustment */
        output.bg_color         = (uint32_t)0x00000000u;                  /* Background color in 24-bit RGB color format */
        /* Display output */
        error = R_VDC5_DisplayOutput( ch, &output );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }
    return drv_error;
}   /* End of function DRV_Graphics_Init() */

/**************************************************************************//**
 * @brief       Video initialization processing
 * @param[in]   drv_video_ext_in_config   : Video configuration
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Video_init( drv_video_input_sel_t drv_video_input_sel, drv_video_ext_in_config_t * drv_video_ext_in_config )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    vdc5_error_t            error;
    vdc5_input_t            input;
    vdc5_ext_in_sig_t       ext_in_sig;
    vdc5_sync_delay_t       sync_delay;

    if ((drv_video_input_sel != DRV_INPUT_SEL_EXT) && (drv_video_input_sel != DRV_INPUT_SEL_CEU)) {
        return DRV_GRAPHICS_VDC5_ERR;
    }

    _drv_video_input_sel = drv_video_input_sel;
    if( drv_video_input_sel == DRV_INPUT_SEL_EXT ) {
        input.inp_sel   = (vdc5_input_sel_t)drv_video_input_sel; /* Input select */
        input.inp_fh50  = (uint16_t)VSYNC_1_2_FH_TIMING;         /* Vsync signal 1/2fH phase timing */
        input.inp_fh25  = (uint16_t)VSYNC_1_4_FH_TIMING;         /* Vsync signal 1/4fH phase timing */

        ext_in_sig.inp_format     = (vdc5_extin_format_t)drv_video_ext_in_config->inp_format;
        ext_in_sig.inp_pxd_edge   = (vdc5_edge_t)drv_video_ext_in_config->inp_pxd_edge;
        ext_in_sig.inp_vs_edge    = (vdc5_edge_t)drv_video_ext_in_config->inp_vs_edge;
        ext_in_sig.inp_hs_edge    = (vdc5_edge_t)drv_video_ext_in_config->inp_hs_edge;
        ext_in_sig.inp_endian_on  = (vdc5_onoff_t)drv_video_ext_in_config->inp_endian_on;
        ext_in_sig.inp_swap_on    = (vdc5_onoff_t)drv_video_ext_in_config->inp_swap_on;
        ext_in_sig.inp_vs_inv     = (vdc5_sig_pol_t)drv_video_ext_in_config->inp_vs_inv;
        ext_in_sig.inp_hs_inv     = (vdc5_sig_pol_t)drv_video_ext_in_config->inp_hs_inv;
        ext_in_sig.inp_h_edge_sel = (vdc5_extin_ref_hsync_t)drv_video_ext_in_config->inp_hs_edge;
        ext_in_sig.inp_f525_625   = (vdc5_extin_input_line_t)drv_video_ext_in_config->inp_f525_625;
        ext_in_sig.inp_h_pos      = (vdc5_extin_h_pos_t)drv_video_ext_in_config->inp_h_pos;

        sync_delay.inp_vs_dly_l   = 0u;
        sync_delay.inp_vs_dly     = 16u;
        sync_delay.inp_hs_dly     = 16u;
        sync_delay.inp_fld_dly    = 16u;

        input.dly       = &sync_delay;   /* Sync signal delay adjustment */
        input.ext_sig   = &ext_in_sig;   /* External input signal        */

        /* Video input 0ch */
        error = R_VDC5_VideoInput( VDC5_CHANNEL_0, &input );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    } else if (drv_video_input_sel == DRV_INPUT_SEL_CEU) {
        R_CEU_Initialize(&R_CEU_OnInitialize, 0);
        return DRV_GRAPHICS_OK;
    } else {
        drv_error = DRV_GRAPHICS_VDC5_ERR;
    }

    return drv_error;
}   /* End of function DRV_Video_Init() */

/**************************************************************************//**
 * @brief       Start the graphics surface read process
 * @param[in]   layer_id                : Graphics layer ID
 * @retval      drv_graphics_error_t
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Start ( drv_graphics_layer_t layer_id )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    vdc5_error_t            error;
    vdc5_start_t            start;
    vdc5_gr_disp_sel_t      gr_disp_sel;
    vdc5_layer_id_t         vdc5_layer_id;

    switch( layer_id ) {
        case DRV_GRAPHICS_LAYER_0:
            vdc5_layer_id   = VDC5_LAYER_ID_0_RD;
            gr_disp_sel     = VDC5_DISPSEL_CURRENT;
            break;
        case DRV_GRAPHICS_LAYER_2:
            vdc5_layer_id   = VDC5_LAYER_ID_2_RD;
            gr_disp_sel     = VDC5_DISPSEL_BLEND;
            break;
        case DRV_GRAPHICS_LAYER_3:
            vdc5_layer_id   = VDC5_LAYER_ID_3_RD;
            gr_disp_sel     = VDC5_DISPSEL_BLEND;
            break;
        default:
            drv_error = DRV_GRAPHICS_LAYER_ERR;
            break;
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        /* Start process */
        start.gr_disp_sel = &gr_disp_sel;
        error = R_VDC5_StartProcess( ch, vdc5_layer_id, &start );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }
    return drv_error;
}   /* End of function DRV_Graphics_Start() */

/**************************************************************************//**
 * @brief       Stop the graphics surface read process
 * @param[in]   layer_id                : Graphics layer ID
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Stop ( drv_graphics_layer_t layer_id )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    vdc5_error_t            error;
    vdc5_layer_id_t         vdc5_layer_id;

    switch( layer_id ) {
        case DRV_GRAPHICS_LAYER_0:
            vdc5_layer_id   = VDC5_LAYER_ID_0_RD;
            break;
        case DRV_GRAPHICS_LAYER_2:
            vdc5_layer_id   = VDC5_LAYER_ID_2_RD;
            break;
        case DRV_GRAPHICS_LAYER_3:
            vdc5_layer_id   = VDC5_LAYER_ID_3_RD;
            break;
        default:
            drv_error = DRV_GRAPHICS_LAYER_ERR;
            break;
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        /* Stop process */
        error = R_VDC5_StopProcess ( ch, vdc5_layer_id );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }
    return drv_error;
}   /* End of function DRV_Graphics_Stop() */

/**************************************************************************//**
 * @brief      Start the video surface write process
 * @param[in]   video_input_ch          : Video input channel
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Video_Start ( drv_video_input_channel_t video_input_ch )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    if (_drv_video_input_sel == DRV_INPUT_SEL_CEU) {
        R_CEU_Start();
    } else {
        vdc5_channel_t          ch          = VDC5_CHANNEL_0;
        vdc5_error_t            error;
        vdc5_start_t            start;
        vdc5_gr_disp_sel_t      gr_disp_sel;
        vdc5_layer_id_t         vdc5_layer_id;

        if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_0 ) {
            vdc5_layer_id   = VDC5_LAYER_ID_0_WR;
        } else {
            drv_error = DRV_GRAPHICS_LAYER_ERR;
        }

        if( drv_error == DRV_GRAPHICS_OK ) {
            /* Start process */
            gr_disp_sel         = VDC5_DISPSEL_CURRENT;    /* CURRENT fixed for weave input mode */
            start.gr_disp_sel   = &gr_disp_sel;
            error = R_VDC5_StartProcess( ch, vdc5_layer_id, &start );
            if (error != VDC5_OK) {
                drv_error = DRV_GRAPHICS_VDC5_ERR;
            }
        }
    }
    return drv_error;
}   /* End of function DRV_Video_Start() */

/**************************************************************************//**
 * @brief       Stop the video surface write process
 * @param[in]   video_input_ch          : Video input channel
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Video_Stop ( drv_video_input_channel_t video_input_ch )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;

    if (_drv_video_input_sel == DRV_INPUT_SEL_CEU) {
        R_CEU_Stop();
    } else {
        vdc5_channel_t          ch          = VDC5_CHANNEL_0;
        vdc5_error_t            error;
        vdc5_layer_id_t         vdc5_layer_id;

        switch (video_input_ch) {
            case DRV_VIDEO_INPUT_CHANNEL_0:
                vdc5_layer_id   = VDC5_LAYER_ID_0_WR;
                break;
            default:
                drv_error = DRV_GRAPHICS_LAYER_ERR;
                break;
        }

        if( drv_error == DRV_GRAPHICS_OK ) {
            /* Stop process */
            error = R_VDC5_StopProcess ( ch, vdc5_layer_id );
            if (error != VDC5_OK) {
                drv_error = DRV_GRAPHICS_VDC5_ERR;
            }
        }
    }
    return drv_error;
}   /* End of function DRV_Video_Stop() */

/**************************************************************************//**
 * @brief       Graphics surface read process setting
 *
 *              Description:<br>
 *              This function supports the following 4 image format.
 *                  YCbCr422, RGB565, RGB888, ARGB8888
 * @param[in]   layer_id                : Graphics layer ID
 * @param[in]   framebuff               : Base address of the frame buffer
 * @param[in]   fb_stride               : Line offset address of the frame buffer
 * @param[in]   gr_format               : Format of the frame buffer read signal
 * @param[in]   gr_rect                 : Graphics display area
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Read_Setting (
    drv_graphics_layer_t        layer_id,
    void                      * framebuff,
    uint32_t                    fb_stride,
    drv_graphics_format_t       gr_format,
    drv_wr_rd_swa_t             wr_rd_swa,
    drv_rect_t                * gr_rect,
    drv_clut_t                * gr_clut )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    vdc5_error_t            error;
    vdc5_layer_id_t         vdc5_layer_id;
    vdc5_gr_format_t        vdc5_gr_format;
    vdc5_read_t             read;

    switch(layer_id) {
        case DRV_GRAPHICS_LAYER_0:
            vdc5_layer_id = VDC5_LAYER_ID_0_RD;
            break;
        case DRV_GRAPHICS_LAYER_2:
            vdc5_layer_id = VDC5_LAYER_ID_2_RD;
            break;
        case DRV_GRAPHICS_LAYER_3:
            vdc5_layer_id = VDC5_LAYER_ID_3_RD;
            break;
        default:
            drv_error = DRV_GRAPHICS_LAYER_ERR;
            break;
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        switch( gr_format ) {
            case DRV_GRAPHICS_FORMAT_YCBCR422:
                vdc5_gr_format = VDC5_GR_FORMAT_YCBCR422;
                break;
            case DRV_GRAPHICS_FORMAT_RGB565:
                vdc5_gr_format = VDC5_GR_FORMAT_RGB565;
                break;
            case DRV_GRAPHICS_FORMAT_RGB888:
                vdc5_gr_format = VDC5_GR_FORMAT_RGB888;
                break;
            case DRV_GRAPHICS_FORMAT_ARGB8888:
                vdc5_gr_format = VDC5_GR_FORMAT_ARGB8888;
                break;
            case DRV_GRAPHICS_FORMAT_ARGB4444:
                vdc5_gr_format = VDC5_GR_FORMAT_ARGB4444;
                break;
            case DRV_GRAPHICS_FORMAT_CLUT8:
                vdc5_gr_format = VDC5_GR_FORMAT_CLUT8;
                break;
            case DRV_GRAPHICS_FORMAT_CLUT4:
                vdc5_gr_format = VDC5_GR_FORMAT_CLUT4;
                break;
            case DRV_GRAPHICS_FORMAT_CLUT1:
                vdc5_gr_format = VDC5_GR_FORMAT_CLUT1;
                break;
            default:
                drv_error = DRV_GRAPHICS_FORMAT_ERR;
                break;
        }
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        /* Read data parameter */
        read.gr_ln_off_dir  = VDC5_GR_LN_OFF_DIR_INC;   /* Line offset address direction of the frame buffer */
        read.gr_flm_sel     = VDC5_GR_FLM_SEL_FLM_NUM;  /* Selects a frame buffer address setting signal */
        read.gr_imr_flm_inv = VDC5_OFF;                 /* Frame buffer number for distortion correction */
        read.gr_bst_md      = VDC5_BST_MD_32BYTE;       /* Frame buffer burst transfer mode */
        read.gr_base        = framebuff;                /* Frame buffer base address */
        read.gr_ln_off      = fb_stride;                /* Frame buffer line offset address */

        read.width_read_fb  = NULL;                     /* Width of the image read from frame buffer */

        read.adj_sel        = VDC5_OFF;                 /* Measures to decrease the influence
                                                           by folding pixels/lines (on/off) */
        read.gr_format      = vdc5_gr_format;           /* Format of the frame buffer read signal */
        read.gr_ycc_swap    = VDC5_GR_YCCSWAP_CBY0CRY1; /* Controls swapping of data read from buffer
                                                           in the YCbCr422 format */
        read.gr_rdswa       = (vdc5_wr_rd_swa_t)wr_rd_swa; /* Frame buffer swap setting */
        /* Display area */
        read.gr_grc.vs      = gr_rect->vs;
        read.gr_grc.vw      = gr_rect->vw;
        read.gr_grc.hs      = gr_rect->hs;
        read.gr_grc.hw      = gr_rect->hw;

        /* Read data control */
        error = R_VDC5_ReadDataControl( ch, vdc5_layer_id, &read );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        if ((vdc5_gr_format == VDC5_GR_FORMAT_CLUT8)
         || (vdc5_gr_format == VDC5_GR_FORMAT_CLUT4)
         || (vdc5_gr_format == VDC5_GR_FORMAT_CLUT1)) {
            vdc5_clut_t vdc5_clut;

            if (gr_clut != NULL) {
                vdc5_clut.color_num = gr_clut->color_num;
                vdc5_clut.clut = gr_clut->clut;
            } else if (vdc5_gr_format == VDC5_GR_FORMAT_CLUT8) {
                vdc5_clut.color_num = CLUT8_TABLE_NUM;
                vdc5_clut.clut = color_table256;
            } else if (vdc5_gr_format == VDC5_GR_FORMAT_CLUT4) {
                vdc5_clut.color_num = CLUT4_TABLE_NUM;
                vdc5_clut.clut = color_table16;
            } else {
                vdc5_clut.color_num = CLUT1_TABLE_NUM;
                vdc5_clut.clut = color_table2;
            }
            error = R_VDC5_CLUT(ch, vdc5_layer_id, &vdc5_clut);
            if (error != VDC5_OK) {
                drv_error = DRV_GRAPHICS_VDC5_ERR;
            }
        }
    }

    return drv_error;
}   /* End of function DRV_Graphics_Read_Setting() */

/**************************************************************************//**
 * @brief       Graphics surface read buffer change process
 * @param[in]   layer_id                : Graphics layer ID
 * @param[in]   framebuff               : Base address of the frame buffer
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Read_Change (
    drv_graphics_layer_t    layer_id,
    void                 *  framebuff)
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    vdc5_error_t            error;
    vdc5_layer_id_t         vdc5_layer_id;
    vdc5_read_chg_t         read_chg;

    switch(layer_id) {
        case DRV_GRAPHICS_LAYER_0:
            vdc5_layer_id = VDC5_LAYER_ID_0_RD;
            break;
        case DRV_GRAPHICS_LAYER_2:
            vdc5_layer_id = VDC5_LAYER_ID_2_RD;
            break;
        case DRV_GRAPHICS_LAYER_3:
            vdc5_layer_id = VDC5_LAYER_ID_3_RD;
            break;
        default:
            drv_error = DRV_GRAPHICS_LAYER_ERR;
            break;
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        /* Read data parameter */
        read_chg.width_read_fb  = NULL;         /* Width of the image read from frame buffer */
        read_chg.gr_grc         = NULL;         /* Display area */
        read_chg.gr_disp_sel    = NULL;         /* Graphics display mode */
        read_chg.gr_base        = framebuff;     /* Frame buffer base address */

        /* Change read process */
        error = R_VDC5_ChangeReadProcess( ch, vdc5_layer_id, &read_chg );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }
    return drv_error;
}   /* End of function DRV_Graphics_Read_Change() */

/**************************************************************************//**
 * @brief       Video surface write process setting
 *
 *              Description:<br>
 *              This function set the video write process. Input form is weave
 *              (progressive) mode fixed.
 *              This function supports the following 3 image format.
 *                  YCbCr422, RGB565, RGB888
 * @param[in]   video_input_ch          : Video input channel
 * @param[in]   col_sys                 : Analog video signal color system
 * @param[in]   adc_vinsel              : Video input pin
 * @param[in]   framebuff               : Base address of the frame buffer
 * @param[in]   fb_stride [byte]        : Line offset address of the frame buffer
 * @param[in]   video_format            : Frame buffer video-signal writing format
 * @param[in]   wr_rd_swa               : Frame buffer swap setting
 * @param[in]   video_write_size_vw [px]: output height
 * @param[in]   video_write_size_hw [px]: output width
 * @param[in]   video_adc_vinsel        : Input pin control
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Video_Write_Setting (
    drv_video_input_channel_t       video_input_ch,
    drv_graphics_video_col_sys_t    col_sys,
    void                          * framebuff,
    uint32_t                        fb_stride,
    drv_video_format_t              video_format,
    drv_wr_rd_swa_t                 wr_rd_swa,
    uint16_t                        video_write_buff_vw,
    uint16_t                        video_write_buff_hw,
    drv_video_adc_vinsel_t          video_adc_vinsel )
{
    return DRV_GRAPHICS_VDC5_ERR;
}   /* End of function DRV_Video_Write_Setting() */

/**************************************************************************//**
 * @brief       Video surface write process setting for digital input
 *
 *              Description:<br>
 *              This function set the video write process for digital input.
 *              This function supports the following 3 image format.
 *                  YCbCr422, RGB565, RGB888
 * @param[in]   framebuff               : Base address of the frame buffer
 * @param[in]   fb_stride [byte]        : Line offset address of the frame buffer
 * @param[in]   video_format            : Frame buffer video-signal writing format
 * @param[in]   wr_rd_swa               : Frame buffer swap setting
 * @param[in]   video_write_size_vw [px]: output height
 * @param[in]   video_write_size_hw [px]: output width
 * @param[in]   cap_area                : Capture area
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Video_Write_Setting_Digital (
    void                          * framebuff,
    uint32_t                        fb_stride,
    drv_video_format_t              video_format,
    drv_wr_rd_swa_t                 wr_rd_swa,
    uint16_t                        video_write_buff_vw,
    uint16_t                        video_write_buff_hw,
    drv_rect_t                    * cap_area )
{
    drv_graphics_error_t        drv_error   = DRV_GRAPHICS_OK;
    vdc5_channel_t              ch          = VDC5_CHANNEL_0;
    vdc5_error_t                error;
    vdc5_layer_id_t             vdc5_layer_id;
    vdc5_write_t                write;
    vdc5_scalingdown_rot_t    * scldw_rot;
    vdc5_res_md_t               res_md;

    vdc5_layer_id = VDC5_LAYER_ID_0_WR;

    if( drv_error == DRV_GRAPHICS_OK ) {
        if( video_format == DRV_VIDEO_FORMAT_YCBCR422 ) {
            res_md = VDC5_RES_MD_YCBCR422;
        } else if( video_format == DRV_VIDEO_FORMAT_RGB888 ) {
            res_md = VDC5_RES_MD_RGB888;
        } else if( video_format == DRV_VIDEO_FORMAT_RGB565 ) {
            res_md = VDC5_RES_MD_RGB565;
        } else {
            drv_error = DRV_GRAPHICS_FORMAT_ERR;
        }
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        /* Scaling-down and rotation parameter */
        scldw_rot = &write.scalingdown_rot;
        /* Image area to be captured */
        scldw_rot->res.vs   = (uint16_t)((uint32_t)cap_area->vs - 1u);
        scldw_rot->res.vw   = cap_area->vw;
        scldw_rot->res.hs   = cap_area->hs;
        scldw_rot->res.hw   = cap_area->hw;

        /* Write data parameter */
        scldw_rot->res_pfil_sel = VDC5_ON;                   /* Prefilter mode select for brightness signals (on/off) */
        scldw_rot->res_out_vw   = video_write_buff_vw ;      /* Number of valid lines in vertical direction
                                                                output by scaling-down control block */
        scldw_rot->res_out_hw   = video_write_buff_hw;       /* Number of valid horizontal pixels
                                                                output by scaling-down control block */
        scldw_rot->adj_sel      = VDC5_ON;                   /* Measures to decrease the influence
                                                                by lack of last-input line (on/off) */
        scldw_rot->res_ds_wr_md = VDC5_WR_MD_NORMAL;         /* Frame buffer writing mode */
        write.res_wrswa     = (vdc5_wr_rd_swa_t)wr_rd_swa;   /* Frame buffer swap setting */
        write.res_md        = res_md;                        /* Frame buffer video-signal writing format */
        write.res_bst_md    = VDC5_BST_MD_32BYTE;            /* Transfer burst length for frame buffer */
        write.res_inter     = VDC5_RES_INTER_PROGRESSIVE;    /* Field operating mode select */
        write.res_fs_rate   = VDC5_RES_FS_RATE_PER1;         /* Writing rate */
        write.res_fld_sel   = VDC5_RES_FLD_SEL_TOP;          /* Write field select */
        write.res_dth_on    = VDC5_ON;                       /* Dither correction on/off */
        write.base          = framebuff;                     /* Frame buffer base address */
        write.ln_off        = fb_stride;
        /* Frame buffer line offset address [byte] */
        write.flm_num       = (uint32_t)(1u - 1u);           /* Number of frames of buffer (res_flm_num + 1) */
        /* Frame buffer frame offset address */
        write.flm_off       = fb_stride * (uint32_t)scldw_rot->res_out_vw;
        write.btm_base      = NULL;                          /* Frame buffer base address for bottom */

        /* Write data control */
        error = R_VDC5_WriteDataControl( ch, vdc5_layer_id, &write );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }
    return drv_error;
}   /* End of function DRV_Video_Write_Setting_Digital() */

drv_graphics_error_t DRV_Video_Write_Setting_Ceu (
    void                          * framebuff,
    uint32_t                        fb_stride,
    drv_video_format_t              video_format,
    drv_wr_rd_swa_t                 wr_rd_swa,
    uint16_t                        video_write_buff_vw,
    uint16_t                        video_write_buff_hw,
    drv_video_ext_in_config_t     * drv_video_ext_in_config)
{
    drv_graphics_error_t        drv_error   = DRV_GRAPHICS_OK;
    ceu_cap_rect_t              ceu_cap;
    ceu_config_t                ceu_config;

    if (video_format != DRV_VIDEO_FORMAT_YCBCR422) {
        return DRV_GRAPHICS_FORMAT_ERR;
    }

    /* CEU mode */
    ceu_config.jpg = CEU_DATA_SYNC_MODE;

    /* Capture timing */
    if (drv_video_ext_in_config->cap_hs_pos <= 100) {
        ceu_cap.hofst = drv_video_ext_in_config->cap_hs_pos * 4;
    } else {
        ceu_cap.hofst = 0;
    }
    if (drv_video_ext_in_config->cap_vs_pos >= 10) {
        ceu_cap.vofst = drv_video_ext_in_config->cap_vs_pos * 4;
    } else {
        ceu_cap.vofst = 0;
    }
    ceu_cap.hwdth = video_write_buff_hw * 2;
    ceu_cap.vwdth = video_write_buff_vw;
    ceu_config.cap = &ceu_cap;

    /* Capture Filter Size (Set this param if Image capture mode.) */
    ceu_config.clp = NULL;

    /* Data bus */
    ceu_config.dtif = CEU_8BIT_DATA_PINS; /* Data pin 8bit  */

    /* Write Data Swap */
    switch (wr_rd_swa) {
        default:
        case DRV_WR_RD_WRSWA_NON:               /* Not swapped: 1-2-3-4-5-6-7-8 */
            ceu_config.cols = CEU_OFF;
            ceu_config.cows = CEU_OFF;
            ceu_config.cobs = CEU_OFF;
            break;
        case DRV_WR_RD_WRSWA_8BIT:              /* Swapped in 8-bit units: 2-1-4-3-6-5-8-7 */
            ceu_config.cols = CEU_OFF;
            ceu_config.cows = CEU_OFF;
            ceu_config.cobs = CEU_ON;
            break;
        case DRV_WR_RD_WRSWA_16BIT:             /* Swapped in 16-bit units: 3-4-1-2-7-8-5-6 */
            ceu_config.cols = CEU_OFF;
            ceu_config.cows = CEU_ON;
            ceu_config.cobs = CEU_OFF;
            break;
        case DRV_WR_RD_WRSWA_16_8BIT:           /* Swapped in 16-bit units + 8-bit units: 4-3-2-1-8-7-6-5 */
            ceu_config.cols = CEU_OFF;
            ceu_config.cows = CEU_ON;
            ceu_config.cobs = CEU_ON;
            break;
        case DRV_WR_RD_WRSWA_32BIT:             /* Swapped in 32-bit units: 5-6-7-8-1-2-3-4 */
            ceu_config.cols = CEU_ON;
            ceu_config.cows = CEU_OFF;
            ceu_config.cobs = CEU_OFF;
            break;
        case DRV_WR_RD_WRSWA_32_8BIT:           /* Swapped in 32-bit units + 8-bit units: 6-5-8-7-2-1-4-3 */
            ceu_config.cols = CEU_ON;
            ceu_config.cows = CEU_OFF;
            ceu_config.cobs = CEU_ON;
            break;
        case DRV_WR_RD_WRSWA_32_16BIT:          /* Swapped in 32-bit units + 16-bit units: 7-8-5-6-3-4-1-2 */
            ceu_config.cols = CEU_ON;
            ceu_config.cows = CEU_ON;
            ceu_config.cobs = CEU_OFF;
            break;
        case DRV_WR_RD_WRSWA_32_16_8BIT:        /* Swapped in 32-bit units + 16-bit units + 8-bit units: 8-7-6-5-4-3-2-1 */
            ceu_config.cols = CEU_ON;
            ceu_config.cows = CEU_ON;
            ceu_config.cobs = CEU_ON;
            break;
    }

    /* input order */
    switch (drv_video_ext_in_config->inp_h_pos) {
        default:
        case DRV_EXTIN_H_POS_CBYCRY:        /*!< Cb/Y/Cr/Y (BT656/601), Cb/Cr (YCbCr422) */
            ceu_config.dtary = CEU_CB0_Y0_CR0_Y1;
            if (ceu_config.cows == CEU_ON) {
                ceu_config.cows = CEU_OFF;
            } else {
                ceu_config.cows = CEU_ON;
            }
            break;
        case DRV_EXTIN_H_POS_YCRYCB:        /*!< Y/Cr/Y/Cb (BT656/601), setting prohibited (YCbCr422) */
            ceu_config.dtary = CEU_Y0_CR0_Y1_CB0;
            if (ceu_config.cobs == CEU_ON) {
                ceu_config.cobs = CEU_OFF;
            } else {
                ceu_config.cobs = CEU_ON;
            }
            if (ceu_config.cows == CEU_ON) {
                ceu_config.cows = CEU_OFF;
            } else {
                ceu_config.cows = CEU_ON;
            }
            break;
        case DRV_EXTIN_H_POS_CRYCBY:        /*!< Cr/Y/Cb/Y (BT656/601), setting prohibited (YCbCr422) */
            ceu_config.dtary = CEU_CR0_Y0_CB0_Y1;
            break;
        case DRV_EXTIN_H_POS_YCBYCR:        /*!< Y/Cb/Y/Cr (BT656/601), Cr/Cb (YCbCr422) */
            ceu_config.dtary = CEU_Y0_CB0_Y1_CR0;
            if (ceu_config.cobs == CEU_ON) {
                ceu_config.cobs = CEU_OFF;
            } else {
                ceu_config.cobs = CEU_ON;
            }
            break;
    }

    /* Signal polarity */
    /* Hsync */
    if (drv_video_ext_in_config->inp_hs_inv == DRV_SIG_POL_NOT_INVERTED) {
        ceu_config.hdpol = CEU_HIGH_ACTIVE;
    } else {
        ceu_config.hdpol = CEU_LOW_ACTIVE;
    }
    if (drv_video_ext_in_config->cap_hs_pos >= 100) {
        /* inverted */
        if (ceu_config.hdpol == CEU_HIGH_ACTIVE) {
            ceu_config.hdpol = CEU_LOW_ACTIVE;
        } else {
            ceu_config.hdpol = CEU_HIGH_ACTIVE;
        }
    }

    /* Vsync */
    if (drv_video_ext_in_config->inp_vs_inv == DRV_SIG_POL_NOT_INVERTED) {
        ceu_config.vdpol = CEU_HIGH_ACTIVE;
    } else {
        ceu_config.vdpol = CEU_LOW_ACTIVE;
    }

    R_CEU_Open(&ceu_config);
    R_CEU_Execute_Setting(framebuff, (void *)NULL, fb_stride, CEU_ON);

    return drv_error;
}

/**************************************************************************//**
 * @brief       Video surface write buffer change process
 * @param[in]   video_input_ch          : Video input channle
 * @param[in]   framebuff               : Base address of the frame buffer
 * @param[in]   fb_stride               : Line offset address of the frame buffer
 * @retval      Error code
******************************************************************************/
drv_graphics_error_t DRV_Video_Write_Change (
    drv_video_input_channel_t     video_input_ch,
    void                        * framebuff,
    uint32_t                      fb_stride )
{
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    uint8_t               * framebuffer_t;
    uint8_t               * framebuffer_b;

    framebuffer_t = (uint8_t *)((uint32_t)framebuff & ~0x1F);
    framebuffer_b = &framebuffer_t[fb_stride];

    if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_0 ) {
        VDC50.SC0_SCL1_WR2 = (uint32_t)framebuffer_t;
        VDC50.SC0_SCL1_WR8 = (uint32_t)framebuffer_b;
        VDC50.SC0_SCL1_UPDATE = 0x10;
    } else {
        drv_error = DRV_GRAPHICS_CHANNEL_ERR;
    }
    return drv_error;
} /* End of function DRV_Video_Write_Change() */

/* End of file */
