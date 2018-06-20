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

#include "TouchKey_4_3inch.h"

TouchKey_4_3inch::TouchKey_4_3inch(PinName tprst, PinName tpint, PinName sda, PinName scl) :
 TouchKey(tprst, tpint), i2c(sda, scl) {
}

int TouchKey_4_3inch::GetMaxTouchNum(void) {
    return 2;
}

int TouchKey_4_3inch::GetCoordinates(int touch_buff_num, touch_pos_t * p_touch) {
    char buf[8];
    stx_report_data_t *pdata;
    touch_pos_t * wk_touch;
    int count = 0;
    int i;
    int read_size;

    if (touch_buff_num > GetMaxTouchNum()) {
        touch_buff_num =  GetMaxTouchNum();
    }
    if (touch_buff_num < 2) {
        read_size = 5;
    } else {
        read_size = 8;
    }

    if (p_touch != NULL) {
        for (i = 0; i < touch_buff_num; i++) {
            wk_touch        = &p_touch[i];
            wk_touch->x     = 0;
            wk_touch->y     = 0;
            wk_touch->valid = false;
        }
        if (i2c.read((0x55 << 1), buf, read_size) == 0) {
            pdata = (stx_report_data_t *)buf;
            if (pdata->fingers) {
                for (i = 0; i < touch_buff_num; i++) {
                    if (pdata->xyz_data[i].valid) {
                        wk_touch        = &p_touch[i];
                        wk_touch->x     = (pdata->xyz_data[i].x_h << 8) | pdata->xyz_data[i].x_l;
                        wk_touch->y     = (pdata->xyz_data[i].y_h << 8) | pdata->xyz_data[i].y_l;
                        wk_touch->valid = true;
                        count++;
                    }
                }
            }
        }
    }

    return count;
}


