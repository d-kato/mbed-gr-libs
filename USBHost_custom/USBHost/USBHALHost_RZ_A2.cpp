/* mbed USBHost Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(TARGET_RZ_A2XX)

#include "mbed.h"
#include "USBHALHost.h"
#include "dbg.h"
#include "pinmap.h"

/**** User Selection ****/
#if defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_EVB_HF)
#define USB_HOST_CH        1
#elif defined(TARGET_RZ_A2M_SBEV)
#define USB_HOST_CH        0
#endif

#if USB_HOST_CH == 0
#define USB_MX       USB01
#define USBX0        USB00
#define USBHIX_IRQn  USBHI0_IRQn
#else
#define USB_MX       USB11
#define USBX0        USB10
#define USBHIX_IRQn  USBHI1_IRQn
#endif

#define HCCA_SIZE sizeof(HCCA)
#define ED_SIZE sizeof(HCED)
#define TD_SIZE sizeof(HCTD)

#define TOTAL_SIZE (HCCA_SIZE + (MAX_ENDPOINT*ED_SIZE) + (MAX_TD*TD_SIZE))

static volatile uint8_t usb_buf[TOTAL_SIZE] __attribute((section("NC_BSS"),aligned(256)));  //256 bytes aligned!

USBHALHost * USBHALHost::instHost;

USBHALHost::USBHALHost() {
    instHost = this;
    memInit();
    memset((void*)usb_hcca, 0, HCCA_SIZE);
    for (int i = 0; i < MAX_ENDPOINT; i++) {
        edBufAlloc[i] = false;
    }
    for (int i = 0; i < MAX_TD; i++) {
        tdBufAlloc[i] = false;
    }
}

void USBHALHost::init() {
    volatile uint8_t dummy_8;

    GIC_DisableIRQ(USBHIX_IRQn);

#if USB_HOST_CH == 0
    pin_function(PC_6, 1); // VBUSEN0
    pin_function(PC_7, 1); // OVRCUR0
    CPG.STBCR6.BIT.MSTP61 = 0;
    dummy_8 = CPG.STBCR6.BYTE;
    CPG.STBREQ3.BYTE &= ~0x03;
    dummy_8 = CPG.STBREQ3.BYTE;
#else
    pin_function(PC_0, 1); // VBUSIN1
    pin_function(PC_5, 1); // VBUSEN1
    pin_function(P7_5, 5); // OVRCUR1
    CPG.STBCR6.BIT.MSTP60 = 0;
    dummy_8 = CPG.STBCR6.BYTE;
    CPG.STBREQ3.BYTE &= ~0x0C;
    dummy_8 = CPG.STBREQ3.BYTE;
#endif
    (void)dummy_8;

#if defined(TARGET_RZ_A2M_SBEV)
    USBX0.PHYCLK_CTRL.BIT.UCLKSEL = 0;       /* EXTAL */
#else
    USBX0.PHYCLK_CTRL.BIT.UCLKSEL = 1;       /* USB_X1 */
#endif

    USBX0.PHYIF_CTRL.LONG = 0x00000000;
    USBX0.COMMCTRL.BIT.OTG_PERI = 0;        /* 0 : Host, 1 : Peri */
    USB_MX.LPSTS.WORD   |= 0x4000u;
    USBX0.USBCTR.LONG = 0x00000000;
    wait_ms(1);
    USBX0.LINECTRL1.LONG = 0;

    USBX0.HCCONTROL.LONG       = 0; // HARDWARE RESET
    USBX0.HCCONTROLHEADED.LONG = 0; // Initialize Control list head to Zero
    USBX0.HCBULKHEADED.LONG    = 0; // Initialize Bulk list head to Zero

    wait_ms(1);
    USBX0.HCRHPORTSTATUS1.LONG = 0x00000100;

    // Wait 100 ms before apply reset
    wait_ms(100);

    // software reset
    USBX0.HCCOMMANDSTATUS.LONG = OR_CMD_STATUS_HCR;

    // Write Fm Interval and Largest Data Packet Counter
    USBX0.HCFMINTERVAL.LONG    = DEFAULT_FMINTERVAL;
    USBX0.HCPERIODICSTART.LONG = FI * 90 / 100;

    // Put HC in operational state
    USBX0.HCCONTROL.LONG  = (USBX0.HCCONTROL.LONG & (~OR_CONTROL_HCFS)) | OR_CONTROL_HC_OPER;
    // Set Global Power
    USBX0.HCRHSTATUS.LONG = OR_RH_STATUS_LPSC;

    USBX0.HCHCCA.LONG = (uint32_t)(usb_hcca);

    // Clear Interrrupt Status
    USBX0.HCINTERRUPTSTATUS.LONG |= USBX0.HCINTERRUPTSTATUS.LONG;

    USBX0.HCINTERRUPTENABLE.LONG  = OR_INTR_ENABLE_MIE | OR_INTR_ENABLE_WDH | OR_INTR_ENABLE_RHSC;

    // Enable the USB Interrupt
    InterruptHandlerRegister(USBHIX_IRQn, &_usbisr);
    GIC_SetConfiguration(USBHIX_IRQn, 1);
    GIC_SetPriority(USBHIX_IRQn, 0x80);

    USBX0.INT_ENABLE.BIT.USBH_INTAEN = 1;
    USBX0.REGEN_CG_CTRL.LONG = 0;

    USBX0.HCRHPORTSTATUS1.LONG = OR_RH_PORT_CSC;
    USBX0.HCRHPORTSTATUS1.LONG = OR_RH_PORT_PRSC;

    GIC_EnableIRQ(USBHIX_IRQn);

    // Check for any connected devices
    if (USBX0.HCRHPORTSTATUS1.LONG & OR_RH_PORT_CCS) {
        //Device connected
        wait_ms(150);
        USB_DBG("Device connected (%08x)\n\r", USBX0.HCRHPORTSTATUS1.LONG);
        deviceConnected(0, 1, USBX0.HCRHPORTSTATUS1.LONG & OR_RH_PORT_LSDA);
    }
}

