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
* @file          TouchKey.h
* @brief         TouchKey API
******************************************************************************/

#ifndef TOUCH_KEY_H
#define TOUCH_KEY_H

#include "mbed.h"

/**
 * The class to acquire touch coordinates
 */
class TouchKey {

public:
    /** Touch position structure */
    typedef struct {
        uint32_t x;      /**< The position of the x-coordinate. */
        uint32_t y;      /**< The position of the y-coordinate. */
        bool     valid;  /**< Whether a valid data.. */
    } touch_pos_t;


    /** Create a TouchKey object
     * 
     * @param tprst tprst pin
     * @param tpint tpint pin
     */
    TouchKey(PinName tprst, PinName tpint) : touch_int(tpint) {
        if (tprst != NC) {
            p_touch_reset = new DigitalOut(tprst);
        } else {
            p_touch_reset = NULL;
        }
    }

    ~TouchKey() {
        if (p_touch_reset != NULL) {
            delete p_touch_reset;
        }
    }

    /** Initialization of touch panel IC
     * 
     */
    void Reset(void) {
        if (p_touch_reset != NULL) {
            *p_touch_reset = 0;
            ThisThread::sleep_for(1);
            *p_touch_reset = 1;
        }
    }

    /** Attach a function to call when touch panel int
     *
     *  @param fptr A pointer to a void function, or 0 to set as none
     */
    void SetCallback(void (*fptr)(void)) {
        touch_int.fall(fptr);
    }

    /** Attach a member function to call when touch panel int
     *
     *  @param tptr pointer to the object to call the member function on
     *  @param mptr pointer to the member function to be called
     */
    template<typename T>
    void SetCallback(T* tptr, void (T::*mptr)(void)) {
        touch_int.fall(tptr, mptr);
    }

    /** Get the maximum number of simultaneous touches 
     * 
     * @return The maximum number of simultaneous touches.
     */
    virtual int GetMaxTouchNum(void) = 0;

   /** Get the coordinates
     *
     * @param touch_buff_num The number of structure p_touch.
     * @param p_touch Touch position information.
     * @return The number of touch points.
     */
    virtual int GetCoordinates(int touch_buff_num, touch_pos_t * p_touch) = 0;

private:
    DigitalOut * p_touch_reset;
    InterruptIn touch_int;
};

#endif
