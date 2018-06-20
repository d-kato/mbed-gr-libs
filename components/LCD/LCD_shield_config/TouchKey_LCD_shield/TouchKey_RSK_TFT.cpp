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

#include "TouchKey_RSK_TFT.h"

TouchKey_RSK_TFT::TouchKey_RSK_TFT(PinName tprst, PinName tpint, PinName sda, PinName scl) :
 TouchKey(tprst, tpint), i2c(sda, scl) {
}

int TouchKey_RSK_TFT::GetMaxTouchNum(void) {
    return 5;
}

int TouchKey_RSK_TFT::GetCoordinates(int touch_buff_num, touch_pos_t * p_touch) {
    char buf[32];
    uint32_t wk_x;
    uint32_t wk_y;
    touch_pos_t * wk_touch;
    int count = 0;
    int i;
    int read_size;

    if (touch_buff_num > GetMaxTouchNum()) {
        touch_buff_num =  GetMaxTouchNum();
    }
    read_size = 2 + 6 * touch_buff_num;

    if (p_touch != NULL) {
        for (i = 0; i < touch_buff_num; i++) {
            wk_touch        = &p_touch[i];
            wk_touch->x     = 0;
            wk_touch->y     = 0;
            wk_touch->valid = false;
        }
        if (i2c.read((0x38 << 1), buf, read_size) == 0) {
            for (i = 0; i < touch_buff_num; i++) {
                if (buf[2] > i) {
                    wk_touch = &p_touch[i];
                    wk_x = (((uint32_t)buf[3 + (6 * i)] & 0x0F) << 8) | buf[4 + (6 * i)];
                    if (wk_x < 800) {
                        wk_touch->x = 800 - wk_x;
                    } else {
                        wk_touch->x = 0;
                    }
                    wk_y = (((uint32_t)buf[5 + (6 * i)] & 0x0F) << 8) | buf[6 + (6 * i)];
                    if (wk_y < 480) {
                        wk_touch->y = 480 - wk_y;
                    } else {
                        wk_touch->y = 0;
                    }
                    wk_touch->valid = 1;
                    count++;
                }
            }
        }
    }

    return count;
}


