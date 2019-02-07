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
* http://www.renesas.com/disclaimer *
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* System Name  : SDHI Driver Sample Program
* File Name    : r_sd_cfg.h
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : SD Memory card driver configuration
* Operation    : 
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
******************************************************************************/
#ifndef _R_SD_CFG_H_
#define _R_SD_CFG_H_

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
/* ------------------------------------------------------
  Set SDHI Base Address
--------------------------------------------------------*/
#define SDCFG_IP0_BASE      SD_SCC_IP0_BASE_ADDR       /* Set the base address of SCC ch0. */
#define SDCFG_IP1_BASE      SD_SCC_IP1_BASE_ADDR       /* Set the base address of SCC ch1. */

/* ------------------------------------------------------
  Set the method of check SD Status
--------------------------------------------------------*/
#define SDCFG_HWINT
//#define SDCFG_POLL

/* ------------------------------------------------------
  Set the method of data transfer
--------------------------------------------------------*/
#define SDCFG_TRNS_DMA
//#define SDCFG_TRNS_SW

/* ------------------------------------------------------
  Set the card type to support
--------------------------------------------------------*/
#define SDCFG_MEM

/* ------------------------------------------------------
  Set the version to support
--------------------------------------------------------*/
//#define SDCFG_VER1X            /* Version 1.1 */
#define SDCFG_VER2X            /* Version 2.x */

/* ------------------------------------------------------
  Set the method to detect card
--------------------------------------------------------*/
#define SDCFG_CD_INT

#ifdef SDCFG_CD_INT
    #ifndef SDCFG_HWINT
        #error    please define SDCFG_HWINT
    #endif
#endif

/* ==== end of the setting ==== */
#if    defined(SDCFG_HWINT)
    #if    defined(SDCFG_TRNS_DMA)
            #if    defined(SDCFG_VER2X)
                #define SDCFG_DRIVER_MODE     (SD_MODE_HWINT|SD_MODE_DMA|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER2X)
            #else
                #define SDCFG_DRIVER_MODE     (SD_MODE_HWINT|SD_MODE_DMA|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER1X)
            #endif
    #else
            #if    defined(SDCFG_VER2X)
                #define SDCFG_DRIVER_MODE     (SD_MODE_HWINT|SD_MODE_SW|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER2X)
            #else
                #define SDCFG_DRIVER_MODE     (SD_MODE_HWINT|SD_MODE_SW|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER1X)
            #endif
    #endif
#else
    #if    defined(SDCFG_TRNS_DMA)
            #if    defined(SDCFG_VER2X)
                #define SDCFG_DRIVER_MODE     (SD_MODE_POLL|SD_MODE_DMA|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER2X)
            #else
                #define SDCFG_DRIVER_MODE     (SD_MODE_POLL|SD_MODE_DMA|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER1X)
            #endif
    #else
            #if    defined(SDCFG_VER2X)
                #define SDCFG_DRIVER_MODE     (SD_MODE_POLL|SD_MODE_SW|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER2X)
            #else
                #define SDCFG_DRIVER_MODE     (SD_MODE_POLL|SD_MODE_SW|SD_MODE_MEM|SD_MODE_DS|SD_MODE_VER1X)
            #endif
    #endif
#endif    

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#endif /* _R_SD_CFG_H_    */

/* End of File */
