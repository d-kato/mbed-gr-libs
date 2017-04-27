/**************************************************************************//**
* @file          SdUsbConnect.h
* @brief         
******************************************************************************/
#ifndef SD_USB_CONNECT_H
#define SD_USB_CONNECT_H

#include "mbed.h"
#include "FATFileSystem.h"
#include "SDBlockDevice_GRBoard.h"
#include "USBHostMSD.h"

/** A class to communicate a StorageConnect
 *
 */
class SdUsbConnect {
public:

    typedef enum {
        STORAGE_NON = 0,           /*!< NON         */
        STORAGE_SD,                /*!< SD          */
        STORAGE_USB                /*!< USB         */
    } storage_type_t;

    SdUsbConnect(const char *name) : fs(name), storage_type(STORAGE_NON) {}

    bool connected(void) {
        if (storage_type == STORAGE_SD) {
            return sd.connected();
        }
        if (storage_type == STORAGE_USB) {
            return usb.connected();
        }

        return false;
    }

    storage_type_t connect(void) {
        if ((storage_type == STORAGE_SD) && (!sd.connected())) {
            fs.unmount();
            storage_type = STORAGE_NON;
        }
        if ((storage_type == STORAGE_USB) && (!usb.connected())) {
            fs.unmount();
            storage_type = STORAGE_NON;
        }
        if (storage_type == STORAGE_NON) {
            if (sd.connect()) {
                fs.mount(&sd);
                storage_type = STORAGE_SD;
            } else if (usb.connect()) {
                fs.mount(&usb);
                storage_type = STORAGE_USB;
            } else {
                // do nothing
            }
        }

        return storage_type;
    }

    storage_type_t wait_connect(void) {
        while (connect() == STORAGE_NON) {
            Thread::wait(100);
        }
        return storage_type;
    }

private:
    FATFileSystem fs;
    SDBlockDevice_GRBoard sd;
    USBHostMSD usb;
    storage_type_t storage_type;
};

#endif