uint32_t USBHALHost::controlHeadED() {
    return USBX0.HCCONTROLHEADED.LONG;
}

uint32_t USBHALHost::bulkHeadED() {
    return USBX0.HCBULKHEADED.LONG;
}

uint32_t USBHALHost::interruptHeadED() {
    return usb_hcca->IntTable[0];
}

void USBHALHost::updateBulkHeadED(uint32_t addr) {
    USBX0.HCBULKHEADED.LONG = addr;
}


void USBHALHost::updateControlHeadED(uint32_t addr) {
    USBX0.HCCONTROLHEADED.LONG = addr;
}

void USBHALHost::updateInterruptHeadED(uint32_t addr) {
    usb_hcca->IntTable[0] = addr;
}

void USBHALHost::enableList(ENDPOINT_TYPE type) {
    switch(type) {
        case CONTROL_ENDPOINT:
            USBX0.HCCOMMANDSTATUS.LONG = OR_CMD_STATUS_CLF;
            USBX0.HCCONTROL.LONG |= OR_CONTROL_CLE;
            break;
        case ISOCHRONOUS_ENDPOINT:
            break;
        case BULK_ENDPOINT:
            USBX0.HCCOMMANDSTATUS.LONG = OR_CMD_STATUS_BLF;
            USBX0.HCCONTROL.LONG |= OR_CONTROL_BLE;
            break;
        case INTERRUPT_ENDPOINT:
            USBX0.HCCONTROL.LONG |= OR_CONTROL_PLE;
            break;
    }
}

bool USBHALHost::disableList(ENDPOINT_TYPE type) {
    switch(type) {
        case CONTROL_ENDPOINT:
            if(USBX0.HCCONTROL.LONG & OR_CONTROL_CLE) {
                USBX0.HCCONTROL.LONG &= ~OR_CONTROL_CLE;
                return true;
            }
            return false;
        case ISOCHRONOUS_ENDPOINT:
            return false;
        case BULK_ENDPOINT:
            if(USBX0.HCCONTROL.LONG & OR_CONTROL_BLE){
                USBX0.HCCONTROL.LONG &= ~OR_CONTROL_BLE;
                return true;
            }
            return false;
        case INTERRUPT_ENDPOINT:
            if(USBX0.HCCONTROL.LONG & OR_CONTROL_PLE) {
                USBX0.HCCONTROL.LONG &= ~OR_CONTROL_PLE;
                return true;
            }
            return false;
    }
    return false;
}

