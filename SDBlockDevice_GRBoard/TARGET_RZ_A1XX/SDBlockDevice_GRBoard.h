/* Copyright (c) 2017 dkato
 * SPDX-License-Identifier: Apache-2.0
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
      SDBlockDevice(P8_5, P8_6, P8_3, P8_4, 15000000), _sd_cd(P7_8),
#elif defined(TARGET_GR_LYCHEE)
      SDBlockDevice(P5_6, P5_7, P5_4, P5_5, 15000000), _sd_cd(P3_8),
#endif
      _connect(false) {
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
