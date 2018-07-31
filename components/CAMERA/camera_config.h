
#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

#include "mbed.h"
#include "DisplayBace.h"

class camera_config {

public:

    /** Initialise
     *
     * @return true = success, false = failure
     */
    virtual bool Initialise() = 0;

    virtual void SetExtInConfig(DisplayBase::video_ext_in_config_t * p_cfg) = 0;

};

#endif