void USBHALHost::memInit() {
    usb_hcca = (volatile HCCA *)usb_buf;
    usb_edBuf = usb_buf + HCCA_SIZE;
    usb_tdBuf = usb_buf + HCCA_SIZE + (MAX_ENDPOINT*ED_SIZE);
}

volatile uint8_t * USBHALHost::getED() {
    for (int i = 0; i < MAX_ENDPOINT; i++) {
        if ( !edBufAlloc[i] ) {
            edBufAlloc[i] = true;
            return (volatile uint8_t *)(usb_edBuf + i*ED_SIZE);
        }
    }
    perror("Could not allocate ED\r\n");
    return NULL; //Could not alloc ED
}

volatile uint8_t * USBHALHost::getTD() {
    int i;
    for (i = 0; i < MAX_TD; i++) {
        if ( !tdBufAlloc[i] ) {
            tdBufAlloc[i] = true;
            return (volatile uint8_t *)(usb_tdBuf + i*TD_SIZE);
        }
    }
    perror("Could not allocate TD\r\n");
    return NULL; //Could not alloc TD
}

void USBHALHost::freeED(volatile uint8_t * ed) {
    int i;
    i = (ed - usb_edBuf) / ED_SIZE;
    edBufAlloc[i] = false;
}

void USBHALHost::freeTD(volatile uint8_t * td) {
    int i;
    i = (td - usb_tdBuf) / TD_SIZE;
    tdBufAlloc[i] = false;
}

void USBHALHost::resetRootHub() {
    // Initiate port reset
    USBX0.HCRHPORTSTATUS1.LONG = OR_RH_PORT_PRS;

    while (USBX0.HCRHPORTSTATUS1.LONG & OR_RH_PORT_PRS);

    // ...and clear port reset signal
    USBX0.HCRHPORTSTATUS1.LONG = OR_RH_PORT_PRSC;
}

void USBHALHost::_usbisr(void) {
    if (instHost) {
        instHost->UsbIrqhandler();
    }
}

void USBHALHost::UsbIrqhandler() {
    if( USBX0.HCINTERRUPTSTATUS.LONG & USBX0.HCINTERRUPTENABLE.LONG ) //Is there something to actually process?
    {

        uint32_t int_status = USBX0.HCINTERRUPTSTATUS.LONG & USBX0.HCINTERRUPTENABLE.LONG;

        // Root hub status change interrupt
        if (int_status & OR_INTR_STATUS_RHSC) {
            if (USBX0.HCRHPORTSTATUS1.LONG & OR_RH_PORT_CSC) {
                if (USBX0.HCRHSTATUS.LONG & OR_RH_STATUS_DRWE) {
                    // When DRWE is on, Connect Status Change
                    // means a remote wakeup event.
                } else {

                    //Root device connected
                    if (USBX0.HCRHPORTSTATUS1.LONG & OR_RH_PORT_CCS) {
                        //Hub 0 (root hub), Port 1 (count starts at 1), Low or High speed
                        deviceConnected(0, 1, USBX0.HCRHPORTSTATUS1.LONG & OR_RH_PORT_LSDA);
                    }

                    //Root device disconnected
                    else {
                        deviceDisconnected(0, 1, NULL, usb_hcca->DoneHead & 0xFFFFFFFE);
                    }
                }
                USBX0.HCRHPORTSTATUS1.LONG = OR_RH_PORT_CSC;
            }
            if (USBX0.HCRHPORTSTATUS1.LONG & OR_RH_PORT_PRSC) {
                USBX0.HCRHPORTSTATUS1.LONG = OR_RH_PORT_PRSC;
            }
            USBX0.HCINTERRUPTSTATUS.LONG = OR_INTR_STATUS_RHSC;
        }

        // Writeback Done Head interrupt
        if (int_status & OR_INTR_STATUS_WDH) {
            transferCompleted(usb_hcca->DoneHead & 0xFFFFFFFE);
            USBX0.HCINTERRUPTSTATUS.LONG = OR_INTR_STATUS_WDH;
        }
    }
}
#endif
