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
* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* $FileName: jcu_user.c $
* $Module: JCU $ $PublicVersion: 1.00 $ (=JCU_VERSION)
* $Rev: 35 $
* $Date:: 2014-02-26 13:18:53 +0900#$
* Description : JCU driver porting layer
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#include  "iodefine.h"
#include  "r_jcu_api.h"
#ifndef R_OSPL_NDEBUG
#include  <stdio.h>
#endif

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


/**************************************************************************//**
* Function Name: [R_JCU_PrintRegisters]
* @brief
* @retval        None
******************************************************************************/
#ifndef R_OSPL_NDEBUG
void  R_JCU_PrintRegisters()
{
    printf( "\n" );
    printf( "JCU Registers\n" );
    printf( "JCMOD  = 0x%02X\n", JCU.JCMOD  );
    printf( "JCCMD  = 0x%02X\n", JCU.JCCMD  );
    printf( "JCQTN  = 0x%02X\n", JCU.JCQTN  );
    printf( "JCHTN  = 0x%02X\n", JCU.JCHTN  );
    printf( "JCDRIU = 0x%02X\n", JCU.JCDRIU );
    printf( "JCDRID = 0x%02X\n", JCU.JCDRID );
    printf( "JCVSZU = 0x%02X\n", JCU.JCVSZU );
    printf( "JCVSZD = 0x%02X\n", JCU.JCVSZD );
    printf( "JCHSZU = 0x%02X\n", JCU.JCHSZU );
    printf( "JCHSZD = 0x%02X\n", JCU.JCHSZD );
    printf( "JCDTCU = 0x%02X\n", JCU.JCDTCU );
    printf( "JCDTCM = 0x%02X\n", JCU.JCDTCM );
    printf( "JCDTCD = 0x%02X\n", JCU.JCDTCD );
    printf( "JINTE0 = 0x%02X\n", JCU.JINTE0 );
    printf( "JINTS0 = 0x%02X\n", JCU.JINTS0 );
    printf( "JCDERR = 0x%02X\n", JCU.JCDERR );
    printf( "JCRST  = 0x%02X\n", JCU.JCRST  );
    printf( "JIFECNT   = 0x%08X\n", (uint_fast32_t) JCU.JIFECNT   );
    /* Cast is for avoiding "format" warning of GCC_ARM */
    printf( "JIFESA    = 0x%08X\n", (uint_fast32_t) JCU.JIFESA    );
    printf( "JIFESOFST = 0x%08X\n", (uint_fast32_t) JCU.JIFESOFST );
    printf( "JIFEDA    = 0x%08X\n", (uint_fast32_t) JCU.JIFEDA    );
    printf( "JIFESLC   = 0x%08X\n", (uint_fast32_t) JCU.JIFESLC   );
    printf( "JIFEDDC   = 0x%08X\n", (uint_fast32_t) JCU.JIFEDDC   );
    printf( "JIFDCNT   = 0x%08X\n", (uint_fast32_t) JCU.JIFDCNT   );
    printf( "JIFDSA    = 0x%08X\n", (uint_fast32_t) JCU.JIFDSA    );
    printf( "JIFDDOFST = 0x%08X\n", (uint_fast32_t) JCU.JIFDDOFST );
    printf( "JIFDDA    = 0x%08X\n", (uint_fast32_t) JCU.JIFDDA    );
    printf( "JIFDSDC   = 0x%08X\n", (uint_fast32_t) JCU.JIFDSDC   );
    printf( "JIFDDLC   = 0x%08X\n", (uint_fast32_t) JCU.JIFDDLC   );
    printf( "JIFDADT   = 0x%08X\n", (uint_fast32_t) JCU.JIFDADT   );
    printf( "JINTE1    = 0x%08X\n", (uint_fast32_t) JCU.JINTE1    );
    printf( "JINTS1    = 0x%08X\n", (uint_fast32_t) JCU.JINTS1    );
    printf( "JIFESVSZ  = 0x%08X\n", (uint_fast32_t) JCU.JIFESVSZ  );
    printf( "JIFESHSZ  = 0x%08X\n", (uint_fast32_t) JCU.JIFESHSZ  );
    printf( "JCQTBL0[0] = 0x%02X\n", (uint_fast32_t) JCU.JCQTBL0 );
    printf( "JCQTBL1[0] = 0x%02X\n", (uint_fast32_t) JCU.JCQTBL1 );
    printf( "JCQTBL2[0] = 0x%02X\n", (uint_fast32_t) JCU.JCQTBL2 );
    printf( "JCQTBL3[0] = 0x%02X\n", (uint_fast32_t) JCU.JCQTBL3 );
    printf( "JCHTBD0[0] = 0x%02X\n", (uint_fast32_t) JCU.JCHTBD0 );
    printf( "JCHTBA0[0] = 0x%02X\n", (uint_fast32_t) JCU.JCHTBA0 );
    printf( "JCHTBD1[0] = 0x%02X\n", (uint_fast32_t) JCU.JCHTBD1 );
    printf( "JCHTBA1[0] = 0x%02X\n", (uint_fast32_t) JCU.JCHTBA1 );
}
#endif

