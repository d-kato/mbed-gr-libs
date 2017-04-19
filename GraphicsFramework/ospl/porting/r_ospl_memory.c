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
* @file  r_ospl_memory.c
* @brief   Memory map
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: $
* $Date::                           $
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#ifdef R_OSPL_TEST_CODE
#include  "r_ospl_test.h"
#endif
#include  "r_ospl_private.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @struct  gs_address_table_line_t
* @brief  gs_address_table_line_t
*/
typedef struct st_gs_address_table_line_t  gs_address_table_line_t;
struct st_gs_address_table_line_t {

    /** Address */
    uintptr_t  Address;

    /** Value */
    int_fast32_t  Value;
};


/**
* @struct  gs_address_table_line_c_t
* @brief  gs_address_table_line_c_t
*/
typedef struct st_gs_address_table_line_c_t  gs_address_table_line_c_t;
struct st_gs_address_table_line_c_t {

    /** Address */
    uintptr_t  Address;

    /** Value */
    r_ospl_axi_cache_attribute_t  Value;
};


/******************************************************************************
Macro definitions
******************************************************************************/

/* ->MISRA 17.4 : These are addresses */

#define GS_MIRROR_SIZE    0x40000000u
#define GS_CACHED_START   0x00000000u
#define GS_CACHED_END     ( GS_CACHED_START + GS_MIRROR_SIZE )
#define GS_UNCACHED_START 0x40000000u
#define GS_UNCACHED_END   ( GS_UNCACHED_START + GS_MIRROR_SIZE )

#define GS_IO_START       0x60A00000u

/* From "scatter.scat" file */
#if defined( RZ_A1L )
#define LRAM_SIZE                   0x00300000u
#else  /* RZ/A1H */
#define LRAM_SIZE                   0x00A00000u
#endif
#define LRAM_START                  0x20000000u
#define LRAM_END                    ( LRAM_START + LRAM_SIZE )
#define UNUSED_UNCACHED_LRAM_START  0x60000000u
#define UNCACHED_LRAM_END           ( UNUSED_UNCACHED_LRAM_START + LRAM_SIZE )

/* <-MISRA 17.4 */

#define  GS_EXTERNAL_START_0x00000000               0x00000000u
#define  GS_EXTERNAL_SDRAM_START_0x08000000         0x08000000u
#define  GS_INTERNAL_RAM_START_0x20000000           0x20000000u
#define  GS_INTERNAL_REGISTERS_START_0x80000000     0x80000000u

#if defined( RZ_A1L_EV_BOARD )
#define  GS_SDRAM_SIZE  0x04000000u
#else
#define  GS_SDRAM_SIZE  0x08000000u
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
#ifdef R_OSPL_TEST_CODE
static  r_ospl_axi_envronment_t  gs_Environment;
#endif


