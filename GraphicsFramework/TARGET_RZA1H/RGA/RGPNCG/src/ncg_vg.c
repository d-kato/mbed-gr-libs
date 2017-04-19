/******************************************************************************
* Copyright(c) 2010-2014 Renesas Electronics Corporation. All rights reserved.
*
* brief  : R-GPVG control functions.
*
* author : Renesas Electronics Corporation
*
* history: 2011.01.20
*          - Created the initial code.
*          2012.08.22
*          - Added return value to "NCGVG_InitResourceSize"
*          2013.05.13
*          - Added function of initialization.
*          - Added function of finalization.
*          2013.06.10
*          - Applied to new VDC5 driver (Version 0.03.0046).
*          2013.11.07
*          - Modified value of NCG_VG_SBO1_SIZE to (4U) from (0U).
*          2014.02.13
*          - Modified the value of definitions.
*              NCG_VG_WB_STRIDE
*              NCG_VG_WB_HEIGHT
*              NCG_VG_SBO0_SIZE
*              NCG_VG_DLB_SIZE
*          2014.03.25
*          - Modified NCGVG_Init() and NCGVG_DeInit().
*          2014.05.16
*          - Modified NCGVG_Init() and NCGVG_DeInit().
*          2014.12.22
*          - Applied to OSPL and RGA.
*
*******************************************************************************/

/*=============================================================================
 * Includes
 */

#include "Project_Config.h"
#include "ncg_defs.h"
#include "ncg_debug.h"
#include "r_ospl.h"

#include "ncg_vg.h"

/* Depending on the build environment */
#include "r_typedefs.h"

#include "iodefine.h"
#include "iobitmasks/cpg_iobitmask.h"

#ifdef  RGAH_VERSION

/*=============================================================================
 * Internal definitions
 */

/* TODO:
    Please change these parameters in accordance with your system.
 */
#define NCG_VG_WB_STRIDE    (256U)
#define NCG_VG_WB_HEIGHT    (480U)

#define NCG_VG_SBO0_SIZE    (16384U)
#define NCG_VG_SBO1_SIZE    (4U)
#define NCG_VG_DLB_SIZE     (5120U)

/*=============================================================================
 *  Prototyping of internal functions
 */


/*=============================================================================
 *  Private global variables and functions
 */


/*=============================================================================
 *  Global Function
 */

/*----------------------------------------------------------------------------
 NAME       : NCGVG_Init
 FUNCTION   : Initialize R-GPVG.
 PARAMETERS : pVGInfo : [IN ] The pointer to the initialization information struct.
 RETURN     : None.
------------------------------------------------------------------------------*/
NCGvoid
NCGVG_Init (
    PNCGVGINFO pVGInfo)
{
    /* Depending on the build environment */
    volatile uint8_t    reg_value;

    NCG_UNREFERENCED_PARAMETER( pVGInfo );

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGVG_Init" );

    reg_value = CPG.STBCR10;
    if ( CPG.STBCR10 & CPG_STBCR10_MSTP100 ) {
        /* Standby control register 10 (STBCR10) : 0 : R-GPVG enable */
        CPG.STBCR10 &= ~CPG_STBCR10_MSTP100;
        /* dummy read */
        reg_value = CPG.STBCR10;

        if ( CPG.STBREQ2 & CPG_STBREQ2_STBRQ20 ) {
            CPG.SWRSTCR3 |=  CPG_SWRSTCR3_SRST32;
            /* dummy read */
            reg_value = CPG.SWRSTCR3;

            CPG.SWRSTCR3 &= ~CPG_SWRSTCR3_SRST32;
            /* dummy read */
            reg_value = CPG.SWRSTCR3;

            CPG.STBREQ2 &= ~CPG_STBREQ2_STBRQ20;
            /* dummy read */
            reg_value = CPG.STBREQ2;
            while ( (CPG.STBACK2 & CPG_STBACK2_STBAK20) != 0  ) ;
        }
    }

    NCG_UNREFERENCED_PARAMETER( reg_value );  /* Avoid warning of "unused-but-set-variable" of GCC_ARM */

    return ;
}

/*----------------------------------------------------------------------------
 NAME       : NCGVG_DeInit
 FUNCTION   : Finalize R-GPVG.
 PARAMETERS : pVGInfo : [IN ] The pointer to the finalization information struct.
 RETURN     : None.
------------------------------------------------------------------------------*/
NCGvoid
NCGVG_DeInit (
    PNCGVGINFO      pVGInfo )
{
    /* Depending on the build environment */
    volatile uint8_t    reg_value;

    NCG_UNREFERENCED_PARAMETER(pVGInfo);

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGVG_DeInit" );

    if ( (CPG.STBCR10 & CPG_STBCR10_MSTP100) != CPG_STBCR10_MSTP100 ) {
        CPG.STBREQ2 |= CPG_STBREQ2_STBRQ20;
        /* dummy read */
        reg_value = CPG.STBREQ2;
        while ( (CPG.STBACK2 & CPG_STBACK2_STBAK20) == 0  ) ;

        /* Standby control register 10 (STBCR10) : 0 : R-GPVG disable */
        CPG.STBCR10 |= CPG_STBCR10_MSTP100;
        /* dummy read */
        reg_value = CPG.STBCR10;
    }

    NCG_UNREFERENCED_PARAMETER( reg_value );  /* Avoid warning of "unused-but-set-variable" of GCC_ARM */

    return ;
}

#endif
/* -- end of file -- */
