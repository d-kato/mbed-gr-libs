/* mbed Microcontroller Library
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
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
/**************************************************************************//**
* @file          TouchKey_RSK_TFT.h
* @brief         TouchKey_RSK_TFT API
******************************************************************************/

#ifndef TOUCH_KEY_RSK_TFT_H
#define TOUCH_KEY_RSK_TFT_H

#include "TouchKey.h"

/**
 * The class to acquire touch coordinates. (GR-PEACH 7.1inch LCD Shield edition)
 */
class TouchKey_RSK_TFT : public TouchKey {

public:

    /** Create a TouchKey_RSK_TFT object
     * 
     * @param sda I2C data line pin
     * @param scl I2C clock line pin
     * @param tprst tprst pin
     * @param tpint tpint pin
     */
    TouchKey_RSK_TFT(PinName tprst, PinName tpint, PinName sda = I2C_SDA, PinName scl = I2C_SCL);

    virtual int GetMaxTouchNum(void);
    virtual int GetCoordinates(int touch_buff_num, touch_pos_t * p_touch);

private:
    I2C         i2c;
};


#endif
