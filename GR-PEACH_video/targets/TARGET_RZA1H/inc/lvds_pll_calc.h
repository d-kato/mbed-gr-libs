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
* @file         lvds_pll_calc.h
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        Debug function header
******************************************************************************/

#ifndef LVDS_PLL_CALC_H
#define LVDS_PLL_CALC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_typedefs.h"


/******************************************************************************
Macro definitions
******************************************************************************/
typedef struct {
    double comparison_freq;
    double output_freq;
    double nidiv;
    double nrd;
    double nfd;
    double nod;
    double nodiv;
} pll_parameter_t;

/******************************************************************************
Typedef definitions
******************************************************************************/
#define LVDS_IF_NOT_USE (0u)
#define LVDS_IF_USE     (1u)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
extern int32_t lvds_pll_calc(
    const double      InClock,
    const double      OuClock,
    const uint32_t    Lvds,
    pll_parameter_t * result  );

#endif  /* LVDS_PLL_CALC_H */


