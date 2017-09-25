#ifndef MBED_SDFILESYSTEM_GRBOARD_H
#define MBED_SDFILESYSTEM_GRBOARD_H

#include "SDBlockDevice.h"

/**
 * A class to communicate a SD
 */
class SDBlockDevice_GRBoard : public SDBlockDevice {
public:

    /**
    * Constructor
    *
    */
    SDBlockDevice_GRBoard() :
#if defined(TARGET_RZ_A1H)
      SDBlockDevice(P8_5, P8_6, P8_3, P8_4), _sd_cd(P7_8),
#elif defined(TARGET_GR_LYCHEE)
      SDBlockDevice(P5_6, P5_7, P5_4, P5_5), _sd_cd(P3_8),
#endif
      _connect(false) {
        // Set SPI clock rate to 20MHz for data transfer
        _transfer_sck = 20000000;
    }

    /**
    * Check if a SD is connected
    *
    * @return true if a SD is connected
    */
    bool connected() {
        if (_sd_cd.read() != 0) {
            _connect = false;
        }
        return _connect;
    }

    /**
     * Try to connect to a SD
     *
     * @return true if connection was successful
     */
    bool connect() {
        if (_sd_cd.read() == 0) {
            _connect = true;
        } else {
            _connect = false;
        }
        return _connect;
    }


private:
    DigitalIn   _sd_cd;
    bool        _connect;
};

#endif
