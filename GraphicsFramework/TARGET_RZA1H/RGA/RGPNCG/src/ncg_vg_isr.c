/******************************************************************************
* Copyright(c) 2010-2015 Renesas Electronics Corporation. All rights reserved.
*
* brief  : R-GPVG control functions
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2011.03.07
*          - Fixed bug : NCGVG_Detach_ISR / [R-GPVG_common_P_033]
*          2012.08.22
*          - Moved definition to "ncg_vg_isr.h".
*            - RGPVG_INT_LEVEL
*          - Moved definition to "ncg_register.h".
*            - RTIP_VG_BASE
*            - VG_REG_VG_ISR
*          2012.08.22
*          - Added error message in the debug mode.
*          - Update all comments
*          2013.02.21
*          - Modified the argument of the NCGVG_RGPVG_ISR.
*          - Modified the call to R_INTC_RegistIntFunc function
*            by NCGVG_Attach_ISR.
*          2014.12.22
*          - Applied to OSPL and RGA.
*
*******************************************************************************/


/*=============================================================================
 * Includes
 */

#include "Project_Config.h"
#include    "ncg_defs.h"
#include    "ncg_debug.h"
#include    "ncg_vg_isr.h"
#include    "RGA.h"

/* Depending on the build environment */
#include    "r_typedefs.h"
#include    "r_ospl.h"

#ifdef  RGAH_VERSION

/*=============================================================================
 * Internal definitions
 */


/*=============================================================================
 *  Prototyping of internal functions
 */
static void NCGVG_RGPVG_ISR(void) ;
static void NCGVG_RGPVG_ISR_0(void) ;
static void NCGVG_RGPVG_ISR_1(void) ;
static void NCGVG_RGPVG_ISR_2(void) ;
static void NCGVG_RGPVG_ISR_3(void) ;


/*=============================================================================
 *  Private global variables and functions
 */
static NCGboolean NCGVG_ISR_Initialized  = NCG_FALSE ;
static NCGVGISRfp NCGVG_pRGPVG_Interrupt = NCG_NULL ;


/*=============================================================================
 *  Global Function
 */

/*----------------------------------------------------------------------------
 NAME       : NCGVG_Attach_ISR
 FUNCTION   : Attach the function pointer of OpenVG ISR.
 PARAMETERS : pfnInterrupt : [IN ] The pointer to the function of OpenVG ISR.
 RETURN     : Error code of the NCG.
------------------------------------------------------------------------------*/
NCGint32
NCGVG_Attach_ISR(
    NCGVGISRfp      pfnInterrupt )
{
    NCGint32        rc_val = NCG_no_err;
    bsp_int_err_t   eb;
    errnum_t        e;

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGVG_Attach_ISR" );

    NCG_ASSERT(pfnInterrupt != NCG_NULL);

    if( NCGVG_ISR_Initialized != NCG_FALSE ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGVG_Attach_ISR(1) Initialized.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        rc_val = NCG_err_isr_management_failed ;
    } else {
        NCGVG_ISR_Initialized  = NCG_TRUE ;
        NCGVG_pRGPVG_Interrupt = pfnInterrupt ;

        /* Regist function */
        eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT0, NCGVG_RGPVG_ISR_0 );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT1, NCGVG_RGPVG_ISR_1 );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT2, NCGVG_RGPVG_ISR_2 );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT3, NCGVG_RGPVG_ISR_3 );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }

        /* set priority */
        e= R_OSPL_SetInterruptPriority( BSP_INT_SRC_INT0, NCGVG_INT_LEVEL );
        IF(e) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        e= R_OSPL_SetInterruptPriority( BSP_INT_SRC_INT1, NCGVG_INT_LEVEL );
        IF(e) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        e= R_OSPL_SetInterruptPriority( BSP_INT_SRC_INT2, NCGVG_INT_LEVEL );
        IF(e) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        e= R_OSPL_SetInterruptPriority( BSP_INT_SRC_INT3, NCGVG_INT_LEVEL );
        IF(e) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }

        /* Enable interrupt from Renesas OpenVG library */
        eb= R_BSP_InterruptControl( BSP_INT_SRC_INT0, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        eb= R_BSP_InterruptControl( BSP_INT_SRC_INT1, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        eb= R_BSP_InterruptControl( BSP_INT_SRC_INT2, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
        eb= R_BSP_InterruptControl( BSP_INT_SRC_INT3, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR );
        IF ( eb != BSP_INT_SUCCESS ) {
            rc_val = NCG_err_isr_management_failed;
            goto fin;
        }
    }

fin:
    return rc_val;
}


/*----------------------------------------------------------------------------
 NAME       : NCGVG_Detach_ISR
 FUNCTION   : Detach the function pointer of OpenVG ISR.
 PARAMETERS : pfnInterrupt : [IN ] The pointer to the function of OpenVG ISR.
 RETURN     : Error code of the NCG.
------------------------------------------------------------------------------*/
NCGint32
NCGVG_Detach_ISR(
    NCGVGISRfp      pfnInterrupt )
{
    NCGint32        rc_val = NCG_no_err;
    bsp_int_err_t   eb;

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGVG_Detach_ISR" );

    NCG_ASSERT(pfnInterrupt != NCG_NULL);

    if ( NCGVG_ISR_Initialized == NCG_FALSE ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGVG_Detach_ISR(1) Not initialized.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        rc_val = NCG_err_isr_management_failed;
    } else {
        NCGVG_ISR_Initialized = NCG_FALSE ;

        if( pfnInterrupt == NCGVG_pRGPVG_Interrupt ) {
            /* Disable interrupt from Renesas OpenVG library */
            eb= R_BSP_InterruptControl( BSP_INT_SRC_INT3, BSP_INT_CMD_INTERRUPT_DISABLE, FIT_NO_PTR );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }
            eb= R_BSP_InterruptControl( BSP_INT_SRC_INT2, BSP_INT_CMD_INTERRUPT_DISABLE, FIT_NO_PTR );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }
            eb= R_BSP_InterruptControl( BSP_INT_SRC_INT1, BSP_INT_CMD_INTERRUPT_DISABLE, FIT_NO_PTR );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }
            eb= R_BSP_InterruptControl( BSP_INT_SRC_INT0, BSP_INT_CMD_INTERRUPT_DISABLE, FIT_NO_PTR );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }

            /* Unregist function */
            eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT3, FIT_NO_FUNC );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }
            eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT2, FIT_NO_FUNC );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }
            eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT1, FIT_NO_FUNC );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }
            eb= R_BSP_InterruptWrite( BSP_INT_SRC_INT0, FIT_NO_FUNC );
            IF ( eb != BSP_INT_SUCCESS ) {
                rc_val = NCG_err_isr_management_failed;
                goto fin;
            }

            NCGVG_pRGPVG_Interrupt = NCG_NULL ;
        } else {
            NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGVG_Detach_ISR(2) Miss match function pointer.%s" )
            NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
            NCG_DEBUG_MAKE_MSG_END();
            NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
            rc_val = NCG_err_isr_management_failed;
        }
    }

