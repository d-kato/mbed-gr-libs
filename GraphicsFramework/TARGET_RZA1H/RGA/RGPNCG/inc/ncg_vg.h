/******************************************************************************
* Copyright(c) 2010-2013 Renesas Electronics Corporation. All rights reserved.
*
* brief  : R-GPVG Functions header
*
* author : Renesas Electronics Corporation
*
* history: 2011.01.12
*          - Created the initial code.
*          2012.08.22
*          - Updated coding format.
*          2013.05.13
*          - Added function of initialization.
*          - Added function of finalization.
*
*******************************************************************************/

#ifndef NCG_VG_H
#define NCG_VG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ncg_defs.h"
/*=============================================================================
 * Definitions
 */

/* VGINFO */
typedef struct VGINFO_TAG {
    NCGuint32   ui32Flags;
} NCGVGINFO, * PNCGVGINFO;

/* RGPINFO */
typedef struct RGPINFO_TAG {
    NCGuint32   ui32WorkBufferStride;       /* Work Buffer Stride */
    NCGuint32   ui32WorkBufferHeight;       /* Work Buffer Height */
    NCGuint32   ui32StreamBuffer0Size;      /* Stream Buffer 0 Size */
    NCGuint32   ui32StreamBuffer1Size;      /* Stream Buffer 1 Size */
    NCGuint32   ui32DisplayListBufferSize;  /* Display List Buffer Size */
} NCGRGPINFO, * PNCGRGPINFO;


/*=============================================================================
 *  Structures
 */


/*=============================================================================
 *  Function prototyping
 */

/*
 *  VG initialization / finalization
 */
NCGvoid
NCGVG_Init (
    PNCGVGINFO      pVGInfo
);

NCGvoid
NCGVG_DeInit (
    PNCGVGINFO      pVGInfo
);


#ifdef __cplusplus
}
#endif

#endif /* NCG_VG_H */
