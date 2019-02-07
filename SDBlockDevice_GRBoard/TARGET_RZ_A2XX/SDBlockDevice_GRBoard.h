#ifndef MBED_SDFILESYSTEM_GRBOARD_H
#define MBED_SDFILESYSTEM_GRBOARD_H

#include "SDHSBlockDevice.h"

/**
 * A class to communicate a SD
 */
class SDBlockDevice_GRBoard : public SDHSBlockDevice {
public:

    /**
    * Constructor
    *
    */
    SDBlockDevice_GRBoard() :
      SDHSBlockDevice(P5_4, P5_5),
      _connect(false) {
    }

    /**
    * Check if a SD is connected
    *
    * @return true if a SD is connected
    */
    bool connected() {
        if (is_connect() == false) {
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
        _connect = is_connect();
        return _connect;
    }

private:
   bool        _connect;
};

#endif
