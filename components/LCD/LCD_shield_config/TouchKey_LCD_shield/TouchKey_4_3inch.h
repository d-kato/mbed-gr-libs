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
* @file          TouchKey_4_3inch.h
* @brief         TouchKey_4_3inch API
******************************************************************************/

#ifndef TOUCH_KEY_4_3INCH_H
#define TOUCH_KEY_4_3INCH_H

#include "TouchKey.h"

/**
 * The class to acquire touch coordinates. (GR-PEACH 4.3inch LCD Shield edition)
 */
class TouchKey_4_3inch : public TouchKey {

public:

    /** Create a TouchKey_4_3inch object
     * 
     * @param tprst tprst pin
     * @param tpint tpint pin
     * @param sda I2C data line pin
     * @param scl I2C clock line pin
     */
    TouchKey_4_3inch(PinName tprst, PinName tpint, PinName sda = I2C_SDA, PinName scl = I2C_SCL);
    virtual int GetMaxTouchNum(void);
    virtual int GetCoordinates(int touch_buff_num, touch_pos_t * p_touch);

private:
    typedef struct {
        uint8_t y_h: 3,
        reserved: 1,
        x_h: 3,
        valid: 1;
        uint8_t x_l;
        uint8_t y_l;
    } xyz_data_t;

    typedef struct {
        uint8_t fingers: 3,
        reserved: 5;
        uint8_t keys;
        xyz_data_t xyz_data[2];
    } stx_report_data_t;

    I2C         i2c;
};

#endif



