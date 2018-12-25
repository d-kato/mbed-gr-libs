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

/**************************************************************************//**
* @file         scux_board.c
* $Rev: $
* $Date::                           $
* @brief        SCUX functions depending on borad
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/

#include "scux.h"

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CheckSsifClockDiv
* @brief         Check SSIF clock divide rate.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @param[in]     ssif_ch : Used ssif channel number.
* @retval        ESUCCESS : Parameter is no problems.
*                EPERM_RBSP : Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_CheckSsifClockDiv(const scux_info_ch_t * const p_scux_info_ch, const uint32_t ssif_ch_num)
{
    int_t retval = ESUCCESS;
    uint32_t input_clk;
    uint32_t dot_clk;
    uint32_t n_syswd_per_smp;
    uint32_t syswd_len =0;
    uint32_t smp_freq;
    uint32_t result;
    uint32_t division;
    scux_ssif_info_t * p_ssif_ch;

    p_ssif_ch = SCUX_GetSsifChInfo((int_t)ssif_ch_num);

    if ((NULL == p_scux_info_ch) || (NULL == p_ssif_ch))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (false == p_ssif_ch->ssif_cfg.mode_master)
        {
            /* In slave mode, a temporary value is to SCUX_SSIF_CKDIV_1 (0) */
            p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_1;
        }
        else
        {
            /* check only SSIF master mode */
            /* get source clock */
            if (false == p_ssif_ch->ssif_cfg.select_audio_clk)
            {
                input_clk = SCUX_AUDIO_X1;
            }
            else
            {
                input_clk = SCUX_AUDIO_CLK;
            }

            if (0u == input_clk)
            {
                retval = EPERM_RBSP;
            }

            if (ESUCCESS == retval)
            {
                switch (p_ssif_ch->ssif_cfg.system_word)
                {
                    case SCUX_SSIF_SYSTEM_LEN_16 :
                        syswd_len = SCUX_SYSTEMWORD_16;
                    break;

                    case SCUX_SSIF_SYSTEM_LEN_24 :
                        syswd_len = SCUX_SYSTEMWORD_24;
                    break;

                    case SCUX_SSIF_SYSTEM_LEN_32 :
                        syswd_len = SCUX_SYSTEMWORD_32;
                    break;

                    case SCUX_SSIF_SYSTEM_LEN_48 :
                        syswd_len = SCUX_SYSTEMWORD_48;
                    break;

                    case SCUX_SSIF_SYSTEM_LEN_64 :
                        syswd_len = SCUX_SYSTEMWORD_64;
                    break;

                    case SCUX_SSIF_SYSTEM_LEN_128 :
                        syswd_len = SCUX_SYSTEMWORD_128;
                    break;

                    case SCUX_SSIF_SYSTEM_LEN_256 :
                        syswd_len = SCUX_SYSTEMWORD_256;
                    break;

                    default :
                        retval = EPERM_RBSP;
                    break;
                }

                smp_freq = p_scux_info_ch->src_cfg.output_ws;

                if (false == p_ssif_ch->ssif_cfg.use_tdm)
                {
                    n_syswd_per_smp = SCUX_USE_CH_2;
                }
                else
                {
                    n_syswd_per_smp = p_scux_info_ch->src_cfg.use_ch;
                }

                dot_clk = syswd_len * n_syswd_per_smp * smp_freq;
                if (0u == dot_clk)
                {
                    retval = EPERM_RBSP;
                }
                else
                {
                    /* check if input audio clock can be divided by dotclock */
                    result = input_clk % dot_clk;

                    if (0U != result)
                    {
                        /* cannot create dotclock from input audio clock */
                        retval = EPERM_RBSP;
                    }
                    else
                    {
                        division = input_clk / dot_clk;

                        switch (division)
                        {
                            case SCUX_SSIF_CLK_DIV1:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_1;
                            break;

                            case SCUX_SSIF_CLK_DIV2:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_2;
                            break;

                            case SCUX_SSIF_CLK_DIV4:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_4;
                            break;

                            case SCUX_SSIF_CLK_DIV8:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_8;
                            break;

                            case SCUX_SSIF_CLK_DIV16:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_16;
                            break;

                            case SCUX_SSIF_CLK_DIV32:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_32;
                            break;

                            case SCUX_SSIF_CLK_DIV64:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_64;
                            break;

                            case SCUX_SSIF_CLK_DIV128:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_128;
                            break;

                            case SCUX_SSIF_CLK_DIV6:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_6;
                            break;

                            case SCUX_SSIF_CLK_DIV12:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_12;
                            break;

                            case SCUX_SSIF_CLK_DIV24:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_24;
                            break;

                            case SCUX_SSIF_CLK_DIV48:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_48;
                            break;

                            case SCUX_SSIF_CLK_DIV96:
                                p_ssif_ch->clk_div = SCUX_SSIF_CKDIV_96;
                            break;

                            default:
                                retval = EPERM_RBSP;
                            break;
                        }
                    }
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CheckSsifClockDiv
******************************************************************************/

#if(1) /* mbed */
void SCUX_SetupSsifGpio(const scux_ssif_ch_num_t ssif_ch)
{
    UNUSED_ARG(ssif_ch);

    return;
}
#else  /* not mbed */

#if defined(TARGET_RZA1H) /* mbed */
/**************************************************************************//**
* Function Name: SCUX_SetupSsifGpio
* @brief         GPIO of SSIF module setup.

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
*                
* @param[in]     ssif_ch :SSIF channel number.
* @retval        None.
******************************************************************************/

void SCUX_SetupSsifGpio(const scux_ssif_ch_num_t ssif_ch)
{
    switch (ssif_ch)
    {
        case SCUX_SSIF_CH_0:
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

        case SCUX_SSIF_CH_1:
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

        case SCUX_SSIF_CH_2:
            /* SSISCK2:  no connection */
            /* SSIWS2:   no connection */
            /* SSIDATA2: no connection */
            break;

        case SCUX_SSIF_CH_3:
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

        case SCUX_SSIF_CH_4:
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

        case SCUX_SSIF_CH_5:
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
            /* ->IPA R3.5.2 Nothing is being processed intentionally. */
            /* <-IPA R3.5.2 */
            /* NOTREACHED on At the time of a normal performance */
        break;
    }

    return;
}
#else /* mbed */
/**************************************************************************//**
* Function Name: SCUX_SetupSsifGpio
* @brief         GPIO of SSIF module setup.

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
*
* @param[in]     ssif_ch :SSIF channel number.
* @retval        None.
******************************************************************************/

void SCUX_SetupSsifGpio(const scux_ssif_ch_num_t ssif_ch)
{
    switch (ssif_ch)
    {
        case SCUX_SSIF_CH_0:
            /* SSISCK0:  no connection */
            /* SSIWS0:   no connection */
            /* SSIRxD0:  no connection */
            /* SSITxD0:  no connection */
        break;

        case SCUX_SSIF_CH_1:
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

        case SCUX_SSIF_CH_2:
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

        case SCUX_SSIF_CH_3:
            /* SSISCK3:  no connection */
            /* SSIWS3:   no connection */
            /* SSIRxD3:  no connection */
            /* SSITxD3:  no connection */
        break;

        default:
            /* ->IPA R3.5.2 Nothing is being processed intentionally. */
            /* <-IPA R3.5.2 */
            /* NOTREACHED on At the time of a normal performance */
        break;
    }

    return;
}
#endif /* mbed */
#endif /* mbed */

/******************************************************************************
End of function SCUX_SetupSsifGpio
******************************************************************************/

