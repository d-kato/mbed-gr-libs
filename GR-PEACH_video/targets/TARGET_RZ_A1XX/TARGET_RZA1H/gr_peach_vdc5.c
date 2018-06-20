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
#include    "video_decoder.h"
#include    "lvds_pll_calc.h"
#include    "gr_board_vdc5.h"

#include    "mbed_assert.h"
#include    "pinmap.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define STP91_BIT   (0x02u)
#define STP90_BIT   (0x01u)
#define STBRQ25_BIT (0x20u)
#define STBAK25_BIT (0x20u)
#define STBRQ24_BIT (0x10u)
#define STBAK24_BIT (0x10u)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum {
    VDC5_CH0,
    VDC5_CH1,
} VDC5Name;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/
static const PinMap PinMap_DV_INPUT_PIN[] = {
    {P8_11 , VDC5_CH0, 8}, /* DV0_CLK    */
    {P10_0 , VDC5_CH0, 1}, /* DV0_CLK    */
    {P1_12 , VDC5_CH0, 2}, /* DV0_VSYNC  */
    {P1_13 , VDC5_CH0, 2}, /* DV0_HSYNC  */
    {P1_1  , VDC5_CH0, 6}, /* DV0_HSYNC  */
    {P1_0  , VDC5_CH0, 6}, /* DV0_VSYNC  */
    {P1_9  , VDC5_CH0, 6}, /* DV0_DATA15 */
    {P2_15 , VDC5_CH0, 3}, /* DV0_DATA15 */
    {P4_7  , VDC5_CH0, 7}, /* DV0_DATA15 */
    {P5_7  , VDC5_CH0, 4}, /* DV0_DATA15 */
    {P1_8  , VDC5_CH0, 6}, /* DV0_DATA14 */
    {P2_14 , VDC5_CH0, 3}, /* DV0_DATA14 */
    {P4_6  , VDC5_CH0, 7}, /* DV0_DATA14 */
    {P5_6  , VDC5_CH0, 4}, /* DV0_DATA14 */
    {P1_7  , VDC5_CH0, 6}, /* DV0_DATA13 */
    {P2_13 , VDC5_CH0, 3}, /* DV0_DATA13 */
    {P4_5  , VDC5_CH0, 7}, /* DV0_DATA13 */
    {P5_5  , VDC5_CH0, 4}, /* DV0_DATA13 */
    {P1_6  , VDC5_CH0, 6}, /* DV0_DATA12 */
    {P4_4  , VDC5_CH0, 7}, /* DV0_DATA12 */
    {P5_4  , VDC5_CH0, 4}, /* DV0_DATA12 */
    {P10_5 , VDC5_CH0, 1}, /* DV0_DATA11 */
    {P2_10 , VDC5_CH0, 3}, /* DV0_DATA10 */
    {P10_14, VDC5_CH0, 1}, /* DV0_DATA10 */
    {P2_9  , VDC5_CH0, 3}, /* DV0_DATA9  */
    {P10_13, VDC5_CH0, 1}, /* DV0_DATA9  */
    {P10_12, VDC5_CH0, 1}, /* DV0_DATA8  */
    {P2_7  , VDC5_CH0, 3}, /* DV0_DATA7  */
    {P2_6  , VDC5_CH0, 3}, /* DV0_DATA6  */
    {P2_5  , VDC5_CH0, 3}, /* DV0_DATA5  */
    {P2_4  , VDC5_CH0, 3}, /* DV0_DATA4  */
    {P2_3  , VDC5_CH0, 3}, /* DV0_DATA3  */
    {P2_2  , VDC5_CH0, 3}, /* DV0_DATA2  */
    {P2_1  , VDC5_CH0, 3}, /* DV0_DATA1  */
    {P2_0  , VDC5_CH0, 3}, /* DV0_DATA0  */
    {NC    , NC   , 0}
};

