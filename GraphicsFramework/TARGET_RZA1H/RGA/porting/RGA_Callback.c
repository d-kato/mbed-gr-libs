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
* Copyright (C) 2013 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  RGA_Callback.c
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/

#include  <string.h>
#include  "RGA.h"
#include  "RGA_Callback_private.h"
#include  "clib_drivers.h"



/** gs_WorkBufferMemory */
static uint8_t  *gs_WorkBufferMemory;
static size_t    gs_WorkBufferMemory_Size;
static uint8_t  *gs_WorkBufferB_Memory;
static size_t    gs_WorkBufferB_Memory_Size;


/**
* @struct  GraphicsDefaultSubClass
* @brief  GraphicsDefaultSubClass
*/
typedef struct _GraphicsDefaultSubClass  GraphicsDefaultSubClass;
struct _GraphicsDefaultSubClass {
    uint8_t  *WorkBufferMemory;
    size_t    work_buffer_size;
    bool_t    IsUsedWorkBuffer;
    uint8_t  *WorkBufferB_Memory;
    size_t    work_buffer_b_size;
    bool_t    IsUsedWorkBufferB;
};


/* Section: Global */
/** gs_GraphicsDefaultSub */
static  GraphicsDefaultSubClass  gs_GraphicsDefaultSub = {
    NULL,      /* WorkBufferMemory */
    0,         /* work_buffer_size */
    false,     /* IsUsedWorkBuffer */
    NULL,      /* WorkBufferB_Memory */
    0,         /* work_buffer_b_size */
    false,     /* IsUsedWorkBufferB */
};



/**
* @brief   R_GRAPHICS_STATIC_OnInitializeDefault
*
* @par Parameters
*    None
* @return  None.
*/
errnum_t  R_GRAPHICS_STATIC_OnInitializeDefault( graphics_t *self, graphics_config_t *in_out_Config,
        void **out_CalleeDefined )
{
    enum { work_buffer_flags =
               F_GRAPHICS_WORK_BUFFER_ADDRESS |
               F_GRAPHICS_WORK_BUFFER_SIZE |
               F_GRAPHICS_MAX_WIDTH_OF_FRAME_BUFFER |
               F_GRAPHICS_MAX_HEIGHT_OF_FRAME_BUFFER
         };

    enum { work_buffer_B_flags =
               F_GRAPHICS_WORK_BUFFER_B_ADDRESS |
               F_GRAPHICS_WORK_BUFFER_B_SIZE
         };

    static r_ospl_c_lock_t  gs_c_lock_object;

    errnum_t  e;

    GraphicsDefaultSubClass  *sub = &gs_GraphicsDefaultSub;

    R_UNREFERENCED_VARIABLE_2( self, out_CalleeDefined );

    R_MEMORY_SECTION_GetMemory_RGA_WorkBuffer( &gs_WorkBufferMemory, &gs_WorkBufferMemory_Size,
            &gs_WorkBufferB_Memory, &gs_WorkBufferB_Memory_Size );


    /* Set default configuration */

    /* Use work buffer */
    if ( IS_ANY_BITS_NOT_SET( in_out_Config->flags, work_buffer_flags ) ) {
        ASSERT_R( IS_ALL_BITS_NOT_SET( in_out_Config->flags, work_buffer_flags ),  e=E_OTHERS; goto fin );
        ASSERT_R( ! sub->IsUsedWorkBuffer,  e=E_OTHERS; goto fin );

#if  RGA_WORK_BUFFER_ADDRESS_ALIGNMENT == 64  /* Check of "R_Ceil_64u" */
        sub->WorkBufferMemory = (uint8_t *) R_Ceil_64u( (uintptr_t) gs_WorkBufferMemory );
#elif  RGA_WORK_BUFFER_ADDRESS_ALIGNMENT == 4  /* Check of "R_Ceil_4u" */
        sub->WorkBufferMemory = (uint8_t *) R_Ceil_4u( (uintptr_t) gs_WorkBufferMemory );
#else
#error
#endif
        sub->work_buffer_size = gs_WorkBufferMemory_Size - ( sub->WorkBufferMemory - gs_WorkBufferMemory );

        in_out_Config->flags |= work_buffer_flags;
        in_out_Config->work_buffer_address = sub->WorkBufferMemory;
        in_out_Config->work_buffer_size    = sub->work_buffer_size;
        in_out_Config->max_width_of_frame_buffer = MAX_WIDTH_OF_FRAME_BUFFER;
        in_out_Config->max_height_of_frame_buffer = MAX_HEIGHT_OF_FRAME_BUFFER;

        sub->IsUsedWorkBuffer = true;
    }

    /* Use work buffer B */
    if ( IS_ANY_BITS_NOT_SET( in_out_Config->flags, work_buffer_B_flags ) ) {
        ASSERT_R( IS_ALL_BITS_NOT_SET( in_out_Config->flags, work_buffer_B_flags ),  e=E_OTHERS; goto fin );
        ASSERT_R( ! sub->IsUsedWorkBufferB,  e=E_OTHERS; goto fin );

#if  RGA_WORK_BUFFER_B_ADDRESS_ALIGNMENT == 32  /* Check of "R_Ceil_32u" */
        sub->WorkBufferB_Memory = (uint8_t *) R_Ceil_32u( (uintptr_t) gs_WorkBufferB_Memory );
#elif  RGA_WORK_BUFFER_B_ADDRESS_ALIGNMENT == 4  /* Check of "R_Ceil_4u" */
        sub->WorkBufferB_Memory = (uint8_t *) R_Ceil_4u( (uintptr_t) gs_WorkBufferB_Memory );
#else
#error
#endif
        sub->work_buffer_b_size = gs_WorkBufferB_Memory_Size - ( sub->WorkBufferB_Memory - gs_WorkBufferB_Memory );

        in_out_Config->flags |= work_buffer_B_flags;
        in_out_Config->work_buffer_b_address = sub->WorkBufferB_Memory;
        in_out_Config->work_buffer_b_size    = sub->work_buffer_b_size;

        sub->IsUsedWorkBufferB = true;
    }

#ifdef  WORK_SIZE_FOR_LIB_PNG
    in_out_Config->flags |= F_GRAPHICS_WORK_SIZE_FOR_LIBPNG;
    in_out_Config->work_size_for_libPNG = WORK_SIZE_FOR_LIB_PNG;
#endif

    R_OSPL_MEMORY_Flush( R_OSPL_FLUSH_WRITEBACK_INVALIDATE );
    /* Avoid write back by initialization to global variables of buffers */

    /* Lock object */
    in_out_Config->flags |= F_GRAPHICS_LOCK_OBJECT;
    in_out_Config->lock_object = &gs_c_lock_object;

    e=0;
fin:
    return  e;
}



