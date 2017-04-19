/******************************************************************************
* Copyright(c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG Type Definitions.
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2011.03.04
*          - Added the type of NCGfp.
*          2012.08.22
*          - Updated coding format.
*          2012.11.14
*          - Added the definition for Multi-Display.
*             NCGDU_CH1
*             NCGDU_CH2
*          - Modified the type of NCGISRfp to NCGDUISRfp and NCGVGISRfp.
*
*******************************************************************************/

#ifndef NCG_DEFS_H
#define NCG_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
#define NCG_NULL    0
#else
#define NCG_NULL    ((void *)0)
#endif

#define NCG_FALSE   0
#define NCG_TRUE    1

#define NCG_UINT32_MAX (0xffffffffU)

#define NCG_TIMEOUT_INFINITE NCG_UINT32_MAX
#define NCG_INVALID_TLSINDEX NCG_UINT32_MAX

/* Display chanel */
#define NCGDU_CH1                   (0x00000001U)
#define NCGDU_CH2                   (0x00000002U)


#if !defined(NCG_UNREFERENCED_PARAMETER)
#define NCG_UNREFERENCED_PARAMETER   R_UNREFERENCED_VARIABLE
#endif

/*
 *  Error codes
 */

#define NCG_FIRST_ERROR (-255)
enum
{
    NCG_no_err = 0,
    NCG_err_unknown                 = NCG_FIRST_ERROR,
    NCG_err_not_implemented         = NCG_FIRST_ERROR+ 1,
    NCG_err_bad_parameter           = NCG_FIRST_ERROR+ 2,
    NCG_err_no_host_mem             = NCG_FIRST_ERROR+ 3,
    NCG_err_lost_host_mem           = NCG_FIRST_ERROR+ 4,
    NCG_err_no_frame_mem            = NCG_FIRST_ERROR+ 5,
    NCG_err_lost_frame_mem          = NCG_FIRST_ERROR+ 6,
    NCG_err_already_created         = NCG_FIRST_ERROR+ 7,
    NCG_err_isr_management_failed   = NCG_FIRST_ERROR+ 8,
    NCG_err_wait_timeout            = NCG_FIRST_ERROR+ 9,
    NCG_err_device_in_use           = NCG_FIRST_ERROR+10,
    NCG_err_not_found               = NCG_FIRST_ERROR+11,
    NCG_err_invalid_handle          = NCG_FIRST_ERROR+12,
    NCG_err_resource_exceeded       = NCG_FIRST_ERROR+13,
    NCG_last_err                    = NCG_FIRST_ERROR+14,
};


/*
 *  NCG data type definitions
 */

typedef void                    NCGvoid;
typedef unsigned int            NCGenum;
typedef unsigned int            NCGboolean;
typedef unsigned int            NCGbitfield;

typedef char                    NCGchar;
typedef signed char             NCGint8;
typedef short                   NCGint16;
typedef int                     NCGint32;
typedef long long               NCGint64;
typedef unsigned char           NCGuint8;
typedef unsigned short          NCGuint16;
typedef unsigned int            NCGuint32;
typedef unsigned long long      NCGuint64;
typedef float                   NCGfloat32;
typedef double                  NCGfloat64;

typedef int                     NCGsizei;
typedef float                   NCGclampf;
typedef long                    NCGfixed;

typedef NCGuint32 (*NCGDUISRfp)(NCGbitfield uiDisplay);
typedef NCGuint32 (*NCGVGISRfp)(void);
typedef NCGvoid (*NCGfp)(NCGvoid *);


#ifdef __cplusplus
}
#endif

#endif /* NCG_DEFS_H */