static const PinMap PinMap_LCD_DISP_PIN[] = {
    {P2_7  , VDC5_CH0, 8}, /* LCD0_DATA23  */
    {P5_7  , VDC5_CH0, 3}, /* LCD0_DATA23  */
    {P10_0 , VDC5_CH0, 5}, /* LCD0_DATA23  */
    {P2_6  , VDC5_CH0, 8}, /* LCD0_DATA22  */
    {P5_6  , VDC5_CH0, 3}, /* LCD0_DATA22  */
    {P10_1 , VDC5_CH0, 5}, /* LCD0_DATA22  */
    {P2_5  , VDC5_CH0, 8}, /* LCD0_DATA21  */
    {P5_5  , VDC5_CH0, 3}, /* LCD0_DATA21  */
    {P10_2 , VDC5_CH0, 5}, /* LCD0_DATA21  */
    {P2_4  , VDC5_CH0, 8}, /* LCD0_DATA20  */
    {P5_4  , VDC5_CH0, 3}, /* LCD0_DATA20  */
    {P10_3 , VDC5_CH0, 5}, /* LCD0_DATA20  */
    {P2_3  , VDC5_CH0, 8}, /* LCD0_DATA19  */
    {P5_3  , VDC5_CH0, 3}, /* LCD0_DATA19  */
    {P10_4 , VDC5_CH0, 5}, /* LCD0_DATA19  */
    {P2_2  , VDC5_CH0, 8}, /* LCD0_DATA18  */
    {P5_2  , VDC5_CH0, 3}, /* LCD0_DATA18  */
    {P10_5 , VDC5_CH0, 5}, /* LCD0_DATA18  */
    {P2_1  , VDC5_CH0, 8}, /* LCD0_DATA17  */
    {P5_1  , VDC5_CH0, 3}, /* LCD0_DATA17  */
    {P10_7 , VDC5_CH0, 5}, /* LCD0_DATA17  */
    {P2_0  , VDC5_CH0, 8}, /* LCD0_DATA16  */
    {P5_0  , VDC5_CH0, 3}, /* LCD0_DATA16  */
    {P10_6 , VDC5_CH0, 5}, /* LCD0_DATA16  */
    {P4_7  , VDC5_CH0, 1}, /* LCD0_DATA15  */
    {P10_8 , VDC5_CH0, 5}, /* LCD0_DATA15  */
    {P4_6  , VDC5_CH0, 1}, /* LCD0_DATA14  */
    {P10_9 , VDC5_CH0, 5}, /* LCD0_DATA14  */
    {P4_5  , VDC5_CH0, 1}, /* LCD0_DATA13  */
    {P10_10, VDC5_CH0, 5}, /* LCD0_DATA13  */
    {P4_4  , VDC5_CH0, 1}, /* LCD0_DATA12  */
    {P10_11, VDC5_CH0, 5}, /* LCD0_DATA12  */
    {P10_12, VDC5_CH0, 5}, /* LCD0_DATA11  */
    {P10_13, VDC5_CH0, 5}, /* LCD0_DATA10  */
    {P10_14, VDC5_CH0, 5}, /* LCD0_DATA9   */
    {P4_0  , VDC5_CH0, 1}, /* LCD0_DATA8   */
    {P10_15, VDC5_CH0, 5}, /* LCD0_DATA8   */
    {P3_15 , VDC5_CH0, 1}, /* LCD0_DATA7   */
    {P11_0 , VDC5_CH0, 5}, /* LCD0_DATA7   */
    {P3_14 , VDC5_CH0, 1}, /* LCD0_DATA6   */
    {P11_1 , VDC5_CH0, 5}, /* LCD0_DATA6   */
    {P3_13 , VDC5_CH0, 1}, /* LCD0_DATA5   */
    {P11_2 , VDC5_CH0, 5}, /* LCD0_DATA5   */
    {P3_12 , VDC5_CH0, 1}, /* LCD0_DATA4   */
    {P11_3 , VDC5_CH0, 5}, /* LCD0_DATA4   */
    {P3_11 , VDC5_CH0, 1}, /* LCD0_DATA3   */
    {P11_4 , VDC5_CH0, 5}, /* LCD0_DATA3   */
    {P3_10 , VDC5_CH0, 1}, /* LCD0_DATA2   */
    {P11_5 , VDC5_CH0, 5}, /* LCD0_DATA2   */
    {P3_9  , VDC5_CH0, 1}, /* LCD0_DATA1   */
    {P11_6 , VDC5_CH0, 5}, /* LCD0_DATA1   */
    {P3_8  , VDC5_CH0, 1}, /* LCD0_DATA0   */
    {P11_7 , VDC5_CH0, 5}, /* LCD0_DATA0   */
    {P11_10, VDC5_CH0, 5}, /* LCD0_TCON4   */
    {P11_11, VDC5_CH0, 5}, /* LCD0_TCON3   */
    {P11_12, VDC5_CH0, 5}, /* LCD0_TCON2   */
    {P3_2  , VDC5_CH0, 1}, /* LCD0_TCON1   */
    {P11_13, VDC5_CH0, 5}, /* LCD0_TCON1   */
    {P11_14, VDC5_CH0, 5}, /* LCD0_TCON0   */
    {P11_15, VDC5_CH0, 5}, /* LCD0_CLK     */
    {NC    , NC   , 0}
};

