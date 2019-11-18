/* Copyright (c) 2018 dkato
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
#if defined(TARGET_GR_MANGO)
      SDHSBlockDevice(P6_4, NC),
#elif defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_EVB_HF)
      SDHSBlockDevice(P5_4, P5_5),
#elif defined(TARGET_RZ_A2M_SBEV)
      SDHSBlockDevice(P5_4, NC),
#else
      SDHSBlockDevice(NC, NC),
#endif
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
