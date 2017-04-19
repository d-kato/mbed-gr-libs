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
* @file  RGA_Cpp.h
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/

#ifndef  RGA_CPP_H
#define  RGA_CPP_H

#ifndef __cplusplus
#error C++ only
#endif


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "RGA.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

typedef float  graphics_matrix_float_t;
typedef double graphics_matrix_other_float_t;


/***********************************************************************
* Class: ObjectHandleClass
************************************************************************/
class  ObjectHandleClass
{
public:
    void  *Entity;

    ObjectHandleClass() {
        this->Entity = NULL;
    }
};

bool_t  operator == ( ObjectHandleClass Left, ObjectHandleClass Right );


/** undefined */
#define  undefined  get_undefined()
extern ObjectHandleClass  get_undefined();


/***********************************************************************
* Class: saveList_st
************************************************************************/
struct saveList_st {
    saveList_st       *beforePoint;
    graphics_status_t  saveData;
};


/***********************************************************************
* Class: Canvas2D_ImageEntityClass
************************************************************************/
class Canvas2D_ImageEntityClass
{
public:
    graphics_image_t  *C_Image;
    bool_t               isImageDataComposition;

    Canvas2D_ImageEntityClass( bool_t composition ) {
        this->C_Image = NULL;
        this->isImageDataComposition = composition;
    }
};


/***********************************************************************
* Class: Canvas2D_ImageClass
************************************************************************/
class  Canvas2D_ImageClass
{
public:
    Canvas2D_ImageEntityClass  *Entity;

    Canvas2D_ImageClass() {
        this->Entity = NULL;
    }

    operator ObjectHandleClass () {
        return  *(ObjectHandleClass *) this;
    }

    Canvas2D_ImageClass &operator = ( ObjectHandleClass Right ) {
        this->Entity = (Canvas2D_ImageEntityClass *) Right.Entity;
        return  *this;
    }


    void  set_imageClass( graphics_image_t *imageClass );
    inline void  operator= ( graphics_image_t *imageClass ) {
        this->set_imageClass( imageClass );
    }

    inline operator graphics_image_t *() {
        return  this->Entity->C_Image;
    }


    void destroy();


    /* "width" property */
    class  widthProperty
    {
    public:
        inline operator int_t() {
            graphics_image_properties_t  prop;
            errnum_t e;
            Canvas2D_ImageClass  *parent = (Canvas2D_ImageClass *)(
                                               (char *) this - offsetof( Canvas2D_ImageClass, width ) );
            e = R_GRAPHICS_IMAGE_GetProperties( parent->Entity->C_Image, &prop );
            if( e ) {
                R_OSPL_SetErrNum( e );
                prop.width = 0;
            }
            return  prop.width;
        }
    } width;

    /* "height" property */
    class  heightProperty
    {
    public:
        inline operator int_t() {
            graphics_image_properties_t  prop;
            errnum_t e;
            Canvas2D_ImageClass  *parent = (Canvas2D_ImageClass *)(
                                               (char *) this - offsetof( Canvas2D_ImageClass, height ) );
            e = R_GRAPHICS_IMAGE_GetProperties( parent->Entity->C_Image, &prop );
            if( e ) {
                R_OSPL_SetErrNum( e );
                prop.height = 0;
            }
            return  prop.height;
        }
    } height;

    /* "data" property */
    class  dataProperty
    {
    public:
        inline operator uint8_t *() {
            graphics_image_properties_t  prop;
            errnum_t e;
            Canvas2D_ImageClass  *parent = (Canvas2D_ImageClass *)(
                                               (char *) this - offsetof( Canvas2D_ImageClass, data ) );
            e = R_GRAPHICS_IMAGE_GetProperties( parent->Entity->C_Image, &prop );
            if( e ) {
                R_OSPL_SetErrNum( e );
                prop.data = NULL;
            }
            return  prop.data;
        }

