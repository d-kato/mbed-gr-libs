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

    bool connected(storage_type_t type = STORAGE_NON) {
        if (type == STORAGE_SD) {
            return sd.connected();
        }
        if (type == STORAGE_USB) {
            return usb.connected();
        }
        if (storage_type == STORAGE_SD) {
            return sd.connected();
        }
        if (storage_type == STORAGE_USB) {
            return usb.connected();
        }

        return false;
    }

    storage_type_t connect(storage_type_t type = STORAGE_NON) {
        if ((storage_type == STORAGE_SD) && (!sd.connected())) {
            fs.unmount();
            storage_type = STORAGE_NON;
        }
        if ((storage_type == STORAGE_USB) && (!usb.connected())) {
            fs.unmount();
            storage_type = STORAGE_NON;
        }
        if ((type == STORAGE_SD) && (storage_type != STORAGE_SD)) {
            if (sd.connect()) {
                if (storage_type != STORAGE_NON) {
                    fs.unmount();
                }
                fs.mount(&sd);
                storage_type = STORAGE_SD;
            }
        } else if ((type == STORAGE_USB) && (storage_type != STORAGE_USB)) {
            if (usb.connect()) {
                if (storage_type != STORAGE_NON) {
                    fs.unmount();
                }
                fs.mount(&usb);
                storage_type = STORAGE_USB;
            }
        } else if (storage_type == STORAGE_NON) {
            if (sd.connect()) {
                fs.mount(&sd);
                storage_type = STORAGE_SD;
            } else if (usb.connect()) {
                fs.mount(&usb);
                storage_type = STORAGE_USB;
            } else {
                // do nothing
            }
        } else {
            // do nothing
        }

        return storage_type;
    }

    storage_type_t wait_connect(storage_type_t type = STORAGE_NON) {
        while (connect(type) == STORAGE_NON) {
            Thread::wait(100);
        }
        return storage_type;
    }

    bool format(storage_type_t type = STORAGE_NON, int allocation_unit = 0) {
        if ((storage_type == STORAGE_SD) && (sd.connected())) {
            fs.unmount();
            fs.format(&sd, allocation_unit);
            fs.mount(&sd);
            return true;
        }
        if ((storage_type == STORAGE_USB) && (usb.connected())) {
            fs.unmount();
            fs.format(&usb, allocation_unit);
            fs.mount(&usb);
            return true;
        }
        return false;
    }

    FATFileSystem * get_fs() {
        return &fs;
    }

private:
    FATFileSystem fs;
    SDBlockDevice_GRBoard sd;
    USBHostMSD usb;
    storage_type_t storage_type;
};

#endif
