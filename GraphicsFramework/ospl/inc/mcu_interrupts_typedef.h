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
* @file  mcu_interrupts_typedef.h
* @brief   Interrupt related FIT BSP. Data types.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef MCU_INTERRUPTS_TYPEDEF_H
#define MCU_INTERRUPTS_TYPEDEF_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "r_multi_compiler_typedef.h"
#if  IS_MBED_USED
#include  "cmsis.h"
#else
#include  "Renesas_RZ_A1.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @enum   bsp_int_err_t
* @brief  Error code defined by FIT BSP.
*
*    - BSP_INT_SUCCESS - 0
*    - BSP_INT_ERR_NO_REGISTERED_CALLBACK - 0x2101
*    - BSP_INT_ERR_INVALID_ARG - 1
*    - BSP_INT_ERR_UNSUPPORTED - 15
*/
typedef enum {
    BSP_INT_SUCCESS = 0,
    BSP_INT_ERR_NO_REGISTERED_CALLBACK = 0x2101,
    BSP_INT_ERR_INVALID_ARG = 1,
    BSP_INT_ERR_UNSUPPORTED = 15
}
                          bsp_int_err_t;


/**
* @brief   Interrupt Handler
*
* @param   int_sense uint32_t
* @return  None
*/
typedef void  (* bsp_int_cb_t )(void);


/**
* @enum   bsp_int_cmd_t
* @brief  Control command related to the interrupt.
*
*    - BSP_INT_CMD_INTERRUPT_ENABLE - 0
*    - BSP_INT_CMD_INTERRUPT_DISABLE - 1
*/
typedef enum {
    BSP_INT_CMD_INTERRUPT_ENABLE,
    BSP_INT_CMD_INTERRUPT_DISABLE
} bsp_int_cmd_t;


/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @def  FIT_NO_PTR
* @brief  None as pointer type
*/
#define  FIT_NO_PTR  ( (void*) 0 )


/**
* @def  FIT_NO_FUNC
* @brief  None as function pointer type
*/
#define  FIT_NO_FUNC ( (void*) 0 )


/**
* @enum   bsp_int_src_t
* @brief  Interrupt number.
*/
typedef IRQn_Type  bsp_int_src_t;
#if defined(TARGET_RZ_A1XX)

