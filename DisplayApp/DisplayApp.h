/* Copyright (c) 2016 dkato
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
/**************************************************************************//**
* @file          DisplayApp.h
* @brief         DisplayApp API
******************************************************************************/

#ifndef DISPLAY_APP_H
#define DISPLAY_APP_H

#include "mbed.h"
#include "rtos.h"
#include "USBSerial.h"

/** A class to communicate a DisplayApp
 *
 */
class DisplayApp {
public:
    /** Touch position structure */
    typedef struct {
        uint32_t x;      /**< The position of the x-coordinate. */
        uint32_t y;      /**< The position of the y-coordinate. */
        bool     valid;  /**< Whether a valid data.. */
    } touch_pos_t;

    /** Constructor: Initializes DisplayApp.
     *
     * @param   tsk_pri        Priority of the thread function. (default: osPriorityNormal).
     * @param   init_pri       Priority of before the USB is connected. (default: osPriorityLow).
     * @param   stack_size     stack size (in bytes) requirements for the thread function. (default: 2048).
     */
    DisplayApp(osPriority tsk_pri = osPriorityNormal, uint32_t stack_size = 2048);

    /** Send RGB888 data
     *
     * @param buf data buffer address
     * @param pic_width picture width
     * @param pic_height picture height
     * @return send data size
     */
    int SendRgb888(uint8_t * buf, uint32_t pic_width, uint32_t pic_height);

    /** Send JPEG data
     *
     * @param buf data buffer address
     * @param size data size
     * @return send data size
     */
    int SendJpeg(uint8_t * buf, uint32_t size);

    /** Attach a function to call whenever a serial interrupt is generated
     *
     * @param func A pointer to a void function, or 0 to set as none
     */
    void SetCallback(Callback<void()> func);

    /** Attach a function to call when touch panel int
     *
     * @param obj pointer to the object to call the member function on
     * @param method pointer to the member function to be called
     */
    template<typename T>
    void SetCallback(T* obj, void (T::*method)()) {
        // Underlying call thread safe
        SetCallback(callback(obj, method));
    }

    /** Attach a member function to call when touch panel int
     *
     * @param obj pointer to the object to call the member function on
     * @param method pointer to the member function to be called
     */
    template<typename T>
    void SetCallback(T* obj, void (*method)(T*)) {
        // Underlying call thread safe
        SetCallback(callback(obj, method));
    }
    /** Get the maximum number of simultaneous touches 
     * 
     * @return The maximum number of simultaneous touches.
     */
    int GetMaxTouchNum(void);

   /** Get the coordinates
     *
     * @param touch_buff_num The number of structure p_touch.
     * @param p_touch Touch position information.
     * @return The number of touch points.
     */
    int GetCoordinates(int touch_buff_num, touch_pos_t * p_touch);

private:
    typedef enum {
        POS_SEQ_INIT,
        POS_SEQ_START,
        POS_SEQ_X,
        POS_SEQ_X_POS,
        POS_SEQ_X_M,
        POS_SEQ_C,
        POS_SEQ_Y,
        POS_SEQ_Y_POS,
        POS_SEQ_Y_M,
        POS_SEQ_END,
    } pos_seq_t;

    USBSerial PcApp;
    Thread displayThread;
    pos_seq_t pos_seq;
    int pos_x;
    int pos_y;
    Callback<void()> event;

    void touch_int_callback(void);
    void display_app_process();
    void SendHeader(uint32_t size);
    void SendData(uint8_t * buf, uint32_t size);
};
#endif
