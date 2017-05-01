/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef USBDEVICE_TYPES_H
#define USBDEVICE_TYPES_H

/* Standard requests */
#ifndef GET_STATUS
#define GET_STATUS        (0)
#endif
#ifndef CLEAR_FEATURE
#define CLEAR_FEATURE     (1)
#endif
#ifndef SET_FEATURE
#define SET_FEATURE       (3)
#endif
#ifndef SET_ADDRESS
#define SET_ADDRESS       (5)
#endif
#ifndef GET_DESCRIPTOR
#define GET_DESCRIPTOR    (6)
#endif
#ifndef SET_DESCRIPTOR
#define SET_DESCRIPTOR    (7)
#endif
#ifndef GET_CONFIGURATION
#define GET_CONFIGURATION (8)
#endif
#ifndef SET_CONFIGURATION
#define SET_CONFIGURATION (9)
#endif
#ifndef GET_INTERFACE
#define GET_INTERFACE     (10)
#endif
#ifndef SET_INTERFACE
#define SET_INTERFACE     (11)
#endif

/* bmRequestType.dataTransferDirection */
#define HOST_TO_DEVICE (0)
#define DEVICE_TO_HOST (1)

/* bmRequestType.Type*/
#define STANDARD_TYPE  (0)
#define CLASS_TYPE     (1)
#define VENDOR_TYPE    (2)
#define RESERVED_TYPE  (3)

/* bmRequestType.Recipient */
#define DEVICE_RECIPIENT    (0)
#define INTERFACE_RECIPIENT (1)
#define ENDPOINT_RECIPIENT  (2)
#define OTHER_RECIPIENT     (3)

/* Descriptors */
#define DESCRIPTOR_TYPE(wValue)  (wValue >> 8)
#define DESCRIPTOR_INDEX(wValue) (wValue & 0xff)

typedef struct {
    struct {
        uint8_t dataTransferDirection;
        uint8_t Type;
        uint8_t Recipient;
    } bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} SETUP_PACKET;

typedef struct {
    SETUP_PACKET setup;
    uint8_t *ptr;
    uint32_t remaining;
    uint8_t direction;
    bool zlp;
    bool notify;
} CONTROL_TRANSFER;

typedef enum {ATTACHED, POWERED, DEFAULT, ADDRESS, CONFIGURED} DEVICE_STATE;

typedef struct {
    volatile DEVICE_STATE state;
    uint8_t configuration;
    bool suspended;
} USB_DEVICE;

#endif
