/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**************************************************************************//**
* @file          DeepStandby.h
* @brief         DeepStandby API
******************************************************************************/

#ifndef DEEP_STANDBY_H
#define DEEP_STANDBY_H

#include "mbed.h"

/** A class to communicate a DeepStandby
 *
 */
class DeepStandby {
public:

    typedef struct {
        bool button0;       /*!< Works with GR_LYCHEE, RZ_A2M_EVB and SEMB1402. */
        bool button1;       /*!< Works with GR_LYCHEE, RZ_A2M_EVB and RZ_A2M_SBEV. */
        bool rtc;           /*!< Works with RZ_A1, GR_LYCHEE, RZ_A2M_EVB, RZ_A2M_SBEV and SEMB1402. */
    } cancel_src_simple_t;

    /** Set deep standby mode
     *
     * @param src     Factors that cancel deep standby.
     */
    static void SetDeepStandbySimple(cancel_src_simple_t *src);

    /** Get deep standby cancel source
     *
     * @param src     Factor that canceled deep standby.
     * @return        true : Deep standby canceled.  false : Reset start.
     */
    static bool GetCancelSourceSimple(cancel_src_simple_t *src);



    typedef struct {
        uint8_t  rramkp;    /*!< Value of RRAMKP register */
        uint16_t dsssr;     /*!< Value of DSSSR register */
        uint16_t dsesr;     /*!< Value of DSESR register */
        uint16_t usbdsssr;  /*!< Value of USBDSSSR register (Used only with RZ/A2M) */
    } cancel_src_direct_t;

    /** Set deep standby mode
     *
     * @param src     Factors that cancel deep standby.
     */
    static void SetDeepStandbyDirect(cancel_src_direct_t *src);

    /** Get deep standby cancel source
     *
     * @param dsfr    Value of DSFR register
     * @param usbdsfr Value of USBDSFR register
     * @return        true : Deep standby canceled.  false : Reset start.
     */
    static bool GetCancelSourceDirect(uint16_t *dsfr, uint8_t *usbdsfr);

};
#endif