/******************************************************************************
* Implement: R_OSPL_ToPhysicalAddress
******************************************************************************/
errnum_t  R_OSPL_ToPhysicalAddress( const volatile void *in_Address, uintptr_t *out_PhysicalAddress )
{
    errnum_t  e;
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    uintptr_t const  address = (uintptr_t) in_Address;
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */

    IF_DQ( out_PhysicalAddress == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    *out_PhysicalAddress = address;

    e=0;
fin:
    return  e;
}


/******************************************************************************
* Implement: R_OSPL_ToCachedAddress
******************************************************************************/
errnum_t  R_OSPL_ToCachedAddress( const volatile void *in_Address, void *out_CachedAddress )
{
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    /* ->QAC 0289 */ /* ->QAC 1002 : Image$$BEGIN_OF_NOCACHE_RAM_BARRIER$$Base */
    errnum_t  e;
#if  IS_MBED_USED
    uintptr_t  address = (uintptr_t) in_Address;
    const uintptr_t  uncached_from_cached = UNUSED_UNCACHED_LRAM_START - LRAM_START;
#else
    uintptr_t const  address = (uintptr_t) in_Address;
#endif

    IF_DQ( out_CachedAddress == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

#if  IS_MBED_USED
    IF ( address < LRAM_START ) {
        e = E_ACCESS_DENIED;
    }
    else if ( address < LRAM_END ) {
        e = 0;
    } else IF ( address < UNUSED_UNCACHED_LRAM_START ) {
        e = E_ACCESS_DENIED;
    }
    else if ( address < UNCACHED_LRAM_END ) {
        address -= uncached_from_cached;
        e = 0;
    } else {
        e = E_ACCESS_DENIED;
    }
#else
    IF ( (address >= (uintptr_t)&Image$$BEGIN_OF_NOCACHE_RAM_BARRIER$$Base)  &&
         (address < (uintptr_t)&Image$$END_OF_INTERNAL_RAM_BARRIER$$ZI$$Limit) ) {
        /* "BEGIN_OF_NOCACHE_RAM_BARRIER" is defined in "scatter.sct" file. */
        /* Image$$ execution region symbols */
        /* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0377d/CHDFHJDJ.html */

        e = E_ACCESS_DENIED;
    }
    else IF ( address >= UNUSED_UNCACHED_LRAM_START ) {
        e = E_ACCESS_DENIED;
    }
    else {
        e = 0;
    }
#endif

    *(void **) out_CachedAddress = (void *) address;

fin:
    return  e;
    /* <-QAC 0289 *//* <-QAC 1002 */
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */
}


/******************************************************************************
* Implement: R_OSPL_ToUncachedAddress
******************************************************************************/
errnum_t  R_OSPL_ToUncachedAddress( const volatile void *in_Address, void *out_UncachedAddress )
{
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    /* ->QAC 0289 */ /* ->QAC 1002 : Image$$BEGIN_OF_CACHED_RAM_BARRIER$$Base */
    errnum_t  e;
#if  IS_MBED_USED
#if  defined ( __GNUC__ ) && ! defined( __CC_ARM )
    uintptr_t  address = (uintptr_t) in_Address;
#else
    uintptr_t  address = (uintptr_t) in_Address;
    uintptr_t  nc_base = UNUSED_UNCACHED_LRAM_START;
    const uintptr_t  uncached_from_cached = UNUSED_UNCACHED_LRAM_START - LRAM_START;
#endif
#else
    uintptr_t const  address = (uintptr_t) in_Address;
#endif

    IF_DQ( out_UncachedAddress == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

#if  IS_MBED_USED
#if  defined ( __GNUC__ ) && ! defined( __CC_ARM )
    e = 0;
#else
    IF ( address < nc_base - uncached_from_cached ) {
        e = E_ACCESS_DENIED;
    }
    else if ( address < LRAM_END ) {
        address += uncached_from_cached;
        e = 0;
    } else IF ( address < nc_base ) {
        e = E_ACCESS_DENIED;
    }
    else if ( address < UNCACHED_LRAM_END ) {
        e = 0;
    } else {
        e = E_ACCESS_DENIED;
    }
#endif
#else
    IF ( (address >= (uintptr_t)&Image$$BEGIN_OF_CACHED_RAM_BARRIER$$Base)  &&
         (address < (uintptr_t)&Image$$BEGIN_OF_NOCACHE_RAM_BARRIER$$ZI$$Limit) ) {
        e = E_ACCESS_DENIED;
    }
    else if ( (address >= (uintptr_t)&Image$$BEGIN_OF_NOCACHE_RAM_BARRIER$$Base)  &&
              (address < (uintptr_t)&Image$$END_OF_INTERNAL_RAM_BARRIER$$ZI$$Limit) ) {
        e = 0;
    } else IF ( R_CUT_IF_ALWAYS_TRUE( (address >= GS_CACHED_START)  &&)  (address < GS_CACHED_END) ) {
        e = E_ACCESS_DENIED;
    }
    else IF ( (address >= UNUSED_UNCACHED_LRAM_START) && (address < GS_IO_START) ) {
        e = E_ACCESS_DENIED;
    }
    else {
        e = 0;
    }
#endif

    *(void **) out_UncachedAddress = (void *) address;

fin:
    return  e;
    /* <-QAC 0289 *//* <-QAC 1002 */
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */
}


/******************************************************************************
* Implement: R_OSPL_MEMORY_GetLevelOfFlush
******************************************************************************/
errnum_t  R_OSPL_MEMORY_GetLevelOfFlush( const void *in_Address, int_fast32_t *out_Level )
{
    uintptr_t  address = (uintptr_t) in_Address;

#ifdef R_OSPL_TEST_CODE
    if ( gs_Environment == R_OSPL_AXI_L2CACHED ) {
#endif
        static const gs_address_table_line_t  table[] = {
            /* .Address                                       .Value */
            { GS_EXTERNAL_START_0x00000000,                        0 },
            { GS_EXTERNAL_SDRAM_START_0x08000000,                  1 },
            { GS_EXTERNAL_SDRAM_START_0x08000000 + GS_SDRAM_SIZE,  0 },
            { GS_INTERNAL_RAM_START_0x20000000,                    1 },
            { GS_INTERNAL_RAM_START_0x20000000 + LRAM_SIZE,        0 }
        };


        if ( address < table[1].Address ) {
            *out_Level = table[0].Value;
        } else if ( address < table[2].Address ) {
            *out_Level = table[1].Value;
        } else if ( address < table[3].Address ) {
            *out_Level = table[2].Value;
        } else if ( address < table[4].Address ) {
            *out_Level = table[3].Value;
        } else {
            *out_Level = table[4].Value;
        }
        R_STATIC_ASSERT( R_COUNT_OF( table ) == 5, "" );

#ifdef R_OSPL_TEST_CODE
    } else {
        static const gs_address_table_line_t  table[] = {
            /* .Address                                       .Value */
            { GS_EXTERNAL_START_0x00000000,                        0 },
            { GS_EXTERNAL_SDRAM_START_0x08000000,                  2 },
            { GS_EXTERNAL_SDRAM_START_0x08000000 + GS_SDRAM_SIZE,  0 },
            { GS_INTERNAL_RAM_START_0x20000000,                    1 },
            { GS_INTERNAL_RAM_START_0x20000000 + LRAM_SIZE,        0 },
            /* { GS_EXTERNAL_SDRAM_MIRROR_START_0x48000000,                 2 }, */
            /* { GS_EXTERNAL_SDRAM_MIRROR_START_0x48000000 + GS_SDRAM_SIZE, 0 }, */
            /* { GS_INTERNAL_RAM_MIRROR_START_0x60000000,                   0 }, */
            /* { GS_INTERNAL_RAM_MIRROR_START_0x60000000 + LRAM_SIZE,       0 } */
        };


        ASSERT_R( gs_Environment == R_OSPL_AXI_L2STRONGLY, R_NOOP() );

        if ( address < table[1].Address ) {
            *out_Level = table[0].Value;
        } else if ( address < table[2].Address ) {
            *out_Level = table[1].Value;
        } else if ( address < table[3].Address ) {
            *out_Level = table[2].Value;
        } else if ( address < table[4].Address ) {
            *out_Level = table[3].Value;
        } else {
            *out_Level = table[4].Value;
        }
        R_STATIC_ASSERT( R_COUNT_OF( table ) == 5, "" );
    }
#endif
    return  0;
}


/******************************************************************************
* Implement: R_OSPL_AXI_Get2ndCacheAttribute
******************************************************************************/
errnum_t  R_OSPL_AXI_Get2ndCacheAttribute( uintptr_t const  in_PhysicalAddress,
        r_ospl_axi_cache_attribute_t *const  out_CacheAttribute )
{
    uintptr_t  address = in_PhysicalAddress;

#ifdef R_OSPL_TEST_CODE
    if ( gs_Environment == R_OSPL_AXI_L2CACHED ) {
#endif
        static const gs_address_table_line_c_t  table[] = {
            /* .Address                                  .Value */
            { GS_EXTERNAL_START_0x00000000,               R_OSPL_AXI_WRITE_BACK_W },
            /*{ GS_EXTERNAL_SDRAM_START_0x08000000,       R_OSPL_AXI_WRITE_BACK_W },*/
            { GS_INTERNAL_RAM_START_0x20000000,           R_OSPL_AXI_CACHE_ZERO },
            /*{ GS_INTERNAL_REGISTERS_START_0x80000000,   R_OSPL_AXI_CACHE_ZERO }*/
            /* If external I/O existed, "*out_CacheAttribute = R_AXI_DEVICE;" */
        };


        if ( address < table[1].Address ) {
            *out_CacheAttribute = table[0].Value;
        } else {
            *out_CacheAttribute = table[1].Value;
        }
        R_STATIC_ASSERT( R_COUNT_OF( table ) == 2, "" );

#ifdef R_OSPL_TEST_CODE
    } else {
        static const gs_address_table_line_c_t  table[] = {
            /* .Address                                  .Value */
            { GS_EXTERNAL_START_0x00000000,               R_OSPL_AXI_STRONGLY },
            /*{ GS_EXTERNAL_SDRAM_START_0x08000000,       R_OSPL_AXI_STRONGLY },*/
            { GS_INTERNAL_RAM_START_0x20000000,           R_OSPL_AXI_CACHE_ZERO },
            /*{ GS_EXTERNAL_MIRROR_START_0x40000000,        R_OSPL_AXI_STRONGLY },*/
            /*{ GS_EXTERNAL_SDRAM_MIRROR_START_0x48000000,R_OSPL_AXI_STRONGLY },*/
            /*{ GS_INTERNAL_RAM_MIRROR_START_0x60000000,    R_OSPL_AXI_CACHE_ZERO },*/
            /*{ GS_INTERNAL_REGISTERS_START_0x80000000,   R_OSPL_AXI_CACHE_ZERO }*/
            /* If external I/O existed, "*out_CacheAttribute = R_AXI_DEVICE;" */
        };


        ASSERT_R( gs_Environment == R_OSPL_AXI_L2STRONGLY, R_NOOP() );

        if ( address < table[1].Address ) {
            *out_CacheAttribute = table[0].Value;
        } else {
            *out_CacheAttribute = table[1].Value;
        }
        R_STATIC_ASSERT( R_COUNT_OF( table ) == 2, "" );
    }
#endif
    return  0;
}


/***********************************************************************
* Implement: R_OSPL_AXI_GetProtection
************************************************************************/
errnum_t  R_OSPL_AXI_GetProtection( uintptr_t const  in_PhysicalAddress,
                                    r_ospl_axi_protection_t *const  out_Protection )
{
    *out_Protection = R_OSPL_AXI_NON_SECURE;  /* Same as CPU(TTB) NS bit */

    return  0;
}


/***********************************************************************
* Implement: R_OSPL_AXI_SetEnvironment
************************************************************************/
#ifdef R_OSPL_TEST_CODE
errnum_t  R_OSPL_AXI_SetEnvironment( r_ospl_axi_envronment_t in_Environment )
{
    gs_Environment = in_Environment;
    return  0;
}
#endif


