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

#include "AlarmTimer.h"
#include "rza_io_regrw.h"

static Callback<void()> event;
static bool init_end = false;

#define RTC_IRQ_NUM    ARM_IRQn

static void int_alarm(void) {
    RTC.RSECAR &= ~0x80;
    RTC.RMINAR &= ~0x80;
    RTC.RHRAR  &= ~0x80;
    RTC.RDAYAR &= ~0x80;
    RTC.RMONAR &= ~0x80;
    RTC.RCR3   &= ~0x80;
    RTC.RCR1   &= ~0x01; // AF = 0
    if (event) {
        event.call();
    }
}

static void set_alarm_int(void) {
    if (init_end != false) {
        return;
    }
    GIC_DisableIRQ(RTC_IRQ_NUM);
    InterruptHandlerRegister(RTC_IRQ_NUM, &int_alarm);
    GIC_SetPriority(RTC_IRQ_NUM, 5);
    GIC_SetConfiguration(RTC_IRQ_NUM, 1);
    GIC_EnableIRQ(RTC_IRQ_NUM);
    init_end = true;
}

static uint8_t hex8_to_dec(uint8_t hex_val) {
    uint32_t calc_data;

    calc_data  = hex_val / 10 * 0x10;
    calc_data += hex_val % 10;

    if (calc_data > 0x99) {
        calc_data = 0;
    }

    return (uint8_t)calc_data;
}

static uint16_t hex16_to_dec(uint16_t hex_val) {
    uint32_t calc_data;

    calc_data  =   hex_val / 1000       * 0x1000;
    calc_data += ((hex_val / 100) % 10) * 0x100;
    calc_data += ((hex_val / 10)  % 10) * 0x10;
    calc_data +=   hex_val        % 10;

    if (calc_data > 0x9999) {
        calc_data = 0;
    }
    return (uint16_t)calc_data;
}

void AlarmTimer::attach(Callback<void()> func) {
    event = func;
}

bool AlarmTimer::set(time_t seconds) {
    if ((RTC.RCR2 & 0x01) == 0) {
        return false;
    }

    struct tm *t = localtime(&seconds);

    RTC.RCR1 &= ~0x08u;   // AIE = 0
    for (int i = 0; (i < 1000) && ((RTC.RCR1 & 0x08) != 0); i++) {
        ;
    }
    RTC.RCR1 &= ~0x01u;   // AF = 0

    set_alarm_int();

    RTC.RSECAR = 0x80 + hex8_to_dec(t->tm_sec);
    RTC.RMINAR = 0x80 + hex8_to_dec(t->tm_min);
    RTC.RHRAR  = 0x80 + hex8_to_dec(t->tm_hour);
    RTC.RDAYAR = 0x80 + hex8_to_dec(t->tm_mday);
    RTC.RMONAR = 0x80 + hex8_to_dec(t->tm_mon + 1);
    RTC.RYRAR  = hex16_to_dec(t->tm_year + 1900);
    RTC.RCR3   = 0x80;

    RTC.RCR1 &= ~0x01u;   // AF = 0
    RTC.RCR1 |= 0x08u;    // AIE = 1

    return true;
}

