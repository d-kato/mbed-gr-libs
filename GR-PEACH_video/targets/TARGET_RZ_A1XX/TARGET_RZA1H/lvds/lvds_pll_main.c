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
* @file         lvds_pll_main.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        lvds pll setting value
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdio.h>
#include    <stdlib.h>
#include    <math.h>

#include    "r_typedefs.h"
#include    "lvds_pll_calc.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* FIN: LVDS PLL input frequency */
#define     FIN_MIN             (9.0)           /*!< The lower limit of the FIN */
#define     FIN_MAX             (30.0)          /*!< The upper limit of the FIN */
/* FREF */
#define     FREF_MIN            (2.5)           /*!< The lower limit of the FREF */
#define     FREF_MAX            (30.0)          /*!< The upper limit of the FREF */
/* FVCO: VCO output frequency */
#define     FVCO_MIN            (750.0)         /*!< The lower limit of the FVCO */
#define     FVCO_MAX            (1630.0)        /*!< The upper limit of the FVCO */
/* FOUT: LVDS PLL output frequency */
#define     FOUT_MIN            (0.0)           /*!< The lower limit of the FOUT */
#define     FOUT_MAX            (609.0)         /*!< The upper limit of the FOUT */

#define     DIVIDER_3           (7.0)           /*!< The frequency dividing value for frequency divider 3 */


/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Variable Externs
******************************************************************************/
extern const double NIDIV_data[];
extern const double NRD_data[];
extern const double NFD_data[];
extern const double NOD_data[];
extern const double NODIV_data[];


/******************************************************************************
Private global variables and functions
******************************************************************************/
static double   InputClock;
static double   OutputClock;
static int      LvdsUsed;



/**************************************************************************//**
 * @brief
 * @param       [in,out]param
 * @param       [in]fout
 * @retval      int
 *****************************************************************************/
static int compare_nodiv (pll_parameter_t * param, double fout)
{
    int             change;
    const double  * nodiv_list;
    double          diff_freq;

    change      = 0;
    nodiv_list  = NODIV_data;
    while (*nodiv_list != 0.0) {
        diff_freq   = fabs(OutputClock - (fout / *nodiv_list));
        if (param->comparison_freq > diff_freq) {
            param->comparison_freq  = diff_freq;
            param->output_freq      = fout / *nodiv_list;
            param->nodiv            = *nodiv_list;
            change                  = 1;
        }
        nodiv_list++;
    }
    return change;
}

/**************************************************************************//**
 * @brief
 * @param       [in,out]param
 * @param       [in]fout
 * @retval      int
 *****************************************************************************/
static int compare_div3 (pll_parameter_t * param, double fout)
{
    double  diff_freq;
    int     change;

    change      = 0;
    diff_freq   = fabs(OutputClock - (fout / DIVIDER_3));

    if (param->comparison_freq > diff_freq) {
        param->comparison_freq  = diff_freq;
        param->output_freq      = fout / DIVIDER_3;
        change                  = 1;
    }
    return change;
}

/**************************************************************************//**
 * @brief
 * @param       [out]param
 * @retval      None
 *****************************************************************************/
static void SeekValue (pll_parameter_t * param)
{
    const double  * nidiv_list;
    const double  * nrd_list;
    const double  * nfd_list;
    const double  * nod_list;

    double  fin;
    double  fref;
    double  fvco;
    double  fout;
    int     ret;

    nidiv_list = NIDIV_data;
    while (*nidiv_list != 0.0) {
        fin = InputClock / *nidiv_list;
        if ((fin < FIN_MIN) || (FIN_MAX < fin)) {
            nidiv_list++;
            continue;
        }
        nrd_list = NRD_data;
        while (*nrd_list != 0.0) {
            fref = fin / *nrd_list;
            if ((fref < FREF_MIN) || (FREF_MAX < fref)) {
                nrd_list++;
                continue;
            }
            nfd_list = NFD_data;
            while (*nfd_list != 0.0) {
                fvco = fref * (*nfd_list);
                if ((fvco < FVCO_MIN) || (FVCO_MAX < fvco)) {
                    nfd_list++;
                    continue;
                }
                nod_list = NOD_data;
                while (*nod_list != 0.0) {
                    fout = fvco / *nod_list;
                    if ((fout < FOUT_MIN) || (FOUT_MAX < fout)) {
                        nod_list++;
                        continue;
                    }

                    if (LvdsUsed == 0) {
                        ret = compare_nodiv(param, fout);
                    } else {
                        ret = compare_div3(param, fout);
                    }
                    if (ret != 0) {
                        param->nidiv    = *nidiv_list;
                        param->nrd      = *nrd_list;
                        param->nfd      = *nfd_list;
                        param->nod      = *nod_list;
                    }
                    nod_list++;
                }
                nfd_list++;
            }
            nrd_list++;
        }
        nidiv_list++;
    }
}
/**************************************************************************//**
 * @brief       PLL value Calculation function
 * @param[in]   InClock     : Input clock frequency [MHz]
 * @param[in]   OuClock     : Output clock frequency [MHz]
 * @param[in]   Lvds        : The LVDS output interface is used (=1) or not (=0)
 * @param[out]  result      : pll_parameter_t
 * @retval      error (-1)
******************************************************************************/
int32_t lvds_pll_calc(
    const double      InClock,
    const double      OuClock,
    const uint32_t    Lvds,
    pll_parameter_t * result  )
{
    result->comparison_freq  = 1000.0;
    result->output_freq      = 0.0;
    result->nidiv            = 0.0;
    result->nrd              = 0.0;
    result->nfd              = 0.0;
    result->nod              = 0.0;
    result->nodiv            = 0.0;

    InputClock   = InClock;
    OutputClock  = OuClock;
    LvdsUsed     = Lvds;

    /* Calculation */
    SeekValue(result);

    result->nidiv = result->nidiv/2;
    result->nodiv = result->nodiv/2;
    if( result->nod <= 4 ) {
        result->nod = result->nod/2;
    } else if( result->nod == 8 ) {
        result->nod = 3;
    } else {
    }
    result->nrd -=1;        /* nrd-1 */
    return 0;
}
