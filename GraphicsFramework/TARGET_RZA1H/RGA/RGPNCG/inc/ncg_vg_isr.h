/******************************************************************************
* Copyright(c) 2010-2013 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG R-GPVG interrupt handling functions header.
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2012.08.22
*          - Updated coding format.
*          2013.01.08
*          - Applied to R-GPVG2 for Aragon.
*          2013.02.21
*          - Modified the value of NCGVG_INT_LEVEL.
*
*******************************************************************************/

#ifndef NCG_VG_ISR_H
#define NCG_VG_ISR_H


#ifdef __cplusplus
extern "C" {
#endif


/*=============================================================================
 * Definitions
 */

#define NCGVG_INT0_VECT     (130)
#define NCGVG_INT1_VECT     (131)
#define NCGVG_INT2_VECT     (132)
#define NCGVG_INT3_VECT     (133)

#define NCGVG_INT_LEVEL     (5)               /* interrupt level */


/*=============================================================================
 *  Structures
 */


/*=============================================================================
 *  Function prototyping
 */

/*
 * VG interrupt handling
 */

NCGint32
NCGVG_Attach_ISR (
    NCGVGISRfp pfnInterrupt
);

NCGint32
NCGVG_Detach_ISR (
    NCGVGISRfp pfnInterrupt
);


#ifdef __cplusplus
}
#endif

#endif /* NCG_VG_ISR_H */