#define BSP_INT_SRC_SW0             ((IRQn_Type)(0))     /* GIC software interrupt             */
#define BSP_INT_SRC_SW1             ((IRQn_Type)(1))     /*                                    */
#define BSP_INT_SRC_SW2             ((IRQn_Type)(2))     /*                                    */
#define BSP_INT_SRC_SW3             ((IRQn_Type)(3))     /*                                    */
#define BSP_INT_SRC_SW4             ((IRQn_Type)(4))     /*                                    */
#define BSP_INT_SRC_SW5             ((IRQn_Type)(5))     /*                                    */
#define BSP_INT_SRC_SW6             ((IRQn_Type)(6))     /*                                    */
#define BSP_INT_SRC_SW7             ((IRQn_Type)(7))     /*                                    */
#define BSP_INT_SRC_SW8             ((IRQn_Type)(8))     /*                                    */
#define BSP_INT_SRC_SW9             ((IRQn_Type)(9))     /*                                    */
#define BSP_INT_SRC_SW10            ((IRQn_Type)(10))    /*                                    */
#define BSP_INT_SRC_SW11            ((IRQn_Type)(11))    /*                                    */
#define BSP_INT_SRC_SW12            ((IRQn_Type)(12))    /*                                    */
#define BSP_INT_SRC_SW13            ((IRQn_Type)(13))    /*                                    */
#define BSP_INT_SRC_SW14            ((IRQn_Type)(14))    /*                                    */
#define BSP_INT_SRC_SW15            ((IRQn_Type)(15))    /*                                    */
#define BSP_INT_SRC_PMUIRQ0         ((IRQn_Type)(16))    /* CPU                                */
#define BSP_INT_SRC_COMMRX0         ((IRQn_Type)(17))    /*                                    */
#define BSP_INT_SRC_COMMTX0         ((IRQn_Type)(18))    /*                                    */
#define BSP_INT_SRC_CTIIRQ0         ((IRQn_Type)(19))    /*                                    */
#define BSP_INT_SRC_IRQ0            ((IRQn_Type)(32))    /* IRQ                                */
#define BSP_INT_SRC_IRQ1            ((IRQn_Type)(33))    /*                                    */
#define BSP_INT_SRC_IRQ2            ((IRQn_Type)(34))    /*                                    */
#define BSP_INT_SRC_IRQ3            ((IRQn_Type)(35))    /*                                    */
#define BSP_INT_SRC_IRQ4            ((IRQn_Type)(36))    /*                                    */
#define BSP_INT_SRC_IRQ5            ((IRQn_Type)(37))    /*                                    */
#define BSP_INT_SRC_IRQ6            ((IRQn_Type)(38))    /*                                    */
#define BSP_INT_SRC_IRQ7            ((IRQn_Type)(39))    /*                                    */
#define BSP_INT_SRC_PL310ERR        ((IRQn_Type)(40))    /* Level 2 cache                      */
#define BSP_INT_SRC_DMAINT0         ((IRQn_Type)(41))    /* Direct memory access controller    */
#define BSP_INT_SRC_DMAINT1         ((IRQn_Type)(42))    /*                                    */
#define BSP_INT_SRC_DMAINT2         ((IRQn_Type)(43))    /*                                    */
#define BSP_INT_SRC_DMAINT3         ((IRQn_Type)(44))    /*                                    */
#define BSP_INT_SRC_DMAINT4         ((IRQn_Type)(45))    /*                                    */
#define BSP_INT_SRC_DMAINT5         ((IRQn_Type)(46))    /*                                    */
#define BSP_INT_SRC_DMAINT6         ((IRQn_Type)(47))    /*                                    */
#define BSP_INT_SRC_DMAINT7         ((IRQn_Type)(48))    /*                                    */
#define BSP_INT_SRC_DMAINT8         ((IRQn_Type)(49))    /*                                    */
#define BSP_INT_SRC_DMAINT9         ((IRQn_Type)(50))    /*                                    */
#define BSP_INT_SRC_DMAINT10        ((IRQn_Type)(51))    /*                                    */
#define BSP_INT_SRC_DMAINT11        ((IRQn_Type)(52))    /*                                    */
#define BSP_INT_SRC_DMAINT12        ((IRQn_Type)(53))    /*                                    */
#define BSP_INT_SRC_DMAINT13        ((IRQn_Type)(54))    /*                                    */
#define BSP_INT_SRC_DMAINT14        ((IRQn_Type)(55))    /*                                    */
#define BSP_INT_SRC_DMAINT15        ((IRQn_Type)(56))    /*                                    */
#define BSP_INT_SRC_DMAERR          ((IRQn_Type)(57))    /*                                    */
#define BSP_INT_SRC_USBI0           ((IRQn_Type)(73))    /* USB 2.0 host/function module       */
#define BSP_INT_SRC_USBI1           ((IRQn_Type)(74))    /*                                    */
#define BSP_INT_SRC_S0_VI_VSYNC0    ((IRQn_Type)(75))    /* Video display controller 5         */
#define BSP_INT_SRC_S0_LO_VSYNC0    ((IRQn_Type)(76))    /*                                    */
#define BSP_INT_SRC_S0_VSYNCERR0    ((IRQn_Type)(77))    /*                                    */
#define BSP_INT_SRC_GR3_VLINE0      ((IRQn_Type)(78))    /*                                    */
#define BSP_INT_SRC_S0_VFIELD0      ((IRQn_Type)(79))    /*                                    */
#define BSP_INT_SRC_IV1_VBUFERR0    ((IRQn_Type)(80))    /*                                    */
#define BSP_INT_SRC_IV3_VBUFERR0    ((IRQn_Type)(81))    /*                                    */
#define BSP_INT_SRC_IV5_VBUFERR0    ((IRQn_Type)(82))    /*                                    */
#define BSP_INT_SRC_IV6_VBUFERR0    ((IRQn_Type)(83))    /*                                    */
#define BSP_INT_SRC_S0_WLINE0       ((IRQn_Type)(84))    /*                                    */
#define BSP_INT_SRC_S1_VI_VSYNC0    ((IRQn_Type)(85))    /*                                    */
#define BSP_INT_SRC_S1_LO_VSYNC0    ((IRQn_Type)(86))    /*                                    */
#define BSP_INT_SRC_S1_VSYNCERR0    ((IRQn_Type)(87))    /*                                    */
#define BSP_INT_SRC_S1_VFIELD0      ((IRQn_Type)(88))    /*                                    */
#define BSP_INT_SRC_IV2_VBUFERR0    ((IRQn_Type)(89))    /*                                    */
#define BSP_INT_SRC_IV4_VBUFERR0    ((IRQn_Type)(90))    /*                                    */
#define BSP_INT_SRC_S1_WLINE0       ((IRQn_Type)(91))    /*                                    */
#define BSP_INT_SRC_OIR_VI_VSYNC0   ((IRQn_Type)(92))    /*                                    */
#define BSP_INT_SRC_OIR_LO_VSYNC0   ((IRQn_Type)(93))    /*                                    */
#define BSP_INT_SRC_OIR_VSYNCERR0   ((IRQn_Type)(94))    /*                                    */
#define BSP_INT_SRC_OIR_VFIELD0     ((IRQn_Type)(95))    /*                                    */
#define BSP_INT_SRC_IV7_VBUFERR0    ((IRQn_Type)(96))    /*                                    */
#define BSP_INT_SRC_IV8_VBUFERR0    ((IRQn_Type)(97))    /*                                    */
#define BSP_INT_SRC_OIR_WLINE0      ((IRQn_Type)(98))    /*                                    */
#define BSP_INT_SRC_S0_VI_VSYNC1    ((IRQn_Type)(99))    /*                                    */
#define BSP_INT_SRC_S0_LO_VSYNC1    ((IRQn_Type)(100))   /*                                    */
#define BSP_INT_SRC_S0_VSYNCERR1    ((IRQn_Type)(101))   /*                                    */
#define BSP_INT_SRC_GR3_VLINE1      ((IRQn_Type)(102))   /*                                    */
#define BSP_INT_SRC_S0_VFIELD1      ((IRQn_Type)(103))   /*                                    */
#define BSP_INT_SRC_IV1_VBUFERR1    ((IRQn_Type)(104))   /*                                    */
#define BSP_INT_SRC_IV3_VBUFERR1    ((IRQn_Type)(105))   /*                                    */
#define BSP_INT_SRC_IV5_VBUFERR1    ((IRQn_Type)(106))   /*                                    */
#define BSP_INT_SRC_IV6_VBUFERR1    ((IRQn_Type)(107))   /*                                    */
#define BSP_INT_SRC_S0_WLINE1       ((IRQn_Type)(108))   /*                                    */
#define BSP_INT_SRC_S1_VI_VSYNC1    ((IRQn_Type)(109))   /*                                    */
#define BSP_INT_SRC_S1_LO_VSYNC1    ((IRQn_Type)(110))   /*                                    */
#define BSP_INT_SRC_S1_VSYNCERR1    ((IRQn_Type)(111))   /*                                    */
#define BSP_INT_SRC_S1_VFIELD1      ((IRQn_Type)(112))   /*                                    */
#define BSP_INT_SRC_IV2_VBUFERR1    ((IRQn_Type)(113))   /*                                    */
#define BSP_INT_SRC_IV4_VBUFERR1    ((IRQn_Type)(114))   /*                                    */
#define BSP_INT_SRC_S1_WLINE1       ((IRQn_Type)(115))   /*                                    */
#define BSP_INT_SRC_OIR_VI_VSYNC1   ((IRQn_Type)(116))   /*                                    */
#define BSP_INT_SRC_OIR_LO_VSYNC1   ((IRQn_Type)(117))   /*                                    */
#define BSP_INT_SRC_OIR_VLINE1      ((IRQn_Type)(118))   /*                                    */
#define BSP_INT_SRC_OIR_VFIELD1     ((IRQn_Type)(119))   /*                                    */
#define BSP_INT_SRC_IV7_VBUFERR1    ((IRQn_Type)(120))   /*                                    */
#define BSP_INT_SRC_IV8_VBUFERR1    ((IRQn_Type)(121))   /*                                    */
#define BSP_INT_SRC_OIR_WLINE1      ((IRQn_Type)(122))   /*                                    */
#define BSP_INT_SRC_IMRDI           ((IRQn_Type)(123))   /* Image renderer                     */
#define BSP_INT_SRC_IMR2I0          ((IRQn_Type)(124))   /*                                    */
#define BSP_INT_SRC_IMR2I1          ((IRQn_Type)(125))   /*                                    */
#define BSP_INT_SRC_JEDI            ((IRQn_Type)(126))   /* JPEG Codec unit                    */
#define BSP_INT_SRC_JDTI            ((IRQn_Type)(127))   /*                                    */
#define BSP_INT_SRC_CMP0            ((IRQn_Type)(128))   /* Display out comparison unit        */
#define BSP_INT_SRC_CMP1            ((IRQn_Type)(129))   /*                                    */
#define BSP_INT_SRC_INT0            ((IRQn_Type)(130))   /* OpenVG-Compliant Renesas graphics processor */
#define BSP_INT_SRC_INT1            ((IRQn_Type)(131))   /*                                    */
#define BSP_INT_SRC_INT2            ((IRQn_Type)(132))   /*                                    */
#define BSP_INT_SRC_INT3            ((IRQn_Type)(133))   /*                                    */
#define BSP_INT_SRC_OSTM0TINT       ((IRQn_Type)(134))   /* OS timer                           */
#define BSP_INT_SRC_OSTM1TINT       ((IRQn_Type)(135))   /*                                    */
#define BSP_INT_SRC_CMI             ((IRQn_Type)(136))   /* Bus state controller               */
#define BSP_INT_SRC_WTOUT           ((IRQn_Type)(137))   /*                                    */
#define BSP_INT_SRC_ITI             ((IRQn_Type)(138))   /* Watchdog timer                     */
#define BSP_INT_SRC_TGI0A           ((IRQn_Type)(139))   /* Multi-function timer pulse unit 2  */
#define BSP_INT_SRC_TGI0B           ((IRQn_Type)(140))   /*                                    */
#define BSP_INT_SRC_TGI0C           ((IRQn_Type)(141))   /*                                    */
#define BSP_INT_SRC_TGI0D           ((IRQn_Type)(142))   /*                                    */
#define BSP_INT_SRC_TGI0V           ((IRQn_Type)(143))   /*                                    */
#define BSP_INT_SRC_TGI0E           ((IRQn_Type)(144))   /*                                    */
#define BSP_INT_SRC_TGI0F           ((IRQn_Type)(145))   /*                                    */
#define BSP_INT_SRC_TGI1A           ((IRQn_Type)(146))   /*                                    */
#define BSP_INT_SRC_TGI1B           ((IRQn_Type)(147))   /*                                    */
#define BSP_INT_SRC_TGI1V           ((IRQn_Type)(148))   /*                                    */
#define BSP_INT_SRC_TGI1U           ((IRQn_Type)(149))   /*                                    */
#define BSP_INT_SRC_TGI2A           ((IRQn_Type)(150))   /*                                    */
#define BSP_INT_SRC_TGI2B           ((IRQn_Type)(151))   /*                                    */
#define BSP_INT_SRC_TGI2V           ((IRQn_Type)(152))   /*                                    */
#define BSP_INT_SRC_TGI2U           ((IRQn_Type)(153))   /*                                    */
#define BSP_INT_SRC_TGI3A           ((IRQn_Type)(154))   /*                                    */
#define BSP_INT_SRC_TGI3B           ((IRQn_Type)(155))   /*                                    */
#define BSP_INT_SRC_TGI3C           ((IRQn_Type)(156))   /*                                    */
#define BSP_INT_SRC_TGI3D           ((IRQn_Type)(157))   /*                                    */
#define BSP_INT_SRC_TGI3V           ((IRQn_Type)(158))   /*                                    */
#define BSP_INT_SRC_TGI4A           ((IRQn_Type)(159))   /*                                    */
#define BSP_INT_SRC_TGI4B           ((IRQn_Type)(160))   /*                                    */
#define BSP_INT_SRC_TGI4C           ((IRQn_Type)(161))   /*                                    */
#define BSP_INT_SRC_TGI4D           ((IRQn_Type)(162))   /*                                    */
#define BSP_INT_SRC_TGI4V           ((IRQn_Type)(163))   /*                                    */
#define BSP_INT_SRC_CMI1            ((IRQn_Type)(164))   /* Motor control PWM timer            */
#define BSP_INT_SRC_CMI2            ((IRQn_Type)(165))   /*                                    */
#define BSP_INT_SRC_SGDEI0          ((IRQn_Type)(166))   /* Sound generator                    */
#define BSP_INT_SRC_SGDEI1          ((IRQn_Type)(167))   /*                                    */
#define BSP_INT_SRC_SGDEI2          ((IRQn_Type)(168))   /*                                    */
#define BSP_INT_SRC_SGDEI3          ((IRQn_Type)(169))   /*                                    */
#define BSP_INT_SRC_ADI             ((IRQn_Type)(170))   /* 12bit A/D converter                */
#define BSP_INT_SRC_LMTI            ((IRQn_Type)(171))   /*                                    */
#define BSP_INT_SRC_SSII0           ((IRQn_Type)(172))   /* Serial sound interface             */
#define BSP_INT_SRC_SSIRXI0         ((IRQn_Type)(173))   /*                                    */
#define BSP_INT_SRC_SSITXI0         ((IRQn_Type)(174))   /*                                    */
#define BSP_INT_SRC_SSII1           ((IRQn_Type)(175))   /*                                    */
#define BSP_INT_SRC_SSIRXI1         ((IRQn_Type)(176))   /*                                    */
#define BSP_INT_SRC_SSITXI1         ((IRQn_Type)(177))   /*                                    */
#define BSP_INT_SRC_SSII2           ((IRQn_Type)(178))   /*                                    */
#define BSP_INT_SRC_SSIRTI2         ((IRQn_Type)(179))   /*                                    */
#define BSP_INT_SRC_SSII3           ((IRQn_Type)(180))   /*                                    */
#define BSP_INT_SRC_SSIRXI3         ((IRQn_Type)(181))   /*                                    */
#define BSP_INT_SRC_SSITXI3         ((IRQn_Type)(182))   /*                                    */
#define BSP_INT_SRC_SSII4           ((IRQn_Type)(183))   /*                                    */
#define BSP_INT_SRC_SSIRTI4         ((IRQn_Type)(184))   /*                                    */
#define BSP_INT_SRC_SSII5           ((IRQn_Type)(185))   /*                                    */
#define BSP_INT_SRC_SSIRXI5         ((IRQn_Type)(186))   /*                                    */
#define BSP_INT_SRC_SSITXI5         ((IRQn_Type)(187))   /*                                    */
#define BSP_INT_SRC_SPDIFI          ((IRQn_Type)(188))   /* Renesas SPDIF interface            */
#define BSP_INT_SRC_INTIICTEI0      ((IRQn_Type)(189))   /* I2C interface                      */
#define BSP_INT_SRC_INTIICRI0       ((IRQn_Type)(190))   /*                                    */
#define BSP_INT_SRC_INTIICTI0       ((IRQn_Type)(191))   /*                                    */
#define BSP_INT_SRC_INTIICSPI0      ((IRQn_Type)(192))   /*                                    */
#define BSP_INT_SRC_INTIICSTI0      ((IRQn_Type)(193))   /*                                    */
#define BSP_INT_SRC_INTIICNAKI0     ((IRQn_Type)(194))   /*                                    */
#define BSP_INT_SRC_INTIICALI0      ((IRQn_Type)(195))   /*                                    */
#define BSP_INT_SRC_INTIICTMOI0     ((IRQn_Type)(196))   /*                                    */
#define BSP_INT_SRC_INTIICTEI1      ((IRQn_Type)(197))   /*                                    */
#define BSP_INT_SRC_INTIICRI1       ((IRQn_Type)(198))   /*                                    */
#define BSP_INT_SRC_INTIICTI1       ((IRQn_Type)(199))   /*                                    */
#define BSP_INT_SRC_INTIICSPI1      ((IRQn_Type)(200))   /*                                    */
#define BSP_INT_SRC_INTIICSTI1      ((IRQn_Type)(201))   /*                                    */
#define BSP_INT_SRC_INTIICNAKI1     ((IRQn_Type)(202))   /*                                    */
#define BSP_INT_SRC_INTIICALI1      ((IRQn_Type)(203))   /*                                    */
#define BSP_INT_SRC_INTIICTMOI1     ((IRQn_Type)(204))   /*                                    */
#define BSP_INT_SRC_INTIICTEI2      ((IRQn_Type)(205))   /*                                    */
#define BSP_INT_SRC_INTIICRI2       ((IRQn_Type)(206))   /*                                    */
#define BSP_INT_SRC_INTIICTI2       ((IRQn_Type)(207))   /*                                    */
#define BSP_INT_SRC_INTIICSPI2      ((IRQn_Type)(208))   /*                                    */
#define BSP_INT_SRC_INTIICSTI2      ((IRQn_Type)(209))   /*                                    */
#define BSP_INT_SRC_INTIICNAKI2     ((IRQn_Type)(210))   /*                                    */
#define BSP_INT_SRC_INTIICALI2      ((IRQn_Type)(211))   /*                                    */
#define BSP_INT_SRC_INTIICTMOI2     ((IRQn_Type)(212))   /*                                    */
#define BSP_INT_SRC_INTIICTEI3      ((IRQn_Type)(213))   /*                                    */
#define BSP_INT_SRC_INTIICRI3       ((IRQn_Type)(214))   /*                                    */
#define BSP_INT_SRC_INTIICTI3       ((IRQn_Type)(215))   /*                                    */
#define BSP_INT_SRC_INTIICSPI3      ((IRQn_Type)(216))   /*                                    */
#define BSP_INT_SRC_INTIICSTI3      ((IRQn_Type)(217))   /*                                    */
#define BSP_INT_SRC_INTIICNAKI3     ((IRQn_Type)(218))   /*                                    */
#define BSP_INT_SRC_INTIICALI3      ((IRQn_Type)(219))   /*                                    */
#define BSP_INT_SRC_INTIICTMOI3     ((IRQn_Type)(220))   /*                                    */
#define BSP_INT_SRC_BRI0            ((IRQn_Type)(221))   /* Serial Communication Interface with FIFO */
#define BSP_INT_SRC_ERI0            ((IRQn_Type)(222))   /*                                    */
#define BSP_INT_SRC_RXI0            ((IRQn_Type)(223))   /*                                    */
#define BSP_INT_SRC_TXI0            ((IRQn_Type)(224))   /*                                    */
#define BSP_INT_SRC_BRI1            ((IRQn_Type)(225))   /*                                    */
#define BSP_INT_SRC_ERI1            ((IRQn_Type)(226))   /*                                    */
#define BSP_INT_SRC_RXI1            ((IRQn_Type)(227))   /*                                    */
#define BSP_INT_SRC_TXI1            ((IRQn_Type)(228))   /*                                    */
#define BSP_INT_SRC_BRI2            ((IRQn_Type)(229))   /*                                    */
#define BSP_INT_SRC_ERI2            ((IRQn_Type)(230))   /*                                    */
#define BSP_INT_SRC_RXI2            ((IRQn_Type)(231))   /*                                    */
#define BSP_INT_SRC_TXI2            ((IRQn_Type)(232))   /*                                    */
#define BSP_INT_SRC_BRI3            ((IRQn_Type)(233))   /*                                    */
#define BSP_INT_SRC_ERI3            ((IRQn_Type)(234))   /*                                    */
#define BSP_INT_SRC_RXI3            ((IRQn_Type)(235))   /*                                    */
#define BSP_INT_SRC_TXI3            ((IRQn_Type)(236))   /*                                    */
#define BSP_INT_SRC_BRI4            ((IRQn_Type)(237))   /*                                    */
#define BSP_INT_SRC_ERI4            ((IRQn_Type)(238))   /*                                    */
#define BSP_INT_SRC_RXI4            ((IRQn_Type)(239))   /*                                    */
#define BSP_INT_SRC_TXI4            ((IRQn_Type)(240))   /*                                    */
#define BSP_INT_SRC_BRI5            ((IRQn_Type)(241))   /*                                    */
#define BSP_INT_SRC_ERI5            ((IRQn_Type)(242))   /*                                    */
#define BSP_INT_SRC_RXI5            ((IRQn_Type)(243))   /*                                    */
#define BSP_INT_SRC_TXI5            ((IRQn_Type)(244))   /*                                    */
#define BSP_INT_SRC_BRI6            ((IRQn_Type)(245))   /*                                    */
#define BSP_INT_SRC_ERI6            ((IRQn_Type)(246))   /*                                    */
#define BSP_INT_SRC_RXI6            ((IRQn_Type)(247))   /*                                    */
#define BSP_INT_SRC_TXI6            ((IRQn_Type)(248))   /*                                    */
#define BSP_INT_SRC_BRI7            ((IRQn_Type)(249))   /*                                    */
#define BSP_INT_SRC_ERI7            ((IRQn_Type)(250))   /*                                    */
#define BSP_INT_SRC_RXI7            ((IRQn_Type)(251))   /*                                    */
#define BSP_INT_SRC_TXI7            ((IRQn_Type)(252))   /*                                    */
#define BSP_INT_SRC_INTRCANGERR     ((IRQn_Type)(253))   /* CAN interface                      */
#define BSP_INT_SRC_INTRCANGRECC    ((IRQn_Type)(254))   /*                                    */
#define BSP_INT_SRC_INTRCAN0REC     ((IRQn_Type)(255))   /*                                    */
#define BSP_INT_SRC_INTRCAN0ERR     ((IRQn_Type)(256))   /*                                    */
#define BSP_INT_SRC_INTRCAN0TRX     ((IRQn_Type)(257))   /*                                    */
#define BSP_INT_SRC_INTRCAN1REC     ((IRQn_Type)(258))   /*                                    */
#define BSP_INT_SRC_INTRCAN1ERR     ((IRQn_Type)(259))   /*                                    */
#define BSP_INT_SRC_INTRCAN1TRX     ((IRQn_Type)(260))   /*                                    */
#define BSP_INT_SRC_INTRCAN2REC     ((IRQn_Type)(261))   /*                                    */
#define BSP_INT_SRC_INTRCAN2ERR     ((IRQn_Type)(262))   /*                                    */
#define BSP_INT_SRC_INTRCAN2TRX     ((IRQn_Type)(263))   /*                                    */
#define BSP_INT_SRC_INTRCAN3REC     ((IRQn_Type)(264))   /*                                    */
#define BSP_INT_SRC_INTRCAN3ERR     ((IRQn_Type)(265))   /*                                    */
#define BSP_INT_SRC_INTRCAN3TRX     ((IRQn_Type)(266))   /*                                    */
#define BSP_INT_SRC_INTRCAN4REC     ((IRQn_Type)(267))   /*                                    */
#define BSP_INT_SRC_INTRCAN4ERR     ((IRQn_Type)(268))   /*                                    */
#define BSP_INT_SRC_INTRCAN4TRX     ((IRQn_Type)(269))   /*                                    */
#define BSP_INT_SRC_SPEI0           ((IRQn_Type)(270))   /* Renesas serial peripheral interface */
#define BSP_INT_SRC_SPRI0           ((IRQn_Type)(271))   /*                                    */
#define BSP_INT_SRC_SPTI0           ((IRQn_Type)(272))   /*                                    */
#define BSP_INT_SRC_SPEI1           ((IRQn_Type)(273))   /*                                    */
#define BSP_INT_SRC_SPRI1           ((IRQn_Type)(274))   /*                                    */
#define BSP_INT_SRC_SPTI1           ((IRQn_Type)(275))   /*                                    */
#define BSP_INT_SRC_SPEI2           ((IRQn_Type)(276))   /*                                    */
#define BSP_INT_SRC_SPRI2           ((IRQn_Type)(277))   /*                                    */
#define BSP_INT_SRC_SPTI2           ((IRQn_Type)(278))   /*                                    */
#define BSP_INT_SRC_SPEI3           ((IRQn_Type)(279))   /*                                    */
#define BSP_INT_SRC_SPRI3           ((IRQn_Type)(280))   /*                                    */
#define BSP_INT_SRC_SPTI3           ((IRQn_Type)(281))   /*                                    */
#define BSP_INT_SRC_SPEI4           ((IRQn_Type)(282))   /*                                    */
#define BSP_INT_SRC_SPRI4           ((IRQn_Type)(283))   /*                                    */
#define BSP_INT_SRC_SPTI4           ((IRQn_Type)(284))   /*                                    */
#define BSP_INT_SRC_IEBBTD          ((IRQn_Type)(285))   /* IEBusTM controller                 */
#define BSP_INT_SRC_IEBBTERR        ((IRQn_Type)(286))   /*                                    */
#define BSP_INT_SRC_IEBBTSTA        ((IRQn_Type)(287))   /*                                    */
#define BSP_INT_SRC_IEBBTV          ((IRQn_Type)(288))   /*                                    */
#define BSP_INT_SRC_ISY             ((IRQn_Type)(289))   /* CD-ROM decoder                     */
#define BSP_INT_SRC_IERR            ((IRQn_Type)(290))   /*                                    */
#define BSP_INT_SRC_ITARG           ((IRQn_Type)(291))   /*                                    */
#define BSP_INT_SRC_ISEC            ((IRQn_Type)(292))   /*                                    */
#define BSP_INT_SRC_IBUF            ((IRQn_Type)(293))   /*                                    */
#define BSP_INT_SRC_IREADY          ((IRQn_Type)(294))   /*                                    */
#define BSP_INT_SRC_FLSTE           ((IRQn_Type)(295))   /* NAND Flash memory controller       */
#define BSP_INT_SRC_FLTENDI         ((IRQn_Type)(296))   /*                                    */
#define BSP_INT_SRC_FLTREQ0I        ((IRQn_Type)(297))   /*                                    */
#define BSP_INT_SRC_FLTREQ1I        ((IRQn_Type)(298))   /*                                    */
#define BSP_INT_SRC_MMC0            ((IRQn_Type)(299))   /* MMC Host interface                 */
#define BSP_INT_SRC_MMC1            ((IRQn_Type)(300))   /*                                    */
#define BSP_INT_SRC_MMC2            ((IRQn_Type)(301))   /*                                    */
#define BSP_INT_SRC_SDHI0_3         ((IRQn_Type)(302))   /* SD Host interface                  */
#define BSP_INT_SRC_SDHI0_0         ((IRQn_Type)(303))   /*                                    */
#define BSP_INT_SRC_SDHI0_1         ((IRQn_Type)(304))   /*                                    */
#define BSP_INT_SRC_SDHI1_3         ((IRQn_Type)(305))   /*                                    */
#define BSP_INT_SRC_SDHI1_0         ((IRQn_Type)(306))   /*                                    */
#define BSP_INT_SRC_SDHI1_1         ((IRQn_Type)(307))   /*                                    */
#define BSP_INT_SRC_ARM             ((IRQn_Type)(308))   /* Real time clock                    */
#define BSP_INT_SRC_PRD             ((IRQn_Type)(309))   /*                                    */
#define BSP_INT_SRC_CUP             ((IRQn_Type)(310))   /*                                    */
#define BSP_INT_SRC_SCUAI0          ((IRQn_Type)(311))   /* SCUX                               */
#define BSP_INT_SRC_SCUAI1          ((IRQn_Type)(312))   /*                                    */
#define BSP_INT_SRC_SCUFDI0         ((IRQn_Type)(313))   /*                                    */
#define BSP_INT_SRC_SCUFDI1         ((IRQn_Type)(314))   /*                                    */
#define BSP_INT_SRC_SCUFDI2         ((IRQn_Type)(315))   /*                                    */
#define BSP_INT_SRC_SCUFDI3         ((IRQn_Type)(316))   /*                                    */
#define BSP_INT_SRC_SCUFUI0         ((IRQn_Type)(317))   /*                                    */
#define BSP_INT_SRC_SCUFUI1         ((IRQn_Type)(318))   /*                                    */
#define BSP_INT_SRC_SCUFUI2         ((IRQn_Type)(319))   /*                                    */
#define BSP_INT_SRC_SCUFUI3         ((IRQn_Type)(320))   /*                                    */
#define BSP_INT_SRC_SCUDVI0         ((IRQn_Type)(321))   /*                                    */
#define BSP_INT_SRC_SCUDVI1         ((IRQn_Type)(322))   /*                                    */
#define BSP_INT_SRC_SCUDVI2         ((IRQn_Type)(323))   /*                                    */
#define BSP_INT_SRC_SCUDVI3         ((IRQn_Type)(324))   /*                                    */
#define BSP_INT_SRC_MLB_CINT        ((IRQn_Type)(325))   /* Media local bus                    */
#define BSP_INT_SRC_MLB_SINT        ((IRQn_Type)(326))   /*                                    */
#define BSP_INT_SRC_DRC0            ((IRQn_Type)(327))   /* Dynamic range compalator           */
#define BSP_INT_SRC_DRC1            ((IRQn_Type)(328))   /*                                    */
#define BSP_INT_SRC_LINI0_INT_T     ((IRQn_Type)(331))   /* LIN/UART interface                 */
#define BSP_INT_SRC_LINI0_INT_R     ((IRQn_Type)(332))   /*                                    */
#define BSP_INT_SRC_LINI0_INT_S     ((IRQn_Type)(333))   /*                                    */
#define BSP_INT_SRC_LINI0_INT_M     ((IRQn_Type)(334))   /*                                    */
#define BSP_INT_SRC_LINI1_INT_T     ((IRQn_Type)(335))   /*                                    */
#define BSP_INT_SRC_LINI1_INT_R     ((IRQn_Type)(336))   /*                                    */
#define BSP_INT_SRC_LINI1_INT_S     ((IRQn_Type)(337))   /*                                    */
#define BSP_INT_SRC_LINI1_INT_M     ((IRQn_Type)(338))   /*                                    */
#define BSP_INT_SRC_SCI_ERI0        ((IRQn_Type)(347))   /* Serial communication interface     */
#define BSP_INT_SRC_SCI_RXI0        ((IRQn_Type)(348))   /*                                    */
#define BSP_INT_SRC_SCI_TXI0        ((IRQn_Type)(349))   /*                                    */
#define BSP_INT_SRC_SCI_TEI0        ((IRQn_Type)(350))   /*                                    */
#define BSP_INT_SRC_SCI_ERI1        ((IRQn_Type)(351))   /*                                    */
#define BSP_INT_SRC_SCI_RXI1        ((IRQn_Type)(352))   /*                                    */
#define BSP_INT_SRC_SCI_TXI1        ((IRQn_Type)(353))   /*                                    */
#define BSP_INT_SRC_SCI_TEI1        ((IRQn_Type)(354))   /*                                    */
#define BSP_INT_SRC_AVBI_DATA       ((IRQn_Type)(355))   /* EthernetAVB                        */
#define BSP_INT_SRC_AVBI_ERROR      ((IRQn_Type)(356))   /*                                    */
#define BSP_INT_SRC_AVBI_MANAGE     ((IRQn_Type)(357))   /*                                    */
#define BSP_INT_SRC_AVBI_MAC        ((IRQn_Type)(358))   /*                                    */
#define BSP_INT_SRC_ETHERI          ((IRQn_Type)(359))   /* Ethernet controller                */
#define BSP_INT_SRC_CEUI            ((IRQn_Type)(364))   /* Capture engine unit                */
#define BSP_INT_SRC_H2XMLB_ERRINT   ((IRQn_Type)(381))   /* Internal bus                       */
#define BSP_INT_SRC_H2XIC1_ERRINT   ((IRQn_Type)(382))   /*                                    */
#define BSP_INT_SRC_X2HPERI1_ERRINT ((IRQn_Type)(383))   /*                                    */
#define BSP_INT_SRC_X2HPERI2_ERRINT ((IRQn_Type)(384))   /*                                    */
#define BSP_INT_SRC_X2HPERI34_ERRINT ((IRQn_Type)(385))  /*                                    */
#define BSP_INT_SRC_X2HPERI5_ERRINT ((IRQn_Type)(386))   /*                                    */
#define BSP_INT_SRC_X2HPERI67_ERRINT ((IRQn_Type)(387))  /*                                    */
#define BSP_INT_SRC_X2HDBGR_ERRINT  ((IRQn_Type)(388))   /*                                    */
#define BSP_INT_SRC_X2HBSC_ERRINT   ((IRQn_Type)(389))   /*                                    */
#define BSP_INT_SRC_X2HSPI1_ERRINT  ((IRQn_Type)(390))   /*                                    */
#define BSP_INT_SRC_X2HSPI2_ERRINT  ((IRQn_Type)(391))   /*                                    */
#define BSP_INT_SRC_PRRI            ((IRQn_Type)(392))   /*                                    */
#define BSP_INT_SRC_IFEI0           ((IRQn_Type)(393))   /* Pixel format converter             */
#define BSP_INT_SRC_OFFI0           ((IRQn_Type)(394))   /*                                    */
#define BSP_INT_SRC_PFVEI0          ((IRQn_Type)(395))   /*                                    */
#define BSP_INT_SRC_IFEI1           ((IRQn_Type)(396))   /*                                    */
#define BSP_INT_SRC_OFFI1           ((IRQn_Type)(397))   /*                                    */
#define BSP_INT_SRC_PFVEI1          ((IRQn_Type)(398))   /*                                    */
#define BSP_INT_SRC_TINT0           ((IRQn_Type)(416))   /* Terminal interrupts                */
#define BSP_INT_SRC_TINT1           ((IRQn_Type)(417))   /*                                    */
#define BSP_INT_SRC_TINT2           ((IRQn_Type)(418))   /*                                    */
#define BSP_INT_SRC_TINT3           ((IRQn_Type)(419))   /*                                    */
#define BSP_INT_SRC_TINT4           ((IRQn_Type)(420))   /*                                    */
#define BSP_INT_SRC_TINT5           ((IRQn_Type)(421))   /*                                    */
#define BSP_INT_SRC_TINT6           ((IRQn_Type)(422))   /*                                    */
#define BSP_INT_SRC_TINT7           ((IRQn_Type)(423))   /*                                    */
#define BSP_INT_SRC_TINT8           ((IRQn_Type)(424))   /*                                    */
#define BSP_INT_SRC_TINT9           ((IRQn_Type)(425))   /*                                    */
#define BSP_INT_SRC_TINT10          ((IRQn_Type)(426))   /*                                    */
#define BSP_INT_SRC_TINT11          ((IRQn_Type)(427))   /*                                    */
#define BSP_INT_SRC_TINT12          ((IRQn_Type)(428))   /*                                    */
#define BSP_INT_SRC_TINT13          ((IRQn_Type)(429))   /*                                    */
#define BSP_INT_SRC_TINT14          ((IRQn_Type)(430))   /*                                    */
#define BSP_INT_SRC_TINT15          ((IRQn_Type)(431))   /*                                    */
#define BSP_INT_SRC_TINT16          ((IRQn_Type)(432))   /*                                    */
#define BSP_INT_SRC_TINT17          ((IRQn_Type)(433))   /*                                    */
#define BSP_INT_SRC_TINT18          ((IRQn_Type)(434))   /*                                    */
#define BSP_INT_SRC_TINT19          ((IRQn_Type)(435))   /*                                    */
#define BSP_INT_SRC_TINT20          ((IRQn_Type)(436))   /*                                    */
#define BSP_INT_SRC_TINT21          ((IRQn_Type)(437))   /*                                    */
#define BSP_INT_SRC_TINT22          ((IRQn_Type)(438))   /*                                    */
#define BSP_INT_SRC_TINT23          ((IRQn_Type)(439))   /*                                    */
#define BSP_INT_SRC_TINT24          ((IRQn_Type)(440))   /*                                    */
#define BSP_INT_SRC_TINT25          ((IRQn_Type)(441))   /*                                    */
#define BSP_INT_SRC_TINT26          ((IRQn_Type)(442))   /*                                    */
#define BSP_INT_SRC_TINT27          ((IRQn_Type)(443))   /*                                    */
#define BSP_INT_SRC_TINT28          ((IRQn_Type)(444))   /*                                    */
#define BSP_INT_SRC_TINT29          ((IRQn_Type)(445))   /*                                    */
#define BSP_INT_SRC_TINT30          ((IRQn_Type)(446))   /*                                    */
#define BSP_INT_SRC_TINT31          ((IRQn_Type)(447))   /*                                    */
#define BSP_INT_SRC_TINT32          ((IRQn_Type)(448))   /*                                    */
#define BSP_INT_SRC_TINT33          ((IRQn_Type)(449))   /*                                    */
#define BSP_INT_SRC_TINT34          ((IRQn_Type)(450))   /*                                    */
#define BSP_INT_SRC_TINT35          ((IRQn_Type)(451))   /*                                    */
#define BSP_INT_SRC_TINT36          ((IRQn_Type)(452))   /*                                    */
#define BSP_INT_SRC_TINT37          ((IRQn_Type)(453))   /*                                    */
#define BSP_INT_SRC_TINT38          ((IRQn_Type)(454))   /*                                    */
#define BSP_INT_SRC_TINT39          ((IRQn_Type)(455))   /*                                    */
#define BSP_INT_SRC_TINT40          ((IRQn_Type)(456))   /*                                    */
#define BSP_INT_SRC_TINT41          ((IRQn_Type)(457))   /*                                    */
#define BSP_INT_SRC_TINT42          ((IRQn_Type)(458))   /*                                    */
#define BSP_INT_SRC_TINT43          ((IRQn_Type)(459))   /*                                    */
#define BSP_INT_SRC_TINT44          ((IRQn_Type)(460))   /*                                    */
#define BSP_INT_SRC_TINT45          ((IRQn_Type)(461))   /*                                    */
#define BSP_INT_SRC_TINT46          ((IRQn_Type)(462))   /*                                    */
#define BSP_INT_SRC_TINT47          ((IRQn_Type)(463))   /*                                    */
#define BSP_INT_SRC_TINT48          ((IRQn_Type)(464))   /*                                    */
#define BSP_INT_SRC_TINT49          ((IRQn_Type)(465))   /*                                    */
#define BSP_INT_SRC_TINT50          ((IRQn_Type)(466))   /*                                    */
#define BSP_INT_SRC_TINT51          ((IRQn_Type)(467))   /*                                    */
#define BSP_INT_SRC_TINT52          ((IRQn_Type)(468))   /*                                    */
#define BSP_INT_SRC_TINT53          ((IRQn_Type)(469))   /*                                    */
#define BSP_INT_SRC_TINT54          ((IRQn_Type)(470))   /*                                    */
#define BSP_INT_SRC_TINT55          ((IRQn_Type)(471))   /*                                    */
#define BSP_INT_SRC_TINT56          ((IRQn_Type)(472))   /*                                    */
#define BSP_INT_SRC_TINT57          ((IRQn_Type)(473))   /*                                    */
#define BSP_INT_SRC_TINT58          ((IRQn_Type)(474))   /*                                    */
#define BSP_INT_SRC_TINT59          ((IRQn_Type)(475))   /*                                    */
#define BSP_INT_SRC_TINT60          ((IRQn_Type)(476))   /*                                    */
#define BSP_INT_SRC_TINT61          ((IRQn_Type)(477))   /*                                    */
#define BSP_INT_SRC_TINT62          ((IRQn_Type)(478))   /*                                    */
#define BSP_INT_SRC_TINT63          ((IRQn_Type)(479))   /*                                    */
#define BSP_INT_SRC_TINT64          ((IRQn_Type)(480))   /*                                    */
#define BSP_INT_SRC_TINT65          ((IRQn_Type)(481))   /*                                    */
#define BSP_INT_SRC_TINT66          ((IRQn_Type)(482))   /*                                    */
#define BSP_INT_SRC_TINT67          ((IRQn_Type)(483))   /*                                    */
#define BSP_INT_SRC_TINT68          ((IRQn_Type)(484))   /*                                    */
#define BSP_INT_SRC_TINT69          ((IRQn_Type)(485))   /*                                    */
#define BSP_INT_SRC_TINT70          ((IRQn_Type)(486))   /*                                    */
#define BSP_INT_SRC_TINT71          ((IRQn_Type)(487))   /*                                    */
#define BSP_INT_SRC_TINT72          ((IRQn_Type)(488))   /*                                    */
#define BSP_INT_SRC_TINT73          ((IRQn_Type)(489))   /*                                    */
#define BSP_INT_SRC_TINT74          ((IRQn_Type)(490))   /*                                    */
#define BSP_INT_SRC_TINT75          ((IRQn_Type)(491))   /*                                    */
#define BSP_INT_SRC_TINT76          ((IRQn_Type)(492))   /*                                    */
#define BSP_INT_SRC_TINT77          ((IRQn_Type)(493))   /*                                    */
#define BSP_INT_SRC_TINT78          ((IRQn_Type)(494))   /*                                    */
#define BSP_INT_SRC_TINT79          ((IRQn_Type)(495))   /*                                    */
#define BSP_INT_SRC_TINT80          ((IRQn_Type)(496))   /*                                    */
#define BSP_INT_SRC_TINT81          ((IRQn_Type)(497))   /*                                    */
#define BSP_INT_SRC_TINT82          ((IRQn_Type)(498))   /*                                    */
#define BSP_INT_SRC_TINT83          ((IRQn_Type)(499))   /*                                    */
#define BSP_INT_SRC_TINT84          ((IRQn_Type)(500))   /*                                    */
#define BSP_INT_SRC_TINT85          ((IRQn_Type)(501))   /*                                    */
#define BSP_INT_SRC_TINT86          ((IRQn_Type)(502))   /*                                    */
#define BSP_INT_SRC_TINT87          ((IRQn_Type)(503))   /*                                    */
#define BSP_INT_SRC_TINT88          ((IRQn_Type)(504))   /*                                    */
#define BSP_INT_SRC_TINT89          ((IRQn_Type)(505))   /*                                    */
#define BSP_INT_SRC_TINT90          ((IRQn_Type)(506))   /*                                    */
#define BSP_INT_SRC_TINT91          ((IRQn_Type)(507))   /*                                    */
#define BSP_INT_SRC_TINT92          ((IRQn_Type)(508))   /*                                    */
#define BSP_INT_SRC_TINT93          ((IRQn_Type)(509))   /*                                    */
#define BSP_INT_SRC_TINT94          ((IRQn_Type)(510))   /*                                    */
#define BSP_INT_SRC_TINT95          ((IRQn_Type)(511))   /*                                    */
#define BSP_INT_SRC_TINT96          ((IRQn_Type)(512))   /*                                    */
#define BSP_INT_SRC_TINT97          ((IRQn_Type)(513))   /*                                    */
#define BSP_INT_SRC_TINT98          ((IRQn_Type)(514))   /*                                    */
#define BSP_INT_SRC_TINT99          ((IRQn_Type)(515))   /*                                    */
#define BSP_INT_SRC_TINT100         ((IRQn_Type)(516))   /*                                    */
#define BSP_INT_SRC_TINT101         ((IRQn_Type)(517))   /*                                    */
#define BSP_INT_SRC_TINT102         ((IRQn_Type)(518))   /*                                    */
#define BSP_INT_SRC_TINT103         ((IRQn_Type)(519))   /*                                    */
#define BSP_INT_SRC_TINT104         ((IRQn_Type)(520))   /*                                    */
#define BSP_INT_SRC_TINT105         ((IRQn_Type)(521))   /*                                    */
#define BSP_INT_SRC_TINT106         ((IRQn_Type)(522))   /*                                    */
#define BSP_INT_SRC_TINT107         ((IRQn_Type)(523))   /*                                    */
#define BSP_INT_SRC_TINT108         ((IRQn_Type)(524))   /*                                    */
#define BSP_INT_SRC_TINT109         ((IRQn_Type)(525))   /*                                    */
#define BSP_INT_SRC_TINT110         ((IRQn_Type)(526))   /*                                    */
#define BSP_INT_SRC_TINT111         ((IRQn_Type)(527))   /*                                    */
#define BSP_INT_SRC_TINT112         ((IRQn_Type)(528))   /*                                    */
#define BSP_INT_SRC_TINT113         ((IRQn_Type)(529))   /*                                    */
#define BSP_INT_SRC_TINT114         ((IRQn_Type)(530))   /*                                    */
#define BSP_INT_SRC_TINT115         ((IRQn_Type)(531))   /*                                    */
#define BSP_INT_SRC_TINT116         ((IRQn_Type)(532))   /*                                    */
#define BSP_INT_SRC_TINT117         ((IRQn_Type)(533))   /*                                    */
#define BSP_INT_SRC_TINT118         ((IRQn_Type)(534))   /*                                    */
#define BSP_INT_SRC_TINT119         ((IRQn_Type)(535))   /*                                    */
#define BSP_INT_SRC_TINT120         ((IRQn_Type)(536))   /*                                    */
#define BSP_INT_SRC_TINT121         ((IRQn_Type)(537))   /*                                    */
#define BSP_INT_SRC_TINT122         ((IRQn_Type)(538))   /*                                    */
#define BSP_INT_SRC_TINT123         ((IRQn_Type)(539))   /*                                    */
#define BSP_INT_SRC_TINT124         ((IRQn_Type)(540))   /*                                    */
#define BSP_INT_SRC_TINT125         ((IRQn_Type)(541))   /*                                    */
#define BSP_INT_SRC_TINT126         ((IRQn_Type)(542))   /*                                    */
#define BSP_INT_SRC_TINT127         ((IRQn_Type)(543))   /*                                    */
#define BSP_INT_SRC_TINT128         ((IRQn_Type)(544))   /*                                    */
#define BSP_INT_SRC_TINT129         ((IRQn_Type)(545))   /*                                    */
#define BSP_INT_SRC_TINT130         ((IRQn_Type)(546))   /*                                    */
#define BSP_INT_SRC_TINT131         ((IRQn_Type)(547))   /*                                    */
#define BSP_INT_SRC_TINT132         ((IRQn_Type)(548))   /*                                    */
#define BSP_INT_SRC_TINT133         ((IRQn_Type)(549))   /*                                    */
#define BSP_INT_SRC_TINT134         ((IRQn_Type)(550))   /*                                    */
#define BSP_INT_SRC_TINT135         ((IRQn_Type)(551))   /*                                    */
#define BSP_INT_SRC_TINT136         ((IRQn_Type)(552))   /*                                    */
#define BSP_INT_SRC_TINT137         ((IRQn_Type)(553))   /*                                    */
#define BSP_INT_SRC_TINT138         ((IRQn_Type)(554))   /*                                    */
#define BSP_INT_SRC_TINT139         ((IRQn_Type)(555))   /*                                    */
#define BSP_INT_SRC_TINT140         ((IRQn_Type)(556))   /*                                    */
#define BSP_INT_SRC_TINT141         ((IRQn_Type)(557))   /*                                    */
#define BSP_INT_SRC_TINT142         ((IRQn_Type)(558))   /*                                    */
#define BSP_INT_SRC_TINT143         ((IRQn_Type)(559))   /*                                    */
#define BSP_INT_SRC_TINT144         ((IRQn_Type)(560))   /*                                    */
#define BSP_INT_SRC_TINT145         ((IRQn_Type)(561))   /*                                    */
#define BSP_INT_SRC_TINT146         ((IRQn_Type)(562))   /*                                    */
#define BSP_INT_SRC_TINT147         ((IRQn_Type)(563))   /*                                    */
#define BSP_INT_SRC_TINT148         ((IRQn_Type)(564))   /*                                    */
#define BSP_INT_SRC_TINT149         ((IRQn_Type)(565))   /*                                    */
#define BSP_INT_SRC_TINT150         ((IRQn_Type)(566))   /*                                    */
#define BSP_INT_SRC_TINT151         ((IRQn_Type)(567))   /*                                    */
#define BSP_INT_SRC_TINT152         ((IRQn_Type)(568))   /*                                    */
#define BSP_INT_SRC_TINT153         ((IRQn_Type)(569))   /*                                    */
#define BSP_INT_SRC_TINT154         ((IRQn_Type)(570))   /*                                    */
#define BSP_INT_SRC_TINT155         ((IRQn_Type)(571))   /*                                    */
#define BSP_INT_SRC_TINT156         ((IRQn_Type)(572))   /*                                    */
#define BSP_INT_SRC_TINT157         ((IRQn_Type)(573))   /*                                    */
#define BSP_INT_SRC_TINT158         ((IRQn_Type)(574))   /*                                    */
#define BSP_INT_SRC_TINT159         ((IRQn_Type)(575))   /*                                    */
#define BSP_INT_SRC_TINT160         ((IRQn_Type)(576))   /*                                    */
#define BSP_INT_SRC_TINT161         ((IRQn_Type)(577))   /*                                    */
#define BSP_INT_SRC_TINT162         ((IRQn_Type)(578))   /*                                    */
#define BSP_INT_SRC_TINT163         ((IRQn_Type)(579))   /*                                    */
#define BSP_INT_SRC_TINT164         ((IRQn_Type)(580))   /*                                    */
#define BSP_INT_SRC_TINT165         ((IRQn_Type)(581))   /*                                    */
#define BSP_INT_SRC_TINT166         ((IRQn_Type)(582))   /*                                    */
#define BSP_INT_SRC_TINT167         ((IRQn_Type)(583))   /*                                    */
#define BSP_INT_SRC_TINT168         ((IRQn_Type)(584))   /*                                    */
#define BSP_INT_SRC_TINT169         ((IRQn_Type)(585))   /*                                    */
#define BSP_INT_SRC_TINT170         ((IRQn_Type)(586))   /*                                    */

#elif defined(TARGET_RZ_A2XX)

#define BSP_INT_SRC_JEDI            ((IRQn_Type)(84))    /* JPEG Codec unit                    */
#define BSP_INT_SRC_JDTI            ((IRQn_Type)(85))    /*                                    */

#endif

INLINE IRQn_Type  R_CAST_bsp_int_src_t_to_IRQn_Type( bsp_int_src_t const  Value )
{
    return  (IRQn_Type) Value;
}


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "mcu_interrupts.h" */


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* MCU_INTERRUPTS_TYPEDEF_H */