        inline operator r8g8b8a8_t *() {
            return  (r8g8b8a8_t *)(uint8_t *) *this;
        }

        inline operator void *() {
            return  (void *)(uint8_t *) *this;
        }

        inline uint8_t  &operator[]( int Index ) {
            return  ( (uint8_t *) *this )[ Index ];
        }

        inline uint8_t  *operator+( int Index ) {
            return  ( (uint8_t *) *this ) + Index;
        }
    } data;

    /* "src" property */
    class  srcProperty
    {
    public:
        inline void  operator= ( const graphics_image_t *imageClass ) {
            Canvas2D_ImageClass  *parent = (Canvas2D_ImageClass *)(
                                               (char *) this - offsetof( Canvas2D_ImageClass, src ) );

            parent->set_imageClass( (graphics_image_t *)imageClass );
        }
        inline operator graphics_image_t *() {
            graphics_image_t *outData = NULL;
            Canvas2D_ImageClass  *parent = (Canvas2D_ImageClass *)(
                                               (char *) this - offsetof( Canvas2D_ImageClass, src ) );

            if( parent->Entity != NULL ) {
                outData = parent->Entity->C_Image;
            }
            return outData;
        }
    } src;
};


/***********************************************************************
* Class: Canvas2D_PatternClass
************************************************************************/
class  Canvas2D_PatternClass
{
public:
    graphics_pattern_t  *Entity;

    Canvas2D_PatternClass() {
        this->Entity = NULL;
    }

    operator ObjectHandleClass () {
        return  *(ObjectHandleClass *) this;
    }

    Canvas2D_PatternClass &operator = ( ObjectHandleClass Right ) {
        this->Entity = (graphics_pattern_t *) Right.Entity;
        return  *this;
    }

    void destroy();
};


/***********************************************************************
* Class: Canvas2D_ContextEntityClass
************************************************************************/
class Canvas2D_ContextEntityClass
{
public:
    graphics_t  *C_Graphics;
    saveList_st    *LastSavePoint;

    Canvas2D_ContextEntityClass() {
        this->C_Graphics = NULL;
        this->LastSavePoint = NULL;
    }
};


/***********************************************************************
* Class: Canvas2D_ContextClass
************************************************************************/
class  Canvas2D_ContextClass
{
public:
    Canvas2D_ContextEntityClass  *Entity;

    Canvas2D_ContextClass() {
        this->Entity = NULL;
    }

    operator ObjectHandleClass () {
        return  *(ObjectHandleClass *) this;
    }

    Canvas2D_ContextClass &operator = ( ObjectHandleClass Right ) {
        this->Entity = (Canvas2D_ContextEntityClass *) Right.Entity;
        return  *this;
    }

    void  destroy();
    void  clearRect( int x, int y, int w, int h );
    void  save();
    void  restore();
    void  drawImage( const graphics_image_t *image, int_t minX, int_t minY );
    void  drawImage( const graphics_image_t *image, int_t minX, int_t minY , int_t width, int_t height );
    void  drawImage( const graphics_image_t *image, int_t srcMinX,  int_t srcMinY,  int_t srcWidth,   int_t srcHeight,
                     int_t destMinx, int_t destMinY, int_t destWidth , int_t destHeight );
    Canvas2D_ImageClass  createImageData( Canvas2D_ImageClass image );
    Canvas2D_ImageClass  createImageData( int_t width, int_t height );
    Canvas2D_ImageClass  getImageData( int_t minX, int_t minY, int_t width, int_t height );
    void  putImageData( Canvas2D_ImageClass imageData, int_t minX, int_t minY );
    void  putImageData ( Canvas2D_ImageClass imageData, int_t minX, int_t minY, int_t dirtyX, int_t dirtyY, int_t dirtyWidth, int_t dirtyHeight );
    void  fillRect( int x, int y, int w, int h );
    Canvas2D_PatternClass  createPattern( const graphics_image_t *image, const char *repetition );
    void  beginPath();
    void  rect( int_t minX, int_t minY, int_t width, int_t height );
    void  clip();

