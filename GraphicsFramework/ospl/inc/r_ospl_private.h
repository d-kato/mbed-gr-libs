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
/**
* @file  r_ospl_private.h
* @brief   OS Porting Layer private API for OS less
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef R_OSPL_PRIVATE_H
#define R_OSPL_PRIVATE_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @struct  r_ospl_table_block_t
* @brief  Block of <r_ospl_table_t>
*/
typedef struct st_r_ospl_table_block_t  r_ospl_table_block_t;
struct st_r_ospl_table_block_t {

    /** this[ SortedKeyIndex ].Key ... */
    const void  *Key;

    /** Index */
    int16_t  Index;

    /** this[ Index ].NextFreeIndex  : Index is not same as SortedKeyIndex */
    int16_t  NextFreeIndex;
};
enum {
    R_OSPL_TABLE_BLOCK_USED    = -1,
    R_OSPL_TABLE_BLOCK_NO_NEXT = -2
};


/**
* @struct  r_ospl_table_searched_t
* @brief  Result of searching
*/
typedef struct st_r_ospl_table_searched_t  r_ospl_table_searched_t;
struct st_r_ospl_table_searched_t {

    /** SortedKeyIndex */
    int_fast32_t  SortedKeyIndex;

    /** IsFound */
    bool_t  IsFound;
};


/* Section: Global */
/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

#if ! IS_MBED_USED
#if  defined( __CC_ARM )

/* ->QAC 0289 */ /* ->QAC 1002 */ /* ->MISRA 5.1 */
extern uint32_t  Image$$BEGIN_OF_CACHED_RAM_BARRIER$$Base;
extern uint32_t  Image$$BEGIN_OF_NOCACHE_RAM_BARRIER$$Base;
extern uint32_t  Image$$BEGIN_OF_NOCACHE_RAM_BARRIER$$ZI$$Limit;
extern uint32_t  Image$$END_OF_INTERNAL_RAM_BARRIER$$ZI$$Limit;
/* <-QAC 0289 */ /* <-QAC 1002 */ /* <-MISRA 5.1 */

#endif
#else  /* IS_MBED_USED */
extern uint32_t  Image$$RW_DATA_NC$$Base;
#endif


/******************************************************************************
Functions Prototypes
******************************************************************************/

/**
* @brief   LockUnlockedChannel
*
* @param   out_ChannelNum ChannelNum
* @param   HardwareIndexMin HardwareIndexMin
* @param   HardwareIndexMax HardwareIndexMax
* @return  Error Code. 0=No Error.
*/
#if ! BSP_CFG_USER_LOCKING_ENABLED
errnum_t  R_OSPL_LockUnlockedChannel( int_fast32_t *out_ChannelNum,
                                      mcu_lock_t  HardwareIndexMin,  mcu_lock_t  HardwareIndexMax );
#endif


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* R_OSPL_PRIVATE_H */