static const PinMap PinMap_LVDS_DISP_PIN[] = {
    {P5_7  , VDC5_CH0, 1}, /* TXOUT0M   */
    {P5_6  , VDC5_CH0, 1}, /* TXOUT0P   */
    {P5_5  , VDC5_CH0, 1}, /* TXOUT1M   */
    {P5_4  , VDC5_CH0, 1}, /* TXOUT1P   */
    {P5_3  , VDC5_CH0, 1}, /* TXOUT2M   */
    {P5_2  , VDC5_CH0, 1}, /* TXOUT2P   */
    {P5_1  , VDC5_CH0, 1}, /* TXCLKOUTM */
    {P5_0  , VDC5_CH0, 1}, /* TXCLKOUTP */
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
    S1_VI_VSYNC0_IRQn,
    S1_LO_VSYNC0_IRQn,
    S1_VSYNCERR0_IRQn,
    S1_VFIELD0_IRQn,
    IV2_VBUFERR0_IRQn,
    IV4_VBUFERR0_IRQn,
    S1_WLINE0_IRQn,
    OIR_VI_VSYNC0_IRQn,
    OIR_LO_VSYNC0_IRQn,
    OIR_VSYNCERR0_IRQn,
    OIR_VFIELD0_IRQn,
    IV7_VBUFERR0_IRQn,
    IV8_VBUFERR0_IRQn
};

