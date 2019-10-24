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

#include "DeepStandby.h"

#if defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_EVB_HF)
#define CANCEL_SRC_BIT_BUTTON0  (1 << 11) // PJ_1
#define CANCEL_SRC_BIT_BUTTON1  (1 << 8)  // NMI
#elif defined(TARGET_RZ_A2M_SBEV)
#define CANCEL_SRC_BIT_BUTTON0  (0)       // Not supported (PC_5)
#define CANCEL_SRC_BIT_BUTTON1  (1 << 8)  // NMI
#elif defined(TARGET_SEMB1402)
#define CANCEL_SRC_BIT_BUTTON0  (1 << 3)  // PE_1
#define CANCEL_SRC_BIT_BUTTON1  (0)       // Not supported (PE_0)
#else
#warning "This board is not supported."
#define CANCEL_SRC_BIT_BUTTON0  (0)       // Not supported
#define CANCEL_SRC_BIT_BUTTON1  (0)       // Not supported
#endif
#define CANCEL_SRC_BIT_RTC      (1 << 7)  // RTCAR1

void DeepStandby::SetDeepStandbySimple(cancel_src_simple_t *src)
{
    cancel_src_direct_t src_direct;

    if (src == NULL) {
        return;
    }

    src_direct.rramkp   = 0x0F;    // RRAMKP0, RRAMKP1, RRAMKP2, RRAMKP3
    src_direct.dsssr    = 0x0000;
    src_direct.dsesr    = 0x0000;
    src_direct.usbdsssr = 0x0000;

    if (src->button0) {
        src_direct.dsssr |= CANCEL_SRC_BIT_BUTTON0;
    }
    if (src->button1) {
        src_direct.dsssr |= CANCEL_SRC_BIT_BUTTON1;
    }
    if (src->rtc) {
        src_direct.dsssr |= CANCEL_SRC_BIT_RTC;
    }

    SetDeepStandbyDirect(&src_direct);
}