fin:
    return rc_val;
}


/*=============================================================================
 *  Internal functions
 */

/*-----------------------------------------------------------------------------
 NAME       : NCGVG_RGPVG_ISR
 FUNCTION   : The Interrrupt from OpenVG.
 PARAMETERS :
 RETURN     : None.
-----------------------------------------------------------------------------*/
static void NCGVG_RGPVG_ISR(void)
{
    NCGuint32 ret ;
    errnum_t  e;

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGVG_RGPVG_ISR" );

    if( NCGVG_pRGPVG_Interrupt == NCG_NULL ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][INFO]NCGVG_RGPVG_ISR Not initialized.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
    } else {
        ret = NCGVG_pRGPVG_Interrupt() ;
        if ( ret != 0 ) {
            NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGVG_RGPVG_ISR(0x%08x)%s" )
            NCG_DEBUG_MAKE_MSG_PARAMETER( ret )
            NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
            NCG_DEBUG_MAKE_MSG_END();
            NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        }

        e= R_GRAPHICS_OnInterrupting();
        if ( e != 0 ) {
            NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGVG_RGPVG_ISR(e=0x%08x)%s" )
            NCG_DEBUG_MAKE_MSG_PARAMETER( e )
            NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
            NCG_DEBUG_MAKE_MSG_END();
            NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        }
    }

    return ;
}

/*-----------------------------------------------------------------------------
 NAME       : NCGVG_RGPVG_ISR_0
 FUNCTION   : The Interrrupt from OpenVG.
 PARAMETERS :
 RETURN     : None.
-----------------------------------------------------------------------------*/
static void NCGVG_RGPVG_ISR_0(void)
{
    NCGVG_RGPVG_ISR();

    GIC_EndInterrupt( BSP_INT_SRC_INT0 );
}

/*-----------------------------------------------------------------------------
 NAME       : NCGVG_RGPVG_ISR_1
 FUNCTION   : The Interrrupt from OpenVG.
 PARAMETERS :
 RETURN     : None.
-----------------------------------------------------------------------------*/
static void NCGVG_RGPVG_ISR_1(void)
{
    NCGVG_RGPVG_ISR();

    GIC_EndInterrupt( BSP_INT_SRC_INT1 );
}

/*-----------------------------------------------------------------------------
 NAME       : NCGVG_RGPVG_ISR_2
 FUNCTION   : The Interrrupt from OpenVG.
 PARAMETERS :
 RETURN     : None.
-----------------------------------------------------------------------------*/
static void NCGVG_RGPVG_ISR_2(void)
{
    NCGVG_RGPVG_ISR();

    GIC_EndInterrupt( BSP_INT_SRC_INT2 );
}

/*-----------------------------------------------------------------------------
 NAME       : NCGVG_RGPVG_ISR_3
 FUNCTION   : The Interrrupt from OpenVG.
 PARAMETERS :
 RETURN     : None.
-----------------------------------------------------------------------------*/
static void NCGVG_RGPVG_ISR_3(void)
{
    NCGVG_RGPVG_ISR();

    GIC_EndInterrupt( BSP_INT_SRC_INT3 );
}

#endif
/* -- end of file -- */
