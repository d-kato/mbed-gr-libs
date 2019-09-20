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

#define RTC_IRQ_NUM    ARM_S_IRQn

static void int_alarm(void) {
    RTC_BCNT1.BCNT0AER.BYTE = 0x00;
    RTC_BCNT1.BCNT1AER.BYTE = 0x00;
    RTC_BCNT1.BCNT2AER.WORD = 0x0000;
    RTC_BCNT1.BCNT3AER.BYTE = 0x00;
    RTC_BCNT1.RSR.BIT.AF    = 0;
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

void AlarmTimer::attach(Callback<void()> func) {
    event = func;
}

bool AlarmTimer::set(time_t seconds) {
    if (RTC_BCNT1.RCR2.BIT.START == 0) {
        return false;
    }

    RTC_BCNT1.RCR1.BIT.AIE = 0;
    for (int i = 0; (i < 1000) && (RTC_BCNT1.RCR1.BIT.AIE != 0); i++) {
        ;
    }
    RTC_BCNT1.RSR.BIT.AF = 0;

    set_alarm_int();

    RTC_BCNT1.BCNT0AR.BYTE = (uint8_t)(seconds >>  0);
    RTC_BCNT1.BCNT1AR.BYTE = (uint8_t)(seconds >>  8);
    RTC_BCNT1.BCNT2AR.BYTE = (uint8_t)(seconds >> 16);
    RTC_BCNT1.BCNT3AR.BYTE = (uint8_t)(seconds >> 24);

    RTC_BCNT1.BCNT0AER.BYTE = 0xFF;
    RTC_BCNT1.BCNT1AER.BYTE = 0xFF;
    RTC_BCNT1.BCNT2AER.WORD = 0x00FF;
    RTC_BCNT1.BCNT3AER.BYTE = 0xFF;

    wait_ms(4);

    RTC_BCNT1.RSR.BIT.AF = 0;
    RTC_BCNT1.RCR1.BIT.AIE = 1;

    return true;
}