    void  setTransform( graphics_matrix_float_t sx,  graphics_matrix_float_t ky,
                        graphics_matrix_float_t kx,  graphics_matrix_float_t sy,
                        graphics_matrix_float_t tx,  graphics_matrix_float_t ty );
    void  setTransform( graphics_matrix_other_float_t sx,  graphics_matrix_other_float_t ky,
                        graphics_matrix_other_float_t kx,  graphics_matrix_other_float_t sy,
                        graphics_matrix_other_float_t tx,  graphics_matrix_other_float_t ty ) {
        setTransform( (graphics_matrix_float_t) sx,  (graphics_matrix_float_t) ky,
                      (graphics_matrix_float_t) kx,  (graphics_matrix_float_t) sy,
                      (graphics_matrix_float_t) tx,  (graphics_matrix_float_t) ty );
    }
    void  setTransform( int_t sx,  int_t ky, int_t kx,  int_t sy, int_t tx,  int_t ty ) {
        setTransform( (graphics_matrix_float_t) sx,  (graphics_matrix_float_t) ky,
                      (graphics_matrix_float_t) kx,  (graphics_matrix_float_t) sy,
                      (graphics_matrix_float_t) tx,  (graphics_matrix_float_t) ty );
    }

    void  translate( graphics_matrix_float_t tx, graphics_matrix_float_t ty );
    void  translate( graphics_matrix_other_float_t tx, graphics_matrix_other_float_t ty ) {
        translate( (graphics_matrix_float_t) tx, (graphics_matrix_float_t) ty );
    }
    void  translate( graphics_matrix_float_t tx, graphics_matrix_other_float_t ty ) {
        translate( tx, (graphics_matrix_float_t) ty );
    }
    void  translate( graphics_matrix_other_float_t tx, graphics_matrix_float_t ty ) {
        translate( (graphics_matrix_float_t) tx, ty );
    }
    void  translate( int_t tx, int_t ty ) {
        translate( (graphics_matrix_float_t) tx, (graphics_matrix_float_t) ty );
    }

    void  scale( graphics_matrix_float_t sx, graphics_matrix_float_t sy );
    void  scale( graphics_matrix_other_float_t sx, graphics_matrix_other_float_t sy ) {
        scale( (graphics_matrix_float_t) sx, (graphics_matrix_float_t) sy );
    }
    void  scale( graphics_matrix_float_t sx, graphics_matrix_other_float_t sy ) {
        scale( sx, (graphics_matrix_float_t) sy );
    }
    void  scale( graphics_matrix_other_float_t sx, graphics_matrix_float_t sy ) {
        scale( (graphics_matrix_float_t) sx, sy );
    }
    void  scale( int_t sx, int_t sy ) {
        scale( (graphics_matrix_float_t) sx, (graphics_matrix_float_t) sy );
    }

    void  rotate( graphics_matrix_float_t angle );
    void  rotate( graphics_matrix_other_float_t angle ) {
        rotate( (graphics_matrix_float_t) angle );
    }

    void  transform( graphics_matrix_float_t sx,  graphics_matrix_float_t ky,
                     graphics_matrix_float_t kx,  graphics_matrix_float_t sy,
                     graphics_matrix_float_t tx,  graphics_matrix_float_t ty );
    void  transform( graphics_matrix_other_float_t sx,  graphics_matrix_other_float_t ky,
                     graphics_matrix_other_float_t kx,  graphics_matrix_other_float_t sy,
                     graphics_matrix_other_float_t tx,  graphics_matrix_other_float_t ty ) {
        transform( (graphics_matrix_float_t) sx,  (graphics_matrix_float_t) ky,
                   (graphics_matrix_float_t) kx,  (graphics_matrix_float_t) sy,
                   (graphics_matrix_float_t) tx,  (graphics_matrix_float_t) ty );
    }
    void  transform( int_t sx,  int_t ky,  int_t kx,  int_t sy,  int_t tx,  int_t ty ) {
        transform( (graphics_matrix_float_t) sx,  (graphics_matrix_float_t) ky,
                   (graphics_matrix_float_t) kx,  (graphics_matrix_float_t) sy,
                   (graphics_matrix_float_t) tx,  (graphics_matrix_float_t) ty );
    }


