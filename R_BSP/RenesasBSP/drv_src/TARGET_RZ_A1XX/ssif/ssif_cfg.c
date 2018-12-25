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
* Copyright (C) 2013-2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* File Name   : ssif_cfg.c
* $Rev: $
* $Date::                           $
* Description : SSIF driver userown functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "dma_if.h"
#include "ssif_if.h"
#include "iodefine.h"
#include "cmsis.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/

/***** Audio Clock Source Configurations *****/
#if defined(TARGET_RZA1H) /* mbed */
/* AUDIO_X1 : Connect to CPU Board X8(22579200Hz) */
#define SSIF_AUDIO_X1  (22579200u)
#else
/* AUDIO_X1 : Connect to CPU Board X8(12000000Hz) */
#define SSIF_AUDIO_X1  (12000000u)
#endif

/* AUDIO_CLK: Connect to option board J7(no clock on board) */
#define SSIF_AUDIO_CLK (0u)

/* SSICR CKDV divieded value */
#define SSIF_AUDIO_CLK_DIV_1    (1u)
#define SSIF_AUDIO_CLK_DIV_2    (2u)
#define SSIF_AUDIO_CLK_DIV_4    (4u)
#define SSIF_AUDIO_CLK_DIV_8    (8u)
#define SSIF_AUDIO_CLK_DIV_16   (16u)
#define SSIF_AUDIO_CLK_DIV_32   (32u)
#define SSIF_AUDIO_CLK_DIV_64   (64u)
#define SSIF_AUDIO_CLK_DIV_128  (128u)
#define SSIF_AUDIO_CLK_DIV_6    (6u)
#define SSIF_AUDIO_CLK_DIV_12   (12u)
#define SSIF_AUDIO_CLK_DIV_24   (24u)
#define SSIF_AUDIO_CLK_DIV_48   (48u)
#define SSIF_AUDIO_CLK_DIV_96   (96u)

/* SSIF channel number */
#define SSIF_CHNUM_0    (0u)
#define SSIF_CHNUM_1    (1u)
#define SSIF_CHNUM_2    (2u)
#define SSIF_CHNUM_3    (3u)
#if defined(TARGET_RZA1H) /* mbed */
#define SSIF_CHNUM_4    (4u)
#define SSIF_CHNUM_5    (5u)
#endif /* mbed */

/* misc constant value */
#define SSIF_I2S_LR_CH  (2u)

/*******************************************************************************
Exported global variables (to be accessed by other files)
*******************************************************************************/

#if(1) /* mbed */
int_t R_SSIF_Userdef_InitPinMux(const uint32_t ssif_ch)
{
    UNUSED_ARG(ssif_ch);

    return ESUCCESS;
}
#else  /* not mbed */
#include  "mbed_critical.h"