/******************************************************************************
Private global variables and functions
******************************************************************************/
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
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
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
    drv_graphics_error_t    drv_error   = DRV_GRAPHICS_OK;
    uint32_t count;

    for( count = 0 ; count < pin_count ; count++ ) {
        pinmap_peripheral(pin[count], PinMap_LVDS_DISP_PIN);
        pinmap_pinout(pin[count], PinMap_LVDS_DISP_PIN);
    }
    return drv_error;
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
    vdc5_lvds_t             vdc5_lvds;
    pll_parameter_t         pll_parameter;
    double                  InputClock   = DEFAULT_INPUT_CLOCK;
    double                  OutputClock  = DEFAULT_OUTPUT_CLOCK;
    uint32_t                LvdsUsed     = LVDS_IF_USE;

    /* Initialization parameter */
    init.panel_icksel   = VDC5_PANEL_ICKSEL_LVDS;      /* Panel clock select */
    init.panel_dcdr     = VDC5_PANEL_CLKDIV_1_1;       /* Panel clock frequency division ratio */

    if( drv_lcd_config != NULL ) {
        InputClock  = drv_lcd_config->intputClock;
        OutputClock = drv_lcd_config->outputClock;

        /* LVDS PLL Setting Calculation */
        if( drv_lcd_config->lcd_type == DRV_LCD_TYPE_LVDS ) {
            LvdsUsed = LVDS_IF_USE;
            init.panel_icksel   = VDC5_PANEL_ICKSEL_LVDS_DIV7;      /* Panel clock select */
        } else {
            LvdsUsed = LVDS_IF_NOT_USE;
        }
    }
    lvds_pll_calc( InputClock, OutputClock, LvdsUsed, &pll_parameter );

    vdc5_lvds.lvds_in_clk_sel  = VDC5_LVDS_INCLK_SEL_PERI; /* P1 */
    vdc5_lvds.lvds_idiv_set    = (vdc5_lvds_ndiv_t)pll_parameter.nidiv;
    vdc5_lvds.lvdspll_tst      = 16u;
    vdc5_lvds.lvds_odiv_set    = (vdc5_lvds_ndiv_t)pll_parameter.nodiv;
    vdc5_lvds.lvdspll_tst      = 16u;
    vdc5_lvds.lvds_vdc_sel     = ch;
    vdc5_lvds.lvdspll_fd       = pll_parameter.nfd;
    vdc5_lvds.lvdspll_rd       = pll_parameter.nrd;
    vdc5_lvds.lvdspll_od       = (vdc5_lvds_pll_nod_t)pll_parameter.nod;
    init.lvds                  = &vdc5_lvds;               /* LVDS parameter */

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
        output.out_swap_on      = VDC5_OFF;                               /* B/R signal swap on/off control */
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

    input.inp_sel   = (vdc5_input_sel_t)drv_video_input_sel; /* Input select */
    input.inp_fh50  = (uint16_t)VSYNC_1_2_FH_TIMING;         /* Vsync signal 1/2fH phase timing */
    input.inp_fh25  = (uint16_t)VSYNC_1_4_FH_TIMING;         /* Vsync signal 1/4fH phase timing */

    if( drv_video_input_sel == DRV_INPUT_SEL_VDEC ) {
        input.dly       = NULL;                      /* Sync signal delay adjustment */
        input.ext_sig   = NULL;                      /* External input signal        */
    } else {
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
    }
    /* Video input 0ch */
    error = R_VDC5_VideoInput( VDC5_CHANNEL_0, &input );
    if (error != VDC5_OK) {
        drv_error = DRV_GRAPHICS_VDC5_ERR;
    }

    if( drv_video_input_sel == DRV_INPUT_SEL_VDEC ) {
        if ( drv_error == DRV_GRAPHICS_OK ) {
            /* Video input 1ch */
            error = R_VDC5_VideoInput( VDC5_CHANNEL_1, &input );
            if (error != VDC5_OK) {
                drv_error = DRV_GRAPHICS_VDC5_ERR;
            }
        }
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
        case DRV_GRAPHICS_LAYER_1:
            vdc5_layer_id   = VDC5_LAYER_ID_1_RD;
            gr_disp_sel     = VDC5_DISPSEL_BLEND;
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
        case DRV_GRAPHICS_LAYER_1:
            vdc5_layer_id   = VDC5_LAYER_ID_1_RD;
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
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    vdc5_error_t            error;
    vdc5_start_t            start;
    vdc5_gr_disp_sel_t      gr_disp_sel;
    vdc5_layer_id_t         vdc5_layer_id;

    if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_0 ) {
        vdc5_layer_id   = VDC5_LAYER_ID_0_WR;
    } else if ( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_1 ) {
        vdc5_layer_id   = VDC5_LAYER_ID_1_WR;
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
    vdc5_channel_t          ch          = VDC5_CHANNEL_0;
    vdc5_error_t            error;
    vdc5_layer_id_t         vdc5_layer_id;

    switch (video_input_ch) {
        case DRV_VIDEO_INPUT_CHANNEL_0:
            vdc5_layer_id   = VDC5_LAYER_ID_0_WR;
            break;
        case DRV_VIDEO_INPUT_CHANNEL_1:
            vdc5_layer_id   = VDC5_LAYER_ID_1_WR;
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
    drv_rect_t                * gr_rect )
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
        case DRV_GRAPHICS_LAYER_1:
            vdc5_layer_id = VDC5_LAYER_ID_1_RD;
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
        case DRV_GRAPHICS_LAYER_1:
            vdc5_layer_id = VDC5_LAYER_ID_1_RD;
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
    drv_graphics_error_t        drv_error   = DRV_GRAPHICS_OK;
    vdc5_channel_t              ch          = VDC5_CHANNEL_0;
    vdc5_error_t                error;
    vdc5_layer_id_t             vdc5_layer_id;
    vdc5_write_t                write;
    vdc5_scalingdown_rot_t    * scldw_rot;
    vdc5_res_md_t               res_md;
    drv_rect_t                  video_in_rect;
    uint8_t                   * framebuffer_t;
    uint8_t                   * framebuffer_b;

    if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_0 ) {
        GRAPHICS_VideoDecoderInit( (vdec_adc_vinsel_t)video_adc_vinsel, VDEC_CHANNEL_0, (graphics_col_sys_t)col_sys );
    } else if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_1 ) {
        GRAPHICS_VideoDecoderInit( (vdec_adc_vinsel_t)video_adc_vinsel, VDEC_CHANNEL_1, (graphics_col_sys_t)col_sys );
    } else {
        drv_error = DRV_GRAPHICS_CHANNEL_ERR;
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_0 ) {
            vdc5_layer_id = VDC5_LAYER_ID_0_WR;
        } else if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_1 ) {
            vdc5_layer_id = VDC5_LAYER_ID_1_WR;
        } else {
            drv_error = DRV_GRAPHICS_CHANNEL_ERR;
        }
    }

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

    if( col_sys == DRV_COL_SYS_NTSC_358 || col_sys == DVV_COL_SYS_NTSC_443 || col_sys == DRV_COL_SYS_NTSC_443_60 ) {
        video_in_rect.hs = IMGCAP_SIZE_NTSC_HS * 2;
        video_in_rect.hw = IMGCAP_SIZE_NTSC_HW * 2;
        video_in_rect.vs = IMGCAP_SIZE_NTSC_VS;
        video_in_rect.vw = IMGCAP_SIZE_NTSC_VW;
    } else {
        video_in_rect.hs = IMGCAP_SIZE_PAL_HS * 2;
        video_in_rect.hw = IMGCAP_SIZE_PAL_HW * 2;
        video_in_rect.vs = IMGCAP_SIZE_PAL_VS;
        video_in_rect.vw = IMGCAP_SIZE_PAL_VW;
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        if( col_sys == DRV_COL_SYS_NTSC_358 || col_sys == DVV_COL_SYS_NTSC_443 || col_sys == DRV_COL_SYS_NTSC_443_60 ) {
            if( (video_write_buff_vw / 2u) > video_in_rect.vw ) {
                drv_error = DRV_GRAPHICS_VIDEO_NTSC_SIZE_ERR;
            }
        } else {
            if( (video_write_buff_vw / 2u) > video_in_rect.vw ) {
                drv_error = DRV_GRAPHICS_VIDEO_PAL_SIZE_ERR;
            }
        }
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        if( video_write_buff_hw > 800 ) {
            drv_error = DRV_GRAPHICS_PARAM_RANGE_ERR;
        }
    }

    if( drv_error == DRV_GRAPHICS_OK ) {
        /* Scaling-down and rotation parameter */
        scldw_rot = &write.scalingdown_rot;
        /* Image area to be captured */
        scldw_rot->res.vs   = (uint16_t)((uint32_t)video_in_rect.vs - 1u);
        scldw_rot->res.vw   = video_in_rect.vw;
        scldw_rot->res.hs   = video_in_rect.hs;
        scldw_rot->res.hw   = video_in_rect.hw;

        /* Write data parameter */
        framebuffer_t      = framebuff;
        framebuffer_b      = &framebuffer_t[fb_stride];
        scldw_rot->res_pfil_sel = VDC5_ON;                   /* Prefilter mode select for brightness signals (on/off) */
        scldw_rot->res_out_vw   = video_write_buff_vw / 2u;  /* Number of valid lines in vertical direction
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
        write.ln_off        = fb_stride * 2u;                /* Frame buffer line offset address [byte] */
        write.flm_num       = (uint32_t)(1u - 1u);            /* Number of frames of buffer (res_flm_num + 1) */
        /* Frame buffer frame offset address */
        write.flm_off       = fb_stride * 2u * (uint32_t)scldw_rot->res_out_vw;
        write.btm_base      = framebuffer_b;                   /* Frame buffer base address for bottom */

        /* Write data control */
        error = R_VDC5_WriteDataControl( ch, vdc5_layer_id, &write );
        if (error != VDC5_OK) {
            drv_error = DRV_GRAPHICS_VDC5_ERR;
        }
    }
    return drv_error;
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
    } else if( video_input_ch == DRV_VIDEO_INPUT_CHANNEL_1 ) {
        VDC50.SC1_SCL1_WR2 = (uint32_t)framebuffer_t;
        VDC50.SC1_SCL1_WR8 = (uint32_t)framebuffer_b;
        VDC50.SC1_SCL1_UPDATE = 0x10;
    } else {
        drv_error = DRV_GRAPHICS_CHANNEL_ERR;
    }
    return drv_error;
} /* End of function DRV_Video_Write_Change() */

/* End of file */
