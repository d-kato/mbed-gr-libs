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
* Copyright (C) 2013-2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file          scux_int.h
* $Rev: $
* $Date::                           $
* @brief         SCUX driver interrupt header
******************************************************************************/

#ifndef SCUX_INT_H
#define SCUX_INT_H

/******************************************************************************
Includes <System Includes>, "Project Includes"
******************************************************************************/

#include "scux.h"
#include "cmsis.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/*************************************************************************
 Enumerated Types
*************************************************************************/

/* Interrupt types */
typedef enum
{
    SCUX_INT_FDI  = 0,   /* FDI interrupt */
    SCUX_INT_FUI  = 1,   /* FUI interrupt */
    SCUX_INT_DVI  = 2,   /* DVI interrupt */
    SCUX_INT_AI   = 3,   /* AI interrupt */
    SCUX_INT_MAX  = 4
} scux_int_type_t;

/* Interrupt level */
typedef enum
{
    SCUX_INT_LEVEL = 0,   /* Level interrupt */
    SCUX_INT_EDGE  = 1    /* Edge interrupt */
} scux_int_leve_t;

/*************************************************************************
 Structures
*************************************************************************/
/* Table Information of Interrupt */
typedef struct
{
    IRQHandler       int_handler;          /* Interrupt handler */
    IRQn_Type        int_num;              /* Interrupt number */
    scux_int_leve_t  info_level;           /* Edge or level  */
} scux_info_int_t;

/***********************************************************************************
 Function Prototypes
***********************************************************************************/

void SCUX_InterruptInit(const int_t channel);
void SCUX_InterruptUninit(const int_t channel);

#endif /* SCUX_INT_H */