void DeepStandby::SetDeepStandbyDirect(cancel_src_direct_t *src)
{
    volatile uint32_t dummy_32;
    volatile uint16_t dummy_16;
    volatile uint8_t dummy_8;

    if (src == NULL) {
        return;
    }

    // 1. Set the standby_mode_en bit of the power control register in the PL310 to 1.
    pl310.REG15_POWER_CTRL.BIT.standby_mode_en = 1;
    dummy_32 = pl310.REG15_POWER_CTRL.LONG;

    // 2. Set the RRAMKP3 to RRAMKP0 bits in RRAMKP for the corresponding on-chip data-retention RAM area
    //    that must be retained. Transfer the programs to be retained to the specified areas of the on-chip
    //    data-retention RAM.
    PMG.RRAMKP.BYTE = src->rramkp;
    dummy_8 = PMG.RRAMKP.BYTE;

    // 3. Set the RAMBOOT and EBUSKEEPE bits in DSCTR to specify the activation method for returning from
    //    deep standby mode and to select whether the external memory control pin status is retained or not.

    // Activation Method : External memory
    PMG.DSCTR.BIT.RAMBOOT   = 0;
    PMG.DSCTR.BIT.EBUSKEEPE = 0;
    dummy_8 = PMG.DSCTR.BYTE;

    // 4. When canceling deep standby mode by an interrupt, set the corresponding bit in DSSSR to select
    //    the pin or source to cancel deep standby mode. In this case, specify the input signal detection
    //    mode for the selected pin with the corresponding bit in DSESR.
    PMG.DSSSR.WORD = src->dsssr;
    dummy_16 = PMG.DSSSR.WORD;

    PMG.DSESR.WORD = src->dsesr;
    dummy_16 = PMG.DSESR.WORD;

    PMG.USBDSSSR.BYTE = src->usbdsssr;
    dummy_8 = PMG.USBDSSSR.BYTE;
    PMG.DSCNT.BIT.CNTD = 0;
    dummy_16 = PMG.DSCNT.WORD;

    // 5. Execute read and write of an arbitrary but the same address for each page in the on-chip data-
    //    retention RAM area. When this is not executed, data last written may not be written to the on-chip
    //    data-retention RAM. If there is a write to the on-chip data-retention RAM after this time, execute
    //    this processing after the last write to the on-chip dataretention RAM.
    L1C_CleanInvalidateDCacheAll();  // Clean and invalidate the whole data cache.

    if ((src->rramkp & 0x01) != 0) {
        // On-Chip Data Retention RAM   Page 1
        dummy_32 = *((uint32_t *)0x80000000);  // Read
        *((uint32_t *)0x80000000) = dummy_32;  // Write
    }
    if ((src->rramkp & 0x02) != 0) {
        // On-Chip Data Retention RAM   Page 1
        dummy_32 = *((uint32_t *)0x80004000);  // Read
        *((uint32_t *)0x80004000) = dummy_32;  // Write
    }
    if ((src->rramkp & 0x04) != 0) {
        // On-Chip Data Retention RAM   Page 2
        dummy_32 = *((uint32_t *)0x80008000);  // Read
        *((uint32_t *)0x80008000) = dummy_32;  // Write
    }
    if ((src->rramkp & 0x08) != 0) {
        // On-Chip Data Retention RAM   Page 3
        dummy_32 = *((uint32_t *)0x80010000);  // Read
        *((uint32_t *)0x80010000) = dummy_32;  // Write
    }

    L1C_CleanDCacheAll();  // Clean the whole data cache.

    // 6. Set the STBY and DEEP bits in the STBCR1 register to 1, and then read this register.

    // deep standby mode
    CPG.STBCR1.BIT.DEEP = 1;
    CPG.STBCR1.BIT.STBY = 1;
    dummy_8 = CPG.STBCR1.BYTE;

    // 7. Clear the flag in the DSFR register.
    dummy_16 = PMG.DSFR.WORD;
    PMG.DSFR.WORD = 0;
    dummy_16 = PMG.DSFR.WORD;
    dummy_8 = PMG.USBDSFR.BYTE;
    PMG.USBDSFR.BYTE = 0;
    dummy_8 = PMG.USBDSFR.BYTE;

    // 8. Set the CPU interface control register (GICC_CTLR) of the interrupt controller to 0 so that the CPU
    //    is not notified of interrupts other than NMIs. Then, read the GICC_CTLR register.
    INTC.GICC_CTLR.LONG = 0;
    dummy_32 = INTC.GICC_CTLR.LONG;

    // Compiler warning measures
    (void)dummy_32;
    (void)dummy_16;
    (void)dummy_8;

    // Execute the WFI instruction
    while (1) {
        __WFI();
    }
}

bool DeepStandby::GetCancelSourceSimple(cancel_src_simple_t *src)
{
    volatile uint16_t dsfr = PMG.DSFR.WORD;

    if (dsfr == 0x0000) {
        return false;
    }

    if (src != NULL) {
        if (dsfr & CANCEL_SRC_BIT_BUTTON0) {
            src->button0 = true;
        } else {
            src->button0 = false;
        }
        if (dsfr & CANCEL_SRC_BIT_BUTTON1) {
            src->button1 = true;
        } else {
            src->button1 = false;
        }
        if (dsfr & CANCEL_SRC_BIT_RTC) {
            src->rtc = true;
        } else {
            src->rtc = false;
        }
    }

    return true;
}

bool DeepStandby::GetCancelSourceDirect(uint16_t *dsfr, uint8_t *usbdsfr)
{
    volatile uint16_t wk_dsfr = PMG.DSFR.WORD;
    volatile uint8_t wk_usbdsfr = PMG.USBDSFR.BYTE;

    if ((wk_dsfr == 0x0000) && (wk_usbdsfr == 0x00)) {
        return false;
    }

    if (dsfr != NULL) {
        *dsfr = wk_dsfr;
    }

    if (usbdsfr != NULL) {
        *usbdsfr = wk_usbdsfr;
    }

    return true;
}