#if defined(TARGET_RZA1H) /* mbed */
/******************************************************************************
* Function Name: R_SSIF_Userdef_InitPinMux
* @brief         This function initialise pin multiplex settings.
*
*                Description:<br>
*                R7S72100 Boards depended pin connections bellow<br>
*                Clock settings<br>
*                  AUDIO_X1 : Private use pin(nothing to do)<br>
*                  AUDIO_X2 : No connection<br>
*                  AUDIO_CLK: Working with SSIF5<br>
*                Channel settings<br>
*                  SSIF0    : Fully connected to WM8978<br>
*                  SSIF1    : Read only (NC:SSITxD1) connected to CD Deck<br>
*                  SSIF2    : No connection<br>
*                  SSIF3    : Write only (NC:SSIRxD3) connected to AK4353<br>
*                  SSIF4    : Fully connected to AK4353<br>
*                  SSIF5    : Fully connected to HCI
* @param[in]     ssif_ch      :channel number.
* @retval        ESUCCESS     :Success.
* @retval        error code   :Failure.
******************************************************************************/
int_t R_SSIF_Userdef_InitPinMux(const uint32_t ssif_ch)
{
    int_t ercd = ESUCCESS;

    core_util_critical_section_enter();

    /* -> IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on writing to 16bit register. */
    switch (ssif_ch)
    {
        case SSIF_CHNUM_0:
            /* SSISCK0(P4_4, Alternative Mode 5,InputOutput) */
            GPIO.PIBC4  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PM4    |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC4   &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PIPC4  &= (uint16_t)~(GPIO_BIT_N4);

            GPIO.PBDC4  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFC4   &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PFCE4  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PFCAE4 |= (uint16_t) (GPIO_BIT_N4);

            GPIO.PIPC4  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC4   |= (uint16_t) (GPIO_BIT_N4);

            /* SSIWS0(P4_5, Alternative Mode 5,InputOutput) */
            GPIO.PIBC4  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PM4    |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PMC4   &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PIPC4  &= (uint16_t)~(GPIO_BIT_N5);

            GPIO.PBDC4  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFC4   &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PFCE4  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PFCAE4 |= (uint16_t) (GPIO_BIT_N5);

            GPIO.PIPC4  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PMC4   |= (uint16_t) (GPIO_BIT_N5);

            /* SSIRxD0(P4_6, Alternative Mode 5,Input) */
            GPIO.PIBC4  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PM4    |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC4   &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PIPC4  &= (uint16_t)~(GPIO_BIT_N6);

            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PFC4   &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PFCE4  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PFCAE4 |= (uint16_t) (GPIO_BIT_N6);

            GPIO.PIPC4  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC4   |= (uint16_t) (GPIO_BIT_N6);

            /* SSITxD0(P4_7, Alternative Mode 5,Output) */
            GPIO.PIBC4  &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PM4    |= (uint16_t) (GPIO_BIT_N7);
            GPIO.PMC4   &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PIPC4  &= (uint16_t)~(GPIO_BIT_N7);

            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PFC4   &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PFCE4  &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PFCAE4 |= (uint16_t) (GPIO_BIT_N7);

            GPIO.PMC4   |= (uint16_t) (GPIO_BIT_N7);
            GPIO.PM4    &= (uint16_t)~(GPIO_BIT_N7);
            break;

        case SSIF_CHNUM_1:
            /* SSISCK1(P3_4, Alternative Mode 3,InputOutput) */
            GPIO.PIBC3  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PBDC3  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PM3    |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC3   &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PIPC3  &= (uint16_t)~(GPIO_BIT_N4);

            GPIO.PBDC3  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFC3   &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PFCE3  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFCAE3 &= (uint16_t)~(GPIO_BIT_N4);

            GPIO.PIPC3  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC3   |= (uint16_t) (GPIO_BIT_N4);

            /* SSIWS1(P3_5, Alternative Mode 3,InputOutput) */
            GPIO.PIBC3  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PBDC3  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PM3    |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PMC3   &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PIPC3  &= (uint16_t)~(GPIO_BIT_N5);

            GPIO.PBDC3  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFC3   &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PFCE3  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFCAE3 &= (uint16_t)~(GPIO_BIT_N5);

            GPIO.PIPC3 |=  (uint16_t) (GPIO_BIT_N5);
            GPIO.PMC3   |= (uint16_t) (GPIO_BIT_N5);

            /* SSIRxD1(P3_6, Alternative Mode 3,Input) */
            GPIO.PIBC3  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PBDC3  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PM3    |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC3   &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PIPC3  &= (uint16_t)~(GPIO_BIT_N6);

            GPIO.PBDC3  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PFC3   &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PFCE3  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PFCAE3 &= (uint16_t)~(GPIO_BIT_N6);

            GPIO.PIPC3  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC3   |= (uint16_t) (GPIO_BIT_N6);

            /* SSITxD1:  no connection */
            break;

        case SSIF_CHNUM_2:
            /* SSISCK2:  no connection */
            /* SSIWS2:   no connection */
            /* SSIDATA2: no connection */
            break;

        case SSIF_CHNUM_3:
            /* SSISCK3(P4_12, Alternative Mode 6,InputOutput) */
            GPIO.PIBC4  &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PM4    |= (uint16_t) (GPIO_BIT_N12);
            GPIO.PMC4   &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PIPC4  &= (uint16_t)~(GPIO_BIT_N12);

            GPIO.PBDC4  |= (uint16_t) (GPIO_BIT_N12);
            GPIO.PFC4   |= (uint16_t) (GPIO_BIT_N12);
            GPIO.PFCE4  &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PFCAE4 |= (uint16_t) (GPIO_BIT_N12);

            GPIO.PIPC4  |= (uint16_t) GPIO_BIT_N12;
            GPIO.PMC4   |= (uint16_t) (GPIO_BIT_N12);

            /* SSIWS3(P4_13, Alternative Mode 6,InputOutput) */
            GPIO.PIBC4  &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PM4    |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PMC4   &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PIPC4  &= (uint16_t)~(GPIO_BIT_N13);

            GPIO.PBDC4  |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PFC4   |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PFCE4  &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PFCAE4 |= (uint16_t) (GPIO_BIT_N13);

            GPIO.PIPC4  |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PMC4   |= (uint16_t) (GPIO_BIT_N13);

            /* SSIRxD3:  no connection */

            /* SSITxD3(P4_15, Alternative Mode 6,Output) */
            GPIO.PIBC4  &= (uint16_t)~(GPIO_BIT_N15);
            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N15);
            GPIO.PM4    |= (uint16_t) (GPIO_BIT_N15);
            GPIO.PMC4   &= (uint16_t)~(GPIO_BIT_N15);
            GPIO.PIPC4  &= (uint16_t)~(GPIO_BIT_N15);

            GPIO.PBDC4  &= (uint16_t)~(GPIO_BIT_N15);
            GPIO.PFC4   |= (uint16_t) (GPIO_BIT_N15);
            GPIO.PFCE4  &= (uint16_t)~(GPIO_BIT_N15);
            GPIO.PFCAE4 |= (uint16_t) (GPIO_BIT_N15);

            GPIO.PMC4   |= (uint16_t) (GPIO_BIT_N15);
            GPIO.PM4    &= (uint16_t)~(GPIO_BIT_N15);
            break;

        case SSIF_CHNUM_4:
            /* SSISCK4(P11_4, Alternative Mode 3,InputOutput) */
            GPIO.PIBC11  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PBDC11  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PM11    |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC11   &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PIPC11  &= (uint16_t)~(GPIO_BIT_N4);

            GPIO.PBDC11  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFC11   &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PFCE11  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFCAE11 &= (uint16_t)~(GPIO_BIT_N4);

            GPIO.PIPC11  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC11   |= (uint16_t) (GPIO_BIT_N4);

            /* SSIWS4(P11_5, Alternative Mode 3,InputOutput) */
            GPIO.PIBC11  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PBDC11  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PM11    |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PMC11   &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PIPC11  &= (uint16_t)~(GPIO_BIT_N5);

            GPIO.PBDC11  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFC11   &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PFCE11  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFCAE11 &= (uint16_t)~(GPIO_BIT_N5);

            GPIO.PIPC11  |= (uint16_t) GPIO_BIT_N5;
            GPIO.PMC11   |= (uint16_t) (GPIO_BIT_N5);

            /* SSIDATA4(P11_6, Alternative Mode 3,InputOutput) */
            GPIO.PIBC11  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PBDC11  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PM11    |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC11   &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PIPC11  &= (uint16_t)~(GPIO_BIT_N6);

            GPIO.PBDC11  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PFC11   &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PFCE11  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PFCAE11 &= (uint16_t)~(GPIO_BIT_N6);

            GPIO.PIPC11  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC11   |= (uint16_t) (GPIO_BIT_N6);
            break;

        case SSIF_CHNUM_5:
            /* SSISCK5(P2_4, Alternative Mode 4,InputOutput) */
            GPIO.PIBC2  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PBDC2  &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PM2    |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC2   &= (uint16_t)~(GPIO_BIT_N4);
            GPIO.PIPC2  &= (uint16_t)~(GPIO_BIT_N4);

            GPIO.PBDC2  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFC2   |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFCE2  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PFCAE2 &= (uint16_t)~(GPIO_BIT_N4);

            GPIO.PIPC2  |= (uint16_t) (GPIO_BIT_N4);
            GPIO.PMC2   |= (uint16_t) (GPIO_BIT_N4);

            /* SSIWS5(P2_5, Alternative Mode 4,InputOutput) */
            GPIO.PIBC2  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PBDC2  &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PM2    |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PMC2   &= (uint16_t)~(GPIO_BIT_N5);
            GPIO.PIPC2  &= (uint16_t)~(GPIO_BIT_N5);

            GPIO.PBDC2  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFC2   |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFCE2  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PFCAE2 &= (uint16_t)~(GPIO_BIT_N5);

            GPIO.PIPC2  |= (uint16_t) (GPIO_BIT_N5);
            GPIO.PMC2   |= (uint16_t) (GPIO_BIT_N5);

            /* SSIRxD5(P2_6, Alternative Mode 4,Input) */
            GPIO.PIBC2  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PBDC2  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PM2    |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC2   &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PIPC2  &= (uint16_t)~(GPIO_BIT_N6);

            GPIO.PBDC2  &= (uint16_t)~(GPIO_BIT_N6);
            GPIO.PFC2   |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PFCE2  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PFCAE2 &= (uint16_t)~(GPIO_BIT_N6);

            GPIO.PIPC2  |= (uint16_t) (GPIO_BIT_N6);
            GPIO.PMC2   |= (uint16_t) (GPIO_BIT_N6);

            /* SSITxD5(P2_7, Alternative Mode 4,Output) */
            GPIO.PIBC2  &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PBDC2  &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PM2    |= (uint16_t) (GPIO_BIT_N7);
            GPIO.PMC2   &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PIPC2  &= (uint16_t)~(GPIO_BIT_N7);

            GPIO.PBDC2  &= (uint16_t)~(GPIO_BIT_N7);
            GPIO.PFC2   |= (uint16_t) (GPIO_BIT_N7);
            GPIO.PFCE2  |= (uint16_t) (GPIO_BIT_N7);
            GPIO.PFCAE2 &= (uint16_t)~(GPIO_BIT_N7);

            GPIO.PMC2   |= (uint16_t) (GPIO_BIT_N7);
            GPIO.PM2    &= (uint16_t)~(GPIO_BIT_N7);

            /* AUDIO_CLK(P3_1, Alternative Mode 6,Input) */
            GPIO.PIBC3  &= (uint16_t)~(GPIO_BIT_N1);
            GPIO.PBDC3  &= (uint16_t)~(GPIO_BIT_N1);
            GPIO.PM3    |= (uint16_t) (GPIO_BIT_N1);
            GPIO.PMC3   &= (uint16_t)~(GPIO_BIT_N1);
            GPIO.PIPC3  &= (uint16_t)~(GPIO_BIT_N1);

            GPIO.PBDC3  &= (uint16_t)~(GPIO_BIT_N1);
            GPIO.PFC3   |= (uint16_t) (GPIO_BIT_N1);
            GPIO.PFCE3  &= (uint16_t)~(GPIO_BIT_N1);
            GPIO.PFCAE3 |= (uint16_t) (GPIO_BIT_N1);

            GPIO.PIPC3  |= (uint16_t) (GPIO_BIT_N1);
            GPIO.PMC3   |= (uint16_t) (GPIO_BIT_N1);
            break;

        default:
            ercd = EINVAL;
            break;
    }
    /* <- IPA R2.4.2 */

    core_util_critical_section_exit();

    return ercd;
}
#else /* mbed */
/******************************************************************************
* Function Name: R_SSIF_Userdef_InitPinMux
* @brief         This function initialise pin multiplex settings.
*
*                Description:<br>
*                R7S72103 Boards depended pin connections bellow<br>
*                Clock settings<br>
*                  AUDIO_X1 : Private use pin(nothing to do)<br>
*                  AUDIO_X2 : No connection<br>
*                  AUDIO_CLK: This port is not set.
*                             (If you don't use EtherAVB, don't need to set.)
*                Channel settings<br>
*                  SSIF0    : No connection<br>
*                  SSIF1    : Fully connected to AK4353<br>
*                  SSIF2    : Fully connected to WM8978<br>
*                  SSIF3    : No connection<br>
* @param[in]     ssif_ch      :channel number.
* @retval        ESUCCESS     :Success.
* @retval        error code   :Failure.
******************************************************************************/
int_t R_SSIF_Userdef_InitPinMux(const uint32_t ssif_ch)
{
    int_t ercd = ESUCCESS;

    core_util_critical_section_enter();

    /* -> IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on writing to 16bit register. */
    switch (ssif_ch)
    {
        case SSIF_CHNUM_0:
            /* SSISCK0:  no connection */
            /* SSIWS0:   no connection */
            /* SSIRxD0:  no connection */
            /* SSITxD0:  no connection */

            break;

        case SSIF_CHNUM_1:
            /* SSISCK1:  P7_8, Alt2(001), Input/Output */
            GPIO.PIBC7  &= (uint16_t)~(GPIO_BIT_N8);
            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N8);
            GPIO.PM7    |= (uint16_t) (GPIO_BIT_N8);
            GPIO.PMC7   &= (uint16_t)~(GPIO_BIT_N8);
            GPIO.PIPC7  &= (uint16_t)~(GPIO_BIT_N8);

            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N8);
            GPIO.PFC7   |= (uint16_t) (GPIO_BIT_N8);
            GPIO.PFCE7  &= (uint16_t)~(GPIO_BIT_N8);
            GPIO.PFCAE7 &= (uint16_t)~(GPIO_BIT_N8);

            GPIO.PIPC7  |= (uint16_t) (GPIO_BIT_N8);
            GPIO.PMC7   |= (uint16_t) (GPIO_BIT_N8);

            /* SSIWS1:   P7_9, Alt2(001), Input/Output */
            GPIO.PIBC7  &= (uint16_t)~(GPIO_BIT_N9);
            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N9);
            GPIO.PM7    |= (uint16_t) (GPIO_BIT_N9);
            GPIO.PMC7   &= (uint16_t)~(GPIO_BIT_N9);
            GPIO.PIPC7  &= (uint16_t)~(GPIO_BIT_N9);

            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N9);
            GPIO.PFC7   |= (uint16_t) (GPIO_BIT_N9);
            GPIO.PFCE7  &= (uint16_t)~(GPIO_BIT_N9);
            GPIO.PFCAE7 &= (uint16_t)~(GPIO_BIT_N9);

            GPIO.PIPC7  |= (uint16_t) (GPIO_BIT_N9);
            GPIO.PMC7   |= (uint16_t) (GPIO_BIT_N9);

            /* SSIRxD1:  P7_11, Alt2(001), Input */
            GPIO.PIBC7  &= (uint16_t)~(GPIO_BIT_N11);
            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N11);
            GPIO.PM7    |= (uint16_t) (GPIO_BIT_N11);
            GPIO.PMC7   &= (uint16_t)~(GPIO_BIT_N11);
            GPIO.PIPC7  &= (uint16_t)~(GPIO_BIT_N11);

            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N11);
            GPIO.PFC7   |= (uint16_t) (GPIO_BIT_N11);
            GPIO.PFCE7  &= (uint16_t)~(GPIO_BIT_N11);
            GPIO.PFCAE7 &= (uint16_t)~(GPIO_BIT_N11);

            GPIO.PIPC7  |= (uint16_t) (GPIO_BIT_N11);
            GPIO.PMC7   |= (uint16_t) (GPIO_BIT_N11);

            /* SSITxD1:  P7_10, Alt2(001), Output */
            GPIO.PIBC7  &= (uint16_t)~(GPIO_BIT_N10);
            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N10);
            GPIO.PM7    |= (uint16_t) (GPIO_BIT_N10);
            GPIO.PMC7   &= (uint16_t)~(GPIO_BIT_N10);
            GPIO.PIPC7  &= (uint16_t)~(GPIO_BIT_N10);

            GPIO.PBDC7  &= (uint16_t)~(GPIO_BIT_N10);
            GPIO.PFC7   |= (uint16_t) (GPIO_BIT_N10);
            GPIO.PFCE7  &= (uint16_t)~(GPIO_BIT_N10);
            GPIO.PFCAE7 &= (uint16_t)~(GPIO_BIT_N10);

            GPIO.PMC7   |= (uint16_t) (GPIO_BIT_N10);
            GPIO.PM7    &= (uint16_t)~(GPIO_BIT_N10);
            break;

        case SSIF_CHNUM_2:
            /* SSISCK2:  P6_12, Alt4(011), Input/Output */
            GPIO.PIBC6  &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PBDC6  &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PM6    |= (uint16_t) (GPIO_BIT_N12);
            GPIO.PMC6   &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PIPC6  &= (uint16_t)~(GPIO_BIT_N12);

            GPIO.PBDC6  &= (uint16_t)~(GPIO_BIT_N12);
            GPIO.PFC6   |= (uint16_t) (GPIO_BIT_N12);
            GPIO.PFCE6  |= (uint16_t) (GPIO_BIT_N12);
            GPIO.PFCAE6 &= (uint16_t)~(GPIO_BIT_N12);

            GPIO.PIPC6  |= (uint16_t) (GPIO_BIT_N12);
            GPIO.PMC6   |= (uint16_t) (GPIO_BIT_N12);

            /* SSIWS2:   P6_13, Alt4(011), Input/Output */
            GPIO.PIBC6  &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PBDC6  &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PM6    |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PMC6   &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PIPC6  &= (uint16_t)~(GPIO_BIT_N13);

            GPIO.PBDC6  &= (uint16_t)~(GPIO_BIT_N13);
            GPIO.PFC6   |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PFCE6  |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PFCAE6 &= (uint16_t)~(GPIO_BIT_N13);

            GPIO.PIPC6  |= (uint16_t) (GPIO_BIT_N13);
            GPIO.PMC6   |= (uint16_t) (GPIO_BIT_N13);

            /* SSIDATA2: P6_14, Alt4(011), Input/Output */
            GPIO.PIBC6  &= (uint16_t)~(GPIO_BIT_N14);
            GPIO.PBDC6  &= (uint16_t)~(GPIO_BIT_N14);
            GPIO.PM6    |= (uint16_t) (GPIO_BIT_N14);
            GPIO.PMC6   &= (uint16_t)~(GPIO_BIT_N14);
            GPIO.PIPC6  &= (uint16_t)~(GPIO_BIT_N14);

            GPIO.PBDC6  &= (uint16_t)~(GPIO_BIT_N14);
            GPIO.PFC6   |= (uint16_t) (GPIO_BIT_N14);
            GPIO.PFCE6  |= (uint16_t) (GPIO_BIT_N14);
            GPIO.PFCAE6 &= (uint16_t)~(GPIO_BIT_N14);

            GPIO.PIPC6  |= (uint16_t) (GPIO_BIT_N14);
            GPIO.PMC6   |= (uint16_t) (GPIO_BIT_N14);
            break;

        case SSIF_CHNUM_3:
            /* SSISCK3:  no connection */
            /* SSIWS3:   no connection */
            /* SSIRxD3:  no connection */
            /* SSITxD3:  no connection */
            break;

        default:
            ercd = EINVAL;
            break;
    }
    /* <- IPA R2.4.2 */

    core_util_critical_section_exit();

    return ercd;
}
#endif /* end mbed */
#endif /* end mbed */