/**
* @brief   R_GRAPHICS_STATIC_OnFinalizeDefault
*
* @par Parameters
*    None
* @return  None.
*/
errnum_t  R_GRAPHICS_STATIC_OnFinalizeDefault( graphics_t *self, void *CalleeDefined, errnum_t e )
{
    GraphicsDefaultSubClass  *sub = &gs_GraphicsDefaultSub;

    R_UNREFERENCED_VARIABLE_2( self, CalleeDefined );
    sub->IsUsedWorkBuffer  = false;
    sub->IsUsedWorkBufferB = false;
    return  e;
}


/**
* @brief   drawCLUTImage
*
* @par Parameters
*    None
* @return  None.
*/
errnum_t drawCLUTImage( uint8_t *SrcAddress, const graphics_image_t *image, frame_buffer_t *frame )
{
    errnum_t e;
    int_t image_w;
    int i;
    uint32_t ImageWidthPoint;
    uint8_t *DestAddress;
    graphics_image_properties_t  image_prop;

    DestAddress = frame->buffer_address[ frame->draw_buffer_index ];

    switch( frame->pixel_format ) {
        case PIXEL_FORMAT_CLUT8:
            ImageWidthPoint = 1;
            break;
        case PIXEL_FORMAT_CLUT4:
            ImageWidthPoint = 2;
            break;
        case PIXEL_FORMAT_CLUT1:
            ImageWidthPoint = 8;
            break;
        default:
            /* never comes here */
            ImageWidthPoint = 1;
            break;
    }

    e = R_GRAPHICS_IMAGE_GetProperties( image, &image_prop );
    IF(e)goto fin;
    ASSERT_R( image_prop.pixelFormat == frame->pixel_format , e=E_NOT_SUPPORTED_PIXEL_FORMAT; goto fin );
    ASSERT_R( ( image->width % ImageWidthPoint ) == 0 , e=E_OTHERS; goto fin );
    ASSERT_R( image->width <= frame->width , e=E_OTHERS; goto fin );
    ASSERT_R( image->height <= frame->height , e=E_OTHERS; goto fin );
    ASSERT_R( frame->stride % 32 == 0 , e=E_OTHERS; goto fin );

    image_w = image_prop.width;
    switch( frame->pixel_format ) {
        case PIXEL_FORMAT_CLUT8:
            SrcAddress += sizeof( uint32_t ) * 256;
            break;
        case PIXEL_FORMAT_CLUT4:
            image_w = image_w / 2;
            SrcAddress += sizeof( uint32_t ) * 16;
            break;
        case PIXEL_FORMAT_CLUT1:
            image_w = image_w / 8;
            SrcAddress += sizeof( uint32_t ) * 2;
            break;
        default:
            e=E_OTHERS;
            goto fin;
    }
    for( i = 0; i < image->height ; i++ ) {
        memcpy(( void *) DestAddress, ( void *)SrcAddress, (size_t)image_w );
        DestAddress += frame->stride;
        SrcAddress += image_w;
    }
    e = 0;
fin:
    return e;
}


