/* mbed NullSpeaker Library
 * Copyright (C) 2016 dkato
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

#ifndef NULL_SPEAKER_H
#define NULL_SPEAKER_H

#include "mbed.h"
#include "AUDIO_RBSP.h"

/** NullSpeaker class
*
*/
class NullSpeaker : public AUDIO_RBSP {
public:
    /** Create a NullSpeaker
     * 
     */
    NullSpeaker() {}

    virtual ~NullSpeaker() {}

    virtual void power(bool type = true) {}

    virtual bool format(char length) {
        return true;
    }

    virtual bool frequency(int hz) {
        return true;
    }

    virtual int write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) {
        if (p_data_conf != NULL) {
            return data_size;
        }
        if (p_data_conf->p_notify_func != NULL) {
            p_data_conf->p_notify_func(p_data, data_size, p_data_conf->p_app_data);
        }
        return 0;
    }

    virtual int read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) {
        return -1;
    }

    virtual bool outputVolume(float leftVolumeOut, float rightVolumeOut) {
        return false;
    }

    virtual bool micVolume(float VolumeIn) {
        return false;
    }
};

#endif // NULL_SPEAKER_H