/******************************************************************************
* Function Name: R_SSIF_Userdef_SetClockDiv
* @brief         This function make a value of divieded audio clock.
*
*                Description:<br>
*
* @param[in]     p_ch_cfg     :pointer of channel configuration parameter.
* @param[in,out] p_clk_div    :pointer of SSICR register CKDV value
* @retval        ESUCCESS     :Success.
* @retval        error code   :Failure.
******************************************************************************/
int_t R_SSIF_Userdef_SetClockDiv(const ssif_channel_cfg_t* const p_ch_cfg, ssif_chcfg_ckdv_t* const p_clk_div)
{
    uint32_t input_clk;
    uint32_t dot_clk;
    uint32_t n_syswd_per_smp;
    uint32_t syswd_len;
    uint32_t smp_freq;
    uint32_t result;
    uint32_t division;
    int_t ret = ESUCCESS;

    if ((NULL == p_ch_cfg) || (NULL == p_clk_div))
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        if (SSIF_CFG_CKS_AUDIO_X1 == p_ch_cfg->clk_select)
        {
            input_clk = SSIF_AUDIO_X1;
        }
        else if (SSIF_CFG_CKS_AUDIO_CLK == p_ch_cfg->clk_select)
        {
            input_clk = SSIF_AUDIO_CLK;
        }
        else
        {
            input_clk = 0u;
        }

        if (0u == input_clk)
        {
            ret = EINVAL_RBSP;
        }

        if (ESUCCESS == ret)
        {
            syswd_len = (uint32_t)R_SSIF_SWLtoLen(p_ch_cfg->system_word);
            smp_freq = p_ch_cfg->sample_freq;

            if (SSIF_CFG_DISABLE_TDM == p_ch_cfg->tdm_mode)
            {
                /* I2S format has 2 system_words */
                n_syswd_per_smp = SSIF_I2S_LR_CH;
            }
            else
            {
                /* TDM frame has [(CHNL+1) * 2] system_words */
                n_syswd_per_smp = (((uint32_t)p_ch_cfg->multi_ch) + 1) * SSIF_I2S_LR_CH;
            }

            dot_clk = syswd_len * n_syswd_per_smp * smp_freq;

            if (0u == dot_clk)
            {
                ret = EINVAL_RBSP;
            }
            else
            {
                /* check if input audio clock can be divided by dotclock */
                result = input_clk % dot_clk;

                if (0u != result)
                {
                    /* cannot create dotclock from input audio clock */
                    ret = EINVAL_RBSP;
                }
                else
                {
                    division = input_clk / dot_clk;

                    switch (division)
                    {
                    case SSIF_AUDIO_CLK_DIV_1:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_1;
                        break;
                    case SSIF_AUDIO_CLK_DIV_2:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_2;
                        break;
                    case SSIF_AUDIO_CLK_DIV_4:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_4;
                        break;
                    case SSIF_AUDIO_CLK_DIV_8:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_8;
                        break;
                    case SSIF_AUDIO_CLK_DIV_16:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_16;
                        break;
                    case SSIF_AUDIO_CLK_DIV_32:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_32;
                        break;
                    case SSIF_AUDIO_CLK_DIV_64:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_64;
                        break;
                    case SSIF_AUDIO_CLK_DIV_128:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_128;
                        break;
                    case SSIF_AUDIO_CLK_DIV_6:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_6;
                        break;
                    case SSIF_AUDIO_CLK_DIV_12:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_12;
                        break;
                    case SSIF_AUDIO_CLK_DIV_24:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_24;
                        break;
                    case SSIF_AUDIO_CLK_DIV_48:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_48;
                        break;
                    case SSIF_AUDIO_CLK_DIV_96:
                        *p_clk_div = SSIF_CFG_CKDV_BITS_96;
                        break;
                    default:
                        ret = EINVAL_RBSP;
                        break;
                    }
                }
            }
        }
    }

    return ret;
}