    /* "fillStyle" property */
    void  Set_fillStyle( const char *Color );
    void  Set_fillStyle( r8g8b8a8_t Color );
    void  Set_fillStylePattern( const Canvas2D_PatternClass Pattern );
#if 0
    void  Set_fillStyleGradient( const Canvas2D_GradientClass Gradient );
#endif
    class  fillStyleProperty
    {
    public:
        inline void  operator= ( const char *Color ) {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, fillStyle ) );

            parent->Set_fillStyle( Color );
        }
        inline void  operator= ( r8g8b8a8_t Color ) {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, fillStyle ) );

            parent->Set_fillStyle( Color );
        }
        inline void  operator= ( const Canvas2D_PatternClass Pattern ) {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, fillStyle ) );

            parent->Set_fillStylePattern( Pattern );
        }
#if 0
        inline void  operator= ( const Canvas2D_GradientClass Gradient ) {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, fillStyle ) );

            parent->Set_fillStyleGradient( Gradient );
        }
#endif
    } fillStyle;


    /* "c_LanguageContext" property */
    class  c_LanguageContextProperty
    {
        inline graphics_t  *get_c_LanguageContext() {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, c_LanguageContext ) );

            return  parent->Entity->C_Graphics;
        }
    public:

        inline operator graphics_t *() {
            return  get_c_LanguageContext();
        }
        inline graphics_t *operator->() {
            return  get_c_LanguageContext();
        }
    } c_LanguageContext;


    /* "globalAlpha" property */
    void set_globalAlpha( const float32_t alpha );
    float32_t get_globalAlpha();
    class  globalAlphaProperty
    {
    public:
        inline void  operator= ( const float32_t alpha ) {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, globalAlpha ) );

            parent->set_globalAlpha( alpha );
        }
        inline void  operator= ( const float64_t alpha ) {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, globalAlpha ) );

            parent->set_globalAlpha( (float32_t) alpha );
        }
        inline operator float32_t() {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, globalAlpha ) );

            return parent->get_globalAlpha();
        }
    } globalAlpha;


    /* "globalCompositeOperation" property */
    void set_globalCompositeOperation( const char *operation );
    char *get_globalCompositeOperation();
    class  globalCompositeOperationProperty
    {
    public:
        inline void  operator= ( const char *operation ) {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, globalCompositeOperation ) );

            parent->set_globalCompositeOperation( operation );
        }
        inline operator char *() {
            Canvas2D_ContextClass  *parent = (Canvas2D_ContextClass *)(
                                                 (char *) this - offsetof( Canvas2D_ContextClass, globalCompositeOperation ) );

            return parent->get_globalCompositeOperation();
        }
    } globalCompositeOperation;

};


/***********************************************************************
* Class: Canvas2D_ContextConfigClass
************************************************************************/
struct  Canvas2D_ContextConfigClass {
    frame_buffer_t  *frame_buffer;
    bool_t           is_fast_manual_flush;

    Canvas2D_ContextConfigClass() {
        frame_buffer = NULL;
        is_fast_manual_flush = false;
    }
};


/***********************************************************************
* Functions: Canvas2D_Constructers
************************************************************************/
Canvas2D_ContextClass  R_RGA_New_Canvas2D_ContextClass( frame_buffer_t *frame_buffer );
Canvas2D_ContextClass  R_RGA_New_Canvas2D_ContextClass( Canvas2D_ContextConfigClass &in_out_Config );
Canvas2D_ImageClass  R_RGA_New_Canvas2D_ImageClass();

#endif  // RGA_CPP_H
